#ifndef MENU_H
#define MENU_H

#include "Graphics.h"

#include <IwUI.h>
#include <string>

void MenuInit(const struct GameSettings& gameSettings);

void MenuTerminate();

/// Returns true if there was a change to the screen size that needs new resources
bool UpdateResourceGroupForScreen(const GameSettings& gameSettings);

void CacheThumbnailsFromDir(const char* path, bool convertTo16Bit, class LoadingScreen* loadingScreen, const char* txt);

void RenderCursor();

void OpenWebsite(const GameSettings& gameSettings);
void BuyFullVersion(const GameSettings& gameSettings);
void RateMe(const GameSettings& gameSettings);
void NewVersion();

/// Returns the number of images per screen when loading
float GetImagesPerLoadScreen(const GameSettings& gameSettings);

/// This loads the texture and caches it, or just returns a previously loaded texture. They all get released on MenuTerminate or ReleaseCachedTextures
Texture* GetCachedTexture(std::string path, bool convertTo16Bit);

void ReleaseCachedTextures();

CIwGxFont* GetFont(const GameSettings& gameSettings);

// Returns the texture that must be deleted when the image is no released
Texture*  AddCentredImage(const char* filename, bool use16Bit, CIwUIImage** image = 0);

struct FixStateForMenus
{
  FixStateForMenus(bool clearOnEnter, bool clearOnExit) : mClearOnExit(clearOnExit) {PrepareForIwGx(clearOnEnter);}
  ~FixStateForMenus() {RecoverFromIwGx(mClearOnExit);}
  bool mClearOnExit;
};

//---------------------------------------------------------------------------------------------------------------------
class Menu : public CIwUIElementEventHandler
{
public:
  Menu();
  ~Menu();

  // Handles the IwUI updates etc and returns the timestep in ms
  virtual int32 Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings);

protected:
  uint32 mUpdateCounter; // frame counter

  CIwUIScrollableView* mScrollArea;

private:
  uint64 mLastTime;
};


#endif

