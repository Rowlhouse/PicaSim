#include "Platform.h"
#include "Window.h"
#include <SDL.h>
#include <cstdio>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h>
    // Undefine Windows macros that conflict with our function names
    #undef CreateDirectory
    #undef Yield
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
#endif

namespace Platform
{

// --------------------------------------------------------------------------
// Platform identification
// --------------------------------------------------------------------------

PlatformID GetPlatformID()
{
#if defined(PICASIM_WINDOWS)
    return PlatformID::Windows;
#elif defined(PICASIM_MACOS)
    return PlatformID::macOS;
#elif defined(PICASIM_LINUX)
    return PlatformID::Linux;
#elif defined(PICASIM_ANDROID)
    return PlatformID::Android;
#elif defined(PICASIM_IOS)
    return PlatformID::iOS;
#else
    return PlatformID::Windows; // Default fallback
#endif
}

const char* GetPlatformName()
{
    return PICASIM_PLATFORM_NAME;
}

bool IsDesktop()
{
#if defined(PICASIM_DESKTOP)
    return true;
#else
    return false;
#endif
}

bool IsMobile()
{
#if defined(PICASIM_MOBILE)
    return true;
#else
    return false;
#endif
}

// --------------------------------------------------------------------------
// Device information
// --------------------------------------------------------------------------

int GetCPUCount()
{
    return SDL_GetCPUCount();
}

int GetSystemRAM()
{
    return SDL_GetSystemRAM();
}

std::string GetDeviceID()
{
    // Generate a device-specific identifier
    // Note: This is a simplified implementation. A real implementation would
    // use platform-specific APIs for a stable device ID.
    static std::string deviceId;
    if (deviceId.empty())
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s-%d-%d",
            GetPlatformName(),
            GetCPUCount(),
            GetSystemRAM());
        deviceId = buffer;
    }
    return deviceId;
}

// --------------------------------------------------------------------------
// Screen/Display
// --------------------------------------------------------------------------

int GetDisplayWidth()
{
    // Return window width if window exists
    if (gWindow && gWindow->IsInitialized())
    {
        return gWindow->GetWidth();
    }
    // Fall back to display mode
    SDL_DisplayMode mode;
    if (SDL_GetCurrentDisplayMode(0, &mode) == 0)
    {
        return mode.w;
    }
    return 1920; // Default fallback
}

int GetDisplayHeight()
{
    // Return window height if window exists
    if (gWindow && gWindow->IsInitialized())
    {
        return gWindow->GetHeight();
    }
    // Fall back to display mode
    SDL_DisplayMode mode;
    if (SDL_GetCurrentDisplayMode(0, &mode) == 0)
    {
        return mode.h;
    }
    return 1080; // Default fallback
}

float GetScreenDPI()
{
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) == 0)
    {
        return ddpi;
    }
#if defined(PICASIM_ANDROID) || defined(__ANDROID__)
    return 160.0f; // Android baseline DPI
#else
    return 96.0f; // Desktop default DPI
#endif
}

float GetSurfaceDiagonalInches()
{
    float dpi = GetScreenDPI();
    if (dpi > 0.0f)
    {
        int w = GetDisplayWidth();
        int h = GetDisplayHeight();
        float d = hypotf((float)w, (float)h);
        return d / dpi;
    }
    return 0.0f;
}

float GetDisplayScale()
{
    // Calculate display scale based on DPI
    // Standard DPI is 96 on Windows, 72 on macOS, 160 on Android
#ifdef _WIN32
    const float baseDPI = 96.0f;
#elif defined(PICASIM_ANDROID) || defined(__ANDROID__)
    const float baseDPI = 160.0f;
#else
    const float baseDPI = 72.0f;
#endif
    return GetScreenDPI() / baseDPI;
}

// --------------------------------------------------------------------------
// Safe area insets for mobile cutouts and rounded corners
// --------------------------------------------------------------------------

#if defined(PICASIM_MOBILE)
static float GetMobileScale()
{
    float dpiScale = GetDisplayScale();
    float diagonal = GetSurfaceDiagonalInches();
    float targetFontDp = (diagonal > 0.0f && diagonal < 7.0f) ? 20.0f :
                         (diagonal <= 11.0f) ? 22.0f : 24.0f;
    float scale = sqrtf(targetFontDp * dpiScale / 18.0f);  // 18.0f = sBaseFontSize
    if (scale < 1.0f) scale = 1.0f;
    return scale;
}
#endif

float GetSafeAreaInsetX()
{
#if defined(PICASIM_MOBILE)
    return GetMobileScale() * 40.0f;
#else
    return 0.0f;
#endif
}

float GetSafeAreaInsetY()
{
#if defined(PICASIM_MOBILE)
    return GetMobileScale() * 12.0f;
#else
    return 0.0f;
#endif
}

