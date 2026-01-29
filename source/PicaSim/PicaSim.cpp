#include "PicaSim.h"

#include "HumanController.h"
#include "ConnectionListener.h"
#include "AeroplaneGraphics.h"
#include "AeroplanePhysics.h"
#include "AIControllerTug.h"
#include "Menus/SettingsMenu.h"
#include "Menus/Menu.h"
#include "Menus/LoadingScreen.h"
#include "Menus/HelpMenu.h"
#include "SimpleObject.h"
#include "WindsockOverlay.h"
#include "ChallengeFreeFly.h"
#include "ChallengeRace.h"
#include "ChallengeLimbo.h"
#include "ChallengeDuration.h"
#include "PicaJoystick.h"

#include "Menus/PicaDialog.h"

#include "../Platform/S3ECompat.h"
#include "Platform.h"

#ifdef PICASIM_VR_SUPPORT
#include "../Platform/VRManager.h"
#include "../Platform/VRRuntime.h"
#include <glad/glad.h>
#endif

float numButtonSlots = 8.0f;

PicaSim* PicaSim::mInstance = 0;

//======================================================================================================================
PicaSim::PicaSim(GameSettings& gameSettings)
    : mCurrentDeltaTime(0.0f), mGameSettings(gameSettings)
{
    mObserver = 0;
    mPauseOverlay = 0;
    mResumeOverlay = 0;
    mRelaunchOverlay = 0;
    mChangeViewOverlay = 0;
    mWalkaboutOverlay = 0;
    mSettingsMenuOverlay = 0;
    mHelpOverlay = 0;
    mStartMenuOverlay = 0;
    mControllerOverlay = 0;
    mControllerOverlayTextOpacity = 1.0f;
    mWindsockOverlay = 0;
    mSound = 0;
    mSoundChannel = -1;
    mChallenge = 0;
    mUpdateCounter = 0;
    mTimeSinceEnabled = 0.0f;
}

