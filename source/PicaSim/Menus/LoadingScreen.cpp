#include "LoadingScreen.h"
#include "UIHelpers.h"
#include "../PicaStrings.h"
#include "../GameSettings.h"
#include "Texture.h"
#include "Platform.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();
void PrepareForIwGx(bool fullscreen);
void RecoverFromIwGx(bool clear);

//======================================================================================================================
static const char* GetTip(const struct GameSettings& gameSettings, const char** tips, size_t numTips)
{
    ++gameSettings.mStatistics.mLoadCounter;
    TRACE_FILE_IF(1) TRACE("GetTip: load counter = %d", gameSettings.mStatistics.mLoadCounter);
    const Language language = gameSettings.mOptions.mLanguage;

    if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_RACE)
    {
        static char txt[1024];
        sprintf(txt, TXT(PS_RACETIP), gameSettings.mChallengeSettings.mInfo.c_str());
        return txt;
    }
    else if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_LIMBO)
    {
        static char txt[1024];
        sprintf(txt, TXT(PS_LIMBOTIP), gameSettings.mChallengeSettings.mInfo.c_str());
        return txt;
    }
    else if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_DURATION)
    {
        static char txt[1024];
        sprintf(txt, TXT(PS_DURATIONTIP), gameSettings.mChallengeSettings.mInfo.c_str());
        return txt;
    }

    // if (gameSettings.mStatistics.mLoadCounter < 5)
    //   return 0;

    // const uint32 cycle = 4;
    // if (gameSettings.mStatistics.mLoadCounter % cycle)
    //   return 0;

    int tip = gameSettings.mStatistics.mLoadCounter % numTips;

    return tips[tip];
}

//======================================================================================================================
LoadingScreen::LoadingScreen(const char* initialText, struct GameSettings& gameSettings, bool showTips, bool clearOnExit, bool showProgress)
    : mFraction(0.0f)
    , mGameSettings(gameSettings)
    , mClearOnExit(clearOnExit)
    , mShowProgress(showProgress)
    , mProgressDisabled(false)
    , mBackgroundTexture(nullptr)
    , mProgressTexture(nullptr)
    , mProgressImageWidth(0)
    , mProgressImageHeight(0)
{
    TRACE_METHOD_ONLY(1);
    mLastTimeMs = Timer::GetMilliseconds();
    mLabelText = initialText ? initialText : "";
    mLabelColour.Set(0, 0, 0, 255);  // Black text

    const Language language = gameSettings.mOptions.mLanguage;
    int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
    int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

    PrepareForIwGx(false);

    // Load background texture
    TRACE_FILE_IF(1) TRACE("Creating background texture");
    mBackgroundTexture = std::make_unique<Texture>();
    mBackgroundTexture->LoadFromFile("Menus/StartBackground.jpg");
    mBackgroundTexture->SetMipMapping(false);
    if (gameSettings.mOptions.m16BitTextures)
    {
        mBackgroundTexture->SetFormatHW(CIwImage::RGB_565);
    }
    mBackgroundTexture->Upload();
    TRACE_FILE_IF(1) TRACE("Background texture loaded");

    // Load progress texture if needed
    if (showProgress)
    {
        TRACE_FILE_IF(1) TRACE("Loading progress texture");
        mProgressTexture = std::make_unique<Texture>();
        mProgressTexture->LoadFromFile("Menus/Progress.png");
        mProgressTexture->SetMipMapping(false);
        mProgressTexture->Upload();

        // Calculate progress image display size
        int texW = mProgressTexture->GetWidth();
        int texH = mProgressTexture->GetHeight();
        float ar = float(texW) / float(texH);
        float desiredWidth = width / 10.0f;
        float desiredHeight = desiredWidth / ar;
        mProgressImageWidth = (int32)desiredWidth;
        mProgressImageHeight = (int32)desiredHeight;
        TRACE_FILE_IF(1) TRACE("Progress texture loaded: %dx%d", mProgressImageWidth, mProgressImageHeight);
    }

    // Get tip text if enabled
    if (showTips)
    {
        const char* tips[] = {
            TXT(PS_TIPS1),
            TXT(PS_TIPS2),
            TXT(PS_TIPS3),
            TXT(PS_TIPS4),
            TXT(PS_TIPS5),
            TXT(PS_TIPS6),
            TXT(PS_TIPS7),
            TXT(PS_TIPS8),
            TXT(PS_TIPS9),
            TXT(PS_TIPS10),
            TXT(PS_TIPS11),
            TXT(PS_TIPS12),
            TXT(PS_TIPS13),
            TXT(PS_TIPS14),
            TXT(PS_TIPS15),
            TXT(PS_TIPS16),
        };
        int numTips = sizeof(tips) / sizeof(tips[0]);
        const char* tip = GetTip(gameSettings, tips, numTips);
        if (tip)
        {
            mTipText = tip;
        }
    }

    // Two empty updates to make sure something is rendered even if the first proper update is slow
    Update(0);
    Update(0);
}

