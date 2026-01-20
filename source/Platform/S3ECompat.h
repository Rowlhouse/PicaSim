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
#include <filesystem>
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

typedef int32 s3eBool;

// Result type
typedef int32 s3eResult;
#define S3E_RESULT_SUCCESS 0
#define S3E_RESULT_ERROR  -1

// Utility macros
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

//==============================================================================
// Device functions (from s3eDevice.h)
//==============================================================================

// Device property IDs
enum s3eDeviceProperty
{
        S3E_DEVICE_MEM_FREE = 0,
        S3E_DEVICE_MEM_TOTAL,
        S3E_DEVICE_NUM_CPU_CORES,
        S3E_DEVICE_OS,
        S3E_DEVICE_OS_VERSION,
        S3E_DEVICE_ID,
        S3E_DEVICE_LANGUAGE,
        S3E_DEVICE_LOCALE
};

// Device language constants
enum s3eDeviceLanguage
{
        S3E_DEVICE_LANGUAGE_ENGLISH = 0,
        S3E_DEVICE_LANGUAGE_GERMAN,
        S3E_DEVICE_LANGUAGE_FRENCH,
        S3E_DEVICE_LANGUAGE_SPANISH,
        S3E_DEVICE_LANGUAGE_ITALIAN,
        S3E_DEVICE_LANGUAGE_PORTUGUESE,
        S3E_DEVICE_LANGUAGE_DUTCH,
        S3E_DEVICE_LANGUAGE_JAPANESE,
        S3E_DEVICE_LANGUAGE_CHINESE
};

// OS IDs
enum s3eDeviceOSID
{
        S3E_OS_ID_UNKNOWN = 0,
        S3E_OS_ID_WINDOWS,
        S3E_OS_ID_OSX,
        S3E_OS_ID_LINUX,
        S3E_OS_ID_IPHONE,
        S3E_OS_ID_ANDROID,
        S3E_OS_ID_NACL,
        S3E_OS_ID_QNX,
        S3E_OS_ID_ROKU,
        S3E_OS_ID_WP8,
        S3E_OS_ID_WIN10
};

/**
  * Get device integer property.
  * Replacement for s3eDeviceGetInt()
  */
inline int32 s3eDeviceGetInt(s3eDeviceProperty property)
{
        switch (property)
        {
        case S3E_DEVICE_NUM_CPU_CORES:
                return Platform::GetCPUCount();
        case S3E_DEVICE_MEM_TOTAL:
                return Platform::GetSystemRAM();
        case S3E_DEVICE_OS:
                switch (Platform::GetPlatformID())
                {
                case Platform::PlatformID::Windows: return S3E_OS_ID_WINDOWS;
                case Platform::PlatformID::macOS:   return S3E_OS_ID_OSX;
                case Platform::PlatformID::Linux:   return S3E_OS_ID_LINUX;
                case Platform::PlatformID::iOS:     return S3E_OS_ID_IPHONE;
                case Platform::PlatformID::Android: return S3E_OS_ID_ANDROID;
                default: return S3E_OS_ID_UNKNOWN;
                }
        default:
                return 0;
        }
}

/**
  * Yield to system.
  * Replacement for s3eDeviceYield()
  * Now also processes SDL events and updates input state.
  */
void s3eDeviceYield(int32 ms);

inline void s3eDeviceYield()
{
        s3eDeviceYield(0);
}

/**
  * Check if application should quit.
  * Returns S3E_TRUE if quit was requested (e.g., window close button pressed).
  */
s3eBool s3eDeviceCheckQuitRequest();

/**
  * Get device string property.
  */
inline const char* s3eDeviceGetString(s3eDeviceProperty property)
{
        switch (property)
        {
        case S3E_DEVICE_ID: return "SDL2_Device";
        default: return "";
        }
}

// Device callbacks
enum s3eDeviceCallback
{
        S3E_DEVICE_PAUSE = 0,
        S3E_DEVICE_UNPAUSE,
        S3E_DEVICE_FOREGROUND,
        S3E_DEVICE_BACKGROUND
};

