#include "Aeroplane.h"

#include "AeroplaneGraphics.h"
#include "AeroplanePhysics.h"
#include "HumanController.h"
#include "AIControllerTug.h"
#include "Challenge.h"
#include "DimensionalScaling.h"
#include "PicaSim.h"
#include "IncomingConnection.h"

#include "tinyxml.h"

#include "Menus/PicaDialog.h"

static const float maxLookPitchAngle = PI * 0.4f;
static const float maxLookYawAngle = PI * 0.5f;

//======================================================================================================================
Aeroplane::Aeroplane(Controller& controller)
    : mController(&controller)
{
    mIncomingConnection = nullptr;
    mGraphics = new AeroplaneGraphics;
    mPhysics = new AeroplanePhysics;
    mFlightTime = 0.0f;
    mLookYaw = mLookPitch = mLookYawRate = mLookPitchRate = 0.0f;
    mCrashFlags = 0;

    PicaSim::GetInstance().AddAeroplane(this);
}

//======================================================================================================================
Aeroplane::~Aeroplane()
{
    PicaSim::GetInstance().RemoveAeroplane(this);
    delete mGraphics;
    delete mPhysics;
}

//======================================================================================================================
void Aeroplane::Init(const AeroplaneSettings& as,
                     const Vector3* basicLaunchPos,
                     LoadingScreenHelper* loadingScreen)
{
    TRACE_METHOD_ONLY(1);
    mAeroplaneSettings = as;

    if (loadingScreen) loadingScreen->Update("Aeroplane");

    mChecksum = 0;
    std::string aeroplaneFile = mAeroplaneSettings.mName + "/Aeroplane.xml";
    GetFileChecksum(mChecksum, aeroplaneFile.c_str());

    TiXmlDocument doc(aeroplaneFile.c_str());
    bool ok = doc.LoadFile();
    if (!ok)
    {
        const Language language = PicaSim::GetInstance().GetSettings().mOptions.mLanguage;
        ShowDialog("PicaSim", "Failed to load aeroplane - using default!", TXT(PS_OK));

        mAeroplaneSettings = AeroplaneSettings();
        doc = TiXmlDocument(mAeroplaneSettings.mName + "/Aeroplane.xml");
        ok = doc.LoadFile();
    }
    IwAssert(ROWLHOUSE, ok);

    // Get physics to initialise in approximately the right place
    if (basicLaunchPos)
    {
        mTM.SetIdentity();
        mTM.SetTrans(*basicLaunchPos);
        mLastLaunchPos = *basicLaunchPos;
    }
    else
    {
        mLastLaunchPos = Vector3(0, 0, 0);
    }

    if (loadingScreen) loadingScreen->Update("Aeroplane physics");
    mPhysics->Init(doc, this, mChecksum);
    if (loadingScreen) loadingScreen->Update("Aeroplane graphics");
    mGraphics->Init(doc, this);

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_LOOP_PRE_PHYSICS);
    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_LOOP_POST_PHYSICS+1);

    if (loadingScreen) loadingScreen->Update("Aeroplane sound");

    TRACE_FILE_IF(1) TRACE("Aeroplane::Init Audio");
    TiXmlHandle audioHandle = doc.FirstChild("Audio");
    for (int iAudio = 0 ; ; ++iAudio)
    {
        TiXmlElement* dataElement = audioHandle.Child("AirFlow", iAudio).ToElement();
        if (!dataElement)
            break;

        SoundSetting soundSetting;

        std::string file = readStringFromXML(dataElement, "file");
        int sampleRate = readIntFromXML(dataElement, "sampleRate");
        soundSetting.mSound = AudioManager::GetInstance().LoadSound(file.c_str(), sampleRate, false, true, true);
        if (soundSetting.mSound)
        {
            soundSetting.mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(mGraphics->GetRenderBoundingRadius(), true);
            if (soundSetting.mSoundChannel != -1)
            {
                AudioManager::GetInstance().StartSoundOnChannel(soundSetting.mSoundChannel, soundSetting.mSound, true);
                soundSetting.mMinSpeedRelAir = readFloatFromXML(dataElement, "mMinSpeedRelAir");
                soundSetting.mFreqScalePerSpeed = readFloatFromXML(dataElement, "mFreqScalePerSpeed");
                soundSetting.mMinFreqScale = readFloatFromXML(dataElement, "mMinFreqScale");
                soundSetting.mMaxFreqScale = readFloatFromXML(dataElement, "mMaxFreqScale");
                soundSetting.mVolScalePerSpeed = readFloatFromXML(dataElement, "mVolScalePerSpeed");
                soundSetting.mVolPow = readFloatFromXML(dataElement, "mVolPow");
                soundSetting.mMaxVolume = readFloatFromXML(dataElement, "mMaxVolume");
            }
        }
        mSoundSettings.push_back(soundSetting);
    }

    // Optional vario sound
