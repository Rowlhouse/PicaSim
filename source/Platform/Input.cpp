#include "Input.h"
#include "Window.h"
#include <cstring>
#include <cstdio>

// Dear ImGui for event processing
#include "imgui.h"
#include "imgui_impl_sdl2.h"

// Global input instance
Input* gInput = nullptr;

Input::Input()
    : mMouseX(0), mMouseY(0)
    , mMousePrevX(0), mMousePrevY(0)
    , mMouseDeltaX(0), mMouseDeltaY(0)
    , mMouseWheelDelta(0)
    , mTouchCount(0)
    , mTouchAvailable(false)
    , mStatesDirty(true)
    , mAccelerometer(nullptr)
    , mAccelerometerAvailable(false)
    , mAccelX(0), mAccelY(0), mAccelZ(1.0f)
    , mKeyboardCallback(nullptr)
    , mKeyboardUserData(nullptr)
    , mPointerButtonCallback(nullptr)
    , mPointerButtonUserData(nullptr)
    , mPointerMotionCallback(nullptr)
    , mPointerMotionUserData(nullptr)
    , mTouchCallback(nullptr)
    , mTouchUserData(nullptr)
    , mTouchMotionCallback(nullptr)
    , mTouchMotionUserData(nullptr)
{
    memset(mKeyStates, 0, sizeof(mKeyStates));
    memset(mKeyDown, 0, sizeof(mKeyDown));
    memset(mKeyDownPrev, 0, sizeof(mKeyDownPrev));
    memset(mMouseButtonStates, 0, sizeof(mMouseButtonStates));
    memset(mMouseButtonDown, 0, sizeof(mMouseButtonDown));
    memset(mMouseButtonDownPrev, 0, sizeof(mMouseButtonDownPrev));
    memset(mTouches, 0, sizeof(mTouches));
}

Input::~Input()
{
    Shutdown();
}

Input& Input::GetInstance()
{
    static Input instance;
    return instance;
}

bool Input::Init()
{
    // Initialize SDL subsystems if needed
    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0)
    {
        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
        {
            fprintf(stderr, "SDL_InitSubSystem(GAMECONTROLLER) failed: %s\n", SDL_GetError());
        }
    }

    // Initialize joystick subsystem for R/C transmitters and other non-gamepad devices
    if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0)
    {
        if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
        {
            fprintf(stderr, "SDL_InitSubSystem(JOYSTICK) failed: %s\n", SDL_GetError());
        }
    }

    // Check for touch support
    mTouchAvailable = SDL_GetNumTouchDevices() > 0;

    // Initialize accelerometer on mobile platforms
#if defined(PS_PLATFORM_ANDROID) || defined(PS_PLATFORM_IOS)
    if (SDL_WasInit(SDL_INIT_SENSOR) == 0)
    {
        SDL_InitSubSystem(SDL_INIT_SENSOR);
    }

    // Look for accelerometer
    int numSensors = SDL_NumSensors();
    for (int i = 0; i < numSensors; ++i)
    {
        if (SDL_SensorGetDeviceType(i) == SDL_SENSOR_ACCEL)
        {
            mAccelerometer = SDL_SensorOpen(i);
            if (mAccelerometer)
            {
                mAccelerometerAvailable = true;
                break;
            }
        }
    }
#endif

    // Open any connected gamepads and joysticks
    OpenGamepads();
    OpenJoysticks();

    gInput = this;
    return true;
}

void Input::Shutdown()
{
    CloseGamepads();
    CloseJoysticks();

    if (mAccelerometer)
    {
        SDL_SensorClose(mAccelerometer);
        mAccelerometer = nullptr;
    }

    gInput = nullptr;
}

void Input::ClearPressedStates()
{
    // Clear edge detection states by syncing prev with current
    // This prevents clicks from "bleeding through" screen transitions
    memcpy(mKeyDownPrev, mKeyDown, sizeof(mKeyDown));
    memcpy(mMouseButtonDownPrev, mMouseButtonDown, sizeof(mMouseButtonDown));

    // Recompute states (will now show DOWN instead of PRESSED for held buttons)
    UpdateKeyStates();
    UpdateMouseStates();

    // Clear touch pressed states
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
        if (mTouches[i].state == POINTER_STATE_PRESSED)
            mTouches[i].state = POINTER_STATE_DOWN;
    }

    // Prevent immediate recomputation
    mStatesDirty = false;
}

