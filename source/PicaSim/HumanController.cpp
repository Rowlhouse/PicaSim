#include "HumanController.h"
#include "PicaSim.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "Aeroplane.h"
#include "Menus/Menu.h"
#include "PicaJoystick.h"
#include "../Platform/S3ECompat.h"
#include "../Platform/Input.h"

//======================================================================================================================
ControllerSettings::ControllerControls GetControlForStick(ControllerSettings::ControllerStick stick, int mode)
{
    switch (mode)
    {
    case 0:
        if (stick == ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_ROLL;
        else if (stick == ControllerSettings::CONTROLLER_RIGHT_VERTICAL)
            return ControllerSettings::CONTROLLER_STICK_SPEED;
        else if (stick == ControllerSettings::CONTROLLER_LEFT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_YAW;
        else
            return ControllerSettings::CONTROLLER_STICK_PITCH;

    case 1:
    default:
        if (stick == ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_ROLL;
        else if (stick == ControllerSettings::CONTROLLER_RIGHT_VERTICAL)
            return ControllerSettings::CONTROLLER_STICK_PITCH;
        else if (stick == ControllerSettings::CONTROLLER_LEFT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_YAW;
        else
            return ControllerSettings::CONTROLLER_STICK_SPEED;

    case 2:
        if (stick == ControllerSettings::CONTROLLER_LEFT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_ROLL;
        else if (stick == ControllerSettings::CONTROLLER_LEFT_VERTICAL)
            return ControllerSettings::CONTROLLER_STICK_SPEED;
        else if (stick == ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_YAW;
        else
            return ControllerSettings::CONTROLLER_STICK_PITCH;

    case 3:
        if (stick == ControllerSettings::CONTROLLER_LEFT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_ROLL;
        else if (stick == ControllerSettings::CONTROLLER_LEFT_VERTICAL)
            return ControllerSettings::CONTROLLER_STICK_PITCH;
        else if (stick == ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL)
            return ControllerSettings::CONTROLLER_STICK_YAW;
        else
            return ControllerSettings::CONTROLLER_STICK_SPEED;
    }
}

enum
{
    STICK_RIGHT            = 1 << 0,
    STICK_RIGHT_HORIZONTAL = 1 << 2,
    STICK_RIGHT_VERTICAL   = 1 << 3,
    STICK_LEFT             = 1 << 4,
    STICK_LEFT_HORIZONTAL  = 1 << 5,
    STICK_LEFT_VERTICAL    = 1 << 6,
    BUTTON_0               = 1 << 7,
    BUTTON_1               = 1 << 8,
    BUTTON_2               = 1 << 9,
};

//======================================================================================================================
static bool GetStickInUse(const ControllerSettings& controllerSettings, int mode, ControllerSettings::ControllerStick stick)
{
    ControllerSettings::ControllerControls stickControllerControl = GetControlForStick(stick, mode);
    for (int iControlPerChannel = 0 ; iControlPerChannel != Controller::MAX_CHANNELS ; ++iControlPerChannel)
    {
        const ControllerSettings::ControllerControls& controllerControl = controllerSettings.mControlPerChannel[iControlPerChannel];
        if (stickControllerControl == controllerControl)
            return true;
    }
    return false;
}

//======================================================================================================================
static bool GetButtonInUse(const ControllerSettings& controllerSettings, int button, const Aeroplane* aeroplane)
{
    bool smokeEnabled = PicaSim::GetInstance().GetSettings().mOptions.mEnableSmoke;
    for (int iControlPerChannel = 0 ; iControlPerChannel != Controller::MAX_CHANNELS ; ++iControlPerChannel)
    {
        if (!smokeEnabled && (iControlPerChannel == Controller::CHANNEL_SMOKE1 || iControlPerChannel == Controller::CHANNEL_SMOKE2))
            continue;

        if (aeroplane)
        {
            Aeroplane::LaunchMode launchMode = aeroplane->GetLaunchMode();
            if (iControlPerChannel == Controller::CHANNEL_HOOK && launchMode == Aeroplane::LAUNCHMODE_NONE)
                continue;
        }

        const ControllerSettings::ControllerControls& controllerControl = controllerSettings.mControlPerChannel[iControlPerChannel];
        if (ControllerSettings::CONTROLLER_BUTTON0 + button == controllerControl)
            return true;
    }
    return false;
}

//======================================================================================================================
uint32 GetStickVisibleMask(const GameSettings& gs, const Aeroplane* aeroplane)
{
    uint32 result = 0;

    if (GetStickInUse(gs.mControllerSettings, gs.mOptions.mControllerMode, ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL))
        result |= STICK_RIGHT | STICK_RIGHT_HORIZONTAL;
    if (GetStickInUse(gs.mControllerSettings, gs.mOptions.mControllerMode, ControllerSettings::CONTROLLER_RIGHT_VERTICAL))
        result |= STICK_RIGHT | STICK_RIGHT_VERTICAL;

    if (GetStickInUse(gs.mControllerSettings, gs.mOptions.mControllerMode, ControllerSettings::CONTROLLER_LEFT_HORIZONTAL))
        result |= STICK_LEFT | STICK_LEFT_HORIZONTAL;
    if (GetStickInUse(gs.mControllerSettings, gs.mOptions.mControllerMode, ControllerSettings::CONTROLLER_LEFT_VERTICAL))
        result |= STICK_LEFT | STICK_LEFT_VERTICAL;

    if (GetButtonInUse(gs.mControllerSettings, 0, aeroplane))
        result |= BUTTON_0;
    if (GetButtonInUse(gs.mControllerSettings, 1, aeroplane))
        result |= BUTTON_1;
    if (GetButtonInUse(gs.mControllerSettings, 2, aeroplane))
        result |= BUTTON_2;

    return result;
}

//======================================================================================================================
HumanController::HumanController(GameSettings& gs)
    : mGameSettings(gs), mAeroplane(0)
{
    mStickIndicatorSize = 0.02f;
    mControllerSizeX = 1.0f;
    mControllerSizeY = 1.0f;

    mRightControllerPosX = 0.75f;
    mRightControllerPosY = 0.25f;
    mLeftControllerPosX = 0.25f;
    mLeftControllerPosY = 0.25f;

    mRightWasTouched = false;
    mRightInitialTouchPosX = 0.0f;
    mRightInitialTouchPosY = 0.0f;
    mLeftWasTouched = false;
    mLeftInitialTouchPosX = 0.0f;
    mLeftInitialTouchPosY = 0.0f;

    mElevatorTrim = 0.0f;

    for (size_t i = 0 ; i != ControllerSettings::CONTROLLER_NUM_CONTROLS ; ++i)
        mInputControls[i] = 0.0f;
    for (unsigned int i = 0 ; i != MAX_CHANNELS ; ++i)
        mOutputControls[i] = 0.0f;

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_CONTROL);
    RenderManager::GetInstance().RegisterRenderOverlayObject(this, 0);
    
    extern float numButtonSlots;
    float buttonSize = PicaSim::GetInstance().GetSettings().mOptions.mPauseButtonsSize / numButtonSlots;
    float buttonOffset = 1.0f / numButtonSlots;
    float paddingFraction = 0.25f;
    GLubyte alpha = 255;
    mButtonOverlay[0] = std::make_unique<ButtonOverlay>("SystemData/Menu/Button1.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, 1.0f, 1.0f - 1.0f * buttonOffset, alpha, true, true);
    mButtonOverlay[1] = std::make_unique<ButtonOverlay>("SystemData/Menu/Button2.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP, 1.0f, 1.0f - 2.0f * buttonOffset, alpha, true, true);
    mButtonOverlay[2] = std::make_unique<ButtonOverlay>("SystemData/Menu/Button3.png", buttonSize, paddingFraction,
        ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_TOP, 0.0f, 1.0f - 1.0f * buttonOffset, alpha, true, true);
}

//======================================================================================================================
HumanController::~HumanController()
{
    TRACE_METHOD_ONLY(ONCE_2);
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_CONTROL);
    RenderManager::GetInstance().UnregisterRenderOverlayObject(this, 0);

    mButtonOverlay[0].reset();
    mButtonOverlay[1].reset();
    mButtonOverlay[2].reset();
}

//======================================================================================================================
void HumanController::UpdateScreenSticks(float deltaTime)
{
    const ControllerSettings::ControlSetting* controlSettings = 
        mGameSettings.mControllerSettings.GetControlSettings();
    const Options& options = mGameSettings.mOptions;

    bool usingStaggered = mGameSettings.mOptions.mControllerStaggered;

    uint32 displayWidth = IwGxGetDisplayWidth(); 
    uint32 displayHeight = IwGxGetDisplayHeight(); 

    float xSize = options.mControllerSize;
    float ySize = options.mControllerSize;
    if (options.mControllerSquare)
    {
        if (displayWidth > displayHeight)
            xSize = (ySize * displayHeight) / displayWidth;
        else
            ySize = (xSize * displayWidth) / displayHeight;
    }
    mControllerSizeX = 0.5f * xSize * displayWidth;
    mControllerSizeY = 0.5f * ySize * displayHeight;

    {
        mLeftControllerPosX = xSize * 0.25f + options.mControllerHorOffset * (0.5f - xSize * 0.5f);
        mRightControllerPosX = 1.0f - mLeftControllerPosX;
    }

    {
        mRightControllerPosY = ySize * 0.25f + options.mControllerVerOffset * (0.5f - ySize * 0.5f);
        if (usingStaggered)
        {
            mLeftControllerPosY = 1.0f - mRightControllerPosY;
        }
        else
        {
            mLeftControllerPosY = mRightControllerPosY;
        }
    }

    float rightX = 0.0f;
    float rightY = 0.0f;
    float leftX = 0.0f;
    float leftY = 0.0f;
    float midX = 0.0f;
    float midY = 0.0f;

    float touchX0 = -1.0f;
    float touchY0 = -1.0f;
    float touchX1 = 2.0f;
    float touchY1 = 2.0f;
    bool leftTouched = false;
    bool rightTouched = false;
    bool midTouched = false;

    s3ePointerState touchState0 = s3ePointerGetTouchState(0);
    s3ePointerState touchState1 = s3ePointerGetTouchState(1);

    int numTouches = 0;
    if (touchState0 == S3E_POINTER_STATE_DOWN)
    {
        touchX0 = ((float) s3ePointerGetTouchX(0)) / displayWidth;
        touchY0 = 1.0f - ((float) s3ePointerGetTouchY(0)) / displayHeight;  
        ++numTouches;
    }

    if (touchState1 == S3E_POINTER_STATE_DOWN)
    {
        touchX1 = ((float) s3ePointerGetTouchX(1)) / displayWidth;
        touchY1 = 1.0f - ((float) s3ePointerGetTouchY(1)) / displayHeight;  
        ++numTouches;
    }

    if (usingStaggered && numTouches == 2)
    {
        // Multitouch doesn't give unique coordinates. Assume that the rightmost touch is supposed to be lower down
        if (
            (touchX0 > touchX1 && touchY0 > touchY1) ||
            (touchX1 > touchX0 && touchY1 > touchY0)
            )
        {
            std::swap(touchY0, touchY1);
        }
    }

    float trimHalfWidth = options.mControllerTrimSize * 0.5f;

    // Not sure if this handles when you start with touch 0, then press touch 1, 
    // if you lift touch 0 does this leave touch 1 still pressed? I expect so...
    if (touchX0 > 0.5f+trimHalfWidth || touchX1 < 0.5f-trimHalfWidth)
    {
        rightX = touchX0;
        rightY = touchY0;
        rightTouched = touchState0 == S3E_POINTER_STATE_DOWN;
        leftX = touchX1;
        leftY = touchY1;
        leftTouched = touchState1 == S3E_POINTER_STATE_DOWN;
    }
    else if (
        (touchX1 > 0.5f+trimHalfWidth && touchX1 <= 1.0f) ||
        (touchX0 < 0.5f-trimHalfWidth && touchX0 >= 0.0f)
        )
    {
        rightX = touchX1;
        rightY = touchY1;
        rightTouched = touchState1 == S3E_POINTER_STATE_DOWN;
        leftX = touchX0;
        leftY = touchY0;
        leftTouched = touchState0 == S3E_POINTER_STATE_DOWN;
    }

    if (options.mControllerEnableTrim)
    {
        if (touchState0 == S3E_POINTER_STATE_DOWN && touchX0 > 0.5f-trimHalfWidth && touchX0 < 0.5f+trimHalfWidth)
        {
            midX = touchX0;
            midY = touchY0;
            midTouched = true;
        }
        else if (touchState1 == S3E_POINTER_STATE_DOWN && touchX1 > 0.5f-trimHalfWidth && touchX1 < 0.5f+trimHalfWidth)
        {
            midX = touchX1;
            midY = touchY1;
            midTouched = true;
        }
    }

    /// Ignore presses high up on the right hand side as they happen when buttons are pressed
    if (rightTouched && rightY > 0.6f)
        rightTouched = false;

    if (usingStaggered)
    {
        if (leftTouched && leftY < 0.4f)
            leftTouched = false;
    }
    else
    {
        if (leftTouched && leftY > 0.6f)
            leftTouched = false;
    }

    if (rightTouched)
    {
        if (mGameSettings.mOptions.mControllerUseAbsolutePosition)
        {
            mRightInitialTouchPosX = mRightControllerPosX;
            mRightInitialTouchPosY = mRightControllerPosY;
        }
        else if (!mRightWasTouched)
        {
            mRightInitialTouchPosX = rightX - mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)] * (xSize * 0.25f);
            mRightInitialTouchPosY = rightY - mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)] * (ySize * 0.25f);
        }
        mRightWasTouched = true;

        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)] = (rightX - mRightInitialTouchPosX) / (xSize * 0.25f);
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)] = (rightY - mRightInitialTouchPosY) / (ySize * 0.25f); 
    }
    else
    {
        mRightWasTouched = false;
        if (controlSettings[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)].mAutoCentre)
        {
            mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)] = ExponentialApproach(
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)], 
                0.0f, 
                deltaTime, mGameSettings.mControllerSettings.mControllerStickDecayTime);
        }
        if (controlSettings[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)].mAutoCentre)
        {
            mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)] = ExponentialApproach(
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)], 
                0.0f, 
                deltaTime, mGameSettings.mControllerSettings.mControllerStickDecayTime);
        }
    }

    if (leftTouched)
    {
        if (mGameSettings.mOptions.mControllerUseAbsolutePosition)
        {
            mLeftInitialTouchPosX = mLeftControllerPosX;
            mLeftInitialTouchPosY = mLeftControllerPosY;
        }
        else if (!mLeftWasTouched)
        {
            mLeftInitialTouchPosX = leftX - mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)] * (xSize * 0.25f);
            mLeftInitialTouchPosY = leftY - mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)] * (ySize * 0.25f);
        }
        mLeftWasTouched = true;
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)] = (leftX - mLeftInitialTouchPosX) / (xSize * 0.25f);
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)] = (leftY - mLeftInitialTouchPosY) / (ySize * 0.25f);
    }
    else
    {
        mLeftWasTouched = false;
        if (controlSettings[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)].mAutoCentre)
        {
            mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)] = ExponentialApproach(
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)], 
                0.0f, 
                deltaTime, mGameSettings.mControllerSettings.mControllerStickDecayTime);
        }
        if (controlSettings[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)].mAutoCentre)
        {
            mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)] = ExponentialApproach(
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)], 
                0.0f, 
                deltaTime, mGameSettings.mControllerSettings.mControllerStickDecayTime);
        }
    }

    if (midTouched)
    {
        float trim = (midY - mRightControllerPosY) / (ySize * 0.25f);
        if (trim >= -1.2f && trim <= 1.2f)
            mElevatorTrim = ClampToRange(trim, -1.0f, 1.0f);
        float delta = 0.1f;
        mElevatorTrim = delta * (int) (mElevatorTrim / delta);
    }

    // Buttons
    for (uint32_t iButton = 0 ; iButton != NUM_BUTTONS ; ++iButton)
    {
        float& control = mInputControls[ControllerSettings::CONTROLLER_BUTTON0 + iButton];
        if (mButtonOverlay[iButton]->IsPressed())
        {
            if (control > 0.0f)
                control = -1.0f;
            else 
                control = 1.0f;
        }
        else
        {
            if (control > 0.0f)
                control = 1.0f;
            else 
                control = -1.0f;
        }
    }
}