#if 1
    TRACE_FILE_IF(1) TRACE("Aeroplane::Init Vario");
    mVarioSoundSetting.mSound = AudioManager::GetInstance().LoadSound("SystemData/Audio/Vario22050Mono.raw", 22050, false, true, true);
    if (mVarioSoundSetting.mSound)
    {
        mVarioSoundSetting.mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(0.5f, true);
        if (mVarioSoundSetting.mSoundChannel != -1)
        {
            AudioManager::GetInstance().StartSoundOnChannel(mVarioSoundSetting.mSoundChannel, mVarioSoundSetting.mSound, true);
            mVarioSoundSetting.mMinAscentRate = 0.03f;
            mVarioSoundSetting.mFreqScalePerSpeed = 0.07f;
            mVarioSoundSetting.mMinFreqScale = 1.0f;
            mVarioSoundSetting.mMaxFreqScale = 2.0f;
        }
    }
#endif

    if (loadingScreen) loadingScreen->Update("Aeroplane sound");

    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
    if (es.mAllowBungeeLaunch && 
            mAeroplaneSettings.mFlatLaunchMethod == AeroplaneSettings::FLAT_LAUNCH_AEROTOW
            )
    {
        TugAeroplaneModifiers tugModifiers;
        tugModifiers.mSizeScale = as.mTugSizeScale;
        tugModifiers.mMassScale = as.mTugMassScale;
        tugModifiers.mEngineScale = as.mTugEngineScale;

        mTugControllerSetting = AIControllersSettings::AIControllerSetting();
        mTugControllerSetting.mAeroplaneFile = mAeroplaneSettings.mTugName;
        mTugControllerSetting.mIncludeInCameraViews = false;
        mTugControllerSetting.mEnableDebugDraw = false;
        mTugController = new AIControllerTug(mTugControllerSetting, tugModifiers, 0);
        mTugController->Init(loadingScreen);

        AeroplaneSettings tas = mTugController->GetAeroplane()->GetAeroplaneSettings();
        tas.mRelaunchTime = FLT_MAX;
        tas.mAIAeroplaneSettings.mControlMaxBankAngle = 25.0f;
        tas.mAIAeroplaneSettings.mControlRollTimeScale = 0.5f;
        tas.mAIAeroplaneSettings.mPoweredControlCruiseSpeed = as.mTugTargetSpeed;
        tas.mAIAeroplaneSettings.mPoweredControlMaxSlopeDown = 0.0f;
        tas.mAIAeroplaneSettings.mPoweredControlMaxSlopeUp = as.mTugMaxClimbSlope;
        tas.mAIAeroplaneSettings.mPoweredControlThrottleControlPerAltitude = 0.1f;
        tas.mAIAeroplaneSettings.mPoweredControlThrottleControlPerSpeed = 1.0f;
        tas.mAIAeroplaneSettings.mPoweredMinHeight = as.mAeroTowHeight;
        tas.mAIAeroplaneSettings.mPoweredMaxHeight = as.mAeroTowHeight;
        tas.mAIAeroplaneSettings.mPoweredMaxDistance = as.mAeroTowCircuitSize;
        tas.mAIAeroplaneSettings.mWaypointTolerance = 30.0f;
        tas.mAIAeroplaneSettings.mPoweredMaxWaypointTime = 30.0f;
        tas.mAIAeroplaneSettings.mPoweredTakeoffDistance = as.mAeroTowCircuitSize;
        mTugController->GetAeroplane()->SetAeroplaneSettings(tas);
        mTugController->Reset();
    }
    else
    {
        mTugController = 0;
    }

    if (basicLaunchPos)
    {
        Launch(*basicLaunchPos);
    }
    else
    {
        ResetControls();
        mPhysics->SetVelocity(mVelocity, mAngularVelocity);

        mSmoothedCameraTargetPos = mTM.TransformVec(Vector3(mAeroplaneSettings.mCameraTargetPosFwd, 0.0f, mAeroplaneSettings.mCameraTargetPosUp) * mAeroplaneSettings.mSizeScale);
        mSmoothedCameraTargetPosRate = Vector3(0,0,0);

        mSmoothedCameraVelocity = mVelocity;
        mSmoothedCameraVelocityRate = Vector3(0,0,0);

        mSmoothedVelocityForCamera = mVelocity;
        mSmoothedVelocityForCameraRate = Vector3(0,0,0);
    }

    mDebugAerofoilIndex = 0;
    mCrashFlags = 0;

    mBungeeAmount = 0.0f;
    mAirDensity = 1.225f;
}