void Input::BeginFrame()
{
    // Save current state as previous for edge detection
    // This must happen BEFORE processing events so that new key presses this frame
    // are detected as transitions from the previous frame's state
    memcpy(mKeyDownPrev, mKeyDown, sizeof(mKeyDown));
    memcpy(mMouseButtonDownPrev, mMouseButtonDown, sizeof(mMouseButtonDown));

    // Mark states as dirty so they'll be recomputed after events are processed
    mStatesDirty = true;
}

void Input::Update()
{
    // Only update states once per frame
    // This prevents PRESSED from transitioning to DOWN when Update() is called multiple times
    if (mStatesDirty)
    {
        // Update key/mouse/touch states (uses current vs previous for edge detection)
        // prev was captured in BeginFrame(), current was updated by ProcessEvent()
        UpdateKeyStates();
        UpdateMouseStates();
        UpdateTouchStates();

        mStatesDirty = false;
    }

    // Update mouse delta (always update for smooth mouse movement)
    mMouseDeltaX = mMouseX - mMousePrevX;
    mMouseDeltaY = mMouseY - mMousePrevY;
    mMousePrevX = mMouseX;
    mMousePrevY = mMouseY;

    // Reset wheel delta (it's per-frame)
    mMouseWheelDelta = 0;

    // Update accelerometer
    if (mAccelerometer)
    {
        float data[3];
        if (SDL_SensorGetData(mAccelerometer, data, 3) == 0)
        {
            mAccelX = data[0] / 9.81f;  // Convert to g
            mAccelY = data[1] / 9.81f;
            mAccelZ = data[2] / 9.81f;
        }
    }
}

