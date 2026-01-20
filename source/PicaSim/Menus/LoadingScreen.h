#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include "LoadingScreenHelper.h"
#include "Helpers.h"
#include <string>
#include <memory>

class Texture;
struct GameSettings;

class LoadingScreen : public LoadingScreenHelper
{
public:
    LoadingScreen(const char* initialText, struct GameSettings& gameSettings, bool showTips, bool clearOnExit, bool showProgress);
    ~LoadingScreen();

    void Update(const char* moduleName) OVERRIDE;

    void DisableProgressIndicator();

    void SetLabelColour(const Colour& colour);

private:
    void Render();

    GameSettings& mGameSettings;
    int64 mLastTimeMs;
    float mFraction;           // Progress animation 0.0-1.0
    bool mClearOnExit;
    bool mShowProgress;
    bool mProgressDisabled;

    // Textures
    std::unique_ptr<Texture> mBackgroundTexture;
    std::unique_ptr<Texture> mProgressTexture;  // Only if showProgress

    // Text
    std::string mLabelText;
    std::string mTipText;       // Empty if no tip
    Colour mLabelColour;

    // Progress image dimensions (in pixels)
    int32 mProgressImageWidth;
    int32 mProgressImageHeight;
};

#endif