//======================================================================================================================
void Aeroplane::ResetControls()
{
    // This is really ugly - but we want to make sure we're not in a weird state after relaunching if possible.
    if (this == PicaSim::GetInstance().GetPlayerAeroplane())
    {
        if (PicaSim::GetInstance().GetSettings().mControllerSettings.mResetAltSettingOnLaunch)
            PicaSim::GetInstance().GetSettings().mControllerSettings.mCurrentAltSetting = 0;
    }
    GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON0, -1.0f);
    GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON1, -1.0f);
    GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON2, -1.0f);
}

//======================================================================================================================
void Aeroplane::Launch(const Vector3& basicLaunchPos)
{
    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;

    bool launched = false;
    if (mTugController)
    {
        if (LaunchAerotow(basicLaunchPos))
        {
            launched = true;
            mLaunchMode = LAUNCHMODE_AEROTOW;
        }
    }
    if (
        !launched && 
        es.mAllowBungeeLaunch && 
        mAeroplaneSettings.mFlatLaunchMethod == AeroplaneSettings::FLAT_LAUNCH_BUNGEE && 
        mAeroplaneSettings.mMaxBungeeAcceleration > 0.0f)
    {
        if (LaunchBungee(basicLaunchPos))
        {
            launched = true;
            mLaunchMode = LAUNCHMODE_BUNGEE;
        }
    }
    if (!launched)
    {
        LaunchNormal(basicLaunchPos);
        mLaunchMode = LAUNCHMODE_NONE;
    }

    mFlightTime = 0.0f;
    mCrashFlags = 0;

    mPhysics->Launched();

    mSmoothedCameraTargetPos = mTM.TransformVec(Vector3(mAeroplaneSettings.mCameraTargetPosFwd, 0.0f, mAeroplaneSettings.mCameraTargetPosUp) * mAeroplaneSettings.mSizeScale);
    mSmoothedCameraTargetPosRate = Vector3(0,0,0);

    mSmoothedCameraVelocity = mVelocity;
    mSmoothedCameraVelocityRate = Vector3(0,0,0);

    mSmoothedVelocityForCamera = mVelocity;
    mSmoothedVelocityForCameraRate = Vector3(0,0,0);

    ResetControls();
}

//======================================================================================================================
float Aeroplane::GetHeightForNoGroundPenetration(const Vector3& launchPos) const
{
    int numPts = 3;
    float r = mPhysics->getAABBRadius();
    float height = -FLT_MAX;
    for (int i = 0 ; i != numPts ; ++i)
    {
        for (int j = 0 ; j != numPts ; ++j)
        {
            float x = numPts > 1 ? launchPos.x - r + (2.0f * r) * i / (numPts - 1.0f) : launchPos.x;
            float y = numPts > 1 ? launchPos.y - r + (2.0f * r) * j / (numPts - 1.0f) : launchPos.y;
            float h = Environment::GetInstance().GetTerrain().GetTerrainHeight(x, y, true);
            if (h > height)
                height = h;
        }
    }
    return height;
}

//======================================================================================================================
void Aeroplane::LaunchNormal(const Vector3& basicLaunchPos)
{
    TRACE_METHOD_ONLY(1);

    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
    const AeroplaneSettings& as = mAeroplaneSettings;
    DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);

    Vector3 fwdDir = -Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    Vector3 leftDir = -fwdDir.Cross(Vector3(0,0,1));
    Quat q(leftDir, -DegreesToRadians(mAeroplaneSettings.mLaunchAngleUp));
    Vector3 launchDir = q.RotateVector(fwdDir);
    SetRowX(mTM, launchDir);
    SetRowY(mTM, leftDir);
    SetRowZ(mTM, mTM.RowX().Cross(mTM.RowY()).GetNormalised());

    mLastLaunchPos = basicLaunchPos + 
        fwdDir * ds.GetScaledLength(mAeroplaneSettings.mLaunchForwards) + 
        leftDir * ds.GetScaledLength(mAeroplaneSettings.mLaunchLeft);

    float height = GetHeightForNoGroundPenetration(mLastLaunchPos);

    mLastLaunchPos.z = height + ds.GetScaledLength(mAeroplaneSettings.mLaunchUp - mAeroplaneSettings.mLaunchOffsetUp);

    mTM.SetTrans(mLastLaunchPos);

    Vector3 windAtLaunchPos = Environment::GetInstance().GetWindAtPosition(mTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    Vector3 basicLaunchVel = mTM.RowX() * ds.GetScaledVel(mAeroplaneSettings.mLaunchSpeed);
    Vector3 launchVel = windAtLaunchPos + basicLaunchVel;

    if (launchVel.Dot(mTM.RowX()) < 0.0f)
        launchVel += -launchVel.Dot(mTM.RowX()) * mTM.RowX();

    SetTransform(mTM, launchVel, Vector3(0,0,0));
}

