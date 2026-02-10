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
#include "../Platform/Input.h"
#include "../Framework/Camera.h"
#include "Platform.h"

#ifdef PICASIM_VR_SUPPORT
#include "../Platform/VRManager.h"
#include "../Platform/VRRuntime.h"
#include <glad/glad.h>
#include "Menus/UIHelpers.h"
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
void IwGxSwapBuffers();
#endif

float numButtonSlots = 8.0f;

std::unique_ptr<PicaSim> PicaSim::mInstance;

//======================================================================================================================
PicaSim::PicaSim(GameSettings& gameSettings)
    : mCurrentDeltaTime(0.0f), mGameSettings(gameSettings)
{
    mControllerOverlayTextOpacity = 1.0f;
    mSound = 0;
    mSoundChannel = -1;
    mUpdateCounter = 0;
    mTimeSinceEnabled = 0.0f;
}

//======================================================================================================================
bool PicaSim::Init(GameSettings& gameSettings, LoadingScreenHelper* loadingScreen)
{
    const ChallengeSettings& cs = gameSettings.mChallengeSettings;
    TRACE_FILE_IF(ONCE_1) TRACE("PicaSim::Init num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());

    mInstance.reset(new PicaSim(gameSettings));

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
            mInstance->mChallenge = std::make_unique<ChallengeFreeFly>();
        }
        break;
    case ChallengeSettings::CHALLENGE_RACE:
        {
            mInstance->mChallenge = std::make_unique<ChallengeRace>(gameSettings);
        }
        break;
    case ChallengeSettings::CHALLENGE_LIMBO:
        {
            mInstance->mChallenge = std::make_unique<ChallengeLimbo>(gameSettings);
        }
        break;
    case ChallengeSettings::CHALLENGE_DURATION:
        {
            mInstance->mChallenge = std::make_unique<ChallengeDuration>(gameSettings);
        }
        break;
    default:
        TRACE_FILE_IF(ONCE_1) TRACE("Unable to load challenge type %d", gameSettings.mChallengeSettings.mChallengeMode);
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
    mInstance->mPlayerController = std::make_unique<HumanController>(mInstance->mGameSettings);
    mInstance->mPlayerAeroplane = std::make_unique<Aeroplane>(*mInstance->mPlayerController);
    mInstance->mPlayerController->SetAeroplane(mInstance->mPlayerAeroplane.get());
    mInstance->mCameraAeroplaneIndex = 0;
    mInstance->mCameraAeroplanes.push_back(mInstance->mPlayerAeroplane.get());

    // Create an observer
    mInstance->mObserver = std::make_unique<Observer>();
//  mInstance->mObserver->Init(Environment::GetInstance().GetObservationPoint(), aeroplane);
    mInstance->mObserver->Init(mInstance->mGameSettings.mEnvironmentSettings.mObserverPosition, mInstance->mPlayerAeroplane.get());

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
    mInstance->mPauseOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/PlayerPause.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, 1.0f, 1.0f,
        mInstance->mGameSettings.mOptions.mPauseButtonOpacity, false, false);
    mInstance->mControllerOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/Joystick.png", buttonSize, 1.0f,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*7, 1.0f, 0, false, true);
    mInstance->mStartMenuOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/Stop.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, 0.0f, 1.0f, alpha, false, false);
    mInstance->mHelpOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/Help.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*1, 1.0f, alpha, false, false);
    mInstance->mWalkaboutOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/AgtMember.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*3, 1.0f, alpha, false, false);
    mInstance->mChangeViewOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/Find.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*4, 1.0f, alpha, false, false);
    mInstance->mSettingsMenuOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/Utilities.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*5, 1.0f, alpha, false, false);
    mInstance->mRelaunchOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/PlayerRew.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*7, 1.0f, alpha, false, false);
    mInstance->mResumeOverlay = std::make_unique<ButtonOverlay>("SystemData/Menu/PlayerPlay.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, buttonOffset*8, 1.0f, alpha, false, false);

    mInstance->mWindsockOverlay = std::make_unique<WindsockOverlay>("SystemData/Menu/Windsock.png",
        mInstance->mGameSettings.mOptions.mWindArrowSize, 0.5f,
        mInstance->mGameSettings.mOptions.mWindArrowSize, 0, 0.0f);

    if (loadingScreen) loadingScreen->Update("Wind sound");
    mInstance->mSound = AudioManager::GetInstance().LoadSound("SystemData/Audio/WindLoop22050Mono.raw", 22050, false, true, true);
    mInstance->mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(1.0f, true);
    if (mInstance->mSoundChannel != -1)
        AudioManager::GetInstance().StartSoundOnChannel(mInstance->mSoundChannel, mInstance->mSound, true);
    else
        TRACE_FILE_IF(ONCE_1) TRACE("Unable to allocate sound channel for wind");

    if (loadingScreen) loadingScreen->Update("Wind sound");

    mInstance->mChallenge->Init(mInstance->mPlayerAeroplane.get(), loadingScreen);

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
    mInstance->mConnectionListener = std::make_unique<ConnectionListener>();
    mInstance->mConnectionListener->Init();
  }
    return true;
}

