#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H


// #include "Menu.h"
#include "ClassesMarmelade.h"
#include "LoadingScreenHelper.h"
#include "Helpers.h"

// #include <IwUI.h>

class LoadingScreen : public LoadingScreenHelper
{
public:
  LoadingScreen(const char* initialText, struct GameSettings& gameSettings, bool showTips, bool clearOnExit, bool showProgress);
  ~LoadingScreen();

  void Update(const char* moduleName) OVERRIDE;

  void DisableProgressIndicator();

  void SetLabelColour(const Colour& colour);

private:
  GameSettings& mGameSettings;
  int64_t mLastTimeMs;
  float mFraction;
  bool mClearOnExit;

  CIwUILabel* mLabel;
  CIwUILabel* mTipLabel;
  CIwUIImage* mProgressImage;
  int32_t mProgressImageWidth; // in pixels of the displayed image
  int32_t mProgressImageHeight; // in pixels of the displayed image
  Texture* mTexture;
};

#endif
