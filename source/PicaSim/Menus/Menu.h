#ifndef MENU_H
#define MENU_H

#include "Graphics.h"
#include "../../Platform/S3ECompat.h"
#include <string>

void MenuInit(const struct GameSettings& gameSettings);

void MenuTerminate();

/// Returns true if there was a change to the screen size that needs new resources
bool UpdateResourceGroupForScreen(const GameSettings& gameSettings);

void CacheThumbnailsFromDir(const char* path, bool convertTo16Bit, class LoadingScreen* loadingScreen, const char* txt);

void OpenWebsite(const GameSettings& gameSettings);
void NewVersion();

/// Returns the number of images per screen when loading
float GetImagesPerLoadScreen(const GameSettings& gameSettings);

/// This loads the texture and caches it, or just returns a previously loaded texture. They all get released on MenuTerminate or ReleaseCachedTextures
Texture* GetCachedTexture(std::string path, bool convertTo16Bit);

void ReleaseCachedTextures();


#endif