//======================================================================================================================
void PicaSim::Terminate()
{
    TRACE_FUNCTION_ONLY(ONCE_1);
    Environment::Terminate();

    if (mInstance)
    {
        if (mInstance->mConnectionListener)
        {
            mInstance->mConnectionListener->Terminate();
            mInstance->mConnectionListener.reset();
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
            mInstance->mPlayerAeroplane.reset();
        }
        if (mInstance->mPlayerController)
        {
            mInstance->mPlayerController.reset();
        }
        if (mInstance->mObserver)
        {
            mInstance->mObserver->Terminate();
            mInstance->mObserver.reset();
        }

        mInstance->mChallenge->Terminate();
        mInstance->mChallenge.reset();

        mInstance->mPauseOverlay.reset();
        mInstance->mResumeOverlay.reset();
        mInstance->mRelaunchOverlay.reset();
        mInstance->mChangeViewOverlay.reset();
        mInstance->mWalkaboutOverlay.reset();
        mInstance->mSettingsMenuOverlay.reset();
        mInstance->mStartMenuOverlay.reset();
        mInstance->mHelpOverlay.reset();
        mInstance->mControllerOverlay.reset();

        mInstance->mWindsockOverlay.reset();

        mInstance->mParticleEngine.Terminate();
    }

    EntityManager::Terminate();

    RenderManager::Terminate();

    if (mInstance->mSoundChannel != -1)
        AudioManager::GetInstance().ReleaseSoundChannel(mInstance->mSoundChannel);
    if (mInstance->mSound)
        AudioManager::GetInstance().UnloadSound(mInstance->mSound);

    mInstance.reset();
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
        mViewport->GetCamera()->SetVROrientationMode(Camera::VROrientationMode::Inherit);
        mViewport->GetCamera()->SetCameraTransform(cameraAeroplane);
        mViewport->GetCamera()->SetCameraTarget(0);
        mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mAeroplaneViewFieldOfView));
        mViewport->GetCamera()->DisableAutoZoom();

        mZoomViewport->Enable(false);
        break;
    case MODE_CHASE:
        mViewport->GetCamera()->SetUserData((void*) CAMERA_CHASE);
        mViewport->GetCamera()->SetVROrientationMode(Camera::VROrientationMode::Inherit);
        mViewport->GetCamera()->SetCameraTransform(cameraAeroplane);
        mViewport->GetCamera()->SetCameraTarget(cameraAeroplane);
        mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mAeroplaneViewFieldOfView));
        mViewport->GetCamera()->DisableAutoZoom();

        mZoomViewport->Enable(false);
        break;
    case MODE_GROUND:
        {
            mViewport->GetCamera()->SetUserData((void*) CAMERA_GROUND);
            mViewport->GetCamera()->SetVROrientationMode(Camera::VROrientationMode::Override);
            mViewport->GetCamera()->SetCameraTransform(mObserver.get());
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
                mZoomViewport->GetCamera()->SetCameraTransform(mInstance->mObserver.get());
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
            mViewport->GetCamera()->SetVROrientationMode(Camera::VROrientationMode::Override);
            mViewport->GetCamera()->SetCameraTransform(mObserver.get());
            mViewport->GetCamera()->SetCameraTarget(0);
            mViewport->GetCamera()->SetVerticalFOV(DegreesToRadians(mGameSettings.mOptions.mGroundViewFieldOfView));
            mViewport->GetCamera()->DisableAutoZoom();
        }
        break;
    default:
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
        TRACE_FILE_IF(ONCE_1) TRACE("Detected help button press");
