#include "AIControllerTug.h"
#include "Menus/PicaDialog.h"
#include "AeroplanePhysics.h"
#include "AeroplaneGraphics.h"
#include "PicaSim.h"

//======================================================================================================================
TugAeroplaneModifiers::TugAeroplaneModifiers()
    :
    mSizeScale(1.0f),
    mEngineScale(1.0f),
    mMassScale(1.0f)
{}

//======================================================================================================================
Vector3 AIControllerTug::GetLaunchPos() const
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIControllersSettings& aics = gs.mAIControllersSettings;

    Vector3 observerCameraPosition = PicaSim::GetInstance().GetObserver().GetCameraTransform(0).GetTrans();
    float altitude = observerCameraPosition.z - Environment::GetInstance().GetTerrain().GetTerrainHeight(observerCameraPosition.x, observerCameraPosition.y, true);

    const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(observerCameraPosition, Environment::WIND_TYPE_SMOOTH);
    Vector3 horWindDir(windVel.x, windVel.y, 0.0f);
    SafeNormalise(horWindDir, Vector3(1, 0, 0));
    const Vector3 horWindLeftDir = horWindDir.Cross(Vector3(0.0f, 0.0f, 1.0f));

    Quat q(horWindDir, -(1.0f + aics.mLaunchDirection) * PI * 0.5f);
    const Vector3 launchDir = q.RotateVector(horWindLeftDir);

    Vector3 launchPos = observerCameraPosition + launchDir *aics.mLaunchSeparationDistance * float(mAIControllerIndex+2);

    launchPos.z = Maximum(launchPos.z, altitude + Environment::GetInstance().GetTerrain().GetTerrainHeight(launchPos.x, launchPos.y, true));

    return launchPos;
}

//======================================================================================================================
AIControllerTug::AIControllerTug(const AIControllersSettings::AIControllerSetting& aiControllerSetting, const TugAeroplaneModifiers& modifiers, int AIControllerIndex)
    : mAIControllerSetting(aiControllerSetting), mAeroplaneModifiers(modifiers)
{
    mAeroplane = 0;
    mAIControllerIndex = AIControllerIndex;
    mGlider = 0;
}

//======================================================================================================================
bool AIControllerTug::Init(LoadingScreenHelper* loadingScreen)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();

    for (unsigned int i = 0 ; i != MAX_CHANNELS ; ++i)
        mOutputControls[i] = 0.0f;

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_CONTROL);

    AeroplaneSettings as;
    if (!as.LoadFromFile(mAIControllerSetting.mAeroplaneFile.c_str()))
    {
        const Language language = gs.mOptions.mLanguage;
        ShowDialog("PicaSim", "Failed to load AI controller", TXT(PS_OK));
        return false;
    }

    as.mEngineScale *= mAeroplaneModifiers.mEngineScale;
    as.mSizeScale *= mAeroplaneModifiers.mSizeScale;
    as.mMassScale *= mAeroplaneModifiers.mMassScale;

    mAeroplane = std::make_unique<Aeroplane>(*this);
    Vector3 launchPos = GetLaunchPos();
    mAeroplane->Init(as, &launchPos, loadingScreen);

    Reset();

    for(size_t i = 0 ; i != MAX_CHANNELS ; ++i)
    {
        mOutputControls[i] = 0.0f;
    }
    return true;
}

//======================================================================================================================
void AIControllerTug::Reset()
{
    mGlider = 0;
    Vector3 launchPos = GetLaunchPos();
    mAeroplane->Launch(launchPos);
    ChooseNewWaypoint(true);
}


//======================================================================================================================
void AIControllerTug::Relaunched()
{
    Vector3 launchPos = PicaSim::GetInstance().GetObserver().GetCameraTransform(0).GetTrans();
    launchPos += Vector3(0,0,2) * float(mAIControllerIndex+1);

    mAeroplane->Launch(launchPos);
    mGlider = 0;

    ChooseNewWaypoint(true);
}


//======================================================================================================================
void AIControllerTug::Terminate()
{
    TRACE_METHOD_ONLY(1);
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_CONTROL);
    PicaSim::GetInstance().RemoveCameraTarget(mAeroplane.get());

    if (mAeroplane)
    {
        mAeroplane->Terminate();
        mAeroplane.reset();
    }
}

