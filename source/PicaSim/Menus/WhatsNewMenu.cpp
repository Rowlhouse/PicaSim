#include "WhatsNewMenu.h"
#include "Menu.h"
#include "UIHelpers.h"
#include "../GameSettings.h"
#include "../PicaStrings.h"
#include "../VersionInfo.h"
#include "Platform.h"
#include "../../Platform/S3ECompat.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();
void PrepareForIwGx(bool fullscreen);
void RecoverFromIwGx(bool clear);

//======================================================================================================================
class WhatsNewMenu
{
public:
    WhatsNewMenu(GameSettings& gameSettings);
    ~WhatsNewMenu();

    bool Update();  // Returns true when finished

private:
    void Render();

    GameSettings& mGameSettings;
    bool mFinished;
};

//======================================================================================================================
WhatsNewMenu::WhatsNewMenu(GameSettings& gameSettings)
    : mGameSettings(gameSettings)
    , mFinished(false)
{
}

//======================================================================================================================
WhatsNewMenu::~WhatsNewMenu()
{
}

//======================================================================================================================
bool WhatsNewMenu::Update()
{
    IwGxClear();
    Render();
    IwGxSwapBuffers();
    PollEvents();

    return mFinished;
}

//======================================================================================================================
void WhatsNewMenu::Render()
{
    int width = Platform::GetDisplayWidth();
    int height = Platform::GetDisplayHeight();
    float scale = UIHelpers::GetFontScale();
    Language language = mGameSettings.mOptions.mLanguage;

    // Begin ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    UIHelpers::ApplyFontScale();

    // Apply menu style (light theme)
    PicaStyle::PushMenuStyle();

    // Full-screen window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
    ImGui::Begin("WhatsNewMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    float buttonH = 32.0f * scale;
    float padding = ImGui::GetStyle().WindowPadding.y;

    // === TITLE: Centered "Welcome to PicaSim!" ===
    const char* title = GetPS(PS_WHATSNEW, language);
    float titleWidth = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((width - titleWidth) * 0.5f);
    ImGui::Text("%s", title);

    // === MAIN CONTENT AREA (scrollable) ===
    float topY = ImGui::GetCursorPosY();
    float bottomButtonY = height - buttonH - padding;
    float contentHeight = bottomButtonY - topY - padding;

    ImGui::BeginChild("Content", ImVec2(-1, contentHeight), true);
    ImGui::TextWrapped("%s", VersionInfo::GetLatestVersionText());
    ImGui::EndChild();

    // === BOTTOM: OK button (full width) ===
    ImGui::SetCursorPosY(bottomButtonY);
    if (ImGui::Button(GetPS(PS_OK, language), ImVec2(-1, buttonH)))
    {
        mFinished = true;
    }

    ImGui::End();
    PicaStyle::PopMenuStyle();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//======================================================================================================================
void DisplayWhatsNewMenu(GameSettings& gameSettings)
{
    AudioManager::GetInstance().SetAllChannelsToZeroVolume();
    PrepareForIwGx(false);

    WhatsNewMenu menu(gameSettings);

    while (!menu.Update())
    {
        if (CheckForQuitRequest() ||
                (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) ||
                (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED))
        {
            break;
        }

        AudioManager::GetInstance().Update(1.0f / 30.0f);
    }

    RecoverFromIwGx(false);
}
