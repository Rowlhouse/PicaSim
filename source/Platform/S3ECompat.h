#ifndef S3E_COMPAT_H
#define S3E_COMPAT_H

/**
  * S3E Compatibility Layer
  *
  * This header provides drop-in replacements for common Marmalade s3e* functions.
  * It allows existing code to compile with minimal changes during the migration.
  *
  * Usage: Replace #include <s3e*.h> with #include "S3ECompat.h"
  */

#include "Platform.h"
#include <cstdint>
#include <string>
#include <vector>
#ifndef PICASIM_ANDROID
#include <filesystem>
#endif
#include "imgui.h"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
// Undefine Windows macros that conflict with our code
#ifdef RGB
#undef RGB
#endif
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#ifdef DELETE
#undef DELETE
#endif
#endif

//==============================================================================
// Type definitions (from s3eTypes.h)
//==============================================================================

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// Character types
typedef char IwChar;

// Boolean type
#ifndef S3E_TRUE
#define S3E_TRUE  1
#define S3E_FALSE 0
#endif

// Result type
typedef int32 s3eResult;
#define S3E_RESULT_SUCCESS 0
#define S3E_RESULT_ERROR  -1


/**
  * Poll and process SDL events, update input state.
  * Call once per frame at the start of the frame.
  */
void PollEvents();

/**
  * Check if application should quit.
  * Returns true if quit was requested (e.g., window close button pressed).
  */
bool CheckForQuitRequest();

/**
  * Reset quit request flag.
  * Must be called at startup to clear stale state from previous Android launches.
  */
void ResetQuitRequest();

//==============================================================================
// Pointer/Touch functions (from s3ePointer.h)
//==============================================================================

enum s3ePointerProperty
{
        S3E_POINTER_AVAILABLE = 0,
        S3E_POINTER_MULTI_TOUCH_AVAILABLE,
        S3E_POINTER_TYPE
};

enum s3ePointerType
{
        S3E_POINTER_TYPE_INVALID = 0,
        S3E_POINTER_TYPE_MOUSE,
        S3E_POINTER_TYPE_STYLUS,
        S3E_POINTER_TYPE_TOUCHSCREEN
};

// Note: Actual pointer functions need to be implemented with SDL2 events

//==============================================================================
// Debug/Error functions (from s3eDebug.h)
//==============================================================================

enum s3eErrorShowResult
{
        S3E_ERROR_SHOW_IGNORE = 0,
        S3E_ERROR_SHOW_STOP,
        S3E_ERROR_SHOW_AGAIN
};

enum s3eMessageType
{
        S3E_MESSAGE_CONTINUE = 0,
        S3E_MESSAGE_CONTINUE_STOP,
        S3E_MESSAGE_CONTINUE_IGNORE,
        S3E_MESSAGE_OK,
        S3E_MESSAGE_YESNO,
        S3E_MESSAGE_YESNOCANCEL
};

/**
  * Show error dialog.
  * Replacement for s3eDebugErrorShow()
  */
inline s3eErrorShowResult s3eDebugErrorShow(s3eMessageType type, const char* title, const char* message)
{
        // For now, just print to stderr
        fprintf(stderr, "Error [%s]: %s\n", title, message);
        return S3E_ERROR_SHOW_IGNORE;
}

//==============================================================================
// IwAssert compatibility macros
//==============================================================================

// Define ROWLHOUSE for assert context
#define ROWLHOUSE 1
#define Rowlhouse 1

// IwAssert - standard assert wrapper
#ifndef IwAssert
#include <cassert>
#define IwAssert(context, condition) assert(condition)
#endif

// IwAssertMsg - assert with message (message is ignored in release)
#ifndef IwAssertMsg
#define IwAssertMsg(context, condition, message) assert(condition)
#endif

//==============================================================================
// Inline modifier
//==============================================================================

#ifndef S3E_INLINE
#define S3E_INLINE inline
#endif

//==============================================================================
// Pointer touch constants and functions (from s3ePointer.h)
//==============================================================================

#define S3E_POINTER_TOUCH_MAX 10

// Pointer states
// Note: PRESSED = 2 (not 3) so bitwise AND checks work correctly
enum s3ePointerState
{
        S3E_POINTER_STATE_UP = 0,
        S3E_POINTER_STATE_DOWN = 1,
        S3E_POINTER_STATE_PRESSED = 2,
        S3E_POINTER_STATE_RELEASED = 4
};

