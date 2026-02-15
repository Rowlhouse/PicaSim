#include "StartMenu.h"
#include "SettingsMenu.h"
#include "HelpMenu.h"
#include "Menu.h"
#include "UIHelpers.h"
#include "../VersionChecker.h"
#include "../PicaJoystick.h"
#include "Texture.h"
#include "Platform.h"
#include "../../Platform/S3ECompat.h"
#include "../../Platform/Input.h"
#include "../../Platform/VRMenuRenderer.h"
#include "../../Framework/Trace.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <memory>

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();

//======================================================================================================================
class StartMenu
{
public:
    StartMenu(GameSettings& gameSettings);
    ~StartMenu();

    // Render one frame, returns the result (STARTMENU_MAX if no action yet)
    StartMenuResult Update();

private:
    void LoadTextures();
    void Render();

    GameSettings& mGameSettings;
    StartMenuResult mResult;

    // Textures
    std::unique_ptr<Texture> mBackgroundTexture;
    std::unique_ptr<Texture> mFacebookTexture;
    std::unique_ptr<Texture> mHelpTexture;
    std::unique_ptr<Texture> mSettingsTexture;
    std::unique_ptr<Texture> mExitTexture;
    std::unique_ptr<Texture> mNewVersionTexture;
};

//======================================================================================================================
StartMenu::StartMenu(GameSettings& gameSettings)
    : mGameSettings(gameSettings)
    , mResult(STARTMENU_MAX)
{
    UIHelpers::NotifyMenuTransition();
    LoadTextures();
}

//======================================================================================================================
StartMenu::~StartMenu()
{
    // unique_ptr handles texture cleanup
}

//======================================================================================================================
void StartMenu::LoadTextures()
{
    // Background
    mBackgroundTexture = std::make_unique<Texture>();
    mBackgroundTexture->LoadFromFile("Menus/StartBackground.jpg");
    mBackgroundTexture->SetMipMapping(false);
    mBackgroundTexture->Upload();

    // Icon buttons
    mFacebookTexture = std::make_unique<Texture>();
    mFacebookTexture->LoadFromFile("Menus/Facebook.png");
    mFacebookTexture->SetMipMapping(false);
    mFacebookTexture->Upload();

    mHelpTexture = std::make_unique<Texture>();
    mHelpTexture->LoadFromFile("Menus/Help.png");
    mHelpTexture->SetMipMapping(false);
    mHelpTexture->Upload();

    mSettingsTexture = std::make_unique<Texture>();
    mSettingsTexture->LoadFromFile("Menus/Utilities.png");
    mSettingsTexture->SetMipMapping(false);
    mSettingsTexture->Upload();

    mExitTexture = std::make_unique<Texture>();
    mExitTexture->LoadFromFile("Menus/Stop.png");
    mExitTexture->SetMipMapping(false);
    mExitTexture->Upload();

#if defined(PICASIM_WINDOWS)
    // NewVersion texture (Windows only)
    mNewVersionTexture = std::make_unique<Texture>();
    mNewVersionTexture->LoadFromFile("Menus/NewVersion.png");
    mNewVersionTexture->SetMipMapping(false);
    mNewVersionTexture->Upload();
#endif
}

//======================================================================================================================
StartMenuResult StartMenu::Update()
{
    mResult = STARTMENU_MAX;

    // Clear and render
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Update - glClear");
    VRMenuRenderer::BeginMenuFrame();
    IwGxClear();
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Update - Render");
    Render();
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Update - SwapBuffers");
    VRMenuRenderer::EndMenuFrame(mGameSettings.mOptions.mVRUIScale, mGameSettings.mOptions.mVROverlayDistance);
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Update - PollEvents");
    PollEvents();
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Update - done");

    // Suppress stale input from the previous menu's touch/click events
    if (UIHelpers::IsInputMuted())
        mResult = STARTMENU_MAX;

    // Check for quit request
    if (CheckForQuitRequest())
    {
        mResult = STARTMENU_QUIT;
    }

    return mResult;
}