//======================================================================================================================
bool PicaSim::Init(GameSettings& gameSettings, LoadingScreenHelper* loadingScreen)
{
    const ChallengeSettings& cs = gameSettings.mChallengeSettings;
    TRACE_FILE_IF(1) TRACE("PicaSim::Init num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());

    mInstance = new PicaSim(gameSettings);

    // TODO make a single Framework::Init

    RenderManager::Init(gameSettings.mOptions.mFrameworkSettings, loadingScreen);

    // Now set up framework
    EntityManager::Init(gameSettings.mOptions.mFrameworkSettings);

    mInstance->mParticleEngine.Init();

    // Note that the challenge can override everything - so set it up first
    switch (gameSettings.mChallengeSettings.mChallengeMode)
    {
    case ChallengeSettings::CHALLENGE_FREEFLY:
        {
            ChallengeFreeFly* challengeFreeFly = new ChallengeFreeFly();
            mInstance->mChallenge = challengeFreeFly;
        }
        break;
    case ChallengeSettings::CHALLENGE_RACE:
        {
            mInstance->mChallenge = new ChallengeRace(gameSettings);
        }
        break;
    case ChallengeSettings::CHALLENGE_LIMBO:
        {
            mInstance->mChallenge = new ChallengeLimbo(gameSettings);
        }
        break;
    case ChallengeSettings::CHALLENGE_DURATION:
        {
            mInstance->mChallenge = new ChallengeDuration(gameSettings);
        }
        break;
    default:
        TRACE_FILE_IF(1) TRACE("Unable to load challenge type %d", gameSettings.mChallengeSettings.mChallengeMode);
        break;
    }

    if (!Environment::Init(loadingScreen))
    {
        Terminate();
        return false;
    }

    Transform tm;
    tm.SetIdentity();

    // Create some environment/dynamic boxes
    const int wallHeight = 0;
    const int wallLength = 0;
    RandomGenerator r(0);
    float range = 0.0f;
    float size = 1.0f;
    float objectZ = size * 0.5f;
    for (int iZ = 0 ; iZ != wallHeight ; ++iZ)
    {
        for (int iX = 0 ; iX != wallLength ; ++iX)
        {
            tm.t = mInstance->mGameSettings.mEnvironmentSettings.mObserverPosition + 
                Vector3(iX * size * 3.0f * powf(float(iX), 0.1f), 0.0f, 0.0f) + 
                Vector3(r.GetValue(-range, range), r.GetValue(-range, range), 0);
            Environment::GetInstance().GetTerrain().GetTerrainHeight(tm.t, true);
            tm.t.z += iZ * size + objectZ;
            BoxObject* box = new BoxObject(
                Vector3(1, 1, 1), tm, Vector3(1,1,1) * 0.5f, 
                "SystemData/Textures/CloudTexture.jpg", 1.0f, true, true, true, true);
            mInstance->mBoxObjects.push_back(box);
        }
    }

    // Create an aeroplane
    mInstance->mPlayerController = new HumanController(mInstance->mGameSettings);
    mInstance->mPlayerAeroplane = new Aeroplane(*mInstance->mPlayerController);
    mInstance->mPlayerController->SetAeroplane(mInstance->mPlayerAeroplane);
    mInstance->mCameraAeroplaneIndex = 0;
    mInstance->mCameraAeroplanes.push_back(mInstance->mPlayerAeroplane);

    // Create an observer
    mInstance->mObserver = new Observer;
//  mInstance->mObserver->Init(Environment::GetInstance().GetObservationPoint(), aeroplane);
    mInstance->mObserver->Init(mInstance->mGameSettings.mEnvironmentSettings.mObserverPosition, mInstance->mPlayerAeroplane);

    // Move/initialise the aeroplane so it's next to the observer
    Vector3 observerPosition = mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans();;
    mInstance->mPlayerAeroplane->Init(
        mInstance->mGameSettings.mAeroplaneSettings,
        &observerPosition,
        loadingScreen);

    // Create the camera & viewport
    // Configure the first camera to fly with the aeroplane
    Camera* camera = RenderManager::GetInstance().CreateCamera();
    mInstance->mViewport = RenderManager::GetInstance().CreateViewport(0.0f, 0.0f, 1.0f, 1.0f, camera);

    Camera* zoomCamera = RenderManager::GetInstance().CreateCamera();
    zoomCamera->SetUserData((void*) CAMERA_ZOOM);
    float zoomSize = mInstance->mGameSettings.mOptions.mZoomViewSize;
    mInstance->mZoomViewport = RenderManager::GetInstance().CreateViewport(1.0f - zoomSize, 1.0f - zoomSize, 1.0f, 1.0f, zoomCamera);
    mInstance->mZoomViewport->Enable(false);

    mInstance->mStatus = STATUS_PAUSED;
    mInstance->mMode = MODE_GROUND;
    mInstance->HandleMode();

    // Button overlays
    mInstance->mShowUI = true;
    float buttonSize = mInstance->mGameSettings.mOptions.mPauseButtonsSize / numButtonSlots;
    float buttonOffset = 1.0f / numButtonSlots;
    float paddingFraction = 0.25f;

    // Overlays for when running / paused
    GLubyte alpha = 255;
    mInstance->mPauseOverlay = new ButtonOverlay("SystemData/Menu/PlayerPause.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, 1.0f, 1.0f,
        mInstance->mGameSettings.mOptions.mPauseButtonOpacity, false, false);
    mInstance->mControllerOverlay = new ButtonOverlay("SystemData/Menu/Joystick.png", buttonSize, 1.0f,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*7, 1.0f, 0, false, true);
    mInstance->mStartMenuOverlay = new ButtonOverlay("SystemData/Menu/Stop.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, 0.0f, 1.0f, alpha, false, false);
    mInstance->mHelpOverlay = new ButtonOverlay("SystemData/Menu/Help.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*1, 1.0f, alpha, false, false);
    mInstance->mWalkaboutOverlay = new ButtonOverlay("SystemData/Menu/AgtMember.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*3, 1.0f, alpha, false, false);
    mInstance->mChangeViewOverlay = new ButtonOverlay("SystemData/Menu/Find.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*4, 1.0f, alpha, false, false);
    mInstance->mSettingsMenuOverlay = new ButtonOverlay("SystemData/Menu/Utilities.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*5, 1.0f, alpha, false, false);
    mInstance->mRelaunchOverlay = new ButtonOverlay("SystemData/Menu/PlayerRew.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*7, 1.0f, alpha, false, false);
    mInstance->mResumeOverlay = new ButtonOverlay("SystemData/Menu/PlayerPlay.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*8, 1.0f, alpha, false, false);

    mInstance->mWindsockOverlay = new WindsockOverlay("SystemData/Menu/Windsock.png",
        mInstance->mGameSettings.mOptions.mWindArrowSize, 0.5f,
        mInstance->mGameSettings.mOptions.mWindArrowSize, 0, 0.0f);

    if (loadingScreen) loadingScreen->Update("Wind sound");
    mInstance->mSound = AudioManager::GetInstance().LoadSound("SystemData/Audio/WindLoop22050Mono.raw", 22050, false, true, true);
    mInstance->mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(1.0f, true);
    if (mInstance->mSoundChannel != -1)
        AudioManager::GetInstance().StartSoundOnChannel(mInstance->mSoundChannel, mInstance->mSound, true);
    else
        TRACE_FILE_IF(1) TRACE("Unable to allocate sound channel for wind");

    if (loadingScreen) loadingScreen->Update("Wind sound");

    mInstance->mChallenge->Init(mInstance->mPlayerAeroplane, loadingScreen);

    mInstance->mShouldExit = false;

    mInstance->mPrevJoystickRelaunch = false;
    mInstance->mPrevJoystickCamera = false;
    mInstance->mPrevJoystickPausePlay = false;
    mInstance->mPrevJoystickRatesCycle = false;
    mInstance->mPrevJoystickButton0Cycle = false;
    mInstance->mPrevJoystickButton1Cycle = false;
    mInstance->mPrevJoystickButton2Cycle = false;

    mInstance->mActualTimeScale = gameSettings.mOptions.mTimeScale;

  mInstance->mShowHelpAfterLoading = 
      gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY &&
      (gameSettings.mOptions.mFreeFlyMode == Options::FREEFLYMODE_TRAINERGLIDER ||
        gameSettings.mOptions.mFreeFlyMode == Options::FREEFLYMODE_TRAINERPOWERED);

  if (gameSettings.mOptions.mEnableSocketController)
  {
    mInstance->mConnectionListener = new ConnectionListener();
    mInstance->mConnectionListener->Init();
  }
  else
  {
      mInstance->mConnectionListener = 0;
  }
    return true;
}

//======================================================================================================================
void PicaSim::Terminate()
{
    TRACE_FUNCTION_ONLY(1);
    Environment::Terminate();

    if (mInstance)
    {
        if (mInstance->mConnectionListener)
        {
            mInstance->mConnectionListener->Terminate();
            delete mInstance->mConnectionListener;
            mInstance->mConnectionListener = 0;
        }

        while (!mInstance->mBoxObjects.empty())
        {
            BoxObject* object = mInstance->mBoxObjects.back();
            delete object;
            mInstance->mBoxObjects.pop_back();
        }

        if (mInstance->mPlayerAeroplane)
        {
            mInstance->mPlayerAeroplane->Terminate();
            delete mInstance->mPlayerAeroplane;
            mInstance->mPlayerAeroplane = 0;
        }
        if (mInstance->mPlayerController)
        {
            delete mInstance->mPlayerController;
            mInstance->mPlayerController = 0;
        }
        if (mInstance->mObserver)
        {
            mInstance->mObserver->Terminate();
            delete mInstance->mObserver;
            mInstance->mObserver = 0;
        }

        mInstance->mChallenge->Terminate();
        delete mInstance->mChallenge;
        mInstance->mChallenge = 0;

        delete mInstance->mPauseOverlay;
        mInstance->mPauseOverlay = 0;
        delete mInstance->mResumeOverlay;
        mInstance->mResumeOverlay = 0;
        delete mInstance->mRelaunchOverlay;
        mInstance->mRelaunchOverlay = 0;
        delete mInstance->mChangeViewOverlay;
        mInstance->mChangeViewOverlay = 0;
        delete mInstance->mWalkaboutOverlay;
        mInstance->mWalkaboutOverlay = 0;
        delete mInstance->mSettingsMenuOverlay;
        mInstance->mSettingsMenuOverlay = 0;
        delete mInstance->mStartMenuOverlay;
        mInstance->mStartMenuOverlay = 0;
        delete mInstance->mHelpOverlay;
        mInstance->mHelpOverlay = 0;
        delete mInstance->mControllerOverlay;
        mInstance->mControllerOverlay = 0;

        delete mInstance->mWindsockOverlay;
        mInstance->mWindsockOverlay = 0;

        mInstance->mParticleEngine.Terminate();
    }

    EntityManager::Terminate();

    RenderManager::Terminate();

    if (mInstance->mSoundChannel != -1)
        AudioManager::GetInstance().ReleaseSoundChannel(mInstance->mSoundChannel);
    if (mInstance->mSound)
        AudioManager::GetInstance().UnloadSound(mInstance->mSound);

    delete mInstance;
    mInstance = 0;
}

//======================================================================================================================
void PicaSim::HandleMode()
{
    Mode origMode = mMode;

    if (mGameSettings.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
        mMode = MODE_GROUND;

    Aeroplane* cameraAeroplane = mCameraAeroplanes[mCameraAeroplaneIndex];

    switch (mMode)
    {
    case MODE_AEROPLANE:
        mViewport->GetCamera()->SetUserData((void*) CAMERA_AEROPLANE);
        mViewport->GetCamera()->SetCameraTransform(cameraAeroplane);
        mViewport->GetCamera()->SetCameraTarget(0);
        mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mAeroplaneViewFieldOfView));
        mViewport->GetCamera()->DisableAutoZoom();

        mZoomViewport->Enable(false);
        break;
    case MODE_CHASE:
        mViewport->GetCamera()->SetUserData((void*) CAMERA_CHASE);
        mViewport->GetCamera()->SetCameraTransform(cameraAeroplane);
        mViewport->GetCamera()->SetCameraTarget(cameraAeroplane);
        mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mAeroplaneViewFieldOfView));
        mViewport->GetCamera()->DisableAutoZoom();

        mZoomViewport->Enable(false);
        break;
    case MODE_GROUND:
        {
            mViewport->GetCamera()->SetUserData((void*) CAMERA_GROUND);
            mViewport->GetCamera()->SetCameraTransform(mObserver);
            if (mGameSettings.mOptions.mGroundViewFollow)
                mViewport->GetCamera()->SetCameraTarget(cameraAeroplane);
            else
                mViewport->GetCamera()->SetCameraTarget(0);
            mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mGroundViewFieldOfView));
            mViewport->GetCamera()->SetAutoZoom(mGameSettings.mOptions.mGroundViewAutoZoom, 
                cameraAeroplane->GetGraphics()->GetRenderBoundingRadius() / mGameSettings.mOptions.mGroundViewAutoZoomScale);

            mZoomViewport->Enable(mGameSettings.mOptions.mEnableZoomView);
            if (mGameSettings.mOptions.mEnableZoomView)
            {
                mZoomViewport->SetDrawFrame(mGameSettings.mOptions.mOnlyPlaneInZoomView);
                float zoomSize = mInstance->mGameSettings.mOptions.mZoomViewSize;
                if (mGameSettings.mOptions.mControllerStaggered)
                    mZoomViewport->Resize(1.0f - zoomSize, 1.0f - zoomSize, zoomSize, zoomSize);
                else
                    mZoomViewport->Resize(0.0f, 1.0f - zoomSize, zoomSize, zoomSize);
                // Configure the zoom camera to look at the aeroplane from the observer
                mZoomViewport->GetCamera()->SetCameraTransform(mInstance->mObserver);
                mZoomViewport->GetCamera()->SetCameraTarget(cameraAeroplane);
                mZoomViewport->GetCamera()->SetAutoZoom(1.0f, 2.0f * cameraAeroplane->GetGraphics()->GetRenderBoundingRadius());
                if (mGameSettings.mOptions.mOnlyPlaneInZoomView)
                {
                    mZoomViewport->AddRenderObject(cameraAeroplane->GetGraphics());
                    if (
                        cameraAeroplane->GetTugController() &&
                        cameraAeroplane->GetTugController()->GetAeroplane()
                        )
                    {
                        const Aeroplane* tugAeroplane = cameraAeroplane->GetTugController()->GetAeroplane();
                        mZoomViewport->AddRenderObject(tugAeroplane->GetGraphics());

                        cameraAeroplane->GetPhysics()->AddRopesToViewport(mZoomViewport);
                    }
                }
                else
                {
                    mZoomViewport->ClearRenderObjects();
                }
            }
        }
        break;
    case MODE_WALK:
        if (!mGameSettings.mOptions.mEnableWalkabout || mStatus == STATUS_FLYING)
        {
            mMode = MODE_GROUND;
        }
        else
        {
            mViewport->GetCamera()->SetUserData((void*) CAMERA_GROUND);
            mViewport->GetCamera()->SetCameraTransform(mObserver);
            mViewport->GetCamera()->SetCameraTarget(0);
            mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mGroundViewFieldOfView));
            mViewport->GetCamera()->DisableAutoZoom();
        }
        break;
    }

    if (origMode == MODE_WALK && mGameSettings.mOptions.mSetWindDirectionOnWalkabout && mMode != MODE_WALK)
    {
        Environment::GetInstance().InitZeroFlowHeightfield();
    }
}

//======================================================================================================================
void PicaSim::AddCameraTarget(Aeroplane* cameraAeroplane)
{
    mCameraAeroplanes.push_back(cameraAeroplane);
}

//======================================================================================================================
void PicaSim::RemoveCameraTarget(Aeroplane* cameraAeroplane)
{
    for (size_t i = 0 ; i != mCameraAeroplanes.size() ; ++i)
    {
        if (mCameraAeroplanes[i] == cameraAeroplane)
        {
            mCameraAeroplanes.erase(mCameraAeroplanes.begin() + i);
            if (mCameraAeroplaneIndex >= i && mCameraAeroplaneIndex > 0)
                --mCameraAeroplaneIndex;
            return;
        }
    }
}

//======================================================================================================================
void PicaSim::AddRemoveCameraTarget(Aeroplane* cameraAeroplane, bool add)
{
    for (size_t i = 0 ; i != mCameraAeroplanes.size() ; ++i)
    {
        if (mCameraAeroplanes[i] == cameraAeroplane)
        {
            if (!add)
            {
                mCameraAeroplanes.erase(mCameraAeroplanes.begin() + i);
                if (mCameraAeroplaneIndex >= i && mCameraAeroplaneIndex > 0)
                    --mCameraAeroplaneIndex;
            }
            return;
        }
    }
    if (add)
        AddCameraTarget(cameraAeroplane);
}

//======================================================================================================================
void PicaSim::AddAeroplane(Aeroplane* aeroplane)
{
    Aeroplanes::iterator it = std::find(mAeroplanes.begin(), mAeroplanes.end(), aeroplane);
    if (it == mAeroplanes.end())
        mAeroplanes.push_back(aeroplane);
}

//======================================================================================================================
void PicaSim::RemoveAeroplane(Aeroplane* aeroplane)
{
    Aeroplanes::iterator it = std::find(mAeroplanes.begin(), mAeroplanes.end(), aeroplane);
    if (it != mAeroplanes.end())
        mAeroplanes.erase(it);
}

//======================================================================================================================
void PicaSim::ReinitOverlays()
{
    mTimeSinceEnabled = 0.0f;
    mStartMenuOverlay->ForceDisable();
}

//======================================================================================================================
void PicaSim::HandleJoystickToggle(const JoystickSettings::JoystickButtonOverride& j, float buttonDown,
                                   bool& joystickRelaunch, bool& joystickChangeView, bool& joystickPausePlay)
{
    bool buttonDownBool = buttonDown != 0.0f;
    if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RATES)
    {
        int newAltSetting = (int) (buttonDown * mGameSettings.mControllerSettings.mNumAltSettings);
        newAltSetting = ClampToRange(newAltSetting, 0, mGameSettings.mControllerSettings.mNumAltSettings);
        if (newAltSetting != mGameSettings.mControllerSettings.mCurrentAltSetting)
            mControllerOverlayTextOpacity = 1.0f;
        mGameSettings.mControllerSettings.mCurrentAltSetting = newAltSetting;
        if (mGameSettings.mControllerSettings.mCurrentAltSetting >= mGameSettings.mControllerSettings.mNumAltSettings)
            mGameSettings.mControllerSettings.mCurrentAltSetting = mGameSettings.mControllerSettings.mNumAltSettings-1;
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RATESCYCLE)
    {
        if (mPrevJoystickRatesCycle != buttonDownBool)
        {
            if (buttonDownBool)
            {
                mGameSettings.mControllerSettings.mCurrentAltSetting = 
                (mGameSettings.mControllerSettings.mCurrentAltSetting + 1) % mGameSettings.mControllerSettings.mNumAltSettings;
                mControllerOverlayTextOpacity = 1.0f;
            }
            mPrevJoystickRatesCycle = buttonDownBool;
        }
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_CAMERA)
    {
        if (mPrevJoystickCamera != buttonDownBool)
        {
            if (buttonDownBool)
                joystickChangeView = true;
            mPrevJoystickCamera = buttonDownBool;
        }
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RELAUNCH)
    {
        if (mPrevJoystickRelaunch != buttonDownBool)
        {
            if (buttonDownBool)
                joystickRelaunch = true;
            mPrevJoystickRelaunch = buttonDownBool;
        }
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_PAUSEPLAY)
    {
        if (mPrevJoystickPausePlay != buttonDownBool)
        {
            if (buttonDown)
                joystickPausePlay = true;
            mPrevJoystickPausePlay = buttonDownBool;
        }
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON0)
    {
        GetPlayerAeroplane()->GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON0, buttonDown ? 1.0f : -1.0f);
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON1)
    {
        GetPlayerAeroplane()->GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON1, buttonDown ? 1.0f : -1.0f);
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON2)
    {
        GetPlayerAeroplane()->GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON2, buttonDown ? 1.0f : -1.0f);
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON0TOGGLE)
    {
        if (mPrevJoystickButton0Cycle != buttonDownBool)
        {
            if (buttonDownBool)
            {
                float currentInput = GetPlayerAeroplane()->GetController().GetInputControl(ControllerSettings::CONTROLLER_BUTTON0);
                GetPlayerAeroplane()->GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON0, 
                    currentInput > 0.0f ? -1.0f : 1.0f);
            }
            mPrevJoystickButton0Cycle = buttonDownBool;
        }
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON1TOGGLE)
    {
        if (mPrevJoystickButton1Cycle != buttonDownBool)
        {
            if (buttonDownBool)
            {
                float currentInput = GetPlayerAeroplane()->GetController().GetInputControl(ControllerSettings::CONTROLLER_BUTTON1);
                GetPlayerAeroplane()->GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON1, 
                    currentInput > 0.0f ? -1.0f : 1.0f);
            }
            mPrevJoystickButton1Cycle = buttonDownBool;
        }
    }
    else if (j.mControl == JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON2TOGGLE)
    {
        if (mPrevJoystickButton2Cycle != buttonDownBool)
        {
            if (buttonDownBool)
            {
                float currentInput = GetPlayerAeroplane()->GetController().GetInputControl(ControllerSettings::CONTROLLER_BUTTON2);
                GetPlayerAeroplane()->GetController().SetInputControl(ControllerSettings::CONTROLLER_BUTTON2, 
                    currentInput > 0.0f ? -1.0f : 1.0f);
            }
            mPrevJoystickButton2Cycle = buttonDownBool;
        }
    }
}

