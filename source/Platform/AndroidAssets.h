#ifndef ANDROID_ASSETS_H
#define ANDROID_ASSETS_H

#ifdef PICASIM_ANDROID

namespace AndroidAssets
{

/**
 * Extract APK assets to internal storage if needed.
 * On first launch (or after app update), reads assets_manifest.txt from the APK
 * and copies all listed files to SDL_AndroidGetInternalStoragePath().
 * Subsequent launches skip extraction if the version marker matches.
 *
 * After extraction, chdir() to the extraction directory so all relative
 * paths (SystemData/, SystemSettings/, Fonts/, etc.) work as-is.
 *
 * Returns true on success, false on failure.
 */
bool ExtractIfNeeded();

} // namespace AndroidAssets

#endif // PICASIM_ANDROID
#endif // ANDROID_ASSETS_H
