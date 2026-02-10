#ifndef PLATFORM_H
#define PLATFORM_H

/**
  * Platform Abstraction Layer for PicaSim
  *
  * This header provides cross-platform abstractions that replace Marmalade SDK
  * functionality. The implementations use SDL2 and related libraries.
  *
  * Migration from Marmalade:
  *   - s3eTimer -> Timer class
  *   - s3eFile  -> FileSystem class
  *   - s3eDevice -> Platform namespace functions
  *   - s3ePointer/s3eKeyboard -> Input class
  *   - IwAssert -> PS_ASSERT macros
  */

#include <cstdint>
#include <string>
#include <vector>
#include <cassert>

// Platform detection macros
#if defined(PICASIM_WINDOWS) || defined(_WIN32)
        #define PICASIM_PLATFORM_NAME "Windows"
        #define PICASIM_DESKTOP 1
        #define PS_PLATFORM_WINDOWS 1
        #ifndef PICASIM_WINDOWS
                #define PICASIM_WINDOWS 1
        #endif
#elif defined(PICASIM_MACOS) || defined(__APPLE__)
        #include <TargetConditionals.h>
        #if TARGET_OS_IOS
                #define PICASIM_PLATFORM_NAME "iOS"
                #define PICASIM_MOBILE 1
                #define PS_PLATFORM_IOS 1
                #ifndef PICASIM_IOS
                        #define PICASIM_IOS 1
                #endif
        #else
                #define PICASIM_PLATFORM_NAME "macOS"
                #define PICASIM_DESKTOP 1
                #define PS_PLATFORM_MACOS 1
                #ifndef PICASIM_MACOS
                        #define PICASIM_MACOS 1
                #endif
        #endif
#elif defined(PICASIM_ANDROID) || defined(__ANDROID__)
        // Note: Android check must come before Linux because __linux__ is also
        // defined on Android NDK.
        #define PICASIM_PLATFORM_NAME "Android"
        #define PICASIM_MOBILE 1
        #define PS_PLATFORM_ANDROID 1
        #ifndef PICASIM_ANDROID
                #define PICASIM_ANDROID 1
        #endif
#elif defined(PICASIM_LINUX) || defined(__linux__)
        #define PICASIM_PLATFORM_NAME "Linux"
        #define PICASIM_DESKTOP 1
        #define PS_PLATFORM_LINUX 1
        #ifndef PICASIM_LINUX
                #define PICASIM_LINUX 1
        #endif
#else
        // Default to desktop if no platform detected
        #define PICASIM_PLATFORM_NAME "Unknown"
        #define PICASIM_DESKTOP 1
#endif

// Undefine Windows macros that conflict with our function names
#ifdef _WIN32
    #ifdef CreateDirectory
        #undef CreateDirectory
    #endif
    #ifdef Yield
        #undef Yield
    #endif
#endif

// --------------------------------------------------------------------------
// Assert macros (replaces IwAssert)
// --------------------------------------------------------------------------