//======================================================================================================================
void Aeroplane::LaunchAtPosition(const Vector3& pos, const Vector3& dir)
{
    TRACE_METHOD_ONLY(1);
    const AeroplaneSettings& as = mAeroplaneSettings;
    DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);

    Vector3 fwd(dir.x, dir.y, 0.0f);
    fwd.Normalise();
    Vector3 up = Vector3(0,0,1);
    Vector3 left = up.Cross(fwd);
    SetRowX(mTM, fwd);
    SetRowY(mTM, left);
    SetRowZ(mTM, up);

    mLastLaunchPos = pos;
    float height = GetHeightForNoGroundPenetration(mLastLaunchPos);
    mLastLaunchPos.z = height - ds.GetScaledLength(mAeroplaneSettings.mLaunchOffsetUp);
    mTM.SetTrans(mLastLaunchPos);
    SetTransform(mTM, Vector3(0,0,0), Vector3(0,0,0));

    mFlightTime = 0.0f;
    mCrashFlags = 0;

    mPhysics->Launched();

    mSmoothedCameraTargetPos = ds.GetScaledLength(mTM.TransformVec(Vector3(mAeroplaneSettings.mCameraTargetPosFwd, 0.0f, mAeroplaneSettings.mCameraTargetPosUp)));
    mSmoothedCameraTargetPosRate = Vector3(0,0,0);

    mSmoothedCameraVelocity = mVelocity;
    mSmoothedCameraVelocityRate = Vector3(0,0,0);

    mSmoothedVelocityForCamera = mVelocity;
    mSmoothedVelocityForCameraRate = Vector3(0,0,0);

    ResetControls();
}

//======================================================================================================================
bool Aeroplane::LaunchBungee(const Vector3& basicLaunchPos)
{
    TRACE_METHOD_ONLY(1);

    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
    const AeroplaneSettings& as = mAeroplaneSettings;
    DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);

    Vector3 fwdDir = -Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    Vector3 leftDir = -fwdDir.Cross(Vector3(0,0,1));
    Quat q(leftDir, -DegreesToRadians(mAeroplaneSettings.mLaunchAngleUp));
    Vector3 launchDir = q.RotateVector(fwdDir);
    SetRowX(mTM, launchDir);
    SetRowY(mTM, leftDir);
    SetRowZ(mTM, mTM.RowX().Cross(mTM.RowY()).GetNormalised());

    mLastLaunchPos = basicLaunchPos + 
        fwdDir * ds.GetScaledLength(mAeroplaneSettings.mLaunchForwards) + 
        leftDir * ds.GetScaledLength(mAeroplaneSettings.mLaunchLeft);

    float height = GetHeightForNoGroundPenetration(mLastLaunchPos);

    mLastLaunchPos.z = height + ds.GetScaledLength(mAeroplaneSettings.mLaunchUp - mAeroplaneSettings.mLaunchOffsetUp);

    mTM.SetTrans(mLastLaunchPos);

    Vector3 windAtLaunchPos = Environment::GetInstance().GetWindAtPosition(mTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    Vector3 basicLaunchVel = mTM.RowX() * ds.GetScaledVel(mAeroplaneSettings.mLaunchSpeed);
    Vector3 launchVel = windAtLaunchPos + basicLaunchVel;

    if (launchVel.Dot(mTM.RowX()) < 0.0f)
        launchVel += -launchVel.Dot(mTM.RowX()) * mTM.RowX();

    SetTransform(mTM, launchVel, Vector3(0,0,0));

    const float bungeeDistance = ds.GetScaledLength(mAeroplaneSettings.mMaxBungeeLength);
    mBungeePosition = mTM.GetTrans() + fwdDir * bungeeDistance;
    mBungeePosition.z = Environment::GetInstance().GetTerrain().GetTerrainHeight(mBungeePosition.x, mBungeePosition.y, true);

    return true;
}