//======================================================================================================================
void StartMenu::Render()
{
    float scale = UIHelpers::GetFontScale();

    // Begin ImGui frame
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Render - ImGui NewFrame");
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    TRACE_FILE_IF(FRAME_1) TRACE("StartMenu::Render - drawing UI");

    // Apply font scaling for button text
    UIHelpers::ApplyFontScale();

    // Draw background
    UIHelpers::DrawBackground(mBackgroundTexture.get());

    // Calculate sizes
    float iconSize = 60.0f * scale;
    float mainButtonW = 200.0f * scale;
    float mainButtonH = 70.0f * scale;
    float buttonInsetAmount = 0.01f;

    // Create full-screen invisible window for button placement
    ImVec2 winSize = UIHelpers::BeginFullscreenWindow("StartMenu",
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);
    float width = winSize.x;
    float height = winSize.y;

    // Exit button (top-left) - not on iPhone/Win10
#if !defined(PICASIM_IOS) && !defined(PICASIM_WIN10)
    if (UIHelpers::DrawImageButton("exit", mExitTexture.get(),
                                   width * buttonInsetAmount, height * buttonInsetAmount, iconSize))
    {
        mResult = STARTMENU_QUIT;
    }
#endif

#if defined(PICASIM_WINDOWS)
    // NewVersion button (top-right) - Windows desktop only
    if (mNewVersionTexture && IsNewVersionAvailable())
    {
        if (UIHelpers::DrawImageButton(
            "newversion", mNewVersionTexture.get(),
            width - iconSize - width * buttonInsetAmount, height * buttonInsetAmount, iconSize))
        {
            NewVersion();
        }
    }
#endif

    // Style for main buttons (using unified style system)
    PicaStyle::PushStartMenuButtonStyle(scale);

    Language language = mGameSettings.mOptions.mLanguage;

    // Free-Fly button (left side, ~20% from top)
    ImGui::SetCursorPos(ImVec2(width * 0.1f, height * 0.2f));
    if (ImGui::Button(TXT(PS_FREEFLY), ImVec2(mainButtonW, mainButtonH)))
    {
        mResult = STARTMENU_FLY;
    }

    // Challenge button (right side, ~20% from top)
    ImGui::SetCursorPos(ImVec2(width * 0.9f - mainButtonW, height * 0.2f));
    if (ImGui::Button(TXT(PS_CHALLENGE), ImVec2(mainButtonW, mainButtonH)))
    {
        mResult = STARTMENU_CHALLENGE;
    }

    PicaStyle::PopStartMenuButtonStyle();

    // Info label (gamepad detected) - Windows only
#if defined(PICASIM_WINDOWS)
    if (!mGameSettings.mJoystickSettings.mEnableJoystick && JoystickAvailable())
    {
        Language language = mGameSettings.mOptions.mLanguage;
        UIHelpers::DrawCenteredText(TXT(PS_GAMEPADDETECTED),
                                     0.5f, UIHelpers::Colors::Yellow);
    }
#endif

    // Bottom icon row — keep icons above bottom edge at small window sizes
    float bottomY = std::min(height * 0.88f, height - iconSize - height * buttonInsetAmount);
    float iconSpacing = iconSize * 1.5f;

    // Facebook (left side)
    float iconX = width * buttonInsetAmount;
    if (UIHelpers::DrawImageButton("facebook", mFacebookTexture.get(),
                                   iconX, bottomY, iconSize))
    {
        Platform::OpenURL("www.facebook.com/233467446753191");
    }

    // Help (centered)
    iconX = (width - iconSize) * 0.5f;
    if (UIHelpers::DrawImageButton("help", mHelpTexture.get(),
                                   iconX, bottomY, iconSize))
    {
        mResult = STARTMENU_HELP;
    }

    // Settings (right side)
    iconX = width - iconSpacing - width * buttonInsetAmount;
    if (UIHelpers::DrawImageButton("settings", mSettingsTexture.get(),
                                   iconX, bottomY, iconSize))
    {
        mResult = STARTMENU_SETTINGS;
    }

    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//======================================================================================================================
StartMenuResult DisplayStartMenu(GameSettings& gameSettings)
{
    StartMenuResult result = STARTMENU_MAX;

    while (result == STARTMENU_MAX)
    {
        StartMenu startMenu(gameSettings);

        // Menu loop
        while (true)
        {
            result = startMenu.Update();

            if (result != STARTMENU_MAX)
                break;

            // Check for back button / quit
            if (CheckForQuitRequest() ||
                    (Input::GetInstance().GetKeyState(SDLK_AC_BACK) & KEY_STATE_PRESSED))
            {
                result = STARTMENU_QUIT;
                break;
            }
        }

        // Handle sub-menus (return to start menu after)
        if (result == STARTMENU_SETTINGS)
        {
            SettingsChangeActions actions;
            DisplaySettingsMenu(gameSettings, actions);
            result = STARTMENU_MAX;  // Return to start menu
        }
        else if (result == STARTMENU_HELP)
        {
            DisplayHelpMenu(gameSettings, false);
            result = STARTMENU_MAX;  // Return to start menu
        }
        else if (result == STARTMENU_REFRESH)
        {
            result = STARTMENU_MAX;  // Just refresh
        }
    }

    return result;
}
