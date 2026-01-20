#ifndef INPUT_H
#define INPUT_H

/**
  * Input System - SDL2-based input handling
  *
  * This replaces the Marmalade s3ePointer and s3eKeyboard APIs.
  * Provides unified handling of keyboard, mouse, touch, and gamepad input.
  */

#include "Platform.h"
#include <SDL.h>
#include <vector>
#include <functional>

//==============================================================================
// Key state enumeration (matches Marmalade s3eKeyState)
//==============================================================================
enum KeyState
{
        KEY_STATE_UP = 0,       // Key is up
        KEY_STATE_DOWN = 1,     // Key is down
        KEY_STATE_PRESSED = 2,  // Key was just pressed this frame
        KEY_STATE_RELEASED = 4  // Key was just released this frame
};

//==============================================================================
// Pointer/Touch state enumeration
//==============================================================================
enum PointerState
{
        POINTER_STATE_UP = 0,
        POINTER_STATE_DOWN = 1,
        POINTER_STATE_PRESSED = 2,
        POINTER_STATE_RELEASED = 4
};

//==============================================================================
// Mouse button enumeration
//==============================================================================
enum MouseButton
{
        MOUSE_BUTTON_LEFT = 0,
        MOUSE_BUTTON_MIDDLE = 1,
        MOUSE_BUTTON_RIGHT = 2,
        MOUSE_BUTTON_COUNT = 3
};

//==============================================================================
// Touch point information
//==============================================================================
struct TouchPoint
{
        int id;             // Touch ID (finger index)
        float x;            // X position (screen coordinates)
        float y;            // Y position (screen coordinates)
        float pressure;     // Pressure (0-1, if available)
        PointerState state; // Current state
        bool active;        // Is this touch point active?
};

//==============================================================================
// Input callback types (for Marmalade-style event callbacks)
//==============================================================================
typedef void (*KeyboardCallback)(int key, int pressed, void* userData);
typedef void (*PointerButtonCallback)(int button, int pressed, int x, int y, void* userData);
typedef void (*PointerMotionCallback)(int x, int y, void* userData);
typedef void (*TouchCallback)(int touchId, int pressed, int x, int y, void* userData);
typedef void (*TouchMotionCallback)(int touchId, int x, int y, void* userData);

//==============================================================================
// Input class - Singleton managing all input
//==============================================================================
class Input
{
public:
        static Input& GetInstance();

        // Initialize/shutdown
        bool Init();
        void Shutdown();

        // Call once per frame to update input state
        void Update();

        // Mark the start of a new frame (allows state transitions to occur)
        void BeginFrame();

        // Clear all button/key pressed states (call on screen transitions to prevent click bleed-through)
        void ClearPressedStates();

        // Process SDL events (call from main event loop)
        void ProcessEvent(const SDL_Event& event);

        //--------------------------------------------------------------------------
        // Keyboard
        //--------------------------------------------------------------------------

        // Get current key state
        KeyState GetKeyState(int keyCode) const;

        // Check if key is currently down
        bool IsKeyDown(int keyCode) const;

        // Check if key was just pressed this frame
        bool IsKeyPressed(int keyCode) const;

        // Check if key was just released this frame
        bool IsKeyReleased(int keyCode) const;

        // Get modifier state
        bool IsShiftDown() const;
        bool IsCtrlDown() const;
        bool IsAltDown() const;

        // Register keyboard callback (Marmalade-style)
        void SetKeyboardCallback(KeyboardCallback callback, void* userData = nullptr);

        //--------------------------------------------------------------------------
        // Mouse/Pointer
        //--------------------------------------------------------------------------

        // Get mouse position
        int GetMouseX() const { return mMouseX; }
        int GetMouseY() const { return mMouseY; }

        // Get mouse delta (movement since last frame)
        int GetMouseDeltaX() const { return mMouseDeltaX; }
        int GetMouseDeltaY() const { return mMouseDeltaY; }

        // Get mouse button state
        PointerState GetMouseButtonState(MouseButton button) const;
        bool IsMouseButtonDown(MouseButton button) const;
        bool IsMouseButtonPressed(MouseButton button) const;
        bool IsMouseButtonReleased(MouseButton button) const;

        // Get mouse wheel delta
        int GetMouseWheelDelta() const { return mMouseWheelDelta; }

        // Register pointer callbacks (Marmalade-style)
        void SetPointerButtonCallback(PointerButtonCallback callback, void* userData = nullptr);
        void SetPointerMotionCallback(PointerMotionCallback callback, void* userData = nullptr);

        //--------------------------------------------------------------------------
        // Touch (multi-touch support)
        //--------------------------------------------------------------------------