typedef int32 (*s3eDeviceCallbackFn)(void*, void*);

inline void s3eDeviceRegister(s3eDeviceCallback callback, s3eDeviceCallbackFn fn, void* userData) {}
inline void s3eDeviceUnRegister(s3eDeviceCallback callback, s3eDeviceCallbackFn fn) {}

// Config functions
inline s3eResult s3eConfigGetInt(const char* section, const char* name, int32* value) {
        if (value) *value = 0;
        return S3E_RESULT_SUCCESS;
}

//==============================================================================
// Surface functions (from s3eSurface.h)
//==============================================================================

enum s3eSurfaceProperty
{
        S3E_SURFACE_WIDTH = 0,
        S3E_SURFACE_HEIGHT,
        S3E_SURFACE_PITCH,
        S3E_SURFACE_PIXEL_TYPE,
        S3E_SURFACE_DEVICE_WIDTH,
        S3E_SURFACE_DEVICE_HEIGHT,
        S3E_SURFACE_DEVICE_BLIT_DIRECTION
};

/**
  * Get surface/screen property.
  * Replacement for s3eSurfaceGetInt()
  */
inline int32 s3eSurfaceGetInt(s3eSurfaceProperty property)
{
        switch (property)
        {
        case S3E_SURFACE_WIDTH:
        case S3E_SURFACE_DEVICE_WIDTH:
                return Platform::GetScreenWidth();
        case S3E_SURFACE_HEIGHT:
        case S3E_SURFACE_DEVICE_HEIGHT:
                return Platform::GetScreenHeight();
        default:
                return 0;
        }
}

//==============================================================================
// Keyboard functions (from s3eKeyboard.h)
//==============================================================================

// Key states
// Note: PRESSED = 2 (not 3) so bitwise AND checks work correctly
// (original Marmalade had PRESSED = DOWN | 2, but codebase uses & to check specific states)
enum s3eKeyState
{
        S3E_KEY_STATE_UP = 0,
        S3E_KEY_STATE_DOWN = 1,
        S3E_KEY_STATE_PRESSED = 2,
        S3E_KEY_STATE_RELEASED = 4
};

// Key codes (subset of commonly used keys)
enum s3eKey
{
        s3eKeyFirst = 0,
        s3eKeyEsc = 0x1B,
        s3eKeyTab = 0x09,
        s3eKeyBackspace = 0x08,
        s3eKeyEnter = 0x0D,
        s3eKeySpace = 0x20,

        s3eKeyLeft = 0x25,
        s3eKeyUp = 0x26,
        s3eKeyRight = 0x27,
        s3eKeyDown = 0x28,

        s3eKey0 = 0x30,
        s3eKey1 = 0x31,
        s3eKey2 = 0x32,
        s3eKey3 = 0x33,
        s3eKey4 = 0x34,
        s3eKey5 = 0x35,
        s3eKey6 = 0x36,
        s3eKey7 = 0x37,
        s3eKey8 = 0x38,
        s3eKey9 = 0x39,

        s3eKeyA = 0x41,
        s3eKeyB = 0x42,
        s3eKeyC = 0x43,
        s3eKeyD = 0x44,
        s3eKeyE = 0x45,
        s3eKeyF = 0x46,
        s3eKeyG = 0x47,
        s3eKeyH = 0x48,
        s3eKeyI = 0x49,
        s3eKeyJ = 0x4A,
        s3eKeyK = 0x4B,
        s3eKeyL = 0x4C,
        s3eKeyM = 0x4D,
        s3eKeyN = 0x4E,
        s3eKeyO = 0x4F,
        s3eKeyP = 0x50,
        s3eKeyQ = 0x51,
        s3eKeyR = 0x52,
        s3eKeyS = 0x53,
        s3eKeyT = 0x54,
        s3eKeyU = 0x55,
        s3eKeyV = 0x56,
        s3eKeyW = 0x57,
        s3eKeyX = 0x58,
        s3eKeyY = 0x59,
        s3eKeyZ = 0x5A,