//======================================================================================================================
bool Aeroplane::LaunchAerotow(const Vector3& basicLaunchPos)
{
    TRACE_METHOD_ONLY(1);

    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
    const AeroplaneSettings& as = mAeroplaneSettings;
    DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);
    Challenge* challenge = PicaSim::GetInstance().GetChallenge();

    // First we reset ourselves. Then we will relaunch the tug, and hook ourselves on.

    Vector3 fwdDir = -Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    Vector3 leftDir = -fwdDir.Cross(Vector3(0,0,1));
    SetRowX(mTM, fwdDir);
    SetRowY(mTM, leftDir);
    SetRowZ(mTM, mTM.RowX().Cross(mTM.RowY()).GetNormalised());

    mLastLaunchPos = basicLaunchPos + 
        fwdDir * ds.GetScaledLength(mAeroplaneSettings.mLaunchForwards) + 
        leftDir * ds.GetScaledLength(mAeroplaneSettings.mLaunchLeft);

    mLastLaunchPos.z = GetHeightForNoGroundPenetration(mLastLaunchPos) - ds.GetScaledLength(mAeroplaneSettings.mLaunchOffsetUp);
    mTM.SetTrans(mLastLaunchPos);

    SetTransform(mTM, Vector3(0,0,0), Vector3(0,0,0));

    mLaunchMode = LAUNCHMODE_AEROTOW;

    // Now the tug
    Aeroplane* tugAeroplane = mTugController->GetAeroplane();
    IwAssert(ROWLHOUSE, tugAeroplane);
    if (tugAeroplane)
    {
        const AeroplaneSettings& tas = tugAeroplane->GetAeroplaneSettings();
        DimensionalScaling tds(tas.mSizeScale, tas.mMassScale, true);

        Vector3 noseHookPos = mTM.TransformVec(ds.GetScaledLength(mAeroplaneSettings.mNoseHookOffset));
        Vector3 tugLaunchPos = noseHookPos + fwdDir * ds.GetScaledLength(mAeroplaneSettings.mAeroTowRopeLength);
        tugLaunchPos += fwdDir * tds.GetScaledLength(tas.mTailHookOffset.x);

        tugAeroplane->LaunchAtPosition(tugLaunchPos, fwdDir);
        mPhysics->AttachToTug(*tugAeroplane);
        mTugController->AttachToGlider(this);
        mTugController->ChooseNewWaypoint(true);
    }
    return true;
}

//======================================================================================================================
void Aeroplane::Terminate()
{
    TRACE_METHOD_ONLY(1);
    if (mTugController)
    {
        mTugController->Terminate();
        delete mTugController;
        mTugController = 0;
    }

    mPhysics->Terminate();
    mGraphics->Terminate();

    for (size_t i = 0 ; i != mSoundSettings.size() ; ++i)
    {
        if (mSoundSettings[i].mSoundChannel != -1)
            AudioManager::GetInstance().ReleaseSoundChannel(mSoundSettings[i].mSoundChannel);
        if (mSoundSettings[i].mSound)
            AudioManager::GetInstance().UnloadSound(mSoundSettings[i].mSound);
    }
    mSoundSettings.clear();

    if (mVarioSoundSetting.mSoundChannel != -1)
            AudioManager::GetInstance().ReleaseSoundChannel(mVarioSoundSetting.mSoundChannel);
    if (mVarioSoundSetting.mSound)
        AudioManager::GetInstance().UnloadSound(mVarioSoundSetting.mSound);

    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_LOOP_PRE_PHYSICS);
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_LOOP_POST_PHYSICS+1);
}

//======================================================================================================================
void Aeroplane::EntityUpdate(float deltaTime, int entityLevel)
{
    if (entityLevel == ENTITY_LEVEL_LOOP_PRE_PHYSICS)
        EntityUpdateLoopPrePhysics(deltaTime);
    else if (entityLevel == ENTITY_LEVEL_LOOP_POST_PHYSICS+1)
        EntityUpdateLoopPostPhysics(deltaTime);
    else
        EntityUpdatePostPhysics(deltaTime);
}

//======================================================================================================================
void Aeroplane::EntityUpdateLoopPostPhysics(float deltaTime)
{
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;

    mVelocity = mPhysics->GetVelocity();
    mAngularVelocity = mPhysics->GetAngularVelocity();
    mTM = mPhysics->GetTransform();

    // Smooth lookaround
    float gameTimeDelta = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
    float yaw = mController->GetControl(Controller::CHANNEL_LOOKYAW);
    float pitch = -mController->GetControl(Controller::CHANNEL_LOOKPITCH);
    SmoothCD(mLookYaw, mLookYawRate, gameTimeDelta, yaw, 0.5f);
    SmoothCD(mLookPitch, mLookPitchRate, gameTimeDelta, pitch, 0.5f);

    // Smooth the velocity of the camera
    SmoothCD(mSmoothedCameraVelocity, mSmoothedCameraVelocityRate, deltaTime, mVelocity, 0.1f);

    // Smoothed version of our position for the camera target
    float smoothTime = options.mGroundViewLag * (1.0f - options.mGroundViewAutoZoom);
    SmoothCD(mSmoothedVelocityForCamera, mSmoothedVelocityForCameraRate, deltaTime, mVelocity, 1.0f);
    Vector3 targetPos = 
            mTM.TransformVec(Vector3(mAeroplaneSettings.mCameraTargetPosFwd, 0.0f, mAeroplaneSettings.mCameraTargetPosUp) * mAeroplaneSettings.mSizeScale)
            + mSmoothedVelocityForCamera * (1.0f * smoothTime);

    SmoothCD(mSmoothedCameraTargetPos, mSmoothedCameraTargetPosRate, deltaTime, targetPos, smoothTime);

    // Perhaps send telemetry data
    if (mIncomingConnection)
        mIncomingConnection->SendAgentMessages(this, deltaTime);
}