        // Get number of active touches
        int GetTouchCount() const;

        // Get touch point by index (0 to GetTouchCount()-1)
        const TouchPoint* GetTouch(int index) const;

        // Get touch point by ID
        const TouchPoint* GetTouchById(int touchId) const;

        // Check if touch input is available
        bool IsTouchAvailable() const { return mTouchAvailable; }

        // Register touch callbacks (Marmalade-style)
        void SetTouchCallback(TouchCallback callback, void* userData = nullptr);
        void SetTouchMotionCallback(TouchMotionCallback callback, void* userData = nullptr);

        //--------------------------------------------------------------------------
        // Gamepad/Joystick
        //--------------------------------------------------------------------------

        // Get number of connected gamepads
        int GetGamepadCount() const;

        // Check if gamepad is connected
        bool IsGamepadConnected(int index) const;

        // Get gamepad axis value (-1 to 1)
        float GetGamepadAxis(int gamepadIndex, int axis) const;

        // Get gamepad button state
        bool IsGamepadButtonDown(int gamepadIndex, int button) const;

        // Gamepad axis indices
        enum GamepadAxis
        {
                GAMEPAD_AXIS_LEFT_X = 0,
                GAMEPAD_AXIS_LEFT_Y = 1,
                GAMEPAD_AXIS_RIGHT_X = 2,
                GAMEPAD_AXIS_RIGHT_Y = 3,
                GAMEPAD_AXIS_TRIGGER_LEFT = 4,
                GAMEPAD_AXIS_TRIGGER_RIGHT = 5
        };

        //--------------------------------------------------------------------------
        // Accelerometer (mobile)
        //--------------------------------------------------------------------------

        // Check if accelerometer is available
        bool IsAccelerometerAvailable() const { return mAccelerometerAvailable; }

        // Get accelerometer values (in g, typically -1 to 1)
        float GetAccelX() const { return mAccelX; }
        float GetAccelY() const { return mAccelY; }
        float GetAccelZ() const { return mAccelZ; }

        //--------------------------------------------------------------------------
        // Utility
        //--------------------------------------------------------------------------

        // Convert SDL keycode to our key code
        static int SDLKeyToKeyCode(SDL_Keycode sdlKey);

        // Convert our key code to SDL keycode
        static SDL_Keycode KeyCodeToSDLKey(int keyCode);

private:
        Input();
        ~Input();
        Input(const Input&) = delete;
        Input& operator=(const Input&) = delete;

        // Keyboard state
        static const int MAX_KEYS = 512;
        KeyState mKeyStates[MAX_KEYS];
        bool mKeyDown[MAX_KEYS];
        bool mKeyDownPrev[MAX_KEYS];

        // Mouse state
        int mMouseX, mMouseY;
        int mMousePrevX, mMousePrevY;
        int mMouseDeltaX, mMouseDeltaY;
        int mMouseWheelDelta;
        PointerState mMouseButtonStates[MOUSE_BUTTON_COUNT];
        bool mMouseButtonDown[MOUSE_BUTTON_COUNT];
        bool mMouseButtonDownPrev[MOUSE_BUTTON_COUNT];

        // Touch state
        static const int MAX_TOUCHES = 10;
        TouchPoint mTouches[MAX_TOUCHES];
        int mTouchCount;
        bool mTouchAvailable;

        // State update tracking - prevents multiple state updates per frame
        bool mStatesDirty;
        Uint32 mLastFrameTime;  // Time of last BeginFrame() call

        // Gamepad state
        std::vector<SDL_GameController*> mGamepads;

        // Accelerometer state
        SDL_Sensor* mAccelerometer;
        bool mAccelerometerAvailable;
        float mAccelX, mAccelY, mAccelZ;

        // Callbacks
        KeyboardCallback mKeyboardCallback;
        void* mKeyboardUserData;
        PointerButtonCallback mPointerButtonCallback;
        void* mPointerButtonUserData;
        PointerMotionCallback mPointerMotionCallback;
        void* mPointerMotionUserData;
        TouchCallback mTouchCallback;
        void* mTouchUserData;
        TouchMotionCallback mTouchMotionCallback;
        void* mTouchMotionUserData;

        // Internal helpers
        void UpdateKeyStates();
        void UpdateMouseStates();
        void UpdateTouchStates();
        void OpenGamepads();
        void CloseGamepads();
        int FindTouchIndex(int touchId) const;
        int FindFreeTouchIndex() const;
};

// Global input instance for convenience
extern Input* gInput;

// Note: Marmalade s3e* compatibility functions are declared in S3ECompat.h
// and implemented in Input.cpp

#endif // INPUT_H
