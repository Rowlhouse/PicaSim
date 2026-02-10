#include "VersionInfo.h"
#include "Trace.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

namespace VersionInfo {

static std::string sAllVersionsText;
static std::string sLatestVersionText;
static std::string sVersionString;
static bool sInitialized = false;

//======================================================================================================================
void Init()
{
    if (sInitialized)
        return;

    sInitialized = true;

    // Try to read VERSIONS.txt from current directory (data/)
    std::ifstream file("VERSIONS.txt");
    if (!file.is_open())
    {
        TRACE("VersionInfo: Could not open VERSIONS.txt");
        sAllVersionsText = "Version information not available.";
        sLatestVersionText = "Version information not available.";
        sVersionString = "unknown";
        return;
    }

    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    sAllVersionsText = buffer.str();
    file.close();

    // Parse version string from first line (e.g., "Version 1.0.0")
    std::istringstream stream(sAllVersionsText);
    std::string firstLine;
    if (std::getline(stream, firstLine))
    {
        // Extract version number after "Version "
        const char* prefix = "Version ";
        size_t prefixLen = strlen(prefix);
        if (firstLine.compare(0, prefixLen, prefix) == 0)
        {
            sVersionString = firstLine.substr(prefixLen);
            // Trim any trailing whitespace
            size_t end = sVersionString.find_last_not_of(" \t\r\n");
            if (end != std::string::npos)
                sVersionString = sVersionString.substr(0, end + 1);
        }
        else
        {
            sVersionString = "unknown";
        }
    }

    // Extract first version block for "What's New"
    // Find the second occurrence of "Version " to know where the first block ends
    size_t firstVersionPos = sAllVersionsText.find("Version ");
    if (firstVersionPos != std::string::npos)
    {
        size_t secondVersionPos = sAllVersionsText.find("Version ", firstVersionPos + 1);
        if (secondVersionPos != std::string::npos)
        {
            // Get text from start to second version, trim trailing whitespace
            sLatestVersionText = sAllVersionsText.substr(0, secondVersionPos);
            size_t end = sLatestVersionText.find_last_not_of(" \t\r\n");
            if (end != std::string::npos)
                sLatestVersionText = sLatestVersionText.substr(0, end + 1);
        }
        else
        {
            // Only one version block, use all of it
            sLatestVersionText = sAllVersionsText;
        }
    }
    else
    {
        sLatestVersionText = sAllVersionsText;
    }

    TRACE("VersionInfo: Loaded version %s", sVersionString.c_str());
}

//======================================================================================================================
const char* GetAllVersionsText()
{
    if (!sInitialized)
        Init();
    return sAllVersionsText.c_str();
}

//======================================================================================================================
const char* GetLatestVersionText()
{
    if (!sInitialized)
        Init();
    return sLatestVersionText.c_str();
}

//======================================================================================================================
const char* GetVersionString()
{
    if (!sInitialized)
        Init();
    return sVersionString.c_str();
}

//======================================================================================================================
void Terminate()
{
    sInitialized = false;
    sAllVersionsText.clear();
    sLatestVersionText.clear();
    sVersionString.clear();
}

} // namespace VersionInfo