//======================================================================================================================
void PicaSim::UpdateJoystickToggles(bool& joystickRelaunch, bool& joystickChangeView, bool& joystickPausePlay)
{
    if (mGameSettings.mJoystickSettings.mEnableJoystick && !mGameSettings.mOptions.mFrameworkSettings.isIOS())
    {
        JoystickData joystick;
        if (S3E_RESULT_SUCCESS == GetJoystickStatus(joystick, mGameSettings.mOptions.mJoystickID))
        {
            for (size_t i = 0 ; i != JoystickSettings::JOYSTICK_NUM_BUTTONS ; ++i)
            {
                const JoystickSettings::JoystickButtonOverride& j = mGameSettings.mJoystickSettings.mJoystickButtonOverrides[i];
                float buttonDown = joystick.mButtons[i] > 64 ? 1.0f : 0.0f;
                buttonDown = j.mInvert ? !buttonDown : buttonDown;
                HandleJoystickToggle(j, buttonDown, joystickRelaunch, joystickChangeView, joystickPausePlay);
            }

            for (size_t i = 0 ; i != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++i)
            {
                const JoystickSettings::JoystickButtonOverride& j = mGameSettings.mJoystickSettings.mJoystickAButtonOverrides[i];
                float input = -1.0f + 2.0f * joystick.mAnalogueInputs[i] / 65535.0f;
                input = j.mInvert ? 1.0f - input : input;
                float buttonDown = ClampToRange(0.5f * (input + 1.0f), 0.0f, 1.0f);
                HandleJoystickToggle(j, buttonDown, joystickRelaunch, joystickChangeView, joystickPausePlay);
            }
        }
    }
}