void Input::ProcessEvent(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        {
            SDL_Scancode scancode = event.key.keysym.scancode;
            if (scancode >= 0 && scancode < MAX_KEYS)
            {
                mKeyDown[scancode] = true;
            }
            if (mKeyboardCallback)
            {
                mKeyboardCallback((int)event.key.keysym.sym, 1, mKeyboardUserData);
            }
        }
        break;

    case SDL_KEYUP:
        {
            SDL_Scancode scancode = event.key.keysym.scancode;
            if (scancode >= 0 && scancode < MAX_KEYS)
            {
                mKeyDown[scancode] = false;
            }
            if (mKeyboardCallback)
            {
                mKeyboardCallback((int)event.key.keysym.sym, 0, mKeyboardUserData);
            }
        }
        break;

    case SDL_MOUSEMOTION:
        mMouseX = event.motion.x;
        mMouseY = event.motion.y;
        if (mPointerMotionCallback)
        {
            mPointerMotionCallback(mMouseX, mMouseY, mPointerMotionUserData);
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        {
            int button = -1;
            if (event.button.button == SDL_BUTTON_LEFT) button = MOUSE_BUTTON_LEFT;
            else if (event.button.button == SDL_BUTTON_MIDDLE) button = MOUSE_BUTTON_MIDDLE;
            else if (event.button.button == SDL_BUTTON_RIGHT) button = MOUSE_BUTTON_RIGHT;

            if (button >= 0 && button < MOUSE_BUTTON_COUNT)
            {
                mMouseButtonDown[button] = true;
            }
            mMouseX = event.button.x;
            mMouseY = event.button.y;

            if (mPointerButtonCallback && button >= 0)
            {
                mPointerButtonCallback(button, 1, mMouseX, mMouseY, mPointerButtonUserData);
            }
        }
        break;

    case SDL_MOUSEBUTTONUP:
        {
            int button = -1;
            if (event.button.button == SDL_BUTTON_LEFT) button = MOUSE_BUTTON_LEFT;
            else if (event.button.button == SDL_BUTTON_MIDDLE) button = MOUSE_BUTTON_MIDDLE;
            else if (event.button.button == SDL_BUTTON_RIGHT) button = MOUSE_BUTTON_RIGHT;

            if (button >= 0 && button < MOUSE_BUTTON_COUNT)
            {
                mMouseButtonDown[button] = false;
            }
            mMouseX = event.button.x;
            mMouseY = event.button.y;

            if (mPointerButtonCallback && button >= 0)
            {
                mPointerButtonCallback(button, 0, mMouseX, mMouseY, mPointerButtonUserData);
            }
        }
        break;

    case SDL_MOUSEWHEEL:
        mMouseWheelDelta = event.wheel.y;
        break;

    case SDL_FINGERDOWN:
        {
            int index = FindFreeTouchIndex();
            if (index >= 0)
            {
                mTouches[index].id = (int)event.tfinger.fingerId;
                mTouches[index].x = event.tfinger.x * Platform::GetDisplayWidth();
                mTouches[index].y = event.tfinger.y * Platform::GetDisplayHeight();
                mTouches[index].pressure = event.tfinger.pressure;
                mTouches[index].state = POINTER_STATE_PRESSED;
                mTouches[index].active = true;
                mTouchCount++;

                if (mTouchCallback)
                {
                    mTouchCallback(mTouches[index].id, 1,
                        (int)mTouches[index].x, (int)mTouches[index].y, mTouchUserData);
                }
            }
        }
        break;

    case SDL_FINGERUP:
        {
            int index = FindTouchIndex((int)event.tfinger.fingerId);
            if (index >= 0)
            {
                mTouches[index].x = event.tfinger.x * Platform::GetDisplayWidth();
                mTouches[index].y = event.tfinger.y * Platform::GetDisplayHeight();
                mTouches[index].state = POINTER_STATE_RELEASED;

                if (mTouchCallback)
                {
                    mTouchCallback(mTouches[index].id, 0,
                        (int)mTouches[index].x, (int)mTouches[index].y, mTouchUserData);
                }

                mTouches[index].active = false;
                mTouchCount--;
            }
        }
        break;

    case SDL_FINGERMOTION:
        {
            int index = FindTouchIndex((int)event.tfinger.fingerId);
            if (index >= 0)
            {
                mTouches[index].x = event.tfinger.x * Platform::GetDisplayWidth();
                mTouches[index].y = event.tfinger.y * Platform::GetDisplayHeight();
                mTouches[index].pressure = event.tfinger.pressure;

                if (mTouchMotionCallback)
                {
                    mTouchMotionCallback(mTouches[index].id,
                        (int)mTouches[index].x, (int)mTouches[index].y, mTouchMotionUserData);
                }
            }
        }
        break;

    case SDL_CONTROLLERDEVICEADDED:
        {
            SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which);
            if (controller)
            {
                mGamepads.push_back(controller);
                printf("Gamepad connected: %s\n", SDL_GameControllerName(controller));
            }
        }
        break;

    case SDL_CONTROLLERDEVICEREMOVED:
        {
            for (auto it = mGamepads.begin(); it != mGamepads.end(); ++it)
            {
                if (SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(*it)) == event.cdevice.which)
                {
                    SDL_GameControllerClose(*it);
                    mGamepads.erase(it);
                    printf("Gamepad disconnected\n");
                    break;
                }
            }
        }
        break;

    case SDL_JOYDEVICEADDED:
        {
            int deviceIndex = event.jdevice.which;
            // Only handle if NOT a game controller (those are handled above)
            if (!SDL_IsGameController(deviceIndex))
            {
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (joystick)
                {
                    mJoysticks.push_back(joystick);
                    printf("Joystick connected: %s (axes=%d, buttons=%d, hats=%d)\n",
                        SDL_JoystickName(joystick),
                        SDL_JoystickNumAxes(joystick),
                        SDL_JoystickNumButtons(joystick),
                        SDL_JoystickNumHats(joystick));
                }
            }
        }
        break;

    case SDL_JOYDEVICEREMOVED:
        {
            SDL_JoystickID instanceId = event.jdevice.which;
            for (auto it = mJoysticks.begin(); it != mJoysticks.end(); ++it)
            {
                if (SDL_JoystickInstanceID(*it) == instanceId)
                {
                    printf("Joystick disconnected: %s\n", SDL_JoystickName(*it));
                    SDL_JoystickClose(*it);
                    mJoysticks.erase(it);
                    break;
                }
            }
        }
        break;
    }
}