        s3eKeyNumPad0 = 0x60,
        s3eKeyNumPad1 = 0x61,
        s3eKeyNumPad2 = 0x62,
        s3eKeyNumPad3 = 0x63,
        s3eKeyNumPad4 = 0x64,
        s3eKeyNumPad5 = 0x65,
        s3eKeyNumPad6 = 0x66,
        s3eKeyNumPad7 = 0x67,
        s3eKeyNumPad8 = 0x68,
        s3eKeyNumPad9 = 0x69,

        s3eKeyF1 = 0x70,
        s3eKeyF2 = 0x71,
        s3eKeyF3 = 0x72,
        s3eKeyF4 = 0x73,
        s3eKeyF5 = 0x74,
        s3eKeyF6 = 0x75,
        s3eKeyF7 = 0x76,
        s3eKeyF8 = 0x77,
        s3eKeyF9 = 0x78,
        s3eKeyF10 = 0x79,
        s3eKeyF11 = 0x7A,
        s3eKeyF12 = 0x7B,

        s3eKeyLShift = 0xA0,
        s3eKeyRShift = 0xA1,
        s3eKeyLeftShift = 0xA0,   // Alias
        s3eKeyRightShift = 0xA1,  // Alias
        s3eKeyLControl = 0xA2,
        s3eKeyRControl = 0xA3,
        s3eKeyLeftControl = 0xA2, // Alias
        s3eKeyRightControl = 0xA3,// Alias
        s3eKeyLAlt = 0xA4,
        s3eKeyRAlt = 0xA5,

        // Additional punctuation keys
        s3eKeyEquals = 0xBB,      // = key
        s3eKeyMinus = 0xBD,       // - key
        s3eKeyPlus = 0xBB,        // + key (same as equals)
        s3eKeySemicolon = 0xBA,   // ; key
        s3eKeyComma = 0xBC,       // , key
        s3eKeyPeriod = 0xBE,      // . key
        s3eKeySlash = 0xBF,       // / key

        // Mobile-specific keys (Android)
        s3eKeyBack = 0xE0,      // Android back button
        s3eKeyMenu = 0xE1,      // Android menu button
        s3eKeyHome = 0xE2,      // Home button
        s3eKeySearch = 0xE3,    // Search button
        s3eKeyAbsGameA = 0xE4,  // Abstract game key A
        s3eKeyAbsGameB = 0xE5,  // Abstract game key B
        s3eKeyAbsGameC = 0xE6,  // Abstract game key C
        s3eKeyAbsGameD = 0xE7,  // Abstract game key D

        s3eKeyLast
};

// Keyboard state functions - forward to Input class
// (declared here, defined in Input.cpp via Input.h)
s3eKeyState s3eKeyboardGetState(s3eKey key);
s3eBool s3eKeyboardIsKeyDown(s3eKey key);
void s3eKeyboardUpdate();  // Called each frame to update keyboard state

// Allow bitwise AND on s3eKeyState - returns bool for use in conditionals
inline bool operator&(s3eKeyState a, s3eKeyState b) { return (static_cast<int>(a) & static_cast<int>(b)) != 0; }

// Device functions (from s3eDevice.h)
inline void s3eDeviceBacklightOn() {}  // Keep screen backlight on
inline void s3ePointerUpdate() {}      // Update pointer/touch state each frame

// Clear pressed states to prevent click bleed-through on screen transitions
void s3eClearPressedStates();

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

// Pointer position - declared here, defined in Input.h (must be included somewhere)
int32 s3ePointerGetX();
int32 s3ePointerGetY();

//==============================================================================
// Accelerometer functions (from s3eAccelerometer.h) - to be replaced with SDL2 sensor
//==============================================================================

enum s3eAccelerometerProperty
{
        S3E_ACCELEROMETER_AVAILABLE = 0
};

