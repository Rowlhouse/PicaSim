#ifdef PICASIM_ANDROID

#include "AndroidAssets.h"
#include <SDL.h>
#include <android/log.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define LOG_TAG "PicaSim"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace AndroidAssets
{

static bool MakeDirectoryRecursive(const std::string& path)
{
    std::string current;
    for (size_t i = 0; i < path.size(); ++i)
    {
        current += path[i];
        if (path[i] == '/' || i == path.size() - 1)
        {
            struct stat st;
            if (stat(current.c_str(), &st) != 0)
            {
                if (mkdir(current.c_str(), 0755) != 0 && errno != EEXIST)
                {
                    LOGE("Failed to create directory: %s (errno=%d)", current.c_str(), errno);
                    return false;
                }
            }
        }
    }
    return true;
}

static std::string GetVersionMarkerPath(const std::string& basePath)
{
    return basePath + "/.assets_version";
}

static std::string GetCurrentVersionString()
{
    // Use a combination of app version info
    // We read VERSIONS.txt from the APK to get the version
    SDL_RWops* rw = SDL_RWFromFile("VERSIONS.txt", "r");
    if (rw)
    {
        char buf[256];
        memset(buf, 0, sizeof(buf));
        size_t bytesRead = SDL_RWread(rw, buf, 1, sizeof(buf) - 1);
        SDL_RWclose(rw);
        if (bytesRead > 0)
        {
            // Use the first line as version
            char* newline = strchr(buf, '\n');
            if (newline) *newline = 0;
            return std::string(buf);
        }
    }
    // Fallback: use a build timestamp
    return std::string(__DATE__) + " " + __TIME__;
}

static bool NeedsExtraction(const std::string& basePath)
{
    std::string markerPath = GetVersionMarkerPath(basePath);
    std::string currentVersion = GetCurrentVersionString();

    FILE* f = fopen(markerPath.c_str(), "r");
    if (!f)
        return true; // No marker = first launch

    char buf[256];
    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf) - 1, f);
    fclose(f);

    // Strip newline
    char* nl = strchr(buf, '\n');
    if (nl) *nl = 0;

    if (std::string(buf) != currentVersion)
    {
        LOGI("Version changed: '%s' -> '%s', re-extracting", buf, currentVersion.c_str());
        return true;
    }

    return false;
}

static void WriteVersionMarker(const std::string& basePath)
{
    std::string markerPath = GetVersionMarkerPath(basePath);
    std::string currentVersion = GetCurrentVersionString();

    FILE* f = fopen(markerPath.c_str(), "w");
    if (f)
    {
        fprintf(f, "%s\n", currentVersion.c_str());
        fclose(f);
    }
}

static bool ExtractFile(const std::string& assetPath, const std::string& destPath)
{
    // Read from APK via SDL
    SDL_RWops* rw = SDL_RWFromFile(assetPath.c_str(), "rb");
    if (!rw)
    {
        LOGE("Failed to open asset: %s", assetPath.c_str());
        return false;
    }

    Sint64 size = SDL_RWsize(rw);
    if (size < 0)
    {
        // Size unknown, read in chunks
        size = 0;
    }

    // Ensure parent directory exists
    std::string dir = destPath;
    size_t lastSlash = dir.rfind('/');
    if (lastSlash != std::string::npos)
    {
        dir = dir.substr(0, lastSlash);
        MakeDirectoryRecursive(dir);
    }

    FILE* outFile = fopen(destPath.c_str(), "wb");
    if (!outFile)
    {
        LOGE("Failed to create file: %s (errno=%d)", destPath.c_str(), errno);
        SDL_RWclose(rw);
        return false;
    }

    char buffer[8192];
    size_t totalWritten = 0;
    while (true)
    {
        size_t bytesRead = SDL_RWread(rw, buffer, 1, sizeof(buffer));
        if (bytesRead == 0)
            break;
        size_t bytesWritten = fwrite(buffer, 1, bytesRead, outFile);
        if (bytesWritten != bytesRead)
        {
            LOGE("Write error for %s", destPath.c_str());
            fclose(outFile);
            SDL_RWclose(rw);
            return false;
        }
        totalWritten += bytesWritten;
    }

    fclose(outFile);
    SDL_RWclose(rw);
    return true;
}

bool ExtractIfNeeded()
{
    const char* internalPath = SDL_AndroidGetInternalStoragePath();
    if (!internalPath)
    {
        LOGE("SDL_AndroidGetInternalStoragePath() returned NULL");
        return false;
    }

    std::string basePath = std::string(internalPath) + "/data";
    LOGI("Asset extraction base path: %s", basePath.c_str());

#ifdef _DEBUG
    LOGI("Debug build: always re-extracting assets");
#else
    if (!NeedsExtraction(basePath))
    {
        LOGI("Assets already extracted and up to date");
        // Change to the data directory so relative paths work
        if (chdir(basePath.c_str()) != 0)
        {
            LOGE("Failed to chdir to %s", basePath.c_str());
            return false;
        }
        return true;
    }
#endif

    LOGI("Extracting assets from APK...");
    MakeDirectoryRecursive(basePath);

    // Read the manifest
    SDL_RWops* manifestRW = SDL_RWFromFile("assets_manifest.txt", "r");
    if (!manifestRW)
    {
        LOGE("Failed to open assets_manifest.txt from APK");
        return false;
    }

    Sint64 manifestSize = SDL_RWsize(manifestRW);
    std::vector<char> manifestData(manifestSize + 1, 0);
    SDL_RWread(manifestRW, manifestData.data(), 1, manifestSize);
    SDL_RWclose(manifestRW);

    // Parse manifest and extract each file
    int totalFiles = 0;
    int extractedFiles = 0;
    int failedFiles = 0;

    char* line = strtok(manifestData.data(), "\n");
    while (line)
    {
        // Skip empty lines and whitespace
        while (*line == ' ' || *line == '\t' || *line == '\r') line++;
        if (*line != '\0')
        {
            totalFiles++;
            std::string assetPath(line);
            // Remove trailing \r if present
            while (!assetPath.empty() && (assetPath.back() == '\r' || assetPath.back() == ' '))
                assetPath.pop_back();

            std::string destPath = basePath + "/" + assetPath;

            if (ExtractFile(assetPath, destPath))
            {
                extractedFiles++;
                if (extractedFiles % 100 == 0)
                {
                    LOGI("Extracted %d/%d files...", extractedFiles, totalFiles);
                }
            }
            else
            {
                failedFiles++;
                // Continue even if some files fail
            }
        }
        line = strtok(nullptr, "\n");
    }

    LOGI("Asset extraction complete: %d extracted, %d failed out of %d total",
         extractedFiles, failedFiles, totalFiles);

    if (failedFiles > 0)
    {
        LOGE("Warning: %d files failed to extract", failedFiles);
    }

    // Write version marker to skip re-extraction next time
    WriteVersionMarker(basePath);

    // Change to the data directory so relative paths work
    if (chdir(basePath.c_str()) != 0)
    {
        LOGE("Failed to chdir to %s", basePath.c_str());
        return false;
    }

    LOGI("Working directory set to: %s", basePath.c_str());
    return true;
}

} // namespace AndroidAssets

#endif // PICASIM_ANDROID