//======================================================================================================================
LoadingScreen::~LoadingScreen()
{
    // unique_ptr automatically handles texture cleanup
    RecoverFromIwGx(mClearOnExit);
}

//======================================================================================================================
void LoadingScreen::DisableProgressIndicator()
{
    mProgressDisabled = true;
}

//======================================================================================================================
void LoadingScreen::SetLabelColour(const Colour& colour)
{
    mLabelColour = colour;
}

//======================================================================================================================
void LoadingScreen::Update(const char* moduleName)
{
    if (moduleName)
        TRACE_FILE_IF(1) TRACE("LoadingScreen::Update %s", moduleName);
    else
        TRACE_FILE_IF(1) TRACE("LoadingScreen::Update()");

    int64 currentTimeMs = Timer::GetMilliseconds();
    int32 deltaTimeMs = (int32)(currentTimeMs - mLastTimeMs);

    if (!moduleName && deltaTimeMs < 33)
        return;

    // Update label text if provided
    if (moduleName)
        mLabelText = moduleName;

    mLastTimeMs = currentTimeMs;

    // Update progress animation
    mFraction += (deltaTimeMs * 0.001f) * 0.25f;
    while (mFraction > 1.0f)
        mFraction -= 1.0f;

    // Render the screen
    Render();

    PollEvents();
}

//======================================================================================================================
void LoadingScreen::Render()
{
    // Get current window size (handles resizing)
    int width = Platform::GetDisplayWidth();
    int height = Platform::GetDisplayHeight();

    // Clear the screen
    IwGxClear();

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Draw background image (full screen with aspect-ratio-correct cropping)
    UIHelpers::DrawBackground(mBackgroundTexture.get());

    // Draw label text at top (10% from top)
    if (!mLabelText.empty())
    {
        ImU32 textColor = UIHelpers::ColourToImU32(mLabelColour);
        UIHelpers::DrawCenteredText(mLabelText.c_str(), 0.1f, textColor);
    }

    // Draw tip text at center (50%)
    if (!mTipText.empty())
    {
        UIHelpers::DrawCenteredText(mTipText.c_str(), 0.5f, UIHelpers::Colors::LightGray);
    }

    // Draw progress image at bottom
    if (mShowProgress && !mProgressDisabled && mProgressTexture)
    {
        GLuint texID = mProgressTexture->GetTextureID();
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        // Calculate position - slides from left to right
        float posX = mFraction * (width + mProgressImageWidth) - mProgressImageWidth;
        float posY = height * 0.95f - mProgressImageHeight;  // 5% from bottom

        drawList->AddImage(
            (ImTextureID)(intptr_t)texID,
            ImVec2(posX, posY),
            ImVec2(posX + mProgressImageWidth, posY + mProgressImageHeight)
        );
    }

    // End ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    IwGxSwapBuffers();
}
