#ifndef VERSIONINFO_H
#define VERSIONINFO_H

namespace VersionInfo {

// Load and parse VERSIONS.txt - call early in startup
void Init();

// Get full version history text (for Help > Versions tab)
const char* GetAllVersionsText();

// Get just the latest version block (for What's New popup)
const char* GetLatestVersionText();

// Get version string (e.g., "1.0.0")
const char* GetVersionString();

} // namespace VersionInfo

#endif // VERSIONINFO_H