#if 0
        AudioManager::GetInstance().SetAllChannelsToZeroVolume();
        DisplayHelpMenu(mGameSettings, true);
        TRACE_FILE_IF(ONCE_1) TRACE("Return from help");
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
        TRACE_FILE_IF(ONCE_1) TRACE("Detected settings button press");

        AudioManager::GetInstance().SetAllChannelsToZeroVolume();
        SettingsChangeActions actions;
        DisplaySettingsMenu(mGameSettings, actions);

        TRACE_FILE_IF(ONCE_1) TRACE("Return from settings");

        LoadingScreen* loadingScreen = 0;

        if (actions.mReloadTerrain)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("Reloading terrain");
            if (!loadingScreen)
                loadingScreen = new LoadingScreen("Loading", mGameSettings, true, true, true);
            Environment::Terminate();
            Environment::Init(loadingScreen);
            actions.mRecalcWind = false;

            Transform tm = mObserver->GetTransform();
            tm.SetTrans(mGameSettings.mEnvironmentSettings.mObserverPosition);
            mObserver->SetTransform(tm);

            mPlayerAeroplane->Terminate();

            Vector3 launchPos = mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans();
            mPlayerAeroplane->Init(
                mGameSettings.mAeroplaneSettings,
                &launchPos, loadingScreen);

                // Reset any challenge
            mChallenge->Terminate();
            mChallenge->Init(mPlayerAeroplane.get(), loadingScreen);
            actions.mReloadAeroplane = false;
        }

        if (actions.mReloadAeroplane)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("Reloading aeroplane");
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
            TRACE_FILE_IF(ONCE_1) TRACE("Recalculating wind");
          if (!loadingScreen)
                loadingScreen = new LoadingScreen("Loading", mGameSettings, false, true, false);
            Environment::GetInstance().InitZeroFlowHeightfield(loadingScreen);
        }

        if (actions.mRestartChallenge)
        {
            if (mGameSettings.mChallengeSettings.mChallengeMode != ChallengeSettings::CHALLENGE_FREEFLY)
                mPlayerAeroplane->Launch(mInstance->mObserver->GetCameraTransform((void*) 0).GetTrans());
            mChallenge->Terminate();
            mChallenge->Init(mPlayerAeroplane.get(), loadingScreen);
        }

        if (actions.mReloadAI)
        {
            if (!mGameSettings.mAIControllersSettings.mAIControllers.empty() && !loadingScreen)
                loadingScreen = new LoadingScreen("Loading AI", mGameSettings, false, true, mGameSettings.mAIControllersSettings.mAIControllers.size() > 1);
            mChallenge->Terminate();
            mChallenge->Init(mPlayerAeroplane.get(), loadingScreen);
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
    if ((Input::GetInstance().GetKeyState(SDLK_l) & KEY_STATE_PRESSED))
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Reloading aeroplane - manual prompt");
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
        ((Input::GetInstance().GetKeyState(SDLK_AC_BACK) & KEY_STATE_PRESSED) && mGameSettings.mOptions.mUseBackButtonToExit)||
        CheckForQuitRequest()
        )
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Returning to start screen");
        TRACE_FILE_IF(ONCE_1) TRACE("Saving settings");
        mGameSettings.SaveToFile((Platform::GetUserSettingsPath() + "settings.xml").c_str());
        // Clear pressed states to prevent click bleed-through to start menu
        Input::GetInstance().ClearPressedStates();
        return UPDATE_START;
    }

    bool resetKeyPressed = (Input::GetInstance().GetKeyState(SDLK_r) & KEY_STATE_PRESSED) != 0;
    bool cameraKeyPressed = (Input::GetInstance().GetKeyState(SDLK_c) & KEY_STATE_PRESSED) != 0;
    bool walkaboutKeyPressed = (Input::GetInstance().GetKeyState(SDLK_m) & KEY_STATE_PRESSED) != 0;
    bool pauseKeyPressed = (Input::GetInstance().GetKeyState(SDLK_p) & KEY_STATE_PRESSED) != 0;
    bool slowmoKeyPressed = (Input::GetInstance().GetKeyState(SDLK_t) & KEY_STATE_PRESSED) != 0;
    bool uiKeyPressed = (Input::GetInstance().GetKeyState(SDLK_u) & KEY_STATE_PRESSED) != 0;
    bool zoomViewKeyPressed = (Input::GetInstance().GetKeyState(SDLK_z) & KEY_STATE_PRESSED) != 0;

    bool zoomInKeyPressed = (Input::GetInstance().GetKeyState(SDLK_EQUALS) & KEY_STATE_DOWN) != 0;
    bool zoomOutKeyPressed = (Input::GetInstance().GetKeyState(SDLK_MINUS) & KEY_STATE_DOWN) != 0;

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
        (Input::GetInstance().GetKeyState(SDLK_RSHIFT) & KEY_STATE_PRESSED)
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
        ((Input::GetInstance().GetKeyState(SDLK_AC_BACK) & KEY_STATE_PRESSED) && !mGameSettings.mOptions.mUseBackButtonToExit)||
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
        (Input::GetInstance().GetKeyState(SDLK_AC_SEARCH) & KEY_STATE_PRESSED) ||
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
    if (Input::GetInstance().GetKeyState(SDLK_k) & KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mRenderPreference = (Options::RenderPreference) ( (mGameSettings.mOptions.mRenderPreference+1) % Options::RENDER_PREFER_MAX );
    }
    if (Input::GetInstance().GetKeyState(SDLK_g) & KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mDrawAeroplaneCoM = (Options::DrawCoMType) ((mGameSettings.mOptions.mDrawAeroplaneCoM + 1) % Options::COM_MAX);
    }
    if (Input::GetInstance().GetKeyState(SDLK_w) & KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mTerrainWireframe = !mGameSettings.mOptions.mTerrainWireframe;
    }
    if (Input::GetInstance().GetKeyState(SDLK_9) & KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mAerofoilInfo = Maximum(mGameSettings.mOptions.mAerofoilInfo-1, -1);
    }
    if (Input::GetInstance().GetKeyState(SDLK_0) & KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mAerofoilInfo = Minimum(mGameSettings.mOptions.mAerofoilInfo+1, 64);
    }
    if (Input::GetInstance().GetKeyState(SDLK_7) & KEY_STATE_PRESSED) 
    {
        mGameSettings.mOptions.mNumWindStreamers = Maximum(mGameSettings.mOptions.mNumWindStreamers-1, 0);
    }
    if (Input::GetInstance().GetKeyState(SDLK_8) & KEY_STATE_PRESSED)
    {
        mGameSettings.mOptions.mNumWindStreamers = Minimum(mGameSettings.mOptions.mNumWindStreamers+1, 64);
    }