//======================================================================================================================
void AIControllerTug::ChooseNewWaypoint(bool isLaunch)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIAeroplaneSettings& aias = mAeroplane->GetAeroplaneSettings().mAIAeroplaneSettings;
    const AIEnvironmentSettings& aies = gs.mEnvironmentSettings.mAIEnvironmentSettings;

    const Vector3& observerPos = PicaSim::GetInstance().GetObserver().GetCameraTransform(0).GetTrans();

    if (isLaunch)
    {
        mTargetWaypoint = observerPos;
        Vector3 delta = mAeroplane->GetTransform().RowX() * aias.mPoweredTakeoffDistance;
        delta.z = aias.mPoweredTakeoffDistance * 0.1f;
        mTargetWaypoint += delta;
        mWaypointTimer = aias.mPoweredMaxWaypointTime;
    }
    else
    {
        mWaypointTimer = aias.mPoweredMaxWaypointTime;

        float r1 = RangedRandom(-1.0f, 1.0f);
        float r2 = RangedRandom(-1.0f, 1.0f);

        mTargetWaypoint = observerPos;
        mTargetWaypoint.x += aias.mPoweredMaxDistance * r1;
        mTargetWaypoint.y += aias.mPoweredMaxDistance * r2;

        const float terrainHeight = Environment::GetInstance().GetTerrain().GetTerrainHeightFast(mTargetWaypoint.x, mTargetWaypoint.y, true);

        mTargetWaypoint.z = terrainHeight + RangedRandom(aias.mPoweredMinHeight, aias.mPoweredMaxHeight);
    }
}

//======================================================================================================================
void AIControllerTug::CheckForNewWaypoint(
    const AIEnvironmentSettings& aies,
    const AIAeroplaneSettings& aias,
    const Vector3& pos,
    Vector3& deltaToTarget, 
    Vector3& horDeltaToTarget)
{
    const float physicsRadius = mAeroplane->GetPhysics()->getAABBRadius();
    const float altitude = fabsf(pos.z -  Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true));

    float horDistToTarget = horDeltaToTarget.GetLength();
    if (
        horDistToTarget < aias.mWaypointTolerance ||
        mWaypointTimer < 0.0f)
    {
        ChooseNewWaypoint(false);
    }
}