// --------------------------------------------------------------------------
// Application lifecycle
// --------------------------------------------------------------------------

void DisableScreenSaver()
{
    SDL_DisableScreenSaver();
}

void EnableScreenSaver()
{
    SDL_EnableScreenSaver();
}

void OpenURL(const char* url)
{
    if (!url || !*url) return;

    // Prepend https:// if URL doesn't have a protocol scheme
    std::string fullUrl = url;
    if (fullUrl.find("://") == std::string::npos)
    {
        fullUrl = "https://" + fullUrl;
    }

    SDL_OpenURL(fullUrl.c_str());
}

// --------------------------------------------------------------------------
// System control
// --------------------------------------------------------------------------

void Yield(int milliseconds)
{
    if (milliseconds > 0)
    {
        SDL_Delay(static_cast<Uint32>(milliseconds));
    }
}

// --------------------------------------------------------------------------
// User data paths
// --------------------------------------------------------------------------

std::string GetUserSettingsPath()
{
    return FileSystem::GetPrefPath() + "UserSettings/";
}

std::string GetUserDataPath()
{
    return FileSystem::GetPrefPath() + "UserData/";
}

std::string GetLogsPath()
{
    return FileSystem::GetPrefPath() + "Logs/";
}

} // namespace Platform

// --------------------------------------------------------------------------
// Timer implementation
// --------------------------------------------------------------------------

uint64_t Timer::GetMilliseconds()
{
    return SDL_GetTicks64();
}

uint64_t Timer::GetMicroseconds()
{
    static uint64_t frequency = 0;
    if (frequency == 0)
    {
        frequency = SDL_GetPerformanceFrequency();
    }

    uint64_t counter = SDL_GetPerformanceCounter();
    return (counter * 1000000ULL) / frequency;
}

double Timer::GetSeconds()
{
    return static_cast<double>(GetMilliseconds()) / 1000.0;
}

// --------------------------------------------------------------------------
// FileSystem implementation
// --------------------------------------------------------------------------

#include <sys/stat.h>
#include <fstream>
#include <algorithm>

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #define stat _stat
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <unistd.h>
    #include <dirent.h>
#endif

bool FileSystem::Exists(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0;
}

bool FileSystem::IsDirectory(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return false;
    return (info.st_mode & S_IFDIR) != 0;
}

bool FileSystem::MakeDirectory(const std::string& path)
{
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
#else
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

bool FileSystem::Delete(const std::string& path)
{
    return remove(path.c_str()) == 0;
}

std::vector<std::string> FileSystem::ListDirectory(const std::string& path)
{
    std::vector<std::string> result;

#ifdef _WIN32
    std::string searchPath = path + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string name = findData.cFileName;
            if (name != "." && name != "..")
            {
                result.push_back(name);
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            std::string name = entry->d_name;
            if (name != "." && name != "..")
            {
                result.push_back(name);
            }
        }
        closedir(dir);
    }
#endif

    return result;
}

std::string FileSystem::GetBasePath()
{
    char* path = SDL_GetBasePath();
    if (path)
    {
        std::string result = path;
        SDL_free(path);
        return result;
    }
    return ".";
}

std::string FileSystem::GetPrefPath()
{
    char* path = SDL_GetPrefPath("Rowlhouse", "PicaSim");
    if (path)
    {
        std::string result = path;
        SDL_free(path);
        return result;
    }
    return ".";
}

std::string FileSystem::JoinPath(const std::string& a, const std::string& b)
{
    if (a.empty()) return b;
    if (b.empty()) return a;

    char lastChar = a.back();
    if (lastChar == '/' || lastChar == '\\')
    {
        return a + b;
    }

#ifdef _WIN32
    return a + "\\" + b;
#else
    return a + "/" + b;
#endif
}

std::string FileSystem::GetExtension(const std::string& path)
{
    size_t pos = path.rfind('.');
    if (pos != std::string::npos && pos > 0)
    {
        return path.substr(pos + 1);
    }
    return "";
}

std::string FileSystem::GetFileName(const std::string& path)
{
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return path.substr(pos + 1);
    }
    return path;
}

std::string FileSystem::GetDirectory(const std::string& path)
{
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return path.substr(0, pos);
    }
    return "";
}

bool FileSystem::ReadFile(const std::string& path, std::vector<uint8_t>& outData)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    outData.resize(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(outData.data()), size))
    {
        outData.clear();
        return false;
    }

    return true;
}

bool FileSystem::ReadTextFile(const std::string& path, std::string& outText)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    outText.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    return true;
}

bool FileSystem::WriteFile(const std::string& path, const void* data, size_t size)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    file.write(static_cast<const char*>(data), size);
    return file.good();
}

bool FileSystem::WriteTextFile(const std::string& path, const std::string& text)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    file << text;
    return file.good();
}