void Input::UpdateKeyStates()
{
    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (mKeyDown[i] && !mKeyDownPrev[i])
            mKeyStates[i] = KEY_STATE_PRESSED;
        else if (!mKeyDown[i] && mKeyDownPrev[i])
            mKeyStates[i] = KEY_STATE_RELEASED;
        else if (mKeyDown[i])
            mKeyStates[i] = KEY_STATE_DOWN;
        else
            mKeyStates[i] = KEY_STATE_UP;
    }
}

void Input::UpdateMouseStates()
{
    for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
    {
        if (mMouseButtonDown[i] && !mMouseButtonDownPrev[i])
            mMouseButtonStates[i] = POINTER_STATE_PRESSED;
        else if (!mMouseButtonDown[i] && mMouseButtonDownPrev[i])
            mMouseButtonStates[i] = POINTER_STATE_RELEASED;
        else if (mMouseButtonDown[i])
            mMouseButtonStates[i] = POINTER_STATE_DOWN;
        else
            mMouseButtonStates[i] = POINTER_STATE_UP;
    }
}

void Input::UpdateTouchStates()
{
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
        if (mTouches[i].active)
        {
            if (mTouches[i].state == POINTER_STATE_PRESSED)
                mTouches[i].state = POINTER_STATE_DOWN;
        }
        else if (mTouches[i].state == POINTER_STATE_RELEASED)
        {
            mTouches[i].state = POINTER_STATE_UP;
        }
    }
}

KeyState Input::GetKeyState(SDL_Keycode key) const
{
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    if (scancode >= 0 && scancode < MAX_KEYS)
        return mKeyStates[scancode];
    return KEY_STATE_UP;
}

bool Input::IsKeyDown(SDL_Keycode key) const
{
    KeyState state = GetKeyState(key);
    return state == KEY_STATE_DOWN || state == KEY_STATE_PRESSED;
}

bool Input::IsKeyPressed(SDL_Keycode key) const
{
    return GetKeyState(key) == KEY_STATE_PRESSED;
}

bool Input::IsKeyReleased(SDL_Keycode key) const
{
    return GetKeyState(key) == KEY_STATE_RELEASED;
}

bool Input::IsShiftDown() const
{
    return (SDL_GetModState() & KMOD_SHIFT) != 0;
}

bool Input::IsCtrlDown() const
{
    return (SDL_GetModState() & KMOD_CTRL) != 0;
}

bool Input::IsAltDown() const
{
    return (SDL_GetModState() & KMOD_ALT) != 0;
}

void Input::SetKeyboardCallback(KeyboardCallback callback, void* userData)
{
    mKeyboardCallback = callback;
    mKeyboardUserData = userData;
}

PointerState Input::GetMouseButtonState(MouseButton button) const
{
    if (button >= 0 && button < MOUSE_BUTTON_COUNT)
        return mMouseButtonStates[button];
    return POINTER_STATE_UP;
}

bool Input::IsMouseButtonDown(MouseButton button) const
{
    PointerState state = GetMouseButtonState(button);
    return state == POINTER_STATE_DOWN || state == POINTER_STATE_PRESSED;
}

bool Input::IsMouseButtonPressed(MouseButton button) const
{
    return GetMouseButtonState(button) == POINTER_STATE_PRESSED;
}

bool Input::IsMouseButtonReleased(MouseButton button) const
{
    return GetMouseButtonState(button) == POINTER_STATE_RELEASED;
}

void Input::SetPointerButtonCallback(PointerButtonCallback callback, void* userData)
{
    mPointerButtonCallback = callback;
    mPointerButtonUserData = userData;
}

void Input::SetPointerMotionCallback(PointerMotionCallback callback, void* userData)
{
    mPointerMotionCallback = callback;
    mPointerMotionUserData = userData;
}

int Input::GetTouchCount() const
{
    return mTouchCount;
}

const TouchPoint* Input::GetTouch(int index) const
{
    int count = 0;
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
        if (mTouches[i].active)
        {
            if (count == index)
                return &mTouches[i];
            count++;
        }
    }
    return nullptr;
}

const TouchPoint* Input::GetTouchById(int touchId) const
{
    int index = FindTouchIndex(touchId);
    if (index >= 0)
        return &mTouches[index];
    return nullptr;
}

