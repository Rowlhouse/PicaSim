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

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <memory>

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();
void PrepareForIwGx(bool fullscreen);
void RecoverFromIwGx(bool clear);

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
    PrepareForIwGx(false);
    LoadTextures();
}

//======================================================================================================================
StartMenu::~StartMenu()
{
    // unique_ptr handles texture cleanup
    RecoverFromIwGx(false);
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
    IwGxClear();
    Render();
    IwGxSwapBuffers();

    s3eDeviceYield();

    // Check for quit request
    if (s3eDeviceCheckQuitRequest())
    {
        mResult = STARTMENU_QUIT;
    }

    return mResult;
}

//======================================================================================================================
void StartMenu::Render()
{
    int width = Platform::GetScreenWidth();
    int height = Platform::GetScreenHeight();
    float scale = UIHelpers::GetFontScale();

    // Begin ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Apply font scaling for button text
    UIHelpers::ApplyFontScale();

    // Draw background
    UIHelpers::DrawBackground(mBackgroundTexture.get());

    // Calculate sizes
    float iconSize = 48.0f * scale;
    float mainButtonW = 200.0f * scale;
    float mainButtonH = 50.0f * scale;
    float smallIconSize = iconSize * 0.75f;

    // Create full-screen invisible window for button placement
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
    ImGui::Begin("StartMenu", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    // Exit button (top-left) - not on iPhone/Win10
#if !defined(PICASIM_IOS) && !defined(PICASIM_WIN10)
    if (UIHelpers::DrawImageButton("exit", mExitTexture.get(),
                                   width * 0.02f, height * 0.02f, smallIconSize))
    {
        mResult = STARTMENU_QUIT;
    }
#endif

#if defined(PICASIM_WINDOWS)
    // NewVersion button (top-right) - Windows desktop only
    if (mNewVersionTexture && IsNewVersionAvailable())
    {
        if (UIHelpers::DrawImageButton("newversion", mNewVersionTexture.get(),
                                       width - smallIconSize - width * 0.02f, height * 0.02f, smallIconSize))
        {
            NewVersion();
        }
    }
#endif

    // Style for main buttons (using unified style system)
    PicaStyle::PushStartMenuButtonStyle(scale);

    // Free-Fly button (left side, ~20% from top)
    ImGui::SetCursorPos(ImVec2(width * 0.1f, height * 0.18f));
    if (ImGui::Button("Free-Fly", ImVec2(mainButtonW, mainButtonH)))
    {
        mResult = STARTMENU_FLY;
    }

    // Challenge button (right side, ~20% from top)
    ImGui::SetCursorPos(ImVec2(width * 0.9f - mainButtonW, height * 0.18f));
    if (ImGui::Button("Challenge", ImVec2(mainButtonW, mainButtonH)))
    {
        mResult = STARTMENU_CHALLENGE;
    }

    PicaStyle::PopStartMenuButtonStyle();

    // Info label (gamepad detected) - Windows only
#if defined(PICASIM_WINDOWS)
    if (!mGameSettings.mJoystickSettings.mEnableJoystick && JoystickAvailable())
    {
        UIHelpers::DrawCenteredText("Gamepad detected. Enable in settings to use it.",
                                     0.5f, UIHelpers::Colors::Yellow);
    }
#endif

    // Bottom icon row
    float bottomY = height * 0.88f;
    float iconSpacing = iconSize * 1.5f;

    // Facebook (left side)
    float iconX = width * 0.05f;
    if (UIHelpers::DrawImageButton("facebook", mFacebookTexture.get(),
                                   iconX, bottomY, iconSize))
    {
        Platform::OpenURL("http://www.facebook.com/233467446753191");
    }

    // Help and Settings (right side)
    iconX = width - iconSpacing * 2 - width * 0.02f;
    if (UIHelpers::DrawImageButton("help", mHelpTexture.get(),
                                   iconX, bottomY, iconSize))
    {
        mResult = STARTMENU_HELP;
    }

    iconX += iconSpacing;
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
            if (s3eDeviceCheckQuitRequest() ||
                    (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED))
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