inline int32 s3eAccelerometerGetInt(s3eAccelerometerProperty property) {
        // TODO: Check SDL2 sensor availability
        return 0; // Not available by default on desktop
}
inline s3eResult s3eAccelerometerStart() { return S3E_RESULT_SUCCESS; }
inline void s3eAccelerometerStop() {}
inline int32 s3eAccelerometerGetX() { return 0; }
inline int32 s3eAccelerometerGetY() { return 0; }
inline int32 s3eAccelerometerGetZ() { return 1000; } // 1g pointing down

//==============================================================================
// IwGx graphics stubs (from IwGx.h) - to be replaced with ImGui/SDL2
//==============================================================================


// IwGx screen dimensions (use Platform functions)
inline int32 IwGxGetScreenWidth() { return Platform::GetScreenWidth(); }
inline int32 IwGxGetScreenHeight() { return Platform::GetScreenHeight(); }

// IwGx print functions (stub - will be replaced with ImGui)
inline void IwGxPrintSetColour(int r, int g, int b) {}
inline void IwGxPrintString(int x, int y, const char* str) {}

// IwGx flags
#define IW_GX_COLOUR_BUFFER_F 1
#define IW_GX_DEPTH_BUFFER_F 2

// IwGx flush/swap/clear - implemented in Graphics.cpp
void IwGxFlush();
void IwGxSwapBuffers();
void IwGxClear();
void IwGxClear(uint32 flags);
void IwGxSetColClear(uint8 r, uint8 g, uint8 b, uint8 a);

// IwGL swap - same as IwGxSwapBuffers
inline void IwGLSwapBuffers() { IwGxSwapBuffers(); }

// EGL stub (not needed with SDL2 but code references it for PVRVFrame)
inline void* eglGetProcAddress(const char* name) { return nullptr; }

// IwGL extension constants
enum IwGLExtension
{
        IW_GL_OES_framebuffer_object = 0,
        IW_GL_OES_texture_npot,
        IW_GL_OES_depth24,
        IW_GL_OES_depth_texture
};

// IwGL extension availability check
inline bool IwGLExtAvailable(IwGLExtension ext) {
        // Most OpenGL 2.0+ contexts support framebuffer objects
        return true;
}

// PrepareForIwGx / RecoverFromIwGx - declared here, defined in Graphics.cpp
void PrepareForIwGx(bool fullscreen);
void RecoverFromIwGx(bool fullscreen);

//==============================================================================
// File I/O stubs (from s3eFile.h) - to be replaced with C++ filesystem
//==============================================================================

// s3eFile is an opaque handle type (actually just FILE*)
typedef FILE s3eFile;
typedef int s3eFileError;

inline s3eFile* s3eFileOpen(const char* filename, const char* mode)
{
        return fopen(filename, mode);
}

inline void s3eFileClose(s3eFile* file)
{
        if (file) fclose(file);
}

inline int32 s3eFileRead(void* buffer, int32 size, int32 count, s3eFile* file)
{
        return file ? (int32)fread(buffer, size, count, file) : 0;
}

inline int32 s3eFileGetSize(s3eFile* file)
{
        if (!file) return 0;
        long pos = ftell(file);
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, pos, SEEK_SET);
        return (int32)size;
}

inline s3eFileError s3eFileGetError() { return 0; }

inline bool s3eFileCheckExists(const char* filename)
{
        FILE* f = fopen(filename, "rb");
        if (f) {
                fclose(f);
                return true;
        }
        return false;
}

// Directory creation
#ifdef _WIN32
#include <direct.h>
inline s3eResult s3eFileMakeDirectory(const char* path) {
        return _mkdir(path) == 0 ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}
inline s3eResult s3eFileDelete(const char* path) {
        return remove(path) == 0 ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}
#else
#include <sys/stat.h>
inline s3eResult s3eFileMakeDirectory(const char* path) {
        return mkdir(path, 0755) == 0 ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}
inline s3eResult s3eFileDelete(const char* path) {
        return remove(path) == 0 ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}
#endif