void Input::SetTouchCallback(TouchCallback callback, void* userData)
{
    mTouchCallback = callback;
    mTouchUserData = userData;
}

void Input::SetTouchMotionCallback(TouchMotionCallback callback, void* userData)
{
    mTouchMotionCallback = callback;
    mTouchMotionUserData = userData;
}

int Input::GetGamepadCount() const
{
    return (int)mGamepads.size();
}

bool Input::IsGamepadConnected(int index) const
{
    return index >= 0 && index < (int)mGamepads.size() && mGamepads[index] != nullptr;
}

float Input::GetGamepadAxis(int gamepadIndex, int axis) const
{
    if (!IsGamepadConnected(gamepadIndex))
        return 0.0f;

    SDL_GameControllerAxis sdlAxis = (SDL_GameControllerAxis)axis;
    int16_t value = SDL_GameControllerGetAxis(mGamepads[gamepadIndex], sdlAxis);
    return value / 32767.0f;
}

bool Input::IsGamepadButtonDown(int gamepadIndex, int button) const
{
    if (!IsGamepadConnected(gamepadIndex))
        return false;

    SDL_GameControllerButton sdlButton = (SDL_GameControllerButton)button;
    return SDL_GameControllerGetButton(mGamepads[gamepadIndex], sdlButton) != 0;
}

void Input::OpenGamepads()
{
    int numJoysticks = SDL_NumJoysticks();
    printf("OpenGamepads: SDL_NumJoysticks() = %d\n", numJoysticks);

    for (int i = 0; i < numJoysticks; ++i)
    {
        if (SDL_IsGameController(i))
        {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller)
            {
                mGamepads.push_back(controller);
                printf("Gamepad found: %s\n", SDL_GameControllerName(controller));
            }
        }
    }
}

void Input::CloseGamepads()
{
    for (SDL_GameController* controller : mGamepads)
    {
        if (controller)
            SDL_GameControllerClose(controller);
    }
    mGamepads.clear();
}

void Input::OpenJoysticks()
{
    // Open joysticks that are NOT recognized as game controllers
    // (e.g. R/C transmitters, flight sticks, etc.)
    int numJoysticks = SDL_NumJoysticks();
    printf("SDL_NumJoysticks() = %d\n", numJoysticks);

    for (int i = 0; i < numJoysticks; ++i)
    {
        const char* name = SDL_JoystickNameForIndex(i);
        bool isGameController = SDL_IsGameController(i);
        printf("  Device %d: '%s' - IsGameController=%s\n",
            i, name ? name : "(null)", isGameController ? "YES" : "NO");

        // Skip devices already opened as game controllers
        if (isGameController)
            continue;

        SDL_Joystick* joystick = SDL_JoystickOpen(i);
        if (joystick)
        {
            mJoysticks.push_back(joystick);
            printf("Joystick found: %s (axes=%d, buttons=%d, hats=%d)\n",
                SDL_JoystickName(joystick),
                SDL_JoystickNumAxes(joystick),
                SDL_JoystickNumButtons(joystick),
                SDL_JoystickNumHats(joystick));
        }
        else
        {
            printf("  Failed to open joystick %d: %s\n", i, SDL_GetError());
        }
    }
}

void Input::CloseJoysticks()
{
    for (SDL_Joystick* joystick : mJoysticks)
    {
        if (joystick)
            SDL_JoystickClose(joystick);
    }
    mJoysticks.clear();
}

int Input::GetJoystickCount() const
{
    return static_cast<int>(mJoysticks.size());
}

int Input::GetJoystickAxisCount(int index) const
{
    if (index >= 0 && index < static_cast<int>(mJoysticks.size()) && mJoysticks[index])
        return SDL_JoystickNumAxes(mJoysticks[index]);
    return 0;
}

int Input::GetJoystickButtonCount(int index) const
{
    if (index >= 0 && index < static_cast<int>(mJoysticks.size()) && mJoysticks[index])
        return SDL_JoystickNumButtons(mJoysticks[index]);
    return 0;
}

int Input::GetJoystickHatCount(int index) const
{
    if (index >= 0 && index < static_cast<int>(mJoysticks.size()) && mJoysticks[index])
        return SDL_JoystickNumHats(mJoysticks[index]);
    return 0;
}