//======================================================================================================================
void HumanController::UpdateAccelerometer(float deltaTime)
{
#if 0
    if (s3eAccelerometerGetInt(S3E_ACCELEROMETER_AVAILABLE) <= 0)
        return;

    bool accelerometerEnabled = false;
    for (int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    {
        accelerometerEnabled |= mGameSettings.mControllerSettings.mControlPerChannel[i] == ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL;
        accelerometerEnabled |= mGameSettings.mControllerSettings.mControlPerChannel[i] == ControllerSettings::CONTROLLER_ACCEL_VERTICAL;
    }
    if (accelerometerEnabled)
    {
        s3eAccelerometerStart();

        int32 iX = s3eAccelerometerGetX();
        int32 iY = s3eAccelerometerGetY();
        int32 iZ = s3eAccelerometerGetZ();

        // Y axis points up the screen. X axis points across screen to right. Z axis points out of the screen
        // Want orientation of device in frame where X is left, Y is fwd, Z is up.
        Vector3 zAxis((float) -iX, (float) -iY, (float) -iZ);
        zAxis.Normalise();

        float pitchSensitivity = 1 + mGameSettings.mControllerSettings.mControllerAccelerometerYSensitivity * 4.0f;
        float rollSensitivity = 1 + mGameSettings.mControllerSettings.mControllerAccelerometerXSensitivity * 4.0f;;

        // Angle is the offset from flat, so 90 would have neutral with the device right out in front,
        // in a vertical mClipPlanes. 0 would have it neutral with it in a horizontal plane.
        float angle = DegreesToRadians(mGameSettings.mControllerSettings.mControllerAccelerometerOffsetAngle);
        Quat q(Vector3(-1,0,0), -angle);
        Vector3 rotatedZAxis = q.RotateVector(zAxis);
        mInputControls[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = -rotatedZAxis.x * rollSensitivity;
        mInputControls[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = -rotatedZAxis.y * pitchSensitivity;
    }
    else
    {
        s3eAccelerometerStop();
        mInputControls[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = 0.0f;
        mInputControls[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = 0.0f;
    }
#endif

    // Accelerometer not available on desktop - would need SDL2 sensor API for mobile
    mInputControls[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = 0.0f;
    mInputControls[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = 0.0f;
}

//======================================================================================================================
void HumanController::UpdateKeyboard(float deltaTime)
{
    float controlRate = 2.0f;
    float controlRateZero = 4.0f;
    float delta = controlRate * deltaTime;
    float deltaZero = controlRateZero * deltaTime;

    for (int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    {
        if (mGameSettings.mControllerSettings.mControlPerChannel[i] == ControllerSettings::CONTROLLER_ARROW_HORIZONTAL)
        {
            float& input = mInputControls[ControllerSettings::CONTROLLER_ARROW_HORIZONTAL];
            if (Input::GetInstance().GetKeyState(SDLK_LEFT) & KEY_STATE_DOWN)
                input -= delta;
            else if (Input::GetInstance().GetKeyState(SDLK_RIGHT) & KEY_STATE_DOWN)
                input += delta;
            else
            {
                if (input > deltaZero)
                    input -= deltaZero;
                else if (input < -deltaZero)
                    input += deltaZero;
                else
                    input = 0.0f;
            }
        }
        else if (mGameSettings.mControllerSettings.mControlPerChannel[i] == ControllerSettings::CONTROLLER_ARROW_VERTICAL)
        {
            float& input = mInputControls[ControllerSettings::CONTROLLER_ARROW_VERTICAL];
            if (Input::GetInstance().GetKeyState(SDLK_DOWN) & KEY_STATE_DOWN)
                input -= delta;
            else if (Input::GetInstance().GetKeyState(SDLK_UP) & KEY_STATE_DOWN)
                input += delta;
            else
            {
                if (input > deltaZero)
                    input -= deltaZero;
                else if (input < -deltaZero)
                    input += deltaZero;
                else
                    input = 0.0f;
            }
        }
    }

    for (uint32_t iButton = 0 ; iButton != NUM_BUTTONS ; ++iButton)
    {
        if (Input::GetInstance().GetKeyState((SDL_Keycode)(SDLK_1 + iButton)) & KEY_STATE_PRESSED)
        {
            float& control = mInputControls[ControllerSettings::CONTROLLER_BUTTON0 + iButton];
            if (control > 0.0f)
                control = -1.0f;
            else 
                control = 1.0f;
        }
    }
}

//======================================================================================================================
void HumanController::UpdateJoystick(float deltaTime)
{
    if (!mGameSettings.mJoystickSettings.mEnableJoystick || mGameSettings.mOptions.mFrameworkSettings.isIOS())
        return;

    const JoystickSettings& js = mGameSettings.mJoystickSettings;
    const Options& options = mGameSettings.mOptions;

    JoystickData joystick;
    if (S3E_RESULT_SUCCESS != GetJoystickStatus(joystick, mGameSettings.mOptions.mJoystickID))
        return;

    //TRACE_FILE_IF(ONCE_1) TRACE("left = %d, %d Right = %d, %d", joystick.sThumbLX, joystick.sThumbLY, joystick.sThumbRX, joystick.sThumbRY);

    bool controlledByJoystick[ControllerSettings::CONTROLLER_NUM_CONTROLS];
    for (int i = 0 ; i != ControllerSettings::CONTROLLER_NUM_CONTROLS ; ++i)
        controlledByJoystick[i] = false;

    for (int i = 0 ; i != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++i)
    {
        const JoystickSettings::JoystickAnalogueOverride& j = js.mJoystickAnalogueOverrides[i];
        if (j.mControl != ControllerSettings::CONTROLLER_NUM_CONTROLS)
        {
            float v = -1.0f + 2.0f * joystick.mAnalogueInputs[i] / 65535.0f;
            v += j.mOffset;
            if (v > -j.mDeadZone && v < j.mDeadZone)
                v = 0.0f;
            else if (v > j.mDeadZone)
                v = (v - j.mDeadZone) / (1.0f - j.mDeadZone);
            else
                v = (v + j.mDeadZone) / (1.0f - j.mDeadZone);
            float inputControl = v > 0.0f ? v * j.mScalePositive : v * j.mScaleNegative;
            mInputControls[j.mControl] = inputControl;
            controlledByJoystick[j.mControl] = true;
        }
    }

    // squarify controls if necessary
    if (js.mAdjustForCircularSticks)
    {
        for (int c = 0 ; c != 2 ; ++c)
        {
            int i1, i2;
            if (c == 0)
            {
                i1 = GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode);
                i2 = GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode);
            }
            else
            {
                i1 = GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode);
                i2 = GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode);
            }
            if (
                controlledByJoystick[i1] && 
                controlledByJoystick[i2]
            )
            {
                float& x = mInputControls[i1];
                float& y = mInputControls[i2];
                if (x*x + y*y > 0.0f)
                {
                    float f = sqrtf(x*x + y*y);
                    float d;
                    if (fabsf(x) > fabsf(y))
                        d = sqrtf(1.0f + Square(y/x));
                    else
                        d = sqrtf(1.0f + Square(x/y));

                    x *= d;
                    y *= d;
                }
            }
        }
    }
}

//======================================================================================================================
void HumanController::EntityUpdate(float deltaTime, int entityLevel)
{
    // Use the real time delta - will ensure we get a good update even when paused
    deltaTime = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
    const ControllerSettings::ControlSetting* controlSettings = 
        mGameSettings.mControllerSettings.GetControlSettings();
    const Options& options = mGameSettings.mOptions;

    UpdateScreenSticks(deltaTime);

    uint32 stickVisibleMask = GetStickVisibleMask(mGameSettings, mAeroplane);
    if (!(stickVisibleMask & STICK_RIGHT_HORIZONTAL))
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)] = 0.0f;
    if (!(stickVisibleMask & STICK_RIGHT_VERTICAL))
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)] = 0.0f;
    if (!(stickVisibleMask & STICK_LEFT_HORIZONTAL))
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)] = 0.0f;
    if (!(stickVisibleMask & STICK_LEFT_VERTICAL))
        mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)] = 0.0f;

    // Override the stick results with the accelerometer if desired
    UpdateAccelerometer(deltaTime);

    // Override the stick results with the keyboard if desired
    UpdateKeyboard(deltaTime);
    
    UpdateJoystick(deltaTime);

    // Constant inputs
    mInputControls[ControllerSettings::CONTROLLER_CONSTANT] = 0.0f;

    // Apply the trim etc modifiers
    for (size_t i = 0 ; i != ControllerSettings::CONTROLLER_NUM_CONTROLS ; ++i)
    {
        // Clamp inputs to range
        mInputControls[i] = ClampToRange(mInputControls[i], -1.0f, 1.0f);
        mProcessedInputControls[i] = mInputControls[i];
    }

    if (!mGameSettings.mOptions.mControllerBrakesForward && mGameSettings.mControllerSettings.mTreatThrottleAsBrakes)
        mProcessedInputControls[ControllerSettings::CONTROLLER_STICK_SPEED] *= -1.0f;

    for (size_t i = 0 ; i != ControllerSettings::CONTROLLER_NUM_CONTROLS ; ++i)
    {
        if (controlSettings[i].mClamp == ControllerSettings::CONTROL_CLAMP_POSITIVE)
            mProcessedInputControls[i] = ClampToRange(mProcessedInputControls[i], 0.0f, 1.0f);
        else if (controlSettings[i].mClamp == ControllerSettings::CONTROL_CLAMP_NEGATIVE)
            mProcessedInputControls[i] = ClampToRange(mProcessedInputControls[i], -1.0f, 0.0f);

        // Process them
        if (mProcessedInputControls[i] >= 0.0f)
            mProcessedInputControls[i] = powf(mProcessedInputControls[i], controlSettings[i].mExponential);
        else
            mProcessedInputControls[i] = -powf(-mProcessedInputControls[i], controlSettings[i].mExponential);
        mProcessedInputControls[i] *= controlSettings[i].mScale;
        mProcessedInputControls[i] += controlSettings[i].mTrim;
    }

    for (size_t i = 0 ; i != MAX_CHANNELS ; ++i)
    {
        if (mGameSettings.mControllerSettings.mControlPerChannel[i] != ControllerSettings::CONTROLLER_NUM_CONTROLS)
            mOutputControls[i] = mProcessedInputControls[mGameSettings.mControllerSettings.mControlPerChannel[i]];
        else
            mOutputControls[i] = 0.0f;
    }

    // Apply channel mixes
    float aileron = mOutputControls[Controller::CHANNEL_AILERONS];
    float elevator = mOutputControls[Controller::CHANNEL_ELEVATOR];
    float rudder = mOutputControls[Controller::CHANNEL_RUDDER];
    float brakes = mOutputControls[Controller::CHANNEL_THROTTLE];
    float flaps = mOutputControls[Controller::CHANNEL_AUX1];

    const ControllerSettings::Mix& mix = mGameSettings.mControllerSettings.GetCurrentMix();
    mOutputControls[Controller::CHANNEL_AUX1] += mix.mMixElevatorToFlaps * elevator;
    mOutputControls[Controller::CHANNEL_RUDDER] += mix.mMixAileronToRudder * aileron;
    mOutputControls[Controller::CHANNEL_ELEVATOR] += mix.mMixFlapsToElevator * flaps;
    mOutputControls[Controller::CHANNEL_ELEVATOR] += mix.mMixBrakesToElevator * brakes;
    mOutputControls[Controller::CHANNEL_ELEVATOR] += mix.mMixRudderToElevator * fabsf(rudder);
    mOutputControls[Controller::CHANNEL_AILERONS] += mix.mMixRudderToAileron * rudder;

    mOutputControls[Controller::CHANNEL_ELEVATOR] += mElevatorTrim * fabsf(mElevatorTrim) * 0.25f;

    // Update the on-screen buttons
    for (uint32_t iButton = 0 ; iButton != NUM_BUTTONS ; ++iButton)
    {
        mButtonOverlay[iButton]->Enable(true);
        float& control = mInputControls[ControllerSettings::CONTROLLER_BUTTON0 + iButton];
        if (control > 0.0f)
        {
            mButtonOverlay[iButton]->SetColour(255, 255, 255);
            mButtonOverlay[iButton]->SetAlpha(255);
            mButtonOverlay[iButton]->SetTextColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else
        {
            mButtonOverlay[iButton]->SetColour(128, 128, 128);
            mButtonOverlay[iButton]->SetAlpha(64);
            mButtonOverlay[iButton]->SetTextColour(Vector4(1.0f, 1.0f, 1.0f, 0.3f));
        }
    }

}

