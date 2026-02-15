#include "HelpMenu.h"
#include "Menu.h"
#include "UIHelpers.h"
#include "../GameSettings.h"
#include "../PicaStrings.h"
#include "../VersionInfo.h"
#include "Platform.h"
#include "../../Platform/S3ECompat.h"
#include "../../Platform/Input.h"
#include "../../Platform/VRMenuRenderer.h"

#include "ScrollHelper.h"
#include "ScrollableTabStrip.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();

//======================================================================================================================
enum TabPanel
{
    TAB_ABOUT,
    TAB_HOW_TO_FLY,
    TAB_HOW_TO_CONFIGURE,
    TAB_HOW_TO_RACE,
    TAB_TIPS,
    TAB_OBJECTEDITING,
    TAB_KEYBOARD,
    TAB_JOYSTICKS,
    TAB_CREDITS,
    TAB_LICENCE,
    TAB_VERSIONS,
    TAB_NUM_TABS
};

//======================================================================================================================
class HelpMenu
{
public:
    HelpMenu(GameSettings& gameSettings, int initialTab);
    ~HelpMenu();

    bool Update();  // Returns true when finished

private:
    void Render();
    const char* GetTabContent(int tab);

    GameSettings& mGameSettings;
    int mSelectedTab;
    bool mFinished;
    bool mResetScroll[TAB_NUM_TABS];  // Reset scroll position per tab when menu opens

    ScrollHelper mScrollHelper;
    ScrollableTabStrip mTabStrip;
};

//======================================================================================================================
HelpMenu::HelpMenu(GameSettings& gameSettings, int initialTab)
    : mGameSettings(gameSettings)
    , mSelectedTab(initialTab)
    , mFinished(false)
{
    UIHelpers::NotifyMenuTransition();

    // Reset scroll positions for all tabs when menu opens
    for (int i = 0; i < TAB_NUM_TABS; ++i)
        mResetScroll[i] = true;
}

//======================================================================================================================
HelpMenu::~HelpMenu()
{
}

//======================================================================================================================
const char* HelpMenu::GetTabContent(int tab)
{
    Language lang = mGameSettings.mOptions.mLanguage;
    switch (tab)
    {
        case TAB_ABOUT:            return GetPS(PS_ABOUTPAIDTEXT, lang);
        case TAB_HOW_TO_FLY:       return GetPS(PS_HOWTOFLYTEXT, lang);
        case TAB_HOW_TO_CONFIGURE: return GetPS(PS_HOWTOCONFIGURETEXT, lang);
        case TAB_HOW_TO_RACE:      return GetPS(PS_HOWTORACETEXT, lang);
        case TAB_TIPS:             return GetPS(PS_TIPSTEXT, lang);
        case TAB_OBJECTEDITING:    return GetPS(PS_OBJECTEDITINGTEXT, lang);
        case TAB_KEYBOARD:         return GetPS(PS_KEYBOARDTEXT, lang);
        case TAB_JOYSTICKS:        return GetPS(PS_JOYSTICKSETUPTEXT, lang);
        case TAB_CREDITS:          return GetPS(PS_CREDITSTEXT, lang);
        case TAB_LICENCE:          return GetPS(PS_LICENCETEXT, lang);
        case TAB_VERSIONS:         return VersionInfo::GetAllVersionsText();
        default:                   return "";
    }
}

//======================================================================================================================
bool HelpMenu::Update()
{
    int savedTab = mSelectedTab;

    VRMenuRenderer::BeginMenuFrame();
    IwGxClear();
    Render();
    VRMenuRenderer::EndMenuFrame(mGameSettings.mOptions.mVRUIScale, mGameSettings.mOptions.mVROverlayDistance);
    PollEvents();

    // Suppress stale input from the previous menu's touch/click events
    if (UIHelpers::IsInputMuted())
    {
        mFinished = false;
        mSelectedTab = savedTab;
    }

    return mFinished;
}