float Input::GetJoystickAxis(int index, int axis) const
{
    if (index >= 0 && index < static_cast<int>(mJoysticks.size()) && mJoysticks[index])
    {
        int16_t value = SDL_JoystickGetAxis(mJoysticks[index], axis);
        return value / 32767.0f;
    }
    return 0.0f;
}

bool Input::IsJoystickButtonDown(int index, int button) const
{
    if (index >= 0 && index < static_cast<int>(mJoysticks.size()) && mJoysticks[index])
    {
        return SDL_JoystickGetButton(mJoysticks[index], button) != 0;
    }
    return false;
}

int Input::GetJoystickHat(int index, int hat) const
{
    if (index >= 0 && index < static_cast<int>(mJoysticks.size()) && mJoysticks[index])
    {
        Uint8 hatState = SDL_JoystickGetHat(mJoysticks[index], hat);
        // Convert SDL hat state to angle in hundredths of degrees
        switch (hatState)
        {
            case SDL_HAT_UP:        return 0;
            case SDL_HAT_RIGHTUP:   return 4500;
            case SDL_HAT_RIGHT:     return 9000;
            case SDL_HAT_RIGHTDOWN: return 13500;
            case SDL_HAT_DOWN:      return 18000;
            case SDL_HAT_LEFTDOWN:  return 22500;
            case SDL_HAT_LEFT:      return 27000;
            case SDL_HAT_LEFTUP:    return 31500;
            default:                return -1;  // Centered
        }
    }
    return -1;
}

int Input::FindTouchIndex(int touchId) const
{
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
        if (mTouches[i].active && mTouches[i].id == touchId)
            return i;
    }
    return -1;
}

int Input::FindFreeTouchIndex() const
{
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
        if (!mTouches[i].active)
            return i;
    }
    return -1;
}

//==============================================================================
// Marmalade s3ePointer compatibility functions
//==============================================================================

#include "S3ECompat.h"

int32 s3ePointerGetInt(s3ePointerProperty property)
{
    Input& input = Input::GetInstance();
    switch (property)
    {
    case S3E_POINTER_AVAILABLE:
        return S3E_TRUE;
    case S3E_POINTER_MULTI_TOUCH_AVAILABLE:
        return input.IsTouchAvailable() ? S3E_TRUE : S3E_FALSE;
    case S3E_POINTER_TYPE:
        // Return mouse type on desktop, touchscreen on mobile
#if defined(PS_PLATFORM_ANDROID) || defined(PS_PLATFORM_IOS)
        return S3E_POINTER_TYPE_TOUCHSCREEN;
#else
        return S3E_POINTER_TYPE_MOUSE;
#endif
    default:
        return 0;
    }
}

s3ePointerState s3ePointerGetTouchState(int32 touchIndex)
{
    Input& input = Input::GetInstance();

    // For touch index 0, also check mouse button (for desktop compatibility)
    if (touchIndex == 0)
    {
        PointerState mouseState = input.GetMouseButtonState(MOUSE_BUTTON_LEFT);
        if (mouseState == POINTER_STATE_DOWN)
            return S3E_POINTER_STATE_DOWN;
        if (mouseState == POINTER_STATE_PRESSED)
            return S3E_POINTER_STATE_PRESSED;
        if (mouseState == POINTER_STATE_RELEASED)
            return S3E_POINTER_STATE_RELEASED;
    }

    // Check actual touch
    if (touchIndex < input.GetTouchCount())
    {
        const TouchPoint* touch = input.GetTouch(touchIndex);
        if (touch && touch->active)
        {
            switch (touch->state)
            {
            case POINTER_STATE_DOWN: return S3E_POINTER_STATE_DOWN;
            case POINTER_STATE_PRESSED: return S3E_POINTER_STATE_PRESSED;
            case POINTER_STATE_RELEASED: return S3E_POINTER_STATE_RELEASED;
            default: return S3E_POINTER_STATE_UP;
            }
        }
    }

    return S3E_POINTER_STATE_UP;
}

