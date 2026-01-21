#include "HelpMenu.h"
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
};

//======================================================================================================================
HelpMenu::HelpMenu(GameSettings& gameSettings, int initialTab)
    : mGameSettings(gameSettings)
    , mSelectedTab(initialTab)
    , mFinished(false)
{
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
    IwGxClear();
    Render();
    IwGxSwapBuffers();
    s3eDeviceYield();

    return mFinished;
}

//======================================================================================================================
void HelpMenu::Render()
{
    int width = Platform::GetScreenWidth();
    int height = Platform::GetScreenHeight();
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
    ImGui::Begin("HelpMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

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

    // Tab bar with scroll policy for many tabs
    // Increase frame padding to make tabs match button height
    float fontSize = ImGui::GetFontSize();
    float tabPaddingY = (buttonH - fontSize) * 0.5f;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f * scale, tabPaddingY));
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 4.0f * scale);
    if (ImGui::BeginTabBar("HelpTabs", ImGuiTabBarFlags_FittingPolicyScroll))
    {
        // Tab definitions
        struct TabInfo { int id; PicaString label; };
        TabInfo tabs[] = {
            {TAB_ABOUT, PS_ABOUT},
            {TAB_HOW_TO_FLY, PS_FLYING},
            {TAB_HOW_TO_CONFIGURE, PS_SETTINGS},
            {TAB_HOW_TO_RACE, PS_RACES},
            {TAB_TIPS, PS_TIPS},
            {TAB_OBJECTEDITING, PS_OBJECTEDITING},
            {TAB_KEYBOARD, PS_KEYBOARD},
            {TAB_JOYSTICKS, PS_JOYSTICK},
            {TAB_CREDITS, PS_CREDITS},
            {TAB_LICENCE, PS_LICENCE},
            {TAB_VERSIONS, PS_VERSIONS}
        };

        for (const auto& tab : tabs)
        {
            ImGuiTabItemFlags flags = 0;
            if (ImGui::BeginTabItem(GetPS(tab.label, language), nullptr, flags))
            {
                mSelectedTab = tab.id;
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::PopStyleVar(2);  // FramePadding and TabRounding

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

    // Reset scroll position for this tab if menu was just opened
    if (mResetScroll[mSelectedTab])
    {
        ImGui::SetScrollY(0.0f);
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
        sprintf(labelTxt, "Build info:\n%s\n%s", MARMALADE_VERSION_STRING_FULL, __TIMESTAMP__);
        ImGui::TextWrapped("%s", labelTxt);

        ImGui::Separator();

        int32 dpi = DPI::dpiGetScreenDPI();
        const char* deviceName = s3eDeviceGetString(S3E_DEVICE_ID);
        int32 numCores = s3eDeviceGetInt(S3E_DEVICE_NUM_CPU_CORES);

        // Calculate screen diagonal
        float diagonal = 0.0f;
        if (dpi > 0)
        {
            int w = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
            int h = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
            float d = hypotf((float)w, (float)h);
            diagonal = d / dpi;
        }

        sprintf(labelTxt, "Device info:\nName: %s\nCores: %d\nSurface: %d dpi %.2f\"",
            deviceName, numCores, dpi, diagonal);
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
    PrepareForIwGx(false);

    HelpMenu helpMenu(gameSettings, showHowToFly ? TAB_HOW_TO_FLY : TAB_ABOUT);

    while (!helpMenu.Update())
    {
        if (s3eDeviceCheckQuitRequest() ||
                (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) ||
                (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED))
        {
            break;
        }

        AudioManager::GetInstance().Update(1.0f / 30.0f);
    }

    RecoverFromIwGx(false);
}