//======================================================================================================================
void Aeroplane::EntityUpdatePostPhysics(float deltaTime)
{
    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;

    mDebugAerofoilIndex = 0;

    mAirDensity = Environment::GetAirDensity(mTM.GetTrans());

    Vector3 velRelWind = mPhysics->GetVelocity() - Environment::GetInstance().GetWindAtPosition(
        mTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    float speedRelWind = velRelWind.GetLength();

    for (size_t i = 0 ; i != mSoundSettings.size() ; ++i)
    {
        const SoundSetting& soundSetting = mSoundSettings[i];
        if (!soundSetting.mSound || soundSetting.mSoundChannel == -1)
            continue;

        float reducedSpeedRelWind = Maximum(speedRelWind - soundSetting.mMinSpeedRelAir, 0.0f);

        float freqScale = reducedSpeedRelWind * soundSetting.mFreqScalePerSpeed;
        freqScale = ClampToRange(freqScale, soundSetting.mMinFreqScale, soundSetting.mMaxFreqScale);

        float volumeScale = powf(reducedSpeedRelWind * soundSetting.mVolScalePerSpeed, soundSetting.mVolPow);
        volumeScale = ClampToRange(volumeScale, 0.0f, soundSetting.mMaxVolume);

        if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_PAUSED)
            volumeScale = 0.0f;

        freqScale *= PicaSim::GetInstance().GetTimeScale();

        AudioManager::GetInstance().SetChannelPositionAndVelocity(soundSetting.mSoundChannel, mTM.GetTrans(), mPhysics->GetVelocity());
        AudioManager::GetInstance().SetChannelFrequencyScale(soundSetting.mSoundChannel, freqScale);
        if (PicaSim::GetInstance().GetMode() == PicaSim::MODE_GROUND)
            AudioManager::GetInstance().SetChannelTargetVolumeScale(soundSetting.mSoundChannel, PicaSim::GetInstance().GetSettings().mOptions.mOutsideAeroplaneVolume * volumeScale);
        else
            AudioManager::GetInstance().SetChannelTargetVolumeScale(soundSetting.mSoundChannel, PicaSim::GetInstance().GetSettings().mOptions.mInsideAeroplaneVolume * volumeScale);
    }

    if (mVarioSoundSetting.mSound && mVarioSoundSetting.mSoundChannel != -1)
    {
        float ascentRate = mPhysics->GetVelocity().z;
        float freqScale = mVarioSoundSetting.mMinFreqScale + 
            mVarioSoundSetting.mFreqScalePerSpeed * (ascentRate - mVarioSoundSetting.mMinAscentRate);
        float volumeScale = 0.0f;
        bool bungeeOrAerotow = mAeroplaneSettings.mFlatLaunchMethod != AeroplaneSettings::FLAT_LAUNCH_HAND && es.mAllowBungeeLaunch;
        if (
            freqScale < mVarioSoundSetting.mMinFreqScale || 
            PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_PAUSED || 
            (mAeroplaneSettings.mHasVariometer == 0) ||
            (mAeroplaneSettings.mHasVariometer == 1 && !bungeeOrAerotow)
            )
        {
            volumeScale = 0.0f;
        }
        else
        {
            volumeScale = PicaSim::GetInstance().GetSettings().mOptions.mVariometerVolume;
        }
        freqScale = Minimum(freqScale, mVarioSoundSetting.mMaxFreqScale);

        if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_PAUSED)
            volumeScale = 0.0f;

        AudioManager::GetInstance().SetChannelPositionAndVelocity(mVarioSoundSetting.mSoundChannel, mTM.GetTrans(), mPhysics->GetVelocity());
        AudioManager::GetInstance().SetChannelFrequencyScale(mVarioSoundSetting.mSoundChannel, freqScale);
        if (PicaSim::GetInstance().GetMode() == PicaSim::MODE_GROUND)
            AudioManager::GetInstance().SetChannelTargetVolumeScale(mVarioSoundSetting.mSoundChannel,
            PicaSim::GetInstance().GetSettings().mOptions.mOutsideAeroplaneVolume * volumeScale);
        else
            AudioManager::GetInstance().SetChannelTargetVolumeScale(mVarioSoundSetting.mSoundChannel, 
            PicaSim::GetInstance().GetSettings().mOptions.mInsideAeroplaneVolume * volumeScale);
    }

    if (mController->GetControl(Controller::CHANNEL_HOOK) > 0.0f)
    {
        mLaunchMode = LAUNCHMODE_NONE;
    }

    if (mTugController && mLaunchMode != LAUNCHMODE_AEROTOW)
    {
        mTugController->AttachToGlider(0);
    }

    Vector3 bungeePosition, hookPosition;
    if (IsUsingBungee(bungeePosition, hookPosition))
    {
        if (deltaTime > 0.0f)
            mBungeeVel = (hookPosition - mBungeeEnd) / deltaTime;
        else
            mBungeeVel = Vector3(0,0,0);
        mBungeeEnd = hookPosition;
        mBungeeAmount = 1.0f;
    }
    else
    {
        mBungeeVel.z -= 9.81f * deltaTime;
        mBungeeVel *= expf(-deltaTime / 1.0f);
        mBungeeEnd += mBungeeVel * deltaTime;
        mBungeeAmount = Maximum(mBungeeAmount - deltaTime * 0.4f, 0.0f);
    }

    if (mBungeeAmount > 0.0f)
    {
        mBungeeRope.SetColour(Vector4(1,1,1,mBungeeAmount * 0.2f));

        static size_t numPts = 10;
        Rope::Points& points = mBungeeRope.GetPoints();
        points.resize(numPts);

        Vector3 start = bungeePosition;
        Vector3 end = mBungeeEnd;

        float A = 0.5f;
        float B = 1.0f - A;

        for (size_t iPt = 0 ; iPt != numPts ; ++iPt)
        {
            float x = float(iPt) / (numPts - 1);
            float y = A * Square(x) + B * x;
            Vector3 p = start + (end - start) * x;
            p.z = start.z + (end.z - start.z) * y;
            points[iPt] = p;
        }

        mBungeeEnd = end;
    }
    else
    {
        mBungeeRope.GetPoints().resize(0);
    }
}