int32 s3ePointerGetTouchX(int32 touchIndex)
{
    Input& input = Input::GetInstance();

    // For touch index 0, also return mouse position (for desktop compatibility)
    if (touchIndex == 0 && input.IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        return input.GetMouseX();
    }

    // Check actual touch
    if (touchIndex < input.GetTouchCount())
    {
        const TouchPoint* touch = input.GetTouch(touchIndex);
        if (touch && touch->active)
        {
            return (int32)touch->x;
        }
    }

    // Default to mouse position
    return input.GetMouseX();
}

int32 s3ePointerGetTouchY(int32 touchIndex)
{
    Input& input = Input::GetInstance();

    // For touch index 0, also return mouse position (for desktop compatibility)
    if (touchIndex == 0 && input.IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        return input.GetMouseY();
    }

    // Check actual touch
    if (touchIndex < input.GetTouchCount())
    {
        const TouchPoint* touch = input.GetTouch(touchIndex);
        if (touch && touch->active)
        {
            return (int32)touch->y;
        }
    }

    // Default to mouse position
    return input.GetMouseY();
}

//==============================================================================
// Marmalade s3eDevice compatibility functions
//==============================================================================

static bool gQuitRequested = false;

s3eBool CheckForQuitRequest()
{
    return gQuitRequested ? S3E_TRUE : S3E_FALSE;
}

void PollEvents()
{
    // Mark start of new input frame
    Input::GetInstance().BeginFrame();

    // Process all pending SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // Pass events to Dear ImGui first
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            gQuitRequested = true;
            break;
        case SDL_KEYDOWN:
            // F11 or Alt+Enter toggles fullscreen globally
            // F key also toggles fullscreen, but only when not typing in a text input field
            if (event.key.keysym.sym == SDLK_F11 ||
                (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT)) ||
                (event.key.keysym.sym == SDLK_f && !ImGui::GetIO().WantTextInput))
            {
                if (gWindow)
                    gWindow->SetFullscreen(!gWindow->IsFullscreen());
            }
            // Pass to input system
            Input::GetInstance().ProcessEvent(event);
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                // Update window size
                if (gWindow)
                {
                    gWindow->SetSize(event.window.data1, event.window.data2);
                }
            }
            // Fall through to pass to input system
        default:
            // Pass event to Input system
            Input::GetInstance().ProcessEvent(event);
            break;
        }
    }

    // Update input state
    Input::GetInstance().Update();
}

//==============================================================================
// Marmalade gamepad compatibility functions (from gamepad.h extension)
//==============================================================================

s3eBool gamepadAvailable()
{
    Input& input = Input::GetInstance();
    return (input.GetGamepadCount() > 0 || input.GetJoystickCount() > 0) ? S3E_TRUE : S3E_FALSE;
}

uint32 gamepadGetNumDevices()
{
    Input& input = Input::GetInstance();
    return static_cast<uint32>(input.GetGamepadCount() + input.GetJoystickCount());
}

uint32 gamepadGetDeviceId(uint32 index)
{
    // Return index as device ID for simplicity
    // Gamepads are indices 0 to gamepadCount-1, joysticks are gamepadCount onwards
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();
    int joystickCount = input.GetJoystickCount();

    if (index < static_cast<uint32>(gamepadCount + joystickCount))
        return index;
    return 0;
}

uint32 gamepadGetNumAxes(uint32 index)
{
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();

    if (index < static_cast<uint32>(gamepadCount))
    {
        // It's a gamepad - 6 axes (standard layout: 2 sticks + 2 triggers)
        return input.IsGamepadConnected(static_cast<int>(index)) ? 6 : 0;
    }
    else
    {
        // It's a raw joystick
        int joystickIndex = static_cast<int>(index) - gamepadCount;
        return static_cast<uint32>(input.GetJoystickAxisCount(joystickIndex));
    }
}

uint32 gamepadGetNumButtons(uint32 index)
{
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();

    if (index < static_cast<uint32>(gamepadCount))
    {
        // It's a gamepad - up to 15 buttons (standard layout)
        return input.IsGamepadConnected(static_cast<int>(index)) ? 15 : 0;
    }
    else
    {
        // It's a raw joystick
        int joystickIndex = static_cast<int>(index) - gamepadCount;
        return static_cast<uint32>(input.GetJoystickButtonCount(joystickIndex));
    }
}

