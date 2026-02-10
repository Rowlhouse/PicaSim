#include "WhatsNewMenu.h"
#include "Menu.h"
#include "UIHelpers.h"
#include "../GameSettings.h"
#include "../PicaStrings.h"
#include "../VersionInfo.h"
#include "Platform.h"
#include "../../Platform/S3ECompat.h"
#include "../../Platform/Input.h"

#include "ScrollHelper.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();

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

    ScrollHelper mScrollHelper;
};

//======================================================================================================================
WhatsNewMenu::WhatsNewMenu(GameSettings& gameSettings)
    : mGameSettings(gameSettings)
    , mFinished(false)
{
    UIHelpers::NotifyMenuTransition();
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

    // Suppress stale input from the previous menu's touch/click events
    if (UIHelpers::IsInputMuted())
        mFinished = false;

    return mFinished;
}

//======================================================================================================================
void WhatsNewMenu::Render()
{
    float scale = UIHelpers::GetFontScale();
    Language language = mGameSettings.mOptions.mLanguage;

    // Begin ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    UIHelpers::ApplyFontScale();

    // Apply menu style (light theme)
    PicaStyle::PushMenuStyle();

    // Full-screen window with safe area insets
    ImVec2 winSize = UIHelpers::BeginFullscreenWindow("WhatsNewMenu",
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    float width = winSize.x;
    float height = winSize.y;

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
    mScrollHelper.ApplyDragScroll("Content");
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

    WhatsNewMenu menu(gameSettings);

    while (!menu.Update())
    {
        if (CheckForQuitRequest() ||
                (Input::GetInstance().GetKeyState(SDLK_AC_BACK) & KEY_STATE_PRESSED) ||
                (Input::GetInstance().GetKeyState(SDLK_ESCAPE) & KEY_STATE_PRESSED))
        {
            break;
        }

        AudioManager::GetInstance().Update(1.0f / 30.0f);
    }

}
