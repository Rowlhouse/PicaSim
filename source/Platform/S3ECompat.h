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

/**
  * Poll and process SDL events, update input state.
  * Call once per frame at the start of the frame.
  */
void PollEvents();

/**
  * Check if application should quit.
  * Returns S3E_TRUE if quit was requested (e.g., window close button pressed).
  */
s3eBool CheckForQuitRequest();

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
void IwGxFlush();
void IwGxSwapBuffers();
void IwGxClear();
void IwGxClear(uint32 flags);
void IwGxSetColClear(uint8 r, uint8 g, uint8 b, uint8 a);

// IwGL swap - same as IwGxSwapBuffers
inline void IwGLSwapBuffers() { IwGxSwapBuffers(); }

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