uint32 gamepadGetButtons(uint32 index)
{
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();

    if (index < static_cast<uint32>(gamepadCount))
    {
        // It's a gamepad
        int idx = static_cast<int>(index);
        if (!input.IsGamepadConnected(idx))
            return 0;

        uint32 buttons = 0;
        // Map SDL_GameControllerButton to a bitmask
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_A)) buttons |= (1 << 0);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_B)) buttons |= (1 << 1);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_X)) buttons |= (1 << 2);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_Y)) buttons |= (1 << 3);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_BACK)) buttons |= (1 << 4);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_GUIDE)) buttons |= (1 << 5);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_START)) buttons |= (1 << 6);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_LEFTSTICK)) buttons |= (1 << 7);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_RIGHTSTICK)) buttons |= (1 << 8);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) buttons |= (1 << 9);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) buttons |= (1 << 10);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_UP)) buttons |= (1 << 11);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) buttons |= (1 << 12);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) buttons |= (1 << 13);
        if (input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) buttons |= (1 << 14);

        return buttons;
    }
    else
    {
        // It's a raw joystick
        int joystickIndex = static_cast<int>(index) - gamepadCount;
        int buttonCount = input.GetJoystickButtonCount(joystickIndex);

        uint32 buttons = 0;
        for (int i = 0; i < buttonCount && i < 32; ++i)
        {
            if (input.IsJoystickButtonDown(joystickIndex, i))
                buttons |= (1 << i);
        }

        return buttons;
    }
}

int32 gamepadGetAxis(uint32 index, uint32 axisIndex)
{
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();

    if (index < static_cast<uint32>(gamepadCount))
    {
        // It's a gamepad
        int idx = static_cast<int>(index);
        if (!input.IsGamepadConnected(idx))
            return 0;

        // Get axis value and convert from [-1,1] to [-4096,4096]
        float value = input.GetGamepadAxis(idx, static_cast<int>(axisIndex));
        return static_cast<int32>(value * 4096.0f);
    }
    else
    {
        // It's a raw joystick
        int joystickIndex = static_cast<int>(index) - gamepadCount;
        float value = input.GetJoystickAxis(joystickIndex, static_cast<int>(axisIndex));
        return static_cast<int32>(value * 4096.0f);
    }
}

uint32 gamepadIsPointOfViewAvailable(uint32 index)
{
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();

    if (index < static_cast<uint32>(gamepadCount))
    {
        // Gamepads emulate POV from D-pad buttons
        return input.IsGamepadConnected(static_cast<int>(index)) ? 1 : 0;
    }
    else
    {
        // Raw joysticks may have actual POV hats
        int joystickIndex = static_cast<int>(index) - gamepadCount;
        return input.GetJoystickHatCount(joystickIndex) > 0 ? 1 : 0;
    }
}

int32 gamepadGetPointOfViewAngle(uint32 index)
{
    Input& input = Input::GetInstance();
    int gamepadCount = input.GetGamepadCount();

    if (index < static_cast<uint32>(gamepadCount))
    {
        // It's a gamepad - emulate POV from D-pad buttons
        int idx = static_cast<int>(index);
        if (!input.IsGamepadConnected(idx))
            return -1; // -1 means centered/no direction

        // Calculate POV angle from D-pad buttons (in hundredths of degrees)
        bool up = input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_UP);
        bool down = input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        bool left = input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        bool right = input.IsGamepadButtonDown(idx, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

        if (up && !down && !left && !right) return 0;        // North
        if (up && !down && !left && right)  return 4500;     // NE
        if (!up && !down && !left && right) return 9000;     // East
        if (!up && down && !left && right)  return 13500;    // SE
        if (!up && down && !left && !right) return 18000;    // South
        if (!up && down && left && !right)  return 22500;    // SW
        if (!up && !down && left && !right) return 27000;    // West
        if (up && !down && left && !right)  return 31500;    // NW

        return -1; // Centered
    }
    else
    {
        // It's a raw joystick - use actual POV hat
        int joystickIndex = static_cast<int>(index) - gamepadCount;
        return input.GetJoystickHat(joystickIndex, 0);
    }
}

void gamepadUpdate()
{
    // Input is updated in PollEvents(), nothing extra needed here
}

void gamepadReset()
{
    // Nothing to reset for SDL game controllers
}

void gamepadCalibrate()
{
    // SDL game controllers are self-calibrating
}