//======================================================================================================================
int PicaSim::ShowInGameDialog(float width, float height, const char* title, const char* text, const char* button0, const char* button1, const char* button2)
{
    return ::ShowInGameDialog(width, height, title, text, button0, button1, button2);
}

//======================================================================================================================
void PicaSim::ShowHelpOverlays()
{
    ::ShowHelpOverlays(mGameSettings);
}

//======================================================================================================================
PicaSim::UpdateResult PicaSim::Update(int64 deltaTimeMs)
{
    // prevent backlight turning off, whatever the device is set to
    s3eDeviceBacklightOn();
    s3eKeyboardUpdate();
    s3ePointerUpdate();
    UpdateJoystick(mGameSettings.mOptions.mJoystickID);

    float deltaTime = deltaTimeMs * 0.001f;

    // Store the unscaled as this gets used for updating non-simulation things
    mCurrentDeltaTime = deltaTime;

    deltaTime *= mActualTimeScale;

    // Store the average FPS
    if (deltaTime > 0.0f)
    {
        float thisFPS = 1.0f / deltaTime;
        float newFrac = 0.2f;
        mGameSettings.mStatistics.mSmoothedFPS = mGameSettings.mStatistics.mSmoothedFPS * (1.0f - newFrac) + thisFPS * newFrac;
        mGameSettings.mStatistics.mFPS = thisFPS;
    }


    if (deltaTime > 0.1f)
        deltaTime = 0.1f;

    float gameDeltaTime = mStatus == STATUS_PAUSED ? 0.0f : deltaTime;

    // Update any toggles when using the joystick
    bool joystickRelaunch = false;
    bool joystickChangeView = false;
    bool joystickPausePlay = false;
    UpdateJoystickToggles(joystickRelaunch, joystickChangeView, joystickPausePlay);

    // Help
    if (mHelpOverlay->IsPressed())
    {
        TRACE_FILE_IF(1) TRACE("Detected help button press");
#if 0
        AudioManager::GetInstance().SetAllChannelsToZeroVolume();
        DisplayHelpMenu(mGameSettings, true);
        TRACE_FILE_IF(1) TRACE("Return from help");
        mTimeSinceEnabled = 0.0f;
        mChallenge->ReinitOverlays();
        mStartMenuOverlay->ForceDisable();
#else
        ShowHelpOverlays();
        //ShowDialog("Rats", "Blah", "OK");
#endif
    }

    // Check for menu button
    if (mSettingsMenuOverlay->IsPressed())
    {
        TRACE_FILE_IF(1) TRACE("Detected settings button press");

        AudioManager::GetInstance().SetAllChannelsToZeroVolume();
        SettingsChangeActions actions;
        DisplaySettingsMenu(mGameSettings, actions);

        TRACE_FILE_IF(1) TRACE("Return from settings");

        LoadingScreen* loadingScreen = 0;

        if (actions.mReloadTerrain)
        {
            TRACE_FILE_IF(1) TRACE("Reloading terrain");
            if (!loadingScreen)
                loadingScreen = new LoadingScreen("Loading", mGameSettings, true, true, true);
            Environment::Terminate();
            Environment::Init(loadingScreen);
            actions.mRecalcWind = false;

            Transform tm = mObserver->GetTransform();
            tm.SetTrans(mGameSettings.mEnvironmentSettings.mObserverPosition);
            mObserver->SetTransform(tm);

            mPlayerAeroplane->Terminate();
            mPlayerAeroplane->Init(
                mGameSettings.mAeroplaneSettings,
                &mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans(), loadingScreen);
            // Reset any challenge
            mChallenge->Terminate();
            mChallenge->Init(mPlayerAeroplane, loadingScreen);
            actions.mReloadAeroplane = false;
        }

        if (actions.mReloadAeroplane)
        {
            TRACE_FILE_IF(1) TRACE("Reloading aeroplane");
            if (!loadingScreen)
                loadingScreen = new LoadingScreen("Loading", mGameSettings, true, true, true);
            mPlayerAeroplane->Terminate();
            mPlayerAeroplane->Init(
                mGameSettings.mAeroplaneSettings,
                0, 
                loadingScreen);
            // Reset any challenge and relaunch if it's a race
            if (mGameSettings.mChallengeSettings.mChallengeMode != ChallengeSettings::CHALLENGE_FREEFLY)
                mPlayerAeroplane->Launch(mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans());
            mChallenge->Relaunched();
        }

        if (actions.mRecalcWind)
        {
            TRACE_FILE_IF(1) TRACE("Recalculating wind");
          if (!loadingScreen)
                loadingScreen = new LoadingScreen("Loading", mGameSettings, false, true, false);
            Environment::GetInstance().InitZeroFlowHeightfield(loadingScreen);
        }

        if (actions.mRestartChallenge)
        {
            if (mGameSettings.mChallengeSettings.mChallengeMode != ChallengeSettings::CHALLENGE_FREEFLY)
                mPlayerAeroplane->Launch(mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans());
            mChallenge->Terminate();
            mChallenge->Init(mPlayerAeroplane, loadingScreen);
        }

        if (actions.mReloadAI)
        {
            if (!mGameSettings.mAIControllersSettings.mAIControllers.empty() && !loadingScreen)
                loadingScreen = new LoadingScreen("Loading AI", mGameSettings, false, true, mGameSettings.mAIControllersSettings.mAIControllers.size() > 1);
            mChallenge->Terminate();
            mChallenge->Init(mPlayerAeroplane, loadingScreen);
            actions.mResetAI = false;
        }

        if (actions.mResetAI)
        {
            mChallenge->Reset();
        }
        delete loadingScreen;
        Environment::GetInstance().GetTerrain().ClearLOD();

        mChallenge->Validate();

        // Update overlays in case they changed
        mPauseOverlay->SetAlpha(mGameSettings.mOptions.mPauseButtonOpacity);

        float numButtonSlots = 8.0f;
        float buttonSize = mGameSettings.mOptions.mPauseButtonsSize / numButtonSlots;
        mPauseOverlay->SetSize(buttonSize);
        mStartMenuOverlay->SetSize(buttonSize);
        mHelpOverlay->SetSize(buttonSize);
        mWalkaboutOverlay->SetSize(buttonSize);
        mChangeViewOverlay->SetSize(buttonSize);
        mSettingsMenuOverlay->SetSize(buttonSize);
        mRelaunchOverlay->SetSize(buttonSize);
        mResumeOverlay->SetSize(buttonSize);
        mControllerOverlay->SetSize(buttonSize);

        HandleMode();

        // Force settings save
        mUpdateCounter = 0;
        mTimeSinceEnabled = 0.0f;
        mChallenge->ReinitOverlays();
        mStartMenuOverlay->ForceDisable();

        // Update the aeroplane settings just in case they got changed
        mPlayerAeroplane->SetAeroplaneSettings(mGameSettings.mAeroplaneSettings);
    }

    // L to reload the plane
    if ((s3eKeyboardGetState(s3eKeyL) & S3E_KEY_STATE_PRESSED))
    {
        TRACE_FILE_IF(1) TRACE("Reloading aeroplane - manual prompt");
        // This is needed to prevent a white screen with textured models
        LoadingScreen loadingScreen("Loading", mGameSettings, true, true, true);
        mPlayerAeroplane->Terminate();
        mPlayerAeroplane->Init(
            mGameSettings.mAeroplaneSettings,
            0, 
            &loadingScreen);
        // Reset any challenge and relaunch if it's a race
        if (mGameSettings.mChallengeSettings.mChallengeMode != ChallengeSettings::CHALLENGE_FREEFLY)
            mPlayerAeroplane->Launch(mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans());
        mChallenge->Relaunched();
    }

    if (
        mStartMenuOverlay->IsPressed() ||
        ((s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) && mGameSettings.mOptions.mUseBackButtonToExit)||
        CheckForQuitRequest()
        )
    {
        TRACE_FILE_IF(1) TRACE("Returning to start screen");
        TRACE_FILE_IF(1) TRACE("Saving settings");
        mGameSettings.SaveToFile((Platform::GetUserSettingsPath() + "settings.xml").c_str());
        // Clear pressed states to prevent click bleed-through to start menu
        s3eClearPressedStates();
        return UPDATE_START;
    }

    bool resetKeyPressed = (s3eKeyboardGetState(s3eKeyR) & S3E_KEY_STATE_PRESSED) != 0;
    bool cameraKeyPressed = (s3eKeyboardGetState(s3eKeyC) & S3E_KEY_STATE_PRESSED) != 0;
    bool walkaboutKeyPressed = (s3eKeyboardGetState(s3eKeyM) & S3E_KEY_STATE_PRESSED) != 0;
    bool pauseKeyPressed = (s3eKeyboardGetState(s3eKeyP) & S3E_KEY_STATE_PRESSED) != 0;
    bool slowmoKeyPressed = (s3eKeyboardGetState(s3eKeyT) & S3E_KEY_STATE_PRESSED) != 0;
    bool uiKeyPressed = (s3eKeyboardGetState(s3eKeyU) & S3E_KEY_STATE_PRESSED) != 0;
    bool zoomViewKeyPressed = (s3eKeyboardGetState(s3eKeyZ) & S3E_KEY_STATE_PRESSED) != 0;

    bool zoomInKeyPressed = (s3eKeyboardGetState(s3eKeyEquals) & S3E_KEY_STATE_DOWN) != 0;
    bool zoomOutKeyPressed = (s3eKeyboardGetState(s3eKeyMinus) & S3E_KEY_STATE_DOWN) != 0;

    // Zoom
    if (mMode == MODE_GROUND)
    {
        float zoomRate = 0.3f;
        if (zoomInKeyPressed)
            mGameSettings.mOptions.mGroundViewAutoZoom += zoomRate * mCurrentDeltaTime;
        else if (zoomOutKeyPressed)
            mGameSettings.mOptions.mGroundViewAutoZoom -= zoomRate * mCurrentDeltaTime;
        mGameSettings.mOptions.mGroundViewAutoZoom = ClampToRange(mGameSettings.mOptions.mGroundViewAutoZoom, 0.0f, 1.0f);
    }
    else
    {
        float zoomRate = 40.0f;
        if (zoomInKeyPressed)
            mGameSettings.mOptions.mAeroplaneViewFieldOfView -= zoomRate * mCurrentDeltaTime;
        else if (zoomOutKeyPressed)
            mGameSettings.mOptions.mAeroplaneViewFieldOfView += zoomRate * mCurrentDeltaTime;
        mGameSettings.mOptions.mAeroplaneViewFieldOfView = ClampToRange(mGameSettings.mOptions.mAeroplaneViewFieldOfView, 10.0f, 120.0f);
    }

    // Rates button
    if (
        mControllerOverlay->IsPressed() ||
        (s3eKeyboardGetState(s3eKeyRightShift) & S3E_KEY_STATE_PRESSED)
        )
    {
        mGameSettings.mControllerSettings.mCurrentAltSetting = (mGameSettings.mControllerSettings.mCurrentAltSetting + 1) % mGameSettings.mControllerSettings.mNumAltSettings;
        mControllerOverlayTextOpacity = 1.0f;
    }

    // Toggle slow motion
    if (mGameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
    {
        if (slowmoKeyPressed)
        {
            if (mGameSettings.mOptions.mTimeScale > 0.99f)
                mGameSettings.mOptions.mTimeScale = 0.2f;
            else
                mGameSettings.mOptions.mTimeScale = 1.0f;
        }
        mActualTimeScale = ExponentialApproach(
            mActualTimeScale, mGameSettings.mOptions.mTimeScale,
            mCurrentDeltaTime, 0.5f);
    }
    else
    {
        mActualTimeScale = 1.0f;
    }
    // UI
    if (uiKeyPressed)
        mShowUI = !mShowUI;

    // Zoom view
    if (zoomViewKeyPressed)
        mGameSettings.mOptions.mEnableZoomView = !mGameSettings.mOptions.mEnableZoomView;

    // relaunch
    if (
        mRelaunchOverlay->IsPressed() ||
        ((s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) && !mGameSettings.mOptions.mUseBackButtonToExit)||
        resetKeyPressed ||
        joystickRelaunch
        )
    {
        // Move/initialise the aeroplane so it's next to the observer
        mPlayerAeroplane->Launch(mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans());
        mChallenge->Relaunched();
    }

    bool panorama = mGameSettings.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA;
    // Change view
    if (
        mChangeViewOverlay->IsPressed() ||
        (s3eKeyboardGetState(s3eKeySearch) & S3E_KEY_STATE_PRESSED) ||
        cameraKeyPressed ||
        joystickChangeView
        )
    {
        if (panorama)
            mMode = MODE_GROUND;
        else
            mMode = (Mode) ((mMode + 1) % (MODE_MAX-1));
        if (mMode == MODE_GROUND)
        {
            mCameraAeroplaneIndex = (mCameraAeroplaneIndex + 1) % mCameraAeroplanes.size();
        }
    }

    if (mWalkaboutOverlay->IsPressed() || walkaboutKeyPressed)
    {
        mMode = MODE_WALK;
    }

    HandleMode();

    // Debugging
    if (s3eKeyboardGetState(s3eKeyK) & S3E_KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mRenderPreference = (Options::RenderPreference) ( (mGameSettings.mOptions.mRenderPreference+1) % Options::RENDER_PREFER_MAX );
    }
    if (s3eKeyboardGetState(s3eKeyG) & S3E_KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mDrawAeroplaneCoM = (Options::DrawCoMType) ((mGameSettings.mOptions.mDrawAeroplaneCoM + 1) % Options::COM_MAX);
    }
    if (s3eKeyboardGetState(s3eKeyW) & S3E_KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mTerrainWireframe = !mGameSettings.mOptions.mTerrainWireframe;
    }
    if (s3eKeyboardGetState(s3eKey9) & S3E_KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mAerofoilInfo = Maximum(mGameSettings.mOptions.mAerofoilInfo-1, -1);
    }
    if (s3eKeyboardGetState(s3eKey0) & S3E_KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mAerofoilInfo = Minimum(mGameSettings.mOptions.mAerofoilInfo+1, 64);
    }
    if (s3eKeyboardGetState(s3eKey7) & S3E_KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mNumWindStreamers = Maximum(mGameSettings.mOptions.mNumWindStreamers-1, 0);
    }
    if (s3eKeyboardGetState(s3eKey8) & S3E_KEY_STATE_PRESSED)
    {
        mGameSettings.mOptions.mNumWindStreamers = Minimum(mGameSettings.mOptions.mNumWindStreamers+1, 64);
    }

#ifdef PICASIM_VR_SUPPORT
    // VR view calibration keys
    if (VRManager::IsAvailable() && VRManager::GetInstance().IsVREnabled())
    {
        VRManager::GetInstance().UseAutoYawOffset(mMode == MODE_GROUND);

        // Keep VRManager updated with current wind direction for auto-reset on session start
        Vector3 upWindDir = -Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH);
        float facingYaw = atan2f(upWindDir.y, upWindDir.x);
        VRManager::GetInstance().SetDefaultFacingYaw(facingYaw);

        // V key - Reset VR view (calibrate headset position/orientation)
        if (s3eKeyboardGetState(s3eKeyV) & S3E_KEY_STATE_PRESSED)
        {
            VRManager::GetInstance().ResetVRView(facingYaw, true);
        }

        // B/N keys - Adjust yaw offset (rotate view left/right)
        if (s3eKeyboardGetState(s3eKeyB) & S3E_KEY_STATE_PRESSED)
        {
            VRManager::GetInstance().AdjustYawOffset(15.0f);  // Rotate left
        }
        if (s3eKeyboardGetState(s3eKeyN) & S3E_KEY_STATE_PRESSED)
        {
            VRManager::GetInstance().AdjustYawOffset(-15.0f);   // Rotate right
        }
    }
#endif

    // Rotate the main plane with keys when paused
    if (GetStatus() == STATUS_PAUSED && mGameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
    {
        Aeroplane* aeroplane = GetPlayerAeroplane();
        Transform tm = aeroplane->GetTransform();
        if (s3eKeyboardGetState(s3eKeyLeftControl) & S3E_KEY_STATE_DOWN) 
        {
            float angle = 1.0f * deltaTime;
            Quat q;
            if (s3eKeyboardGetState(s3eKeyNumPad6) & S3E_KEY_STATE_DOWN) 
                q = Quat(tm.RowX(), angle);
            if (s3eKeyboardGetState(s3eKeyNumPad4) & S3E_KEY_STATE_DOWN) 
                q = Quat(tm.RowX(), -angle);
            if (s3eKeyboardGetState(s3eKeyNumPad8) & S3E_KEY_STATE_DOWN) 
                q = Quat(tm.RowY(), angle);
            if (s3eKeyboardGetState(s3eKeyNumPad2) & S3E_KEY_STATE_DOWN) 
                q = Quat(tm.RowY(), -angle);
            if (s3eKeyboardGetState(s3eKeyNumPad9) & S3E_KEY_STATE_DOWN) 
                q = Quat(tm.RowZ(), -angle);
            if (s3eKeyboardGetState(s3eKeyNumPad7) & S3E_KEY_STATE_DOWN) 
                q = Quat(tm.RowZ(), angle);
            tm.PostRotate(q);;
        }
        else
        {
            float dist = 1.0f * deltaTime;
            Vector3 v(0,0,0);
            if (s3eKeyboardGetState(s3eKeyNumPad6) & S3E_KEY_STATE_DOWN) 
                v = tm.RowY() * -dist;
            if (s3eKeyboardGetState(s3eKeyNumPad4) & S3E_KEY_STATE_DOWN) 
                v = tm.RowY() * dist;
            if (s3eKeyboardGetState(s3eKeyNumPad8) & S3E_KEY_STATE_DOWN) 
                v = tm.RowX() * dist;
            if (s3eKeyboardGetState(s3eKeyNumPad2) & S3E_KEY_STATE_DOWN) 
                v = tm.RowX() * -dist;
            if (s3eKeyboardGetState(s3eKeyNumPad9) & S3E_KEY_STATE_DOWN) 
                v = tm.RowZ() * dist;
            if (s3eKeyboardGetState(s3eKeyNumPad7) & S3E_KEY_STATE_DOWN) 
                v = tm.RowZ() * -dist;
            tm.SetTrans(tm.GetTrans() + v);
        }
        GetPlayerAeroplane()->SetTransform(tm, aeroplane->GetVelocity(), aeroplane->GetAngularVelocity());
    }

    // Pause
    if (
        (s3eKeyboardGetState(s3eKeyMenu) & S3E_KEY_STATE_PRESSED) ||
        pauseKeyPressed ||
        joystickPausePlay
        )
    {
        switch (mStatus)
        {
        case STATUS_FLYING:
            {
                AudioManager::GetInstance().SetAllChannelsToZeroVolume();
                mStatus = STATUS_PAUSED; 
                break;
            }
        case STATUS_PAUSED:
            {
                mStatus = STATUS_FLYING; 
                break;
            }
        default:
            break;
        }
    }

    if (mPauseOverlay->IsPressed())
    {
        mStatus = STATUS_PAUSED;
    }
    if (mResumeOverlay->IsPressed())
    {
        mStatus = STATUS_FLYING;
    }

    // Enable the overlay buttons - but not the start one right at the beginning otherwise there's a danger of getting unwanted clicks.
    {
        bool paused = mStatus == STATUS_PAUSED;
        mInstance->mPauseOverlay->Enable(!paused);
        mInstance->mControllerOverlay->Enable(!paused && mGameSettings.mControllerSettings.mNumAltSettings > 1);
        mInstance->mResumeOverlay->Enable(paused);
        mInstance->mRelaunchOverlay->Enable(paused);
        mInstance->mChangeViewOverlay->Enable(paused && (!panorama || mCameraAeroplanes.size() > 1));
        mInstance->mWalkaboutOverlay->Enable((paused && mGameSettings.mOptions.mEnableWalkabout) && !panorama);
        mInstance->mSettingsMenuOverlay->Enable(paused);
        mInstance->mHelpOverlay->Enable(paused);
        if (mTimeSinceEnabled > 1.0f)
            mInstance->mStartMenuOverlay->Enable(paused);
        else
            mInstance->mStartMenuOverlay->ForceDisable();
    }

    if (mInstance->mControllerOverlay->IsEnabled() && mShowUI)
    {
        const int numColours = 4;
        GLubyte colours[numColours][3] = 
        {
            {255, 255, 255},
            {255, 0,   0  },
            {0,   255, 0  },
            {0,   0,   255}
        };
        int index = mGameSettings.mControllerSettings.mCurrentAltSetting % numColours;
        mControllerOverlay->SetColour(colours[index][0], colours[index][1], colours[index][2]);
        mControllerOverlay->SetAlpha(128);

        Vector4 textColour(colours[index][0]/255.0f, colours[index][1]/255.0f, colours[index][2]/255.0f,mControllerOverlayTextOpacity);
        mControllerOverlay->SetTextColour(textColour);
        const std::string& name = mGameSettings.mControllerSettings.GetCurrentName();

        if (mGameSettings.mOptions.mControllerStaggered)
        {
            mControllerOverlay->SetText(name.c_str(), ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, 0.0f, 0.0f);
            mControllerOverlay->SetAnchor(ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_BOT);
            mControllerOverlay->SetPosition(0.0f, 0.0f);
        }
        else
        {
            mControllerOverlay->SetText(name.c_str(), ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_BOT, 0.0f, 0.0f);
            mControllerOverlay->SetAnchor(ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP);
            mControllerOverlay->SetPosition(0.0f, 1.0f);
        }
        mControllerOverlayTextOpacity = ClampToRange(mControllerOverlayTextOpacity - mCurrentDeltaTime * 0.4f, 0.0f, 1.0f);
    }

    // Update connection manager
    if (mInstance->mConnectionListener)
        mConnectionListener->Update();

    // Updates everything non-graphical, including physics
    EntityManager::GetInstance().UpdateEntities(gameDeltaTime);

    Challenge::ChallengeResult challengeResult = mChallenge->UpdateChallenge(gameDeltaTime);
    if (challengeResult == Challenge::CHALLENGE_RELAUNCH)
    {
        // Move/initialise the aeroplane so it's next to the observer
        mPlayerAeroplane->Launch(mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans());
    }

    RenderManager::GetInstance().EnableStereoscopy(mGameSettings.mOptions.mEnableStereoscopy);
    RenderManager::GetInstance().SetStereoSeparation(mGameSettings.mOptions.mStereoSeparation);

    RenderManager::GetInstance().Update(mCurrentDeltaTime, gameDeltaTime);

    Transform cameraTM;
    cameraTM = mViewport->GetCamera()->GetTransform();
    // Adjust the clipping
    GLint depthBits = 0;
    glGetIntegerv( GL_DEPTH_BITS, &depthBits);
    float cameraTerrainZ = Environment::GetInstance().GetTerrain().GetTerrainHeight(cameraTM.GetTrans().x, cameraTM.GetTrans().y, false);
    float cameraHeight = cameraTM.GetTrans().z - cameraTerrainZ;
    float maxClipDist = depthBits < 24 ? 1.0f : 0.1f;
    float nearClipScale = ClampToRange(cameraHeight - 0.3f, 0.01f, maxClipDist);
    mGameSettings.mOptions.mFrameworkSettings.mNearClipPlaneDistance = nearClipScale * mGameSettings.mOptions.mMaxNearClipDistance;

    // Draw everything
#ifdef PICASIM_VR_SUPPORT
    // Use IsVRReady() to check if headset is active, not just IsVREnabled()
    // This allows falling back to desktop rendering when headset is removed
    if (VRManager::IsAvailable() && VRManager::GetInstance().IsVRReady())
    {
        // VR headset is active - render to VR
        VRFrameInfo vrFrameInfo;
        if (VRManager::GetInstance().GetRuntime())
        {
            RenderManager::GetInstance().RenderUpdateVR(vrFrameInfo);

            // Handle desktop window display based on VR desktop mode
            switch (mGameSettings.mOptions.mVRDesktopMode)
            {
            case Options::VR_DESKTOP_NOTHING:
                // Just clear the desktop window to black
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case Options::VR_DESKTOP_VR_VIEW:
                // Show VR view in desktop window
                RenderManager::GetInstance().RenderMirrorWindow();
                break;
            case Options::VR_DESKTOP_NORMAL_VIEW:
                // Render normal non-VR view to desktop window
                RenderManager::GetInstance().RenderUpdate();
                break;
            }
        }
        else
        {
            RenderManager::GetInstance().RenderUpdate();
        }
    }
    else
#endif
    {
        // No VR or headset not active - render normally to desktop
        RenderManager::GetInstance().RenderUpdate();
    }

    if (!mShowUI)
    {
        mPauseOverlay->SetAlpha(0);
        mHelpOverlay->SetAlpha(0);
        mStartMenuOverlay->SetAlpha(0);
        mResumeOverlay->SetAlpha(0);
        mSettingsMenuOverlay->SetAlpha(0);
        mRelaunchOverlay->SetAlpha(0);
        mChangeViewOverlay->SetAlpha(0);
        mWalkaboutOverlay->SetAlpha(0);
        mControllerOverlay->SetAlpha(0);
        mControllerOverlay->SetText("", ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, 0.0f, 0.0f);
        mWindsockOverlay->SetAlpha(0);
    }
    else
    {
        // Restore overlay visibility
        mPauseOverlay->SetAlpha(mGameSettings.mOptions.mPauseButtonOpacity);
        mHelpOverlay->SetAlpha(255);
        mStartMenuOverlay->SetAlpha(255);
        mResumeOverlay->SetAlpha(255);
        mSettingsMenuOverlay->SetAlpha(255);
        mRelaunchOverlay->SetAlpha(255);
        mChangeViewOverlay->SetAlpha(255);
        mWalkaboutOverlay->SetAlpha(255);
        mControllerOverlay->SetAlpha(255);
        mWindsockOverlay->SetAlpha(255);
    }

    // Audio
    AudioManager::GetInstance().SetVolume(mGameSettings.mOptions.mVolumeScale);
    Vector3 cameraVelocity(0,0,0);
    AudioManager::GetInstance().SetTransformAndVelocity(cameraTM, cameraVelocity);
    AudioManager::GetInstance().Update(deltaTime);

    // Wind sound
    Vector3 pos = cameraTM.GetTrans();
    Vector3 windDir = Environment::GetInstance().GetWindAtPosition(pos, Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
    float windStrength = windDir.GetLength();
    if (windStrength > 0.0f)
        windDir.Normalise();  
    else 
        windDir = Vector3(0,0,0);
    float dist = 2.0f;
    pos -= windDir * dist;

    float freqScale = windStrength / 8.0f;
    freqScale = ClampToRange(freqScale, 0.8f, 1.5f);
    freqScale *= mActualTimeScale;

    float volumeScale = windStrength / 5.0f;
    volumeScale = ClampToRange(volumeScale, 0.0f, 1.0f);
    volumeScale *= volumeScale;
    AudioManager::GetInstance().SetChannelPositionAndVelocity(mSoundChannel, pos, Vector3(0,0,0));
    AudioManager::GetInstance().SetChannelFrequencyScale(mSoundChannel, freqScale);
    AudioManager::GetInstance().SetChannelTargetVolumeScale(mSoundChannel, GetSettings().mOptions.mWindVolume * volumeScale);

    // Windsock
    if (windStrength > 0.02f)
    {
        float windAngle = 270.0f-RadiansToDegrees(atan2f(windDir.Dot(cameraTM.RowX()), windDir.Dot(cameraTM.RowY())));
        mWindsockOverlay->SetAngle(windAngle);
        mWindsockOverlay->SetAlpha(mGameSettings.mOptions.mWindsockOpacity);
        mWindsockOverlay->SetSize(mGameSettings.mOptions.mWindArrowSize);
        mWindsockOverlay->SetPosition(0.5f, mGameSettings.mOptions.mWindArrowSize);
    }
    else
    {
        mInstance->mWindsockOverlay->SetAlpha(0);
    }

    // Save settings after at least one successful update
    if (mUpdateCounter++ == 4)
    {
        TRACE_FILE_IF(1) TRACE("Saving settings");
        mGameSettings.SaveToFile((Platform::GetUserSettingsPath() + "settings.xml").c_str());
    }
    mTimeSinceEnabled += mCurrentDeltaTime;

    if (s3eKeyboardGetState(s3eKeyS) & S3E_KEY_STATE_PRESSED) 
    {
        SaveScreenshot();
    }

    if (mShowHelpAfterLoading && mUpdateCounter > 10 && mStatus == STATUS_PAUSED)
    {
        mTimeSinceEnabled = 1.0f;
        const Language language = mGameSettings.mOptions.mLanguage;
        if (mGameSettings.mOptions.mFreeFlyMode == Options::FREEFLYMODE_TRAINERGLIDER)
        {
            ShowInGameDialog(0.65f, 0.55f, TXT(PS_FLYING), TXT(PS_TRAINERGLIDERTIP), TXT(PS_OK));
        }
        else if (mGameSettings.mOptions.mFreeFlyMode == Options::FREEFLYMODE_TRAINERPOWERED)
        {
            ShowInGameDialog(0.65f, 0.55f, TXT(PS_FLYING), TXT(PS_TRAINERPOWEREDTIP), TXT(PS_OK));
        }
        mShowHelpAfterLoading = false;
    }

    UpdateResourceGroupForScreen(mGameSettings);

    return UPDATE_CONTINUE;
}