//======================================================================================================================
void HelpMenu::Render()
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
    ImVec2 winSize = UIHelpers::BeginFullscreenWindow("HelpMenu",
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    float width = winSize.x;
    float height = winSize.y;

    float buttonH = 32.0f * scale;
    float padding = ImGui::GetStyle().WindowPadding.y;

    // === TOP ROW: Back button + Tab bar ===
    if (ImGui::Button(GetPS(PS_BACK, language), ImVec2(0, buttonH)))
    {
        mFinished = true;
    }

    ImGui::SameLine();

    // Add spacing between Back button and tabs
    ImGui::Dummy(ImVec2(10.0f * scale, 0));
    ImGui::SameLine();

    // Scrollable tab strip
    const char* tabLabels[] = {
        GetPS(PS_ABOUT, language),
        GetPS(PS_FLYING, language),
        GetPS(PS_SETTINGS, language),
        GetPS(PS_RACES, language),
        GetPS(PS_TIPS, language),
        GetPS(PS_OBJECTEDITING, language),
        GetPS(PS_KEYBOARD, language),
        GetPS(PS_JOYSTICK, language),
        GetPS(PS_CREDITS, language),
        GetPS(PS_LICENCE, language),
        GetPS(PS_VERSIONS, language)
    };
    mTabStrip.Render("HelpTabs", tabLabels, TAB_NUM_TABS, mSelectedTab, buttonH);

    // === MAIN CONTENT AREA (scrollable) ===
    // Calculate: content fills space between current position and bottom button
    float topY = ImGui::GetCursorPosY();
    float bottomButtonY = height - buttonH - padding;
    float contentHeight = bottomButtonY - topY - padding;

    // Use per-tab child window IDs so each tab maintains its own scroll position
    static const char* tabContentIds[TAB_NUM_TABS] = {
        "Content_About", "Content_HowToFly", "Content_HowToConfigure", "Content_HowToRace",
        "Content_Tips", "Content_ObjectEditing", "Content_Keyboard", "Content_Joysticks",
        "Content_Credits", "Content_Licence", "Content_Versions"
    };
    ImGui::BeginChild(tabContentIds[mSelectedTab], ImVec2(-1, contentHeight), true);
    mScrollHelper.ApplyDragScroll(tabContentIds[mSelectedTab]);

    // Reset scroll position for this tab if menu was just opened
    if (mResetScroll[mSelectedTab])
    {
        ImGui::SetScrollY(0.0f);
        mScrollHelper.ResetScroll(tabContentIds[mSelectedTab]);
        mResetScroll[mSelectedTab] = false;
    }

    const char* content = GetTabContent(mSelectedTab);

    // Word-wrapped text
    ImGui::TextWrapped("%s", content);

    // Special: TAB_VERSIONS gets build/device info appended
    if (mSelectedTab == TAB_VERSIONS)
    {
        ImGui::Separator();

        char labelTxt[256];
        sprintf(labelTxt, "Build info:\n%s", __TIMESTAMP__);
        ImGui::TextWrapped("%s", labelTxt);

        ImGui::Separator();

        int32 dpi = (int32)Platform::GetScreenDPI();
        int32 numCores = Platform::GetCPUCount();
        float diagonal = Platform::GetSurfaceDiagonalInches();

        sprintf(labelTxt, "Device info:\nCores: %d\nSurface: %d dpi %.2f\"",
            numCores, dpi, diagonal);
        ImGui::TextWrapped("%s", labelTxt);
    }

    ImGui::EndChild();

    // === BOTTOM: Website button (full width) - position at bottom ===
    ImGui::SetCursorPosY(bottomButtonY);
    if (ImGui::Button(GetPS(PS_WEBSITE, language), ImVec2(-1, buttonH)))
    {
        OpenWebsite(mGameSettings);
    }

    ImGui::End();
    PicaStyle::PopMenuStyle();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//======================================================================================================================
void DisplayHelpMenu(GameSettings& gameSettings, bool showHowToFly)
{
    AudioManager::GetInstance().SetAllChannelsToZeroVolume();

    HelpMenu helpMenu(gameSettings, showHowToFly ? TAB_HOW_TO_FLY : TAB_ABOUT);

    while (!helpMenu.Update())
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