// Blit direction for screen rotation
enum s3eSurfaceBlitDirection
{
        S3E_SURFACE_BLIT_DIR_NORMAL = 0,
        S3E_SURFACE_BLIT_DIR_ROT90,
        S3E_SURFACE_BLIT_DIR_ROT180,
        S3E_SURFACE_BLIT_DIR_ROT270
};

// Surface property constants
#define S3E_SURFACE_PIXEL_SIZE_MASK 0xFF

// Pointer button constants for mouse
enum s3ePointerButton
{
        S3E_POINTER_BUTTON_LEFT = 0,
        S3E_POINTER_BUTTON_RIGHT = 1,
        S3E_POINTER_BUTTON_MIDDLE = 2,
        S3E_POINTER_BUTTON_MOUSEWHEELUP = 3,
        S3E_POINTER_BUTTON_MOUSEWHEELDOWN = 4
};

// Forward declaration - implemented in Input.cpp
int32 s3ePointerGetInt(s3ePointerProperty property);
s3ePointerState s3ePointerGetTouchState(int32 touchIndex);
int32 s3ePointerGetTouchX(int32 touchIndex);
int32 s3ePointerGetTouchY(int32 touchIndex);

//==============================================================================
// IwGx graphics stubs (from IwGx.h) - to be replaced with ImGui/SDL2
//==============================================================================


// IwGx display dimensions (use Platform functions)
inline int32 IwGxGetDisplayWidth() { return Platform::GetDisplayWidth(); }
inline int32 IwGxGetDisplayHeight() { return Platform::GetDisplayHeight(); }

// IwGx flags
#define IW_GX_COLOUR_BUFFER_F 1
#define IW_GX_DEPTH_BUFFER_F 2

// IwGx flush/swap/clear - implemented in Graphics.cpp
void IwGxSwapBuffers();
void IwGxClear();
void IwGxSetColClear(uint8 r, uint8 g, uint8 b, uint8 a);

// IwGL swap - same as IwGxSwapBuffers
inline void IwGLSwapBuffers() { IwGxSwapBuffers(); }

//==============================================================================
// Vibration (mobile)
//==============================================================================
inline void s3eVibraVibrate(int32 amount, uint32 ms) {
        // Stub - implement for mobile platforms
}

//==============================================================================
// OS Execution
//==============================================================================
inline void s3eOSExecExecute(const char* command, bool async) {
#ifdef _WIN32
        // Parse command to separate executable from arguments
        // e.g., "control joy.cpl" -> exe="control", args="joy.cpl"
        std::string cmd(command);
        std::string exe, args;

        size_t spacePos = cmd.find(' ');
        if (spacePos != std::string::npos) {
                exe = cmd.substr(0, spacePos);
                args = cmd.substr(spacePos + 1);
                ShellExecuteA(NULL, "open", exe.c_str(), args.c_str(), NULL, SW_SHOWNORMAL);
        } else {
                ShellExecuteA(NULL, "open", command, NULL, NULL, SW_SHOWNORMAL);
        }
#else
        // Stub - implement for other platforms
#endif
}

//==============================================================================
// Gamepad functions (implemented in Input.cpp via SDL2)
//==============================================================================
bool gamepadAvailable();
uint32 gamepadGetNumDevices();
void gamepadCalibrate();
void gamepadUpdate();
uint32 gamepadGetNumAxes(uint32 index);
int32 gamepadGetAxis(uint32 index, uint32 axisIndex);
uint32 gamepadIsPointOfViewAvailable(uint32 index);
int32 gamepadGetPointOfViewAngle(uint32 index);
uint32 gamepadGetButtons(uint32 index);
void gamepadReset();

//==============================================================================
// HTTP client stub (for version checking - reimplement with libcurl if needed)
//==============================================================================
typedef int32 (*IwHTTPCallback)(void*, void*);

class CIwHTTP {
public:
        CIwHTTP() {}
        ~CIwHTTP() {}

        s3eResult Get(const char* uri, IwHTTPCallback headersCallback, void* userData) {
                return S3E_RESULT_ERROR; // Network not implemented
        }
        s3eResult GetStatus() const { return S3E_RESULT_ERROR; }
        uint32 ContentLength() const { return 0; }
        uint32 ContentReceived() const { return 0; }
        uint32 ContentExpected() const { return 0; }
        void ReadContent(char* buffer, uint32 len, IwHTTPCallback callback, void* userData = nullptr) {}
};

#endif // S3E_COMPAT_H