//======================================================================================================================
void Aeroplane::EntityUpdateLoopPrePhysics(float deltaTime)
{
    const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;

    mFlightTime += deltaTime;

    if (mLaunchMode == LAUNCHMODE_BUNGEE)
    {
        Vector3 hookPosition = mTM.TransformVec(mAeroplaneSettings.mBellyHookOffset * mAeroplaneSettings.mSizeScale);
        Vector3 dirToBungee = mBungeePosition - hookPosition;
        float distToBungee = dirToBungee.GetLength();
        dirToBungee *= 1.0f/distToBungee;

        const float stringLen = mAeroplaneSettings.mMaxBungeeLength * 0.5f * mAeroplaneSettings.mSizeScale;
        const float springLen = mAeroplaneSettings.mMaxBungeeLength * mAeroplaneSettings.mSizeScale - stringLen;

        distToBungee -= stringLen;
        if (distToBungee > 0.0f)
        {
            float dot = dirToBungee.Dot(mTM.RowX());
            if (dot > -0.0f)
            {
                const float maxForce = mPhysics->GetMass() * mAeroplaneSettings.mMaxBungeeAcceleration;
                float forceMag = (distToBungee / springLen) * maxForce;
                Vector3 force = forceMag * dirToBungee;

                mPhysics->ApplyWorldForceAtWorldPosition(force, hookPosition);
            }
            else
            {
                mLaunchMode = LAUNCHMODE_NONE;
            }
        }
        else
        {
            mLaunchMode = LAUNCHMODE_NONE;
        }
    }
}

//======================================================================================================================
bool Aeroplane::IsUsingBungee(Vector3& bungeePosition, Vector3& hookPosition) const
{
    bungeePosition = mBungeePosition; 
    hookPosition = mTM.TransformVec(mAeroplaneSettings.mBellyHookOffset * mAeroplaneSettings.mSizeScale);
    return mLaunchMode == LAUNCHMODE_BUNGEE;
}