//======================================================================================================================
float HumanController::GetControl(Channel channel) const
{
    IwAssert(ROWLHOUSE, channel < MAX_CHANNELS);
    return mOutputControls[channel];
}

//======================================================================================================================
void HumanController::RenderStick(
    float xMid, float yMid, bool renderStickCross,
    float controlX, float controlY, 
    float controllerSizeX, float controllerSizeY, 
    float stickIndicatorSizeX, float stickIndicatorSizeY,
    const ControllerShader* controllerShader)
{
    float x0, x1, y0, y1;

    // midpoint of the button
    float x = xMid + controlX * controllerSizeX * 0.5f;
    float y = yMid + controlY * controllerSizeY * 0.5f;
    x0 = x - stickIndicatorSizeX * 0.5f;
    x1 = x + stickIndicatorSizeX * 0.5f;
    y0 = y - stickIndicatorSizeY * 0.5f;
    y1 = y + stickIndicatorSizeY * 0.5f;

    {
        GLfloat pts[] = {
            x0, y0, 0,
            x1, y0, 0,
            x1, y1, 0,
            x0, y1, 0,
        };

        // points
        glVertexAttribPointer(controllerShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    if (renderStickCross)
    {
        float leftX = xMid - 0.5f * controllerSizeX;
        float rightX = xMid + 0.5f * controllerSizeX;
        float topY = yMid + 0.5f * controllerSizeY;
        float bottomY = yMid - 0.5f * controllerSizeY;
        GLfloat pts[] = {
            leftX, y, 0,
            rightX, y, 0,
            x, topY, 0,
            x, bottomY, 0,
        };

        glVertexAttribPointer(controllerShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glDrawArrays(GL_LINES, 0, 4);
    }
}

//======================================================================================================================
void HumanController::RenderController(
    float xMid, float yMid, 
    float controllerSizeX, float controllerSizeY,
    Options::ControllerStyle style,
    const ControllerShader* controllerShader)
{
    float x0, x1, y0, y1;

    x0 = xMid - controllerSizeX * 0.5f;
    x1 = xMid + controllerSizeX * 0.5f;
    y0 = yMid - controllerSizeY * 0.5f;
    y1 = yMid + controllerSizeY * 0.5f;

    if (
        style == Options::CONTROLLER_STYLE_BOX ||
        style == Options::CONTROLLER_STYLE_CROSS_AND_BOX
        )
    {
        GLfloat pts[] = {
            x0, y0, 0,
            x1, y0, 0,
            x1, y1, 0,
            x0, y1, 0,
        };
        glVertexAttribPointer(controllerShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    if (
        style == Options::CONTROLLER_STYLE_CROSS ||
        style == Options::CONTROLLER_STYLE_CROSS_AND_BOX
        )
    {
        GLfloat pts[] = {
            x0, yMid, 0,
            x1, yMid, 0,
            xMid, y0, 0,
            xMid, y1, 0,
        };

        glVertexAttribPointer(controllerShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glDrawArrays(GL_LINES, 0, 4);
    }
}

//======================================================================================================================
void HumanController::RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig)
{
    const Options& options = mGameSettings.mOptions;

    // Hide sticks and buttons when UI is hidden
    if (!PicaSim::GetInstance().GetShowUI())
    {
        mButtonOverlay[0]->Enable(false);
        mButtonOverlay[1]->Enable(false);
        mButtonOverlay[2]->Enable(false);
        return;
    }

    uint32 stickVisibleMask = GetStickVisibleMask(mGameSettings, mAeroplane);

    if (!stickVisibleMask)
        return;

    DisableDepthMask disableDepthMask;
    DisableDepthTest disableDepthTest;
    EnableBlend enableBlend;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const ControllerShader* controllerShader = (ControllerShader*) ShaderManager::GetInstance().GetShader(SHADER_CONTROLLER);

    controllerShader->Use();
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Ensure no VBO bound before client-side arrays
    glEnableVertexAttribArray(controllerShader->a_position);
    esSetModelViewProjectionMatrix(controllerShader->u_mvpMatrix);

    mButtonOverlay[0]->Enable(stickVisibleMask & BUTTON_0 ? true : false);
    mButtonOverlay[1]->Enable(stickVisibleMask & BUTTON_1 ? true : false);
    mButtonOverlay[2]->Enable(stickVisibleMask & BUTTON_2 ? true : false);

    if (mAeroplane)
    {
        const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
        for (uint32_t iButton = 0 ; iButton != 2 ; ++iButton)
        {
            if (!as.mShowButton[iButton])
                mButtonOverlay[iButton]->Enable(false);
        }
    }

    float s = mStickIndicatorSize * (displayConfig.mWidth + displayConfig.mHeight) * 0.5f;

    GLfloat stickColour[] = {1.0f, 1.0f, 1.0f, options.mControllerStickAlpha/255.0f};
    GLfloat controllerColour[] = {0.0f, 0.0f, 0.0f, options.mControllerAlpha/255.0f};

    if (controllerColour[3])
    {
        glUniform4fv(controllerShader->u_colour, 1, controllerColour);
        if (stickVisibleMask & STICK_RIGHT)
            RenderController(displayConfig.mLeft + mRightControllerPosX * displayConfig.mWidth,
                displayConfig.mBottom + mRightControllerPosY * displayConfig.mHeight,
                mControllerSizeX, mControllerSizeY, mGameSettings.mOptions.mControllerStyle, controllerShader);
        if (stickVisibleMask & STICK_LEFT)
            RenderController(displayConfig.mLeft + mLeftControllerPosX * displayConfig.mWidth,
                displayConfig.mBottom + mLeftControllerPosY * displayConfig.mHeight,
                mControllerSizeX, mControllerSizeY, mGameSettings.mOptions.mControllerStyle, controllerShader);
    }
    if (stickColour[3])
    {
        glUniform4fv(controllerShader->u_colour, 1, stickColour);
        if (stickVisibleMask & STICK_RIGHT)
        {
            RenderStick(displayConfig.mLeft + mRightControllerPosX * displayConfig.mWidth,
                displayConfig.mBottom + mRightControllerPosY * displayConfig.mHeight, options.mControllerStickCross,
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_HORIZONTAL, options.mControllerMode)],
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_RIGHT_VERTICAL, options.mControllerMode)],
                mControllerSizeX, mControllerSizeY, s, s, controllerShader);
        }
        if (stickVisibleMask & STICK_LEFT)
        {
            RenderStick(displayConfig.mLeft + mLeftControllerPosX * displayConfig.mWidth,
                displayConfig.mBottom + mLeftControllerPosY * displayConfig.mHeight, options.mControllerStickCross,
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_HORIZONTAL, options.mControllerMode)],
                mInputControls[GetControlForStick(ControllerSettings::CONTROLLER_LEFT_VERTICAL, options.mControllerMode)],
                mControllerSizeX, mControllerSizeY, s, s, controllerShader);
        }
    }

    if (options.mControllerEnableTrim)
    {
        GLfloat trimColour[] = {0.5f, 0.5f, 0.5f, 0.3f};

        glUniform4fv(controllerShader->u_colour, 1, trimColour);
        RenderController(displayConfig.mLeft + 0.5f * displayConfig.mWidth, displayConfig.mBottom + mRightControllerPosY * displayConfig.mHeight, 
            options.mControllerTrimSize * displayConfig.mWidth, mControllerSizeY, Options::CONTROLLER_STYLE_CROSS_AND_BOX, controllerShader);
        RenderStick(displayConfig.mLeft + 0.5f * displayConfig.mWidth, displayConfig.mBottom + mRightControllerPosY * displayConfig.mHeight, false,
            0.0f, mElevatorTrim, options.mControllerTrimSize * displayConfig.mWidth, mControllerSizeY, options.mControllerTrimSize * displayConfig.mWidth, s, controllerShader);
    }

    glDisableVertexAttribArray(controllerShader->a_position);
}

//======================================================================================================================
void HumanController::SetInputControl(int control, float value)
{
    IwAssert(ROWLHOUSE, control >= 0);
    IwAssert(ROWLHOUSE, control < ControllerSettings::CONTROLLER_NUM_CONTROLS);
    mInputControls[control] = value;
}

//======================================================================================================================
float HumanController::GetInputControl(int control) const
{
    IwAssert(ROWLHOUSE, control >= 0);
    IwAssert(ROWLHOUSE, control < ControllerSettings::CONTROLLER_NUM_CONTROLS);
    return mInputControls[control];
}