// Directory listing using std::filesystem
struct s3eFileList
{
        std::vector<std::string> entries;
        size_t currentIndex;
};

inline s3eFileList* s3eFileListDirectory(const char* path)
{
        namespace fs = std::filesystem;

        if (!path)
                return nullptr;

        fs::path dirPath(path);

        // Check if directory exists
        std::error_code ec;
        if (!fs::exists(dirPath, ec) || !fs::is_directory(dirPath, ec))
                return nullptr;

        s3eFileList* fileList = new s3eFileList();
        fileList->currentIndex = 0;

        // Iterate through directory and collect filenames
        for (const auto& entry : fs::directory_iterator(dirPath, ec))
        {
                if (entry.is_regular_file(ec))
                {
                        // Store just the filename, not the full path
                        fileList->entries.push_back(entry.path().filename().string());
                }
        }

        return fileList;
}

inline s3eResult s3eFileListNext(s3eFileList* fileList, char* filename, int filenameLen)
{
        if (!fileList || !filename || filenameLen <= 0)
                return S3E_RESULT_ERROR;

        if (fileList->currentIndex >= fileList->entries.size())
                return S3E_RESULT_ERROR;

        const std::string& entry = fileList->entries[fileList->currentIndex];
        fileList->currentIndex++;

        // Copy filename, ensuring null termination
        strncpy(filename, entry.c_str(), filenameLen - 1);
        filename[filenameLen - 1] = '\0';

        return S3E_RESULT_SUCCESS;
}

inline void s3eFileListClose(s3eFileList* fileList)
{
        delete fileList;
}

//==============================================================================
// 2D rendering stubs (from Iw2D.h) - to be replaced with ImGui
//==============================================================================

struct Vec2f
{
        float x, y;
        Vec2f() : x(0), y(0) {}
        Vec2f(float _x, float _y) : x(_x), y(_y) {}
};

inline void Iw2DInit() {}
inline void Iw2DTerminate() {}

inline void Iw2DFillPolygon(Vec2f* verts, int numVerts) {}
inline void Iw2DSetColour(uint32 col) {}
inline void Iw2DDrawRect(const Vec2f& pos, const Vec2f& size) {}
inline void Iw2DDrawLine(const Vec2f& from, const Vec2f& to) {}
inline void Iw2DSetFont(void* font) {}
inline void Iw2DDrawString(const char* str, int x, int y) {}

//==============================================================================
// Vibration (mobile)
//==============================================================================
inline void s3eVibraVibrate(int32 amount, uint32 ms) {
        // Stub - implement for mobile platforms
}

//==============================================================================
// OS Execution
//==============================================================================
inline void s3eOSExecExecute(const char* url, bool async) {
#ifdef _WIN32
        ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#else
        // Stub - implement for other platforms
#endif
}

//==============================================================================
// iOS Background Audio (stubs for non-iOS)
//==============================================================================
inline bool s3eIOSBackgroundAudioAvailable() { return false; }
inline void s3eIOSBackgroundAudioSetMix(bool mix) {}

//==============================================================================
// DPI (Screen density) - replacement for dpi/dpiInfo.h
//==============================================================================
namespace DPI {
        inline void dpiInit() {}
        inline void dpiTerminate() {}
        inline int32 dpiGetScreenDPI() { return 96; } // Standard Windows DPI
}

//==============================================================================
// Gamepad functions (implemented in Input.cpp via SDL2)
//==============================================================================
s3eBool gamepadAvailable();
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
// Legacy IwUI lifecycle - IwUIInit calls eglInit to create window/OpenGL context
//==============================================================================
int eglInit(bool createSurface, int msaaSamples); // Declared in Graphics.h, implemented in Graphics.cpp
inline void IwUIInit(int msaaSamples = 0) { eglInit(true, msaaSamples); }
inline void IwUITerminate() {}

//==============================================================================
// Version string
//==============================================================================
#define MARMALADE_VERSION_STRING_FULL "SDL2/OpenGL Port"

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