#ifdef PICASIM_VR_SUPPORT
    // VR view calibration keys
    if (VRManager::IsAvailable() && VRManager::GetInstance().IsVREnabled())
    {
        VRManager::GetInstance().UseYawOffsets(mMode == MODE_GROUND);

        // Keep VRManager updated with current wind direction for auto-reset on session start
        Vector3 upWindDir = -Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH);
        float facingYaw = atan2f(upWindDir.y, upWindDir.x);
        VRManager::GetInstance().SetDefaultFacingYaw(facingYaw);

        // V key - Reset VR view (calibrate headset position/orientation)
        if (Input::GetInstance().GetKeyState(SDLK_v) & KEY_STATE_PRESSED)
        {
            VRManager::GetInstance().ResetVRView(facingYaw, true);
        }

        // B/H/N keys - Adjust yaw offset (rotate view left/right)
        if (Input::GetInstance().GetKeyState(SDLK_b) & KEY_STATE_PRESSED)
        {
            VRManager::GetInstance().AdjustYawOffset(15.0f);  // Rotate left
        }
        if (Input::GetInstance().GetKeyState(SDLK_h) & KEY_STATE_PRESSED)
        {
            VRManager::GetInstance().ResetYawOffset();
        }
        if (Input::GetInstance().GetKeyState(SDLK_n) & KEY_STATE_PRESSED)
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
        if (Input::GetInstance().GetKeyState(SDLK_LCTRL) & KEY_STATE_DOWN) 
        {
            float angle = 1.0f * deltaTime;
            Quat q;
            if (Input::GetInstance().GetKeyState(SDLK_KP_6) & KEY_STATE_DOWN) 
                q = Quat(tm.RowX(), angle);
            if (Input::GetInstance().GetKeyState(SDLK_KP_4) & KEY_STATE_DOWN) 
                q = Quat(tm.RowX(), -angle);
            if (Input::GetInstance().GetKeyState(SDLK_KP_8) & KEY_STATE_DOWN) 
                q = Quat(tm.RowY(), angle);
            if (Input::GetInstance().GetKeyState(SDLK_KP_2) & KEY_STATE_DOWN) 
                q = Quat(tm.RowY(), -angle);
            if (Input::GetInstance().GetKeyState(SDLK_KP_9) & KEY_STATE_DOWN) 
                q = Quat(tm.RowZ(), -angle);
            if (Input::GetInstance().GetKeyState(SDLK_KP_7) & KEY_STATE_DOWN) 
                q = Quat(tm.RowZ(), angle);
            tm.PostRotate(q);;
        }
        else
        {
            float dist = 1.0f * deltaTime;
            Vector3 v(0,0,0);
            if (Input::GetInstance().GetKeyState(SDLK_KP_6) & KEY_STATE_DOWN) 
                v = tm.RowY() * -dist;
            if (Input::GetInstance().GetKeyState(SDLK_KP_4) & KEY_STATE_DOWN) 
                v = tm.RowY() * dist;
            if (Input::GetInstance().GetKeyState(SDLK_KP_8) & KEY_STATE_DOWN) 
                v = tm.RowX() * dist;
            if (Input::GetInstance().GetKeyState(SDLK_KP_2) & KEY_STATE_DOWN) 
                v = tm.RowX() * -dist;
            if (Input::GetInstance().GetKeyState(SDLK_KP_9) & KEY_STATE_DOWN) 
                v = tm.RowZ() * dist;
            if (Input::GetInstance().GetKeyState(SDLK_KP_7) & KEY_STATE_DOWN) 
                v = tm.RowZ() * -dist;
            tm.SetTrans(tm.GetTrans() + v);
        }
        GetPlayerAeroplane()->SetTransform(tm, aeroplane->GetVelocity(), aeroplane->GetAngularVelocity());
    }

    // Pause
    if (
        (Input::GetInstance().GetKeyState(SDLK_MENU) & KEY_STATE_PRESSED) ||
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

    RenderManager::GetInstance().Update(mCurrentDeltaTime, gameDeltaTime);

    Transform cameraTM;
    cameraTM = mViewport->GetCamera()->GetTransform();
    // Adjust the clipping
    GLint depthBits = 0;
    glGetIntegerv( GL_DEPTH_BITS, &depthBits);
    static bool loggedDepthBits = false;
    if (!loggedDepthBits) {
        TRACE("Depth buffer bits: %d", depthBits);
        loggedDepthBits = true;
    }
    float cameraTerrainZ = Environment::GetInstance().GetTerrain().GetTerrainHeight(cameraTM.GetTrans().x, cameraTM.GetTrans().y, false);
    float cameraHeight = cameraTM.GetTrans().z - cameraTerrainZ;
    float maxClipDist = depthBits < 24 ? 1.0f : 0.1f;
    float nearClipScale = ClampToRange(cameraHeight - 0.3f, 0.01f, maxClipDist);
    mGameSettings.mOptions.mFrameworkSettings.mNearClipPlaneDistance = nearClipScale * mGameSettings.mOptions.mMaxNearClipDistance;

    // Draw everything
#ifdef PICASIM_VR_SUPPORT
    // Track VR camera transform for audio (captured before desktop rendering can overwrite it)
    Transform vrListenerTM;
    bool hasVRListenerTM = false;

    // Use IsVRReady() to check if headset is active, not just IsVREnabled()
    // This allows falling back to desktop rendering when headset is removed
    if (VRManager::IsAvailable() && VRManager::GetInstance().IsVRReady())
    {
        // VR headset is active - render to VR
        VRFrameInfo vrFrameInfo;
        if (VRManager::GetInstance().GetRuntime())
        {
            // Configure VR settings
            RenderManager& rm = RenderManager::GetInstance();
            bool isPanoramicScene = mGameSettings.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA;
            VRManager::GetInstance().SetPanoramicScene(isPanoramicScene);
            rm.SetVRPanoramaDepthEnabled(mGameSettings.mOptions.mVRPanoramaDepth && isPanoramicScene);
            rm.SetVRSkybox(&Environment::getSkybox());

            rm.RenderUpdateVR(vrFrameInfo);

            // Capture VR camera transform for audio BEFORE desktop rendering can overwrite it
            vrListenerTM = mViewport->GetCamera()->GetTransform();
            hasVRListenerTM = true;

            // Handle desktop window display based on VR desktop mode
            switch (mGameSettings.mOptions.mVRDesktopMode)
            {
            case Options::VR_DESKTOP_NOTHING:
            {
                // Clear the desktop window to black
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // Render "VR Headset in use" message centered on screen
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplSDL2_NewFrame();
                ImGui::NewFrame();
                UIHelpers::DrawCenteredText(GetPS(PS_MIRROR_TEXT, mGameSettings.mOptions.mLanguage), 0.5f, UIHelpers::Colors::LightGray, 1.5f);
                UIHelpers::DrawCenteredText(GetPS(PS_MIRROR_SETTINGS, mGameSettings.mOptions.mLanguage), 0.6f, UIHelpers::Colors::LightGray, 1.0f);
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                IwGxSwapBuffers();
                break;
            }
            case Options::VR_DESKTOP_LEFT_EYE:
                // Show left eye view in desktop window
                RenderManager::GetInstance().RenderMirrorWindow(RenderManager::VR_MIRROR_LEFT_EYE);
                break;
            case Options::VR_DESKTOP_RIGHT_EYE:
                // Show right eye view in desktop window
                RenderManager::GetInstance().RenderMirrorWindow(RenderManager::VR_MIRROR_RIGHT_EYE);
                break;
            case Options::VR_DESKTOP_BOTH_EYES:
                // Show both eyes side by side in desktop window
                RenderManager::GetInstance().RenderMirrorWindow(RenderManager::VR_MIRROR_BOTH_EYES);
                break;
            case Options::VR_DESKTOP_BOTH_EYES_CROSSED:
                // Show both eyes crossed (right-left) for cross-eyed viewing
                RenderManager::GetInstance().RenderMirrorWindow(RenderManager::VR_MIRROR_BOTH_EYES_CROSSED);
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

    Transform listenerTM = cameraTM;  // Default to desktop camera

#ifdef PICASIM_VR_SUPPORT
    // When VR is active, use the VR camera transform that was captured after VR rendering
    // (before desktop rendering could overwrite it)
    // This ensures 3D audio matches what the user sees in the headset
    if (hasVRListenerTM)
    {
        listenerTM = vrListenerTM;
    }
#endif

    AudioManager::GetInstance().SetTransformAndVelocity(listenerTM, cameraVelocity);
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
        TRACE_FILE_IF(ONCE_1) TRACE("Saving settings");
        mGameSettings.SaveToFile((Platform::GetUserSettingsPath() + "settings.xml").c_str());
    }
    mTimeSinceEnabled += mCurrentDeltaTime;

    if (Input::GetInstance().GetKeyState(SDLK_s) & KEY_STATE_PRESSED) 
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