//======================================================================================================================
Transform Aeroplane::GetCameraTransform(void* cameraUserData) const
{
    Transform yawTM;
    yawTM.SetRotZ(mLookYaw * maxLookYawAngle);

    CameraID mode = (CameraID) (size_t) cameraUserData;
    if (mode == CAMERA_AEROPLANE)
    {
        Transform pitchTM;
        pitchTM.SetRotY(mLookPitch * PI * 0.5f + DegreesToRadians(mAeroplaneSettings.mCockpitCamPitch));
        return pitchTM * yawTM * mTM;
    }
    else
    {
        IwAssert(ROWLHOUSE, mode == CAMERA_CHASE);

        Transform tm = mTM;
        Vector3 p = mTM.GetTrans();

        Vector3 horVel(mSmoothedCameraVelocity.x, mSmoothedCameraVelocity.y, mSmoothedCameraVelocity.z * mAeroplaneSettings.mChaseCamVerticalVelMult);
        float horSpeed = horVel.GetLength();

        Vector3 faceDir = tm.RowX();
        Vector3 horFaceDir(faceDir.x, faceDir.y, 0.0f);

        float velFrac = horSpeed / 5.0f;
        velFrac = ClampToRange(velFrac, 0.0f, 1.0f);

        Vector3 dir = horVel * velFrac + horFaceDir;
        dir.Normalise();

        // Look around pitch
        Transform pitchTM;
        Vector3 pitchDir(dir.y, -dir.x, 0.0f);
        pitchDir.Normalise();
        pitchTM.SetAxisAngle(pitchDir, mLookPitch * maxLookPitchAngle);
        Transform lookModifierTM = pitchTM * yawTM;
        dir = lookModifierTM.RotateVec(dir);

        p += -mAeroplaneSettings.mChaseCamDistance * dir * mAeroplaneSettings.mSizeScale;
        p.z += mAeroplaneSettings.mChaseCamHeight * mAeroplaneSettings.mSizeScale;

        Vector3 fixedPos =  
            (-mAeroplaneSettings.mChaseCamDistance * mTM.RowX() + mAeroplaneSettings.mChaseCamHeight * mTM.RowZ()) * mAeroplaneSettings.mSizeScale;
        fixedPos = lookModifierTM.RotateVec(fixedPos);
        fixedPos += mTM.GetTrans();

        p += (fixedPos - p) * (1.0f - mAeroplaneSettings.mChaseCamFlexibility);

        // Prevent going below the terrain
        float z = Environment::GetInstance().GetTerrain().GetTerrainHeight(p.x, p.y, false);
        const float minAltitude = 0.2f;
        if (p.z < z + minAltitude)
            p.z = z + minAltitude;

        tm.SetTrans(p);
        return tm;
    }
}

//======================================================================================================================
Vector3 Aeroplane::GetCameraTargetPosition(
    const Vector3& cameraPosition, 
    const void*    cameraUserData, 
    float&         targetRadius,
    float&         closestDistanceToCamera) const
{
    CameraID mode = (CameraID) (size_t) cameraUserData;
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;

    // If on the ground, blend with a horizon position
    if (mode == CAMERA_GROUND)
    {
        targetRadius = mGraphics->GetRenderBoundingRadius();
        Vector3 targetPos = mSmoothedCameraTargetPos;
    
        float distToCamera = (targetPos - cameraPosition).GetLength();
        float curVerticalOffset = targetPos.z - cameraPosition.z;
        float scale = options.mGroundViewHorizonAmount;
        scale *= 1.0f - options.mGroundViewAutoZoom;

        curVerticalOffset *= scale;
        targetPos.z -= curVerticalOffset;

        // Apply lookabout
        Vector3 delta = targetPos - cameraPosition;

        Transform yawTM;
        yawTM.SetRotZ(mLookYaw * maxLookYawAngle);

        Transform pitchTM;
        Vector3 pitchDir(delta.y, -delta.x, 0.0f);
        pitchDir.Normalise();
        pitchTM.SetAxisAngle(pitchDir, mLookPitch * maxLookPitchAngle);
        delta = (pitchTM * yawTM).RotateVec(delta);

        targetPos = delta + cameraPosition;

        float dist = delta.GetLength();
        closestDistanceToCamera = dist - targetRadius;

        return targetPos;
    }
    else
    {
        Vector3 targetPos = mTM.TransformVec(
            Vector3(mAeroplaneSettings.mCameraTargetPosFwd, 0.0f, mAeroplaneSettings.mCameraTargetPosUp)
            * mAeroplaneSettings.mSizeScale);

        targetRadius = mGraphics->GetRenderBoundingRadius();

        float dist = (targetPos - cameraPosition).GetLength();
        closestDistanceToCamera = dist - targetRadius;

        if (mLaunchMode == LAUNCHMODE_AEROTOW && mTugController && mTugController->GetAeroplane())
        {
            const Aeroplane* tugAeroplane = mTugController->GetAeroplane();
            float tugRadius = 0.0f;
            float junk;
            Vector3 tugTargetPos = tugAeroplane->GetCameraTargetPosition(cameraPosition, cameraUserData, tugRadius, junk);

            Vector3 lineOfSight = (tugTargetPos - cameraPosition).GetNormalised();
            Vector3 delta = (tugTargetPos - targetPos);
            delta += delta.GetNormalised() * (0.5f * (targetRadius + tugRadius));
            delta -= 0.5f * delta.Dot(lineOfSight) * lineOfSight;

            targetRadius = 0.5f * delta.GetLength();

            targetPos = 0.5f * (targetPos + tugTargetPos);
        }
        return targetPos;
    }
}

//======================================================================================================================
void Aeroplane::SetTransform(const Transform& tm, const Vector3& vel, const Vector3& angVel) 
{
    mTM = tm;
    mVelocity = vel;
    mAngularVelocity = angVel;
    mPhysics->SetTransform(tm, vel, angVel);
}