//======================================================================================================================
void AIControllerTug::EntityUpdate(float deltaTime, int entityLevel)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIAeroplaneSettings& aias = mAeroplane->GetAeroplaneSettings().mAIAeroplaneSettings;
    const AIEnvironmentSettings& aies = gs.mEnvironmentSettings.mAIEnvironmentSettings;
    const AIControllersSettings& aics = gs.mAIControllersSettings;

    const Transform tm = mAeroplane->GetTransform();
    const Vector3 pos = mAeroplane->GetTransform().GetTrans();
    const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(pos, Environment::WIND_TYPE_GUSTY);
    const Vector3 vel = mAeroplane->GetVelocity();
    const Vector3 velRelWind = vel - windVel;
    const float fwdSpeedRelAir = velRelWind.Dot(tm.RowX());
    const float physicsRadius = mAeroplane->GetPhysics()->getAABBRadius();
    const float altitude = fabsf(pos.z -  Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true));

    // Update the camera target status if necessary
    PicaSim::GetInstance().AddRemoveCameraTarget(mAeroplane.get(), aics.mIncludeInCameraViews && mAIControllerSetting.mIncludeInCameraViews);

    mWaypointTimer -= deltaTime;

    Vector3 deltaToTarget = mTargetWaypoint - pos;
    Vector3 horDeltaToTarget(deltaToTarget.x, deltaToTarget.y, 0.0f);
    CheckForNewWaypoint(aies, aias, pos, deltaToTarget, horDeltaToTarget);
    const float horDistToTarget = horDeltaToTarget.GetLength();

    const float heading = atan2f(vel.x, vel.y);
    const float desiredHeading = atan2f(horDeltaToTarget.x, horDeltaToTarget.y);

    // Make sure we always turn into wind
    const float headingChange = WrapToRange(desiredHeading - heading, -PI, PI);

    float desiredRoll = ClampToRange(headingChange * aias.mControlBankAnglePerHeadingChange, 
        -DegreesToRadians(aias.mControlMaxBankAngle), DegreesToRadians(aias.mControlMaxBankAngle));
    //if (altitude < physicsRadius)
    //  desiredRoll *= altitude / physicsRadius;
    const float currentRoll = asinf(tm.RowY().z);
    float rollControl = ClampToRange(RadiansToDegrees(desiredRoll - currentRoll) * aias.mControlRollControlPerRollAngle, -1.0f, 1.0f);
    rollControl *= aias.mControlMaxRollControl;

    // Control pitch. Note that +ve is down
    float pitchControl = RadiansToDegrees(fabsf(currentRoll)) * -aias.mControlPitchControlPerRollAngle;

    // Also adjust depending on air speed.
    float currentSlope = velRelWind.z / fwdSpeedRelAir;

    float desiredAltitudeChange = mTargetWaypoint.z - pos.z;

    float targetSlope = desiredAltitudeChange / horDistToTarget;
    targetSlope = ClampToRange(targetSlope, -aias.mPoweredControlMaxSlopeDown, aias.mPoweredControlMaxSlopeUp);

    pitchControl -= (targetSlope - currentSlope) * aias.mPoweredControlPitchControlPerSlope;
    pitchControl = ClampToRange(pitchControl, -1.0f, 1.0f);
    pitchControl *= aias.mControlMaxPitchControl;

    float throttleControl = desiredAltitudeChange * aias.mPoweredControlThrottleControlPerAltitude;
    throttleControl = Minimum(throttleControl, 0.1f);
    throttleControl += (aias.mPoweredControlCruiseSpeed - fwdSpeedRelAir) * aias.mPoweredControlThrottleControlPerSpeed;
    throttleControl = ClampToRange(throttleControl, 0.0f, 1.0f);

    mOutputControls[Controller::CHANNEL_AILERONS] = ExponentialApproach(
        mOutputControls[Controller::CHANNEL_AILERONS], rollControl, deltaTime, aias.mControlRollTimeScale);
    mOutputControls[Controller::CHANNEL_ELEVATOR] = ExponentialApproach(
        mOutputControls[Controller::CHANNEL_ELEVATOR], pitchControl, deltaTime, aias.mControlPitchTimeScale);
    mOutputControls[Controller::CHANNEL_THROTTLE] = ExponentialApproach(
        mOutputControls[Controller::CHANNEL_THROTTLE], throttleControl, deltaTime, 2.0f);
    mOutputControls[Controller::CHANNEL_SMOKE1] = -1.0f; 
    mOutputControls[Controller::CHANNEL_SMOKE2] = -1.0f;
    mOutputControls[Controller::CHANNEL_HOOK]   = -1.0f;

    if (altitude > aias.mPoweredAltitudeForAux)
        mOutputControls[Controller::CHANNEL_AUX1] = 1.0f;
    else
        mOutputControls[Controller::CHANNEL_AUX1] = -1.0f;

    if (aics.mEnableDebugDraw)
    {
        RenderManager::GetInstance().GetDebugRenderer().DrawPoint(mTargetWaypoint, 3.0f, Vector3(1, 0, 0));
        RenderManager::GetInstance().GetDebugRenderer().DrawLine(pos, mTargetWaypoint, Vector3(1, 1, 1));
        float arrowLength = mAeroplane->GetGraphics()->GetRenderBoundingRadius();
        RenderManager::GetInstance().GetDebugRenderer().DrawArrow(pos, pos + horDeltaToTarget.GetNormalised() * arrowLength, Vector3(0, 0, 0));

        if (
            PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTarget() == mAeroplane.get() ||
            PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTransform() == mAeroplane.get()
            )
        {
            char txt[256];
            float startY = 0.25f;
            float deltaY = 0.05f;
            float x = 0.05f;
            int iY = 0;
            sprintf(txt, "altitude = %5.1fm", altitude);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "headingChange = %5.1f deg", RadiansToDegrees(headingChange));
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "desiredRoll = %5.1f deg", RadiansToDegrees(desiredRoll));
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "fwdSpeedRelAir = %5.2f", fwdSpeedRelAir);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "rollControl = %5.2f", rollControl);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "pitchControl = %5.2f", pitchControl);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "throttleControl = %5.2f", throttleControl);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "distance = %5.1fm", horDeltaToTarget.GetLength());
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
        }
    }
}

//======================================================================================================================
bool AIControllerTug::CanTow() const
{
    return mAeroplane->GetAeroplaneSettings().mAIAeroplaneSettings.mCanTow;
}

//======================================================================================================================
bool AIControllerTug::IsTowing() const
{
    return mGlider != 0;
}