#ifdef _DEBUG
        #define PS_ASSERT(condition) assert(condition)
        #define PS_ASSERT_MSG(condition, msg) \
                do { \
                        if (!(condition)) { \
                                fprintf(stderr, "Assertion failed: %s\n  %s:%d\n  %s\n", \
                                        #condition, __FILE__, __LINE__, msg); \
                                assert(condition); \
                        } \
                } while(0)
#else
        #define PS_ASSERT(condition) ((void)0)
        #define PS_ASSERT_MSG(condition, msg) ((void)0)
#endif

// Compatibility macro for existing code that uses IwAssert(ROWLHOUSE, x)
#define IwAssert(module, condition) PS_ASSERT(condition)

namespace Platform
{

// --------------------------------------------------------------------------
// Platform identification (replaces s3eDeviceOSID)
// --------------------------------------------------------------------------

enum class PlatformID
{
        Windows,
        macOS,
        Linux,
        Android,
        iOS
};

PlatformID GetPlatformID();
const char* GetPlatformName();
bool IsDesktop();
bool IsMobile();

// --------------------------------------------------------------------------
// Device information (replaces s3eDevice functions)
// --------------------------------------------------------------------------

int GetCPUCount();
int GetSystemRAM();       // In MB
std::string GetDeviceID();

// --------------------------------------------------------------------------
// Screen/Display (replaces s3eSurface functions)
// --------------------------------------------------------------------------

int GetDisplayWidth();
int GetDisplayHeight();
float GetScreenDPI();
float GetDisplayScale();  // For HiDPI displays
float GetSurfaceDiagonalInches();  // Physical screen diagonal in inches
float GetSafeAreaInsetX();  // Left/right inset in pixels for mobile cutouts (0 on desktop)
float GetSafeAreaInsetY();  // Top/bottom inset in pixels for mobile rounded corners (0 on desktop)

// --------------------------------------------------------------------------
// Application lifecycle
// --------------------------------------------------------------------------

void DisableScreenSaver();
void EnableScreenSaver();
void OpenURL(const char* url);

// --------------------------------------------------------------------------
// System control
// --------------------------------------------------------------------------

void Yield(int milliseconds);  // Replaces s3eDeviceYield

// --------------------------------------------------------------------------
// User data paths
// --------------------------------------------------------------------------

// Returns platform-specific user settings path (e.g. %APPDATA%/Rowlhouse/PicaSim/UserSettings/)
std::string GetUserSettingsPath();

// Returns platform-specific user data path (e.g. %APPDATA%/Rowlhouse/PicaSim/UserData/)
std::string GetUserDataPath();

// Returns platform-specific logs path (e.g. %APPDATA%/Rowlhouse/PicaSim/Logs/)
std::string GetLogsPath();

} // namespace Platform

// --------------------------------------------------------------------------
// Timer (replaces s3eTimer)
// --------------------------------------------------------------------------

class Timer
{
public:
        static uint64_t GetMilliseconds();
        static uint64_t GetMicroseconds();
        static double GetSeconds();
};

// --------------------------------------------------------------------------
// FileSystem (replaces s3eFile)
// --------------------------------------------------------------------------

class FileSystem
{
public:
        // File operations
        static bool Exists(const std::string& path);
        static bool IsDirectory(const std::string& path);
        static bool MakeDirectory(const std::string& path);
        static bool Delete(const std::string& path);

        // Directory listing
        static std::vector<std::string> ListDirectory(const std::string& path);

        // Path utilities
        static std::string GetBasePath();     // Application directory
        static std::string GetPrefPath();     // User data directory
        static std::string JoinPath(const std::string& a, const std::string& b);
        static std::string GetExtension(const std::string& path);
        static std::string GetFileName(const std::string& path);
        static std::string GetDirectory(const std::string& path);

        // Read entire file to memory
        static bool ReadFile(const std::string& path, std::vector<uint8_t>& outData);
        static bool ReadTextFile(const std::string& path, std::string& outText);

        // Write file
        static bool WriteFile(const std::string& path, const void* data, size_t size);
        static bool WriteTextFile(const std::string& path, const std::string& text);
};

// --------------------------------------------------------------------------
// Input (replaces s3ePointer, s3eKeyboard)
// --------------------------------------------------------------------------

// Forward declaration - full implementation in Input.h
class Input;

// Key codes (subset matching common Marmalade keys)
namespace Keys
{
        enum KeyCode
        {
                Unknown = 0,

                // Letters
                A, B, C, D, E, F, G, H, I, J, K, L, M,
                N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

                // Numbers
                Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

                // Function keys
                F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

                // Arrow keys
                Up, Down, Left, Right,

                // Special keys
                Space, Enter, Escape, Tab, Backspace, Delete,
                Shift, Ctrl, Alt,
                Home, End, PageUp, PageDown,
                Insert,

                // Numpad
                Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
                Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
                NumpadPlus, NumpadMinus, NumpadMultiply, NumpadDivide,

                KeyCount
        };
}

// Touch/pointer state
struct TouchState
{
        bool active;
        float x;
        float y;
        int id;
};

// Include Marmalade compatibility layer
// This provides all the s3e*, Iw*, and CIw* type definitions
#include "S3ECompat.h"

#endif // PLATFORM_H
