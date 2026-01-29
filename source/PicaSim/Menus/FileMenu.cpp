#include "FileMenu.h"
#include "UIHelpers.h"
#include "../GameSettings.h"
#include "../PicaStrings.h"
#include "../../Platform/S3ECompat.h"
#include "Platform.h"
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();
void PrepareForIwGx(bool fullscreen);
void RecoverFromIwGx(bool clear);

// Forward declaration from Helpers.cpp
float GetImagesPerLoadScreen(const GameSettings& gameSettings);
Texture* GetCachedTexture(std::string path, bool convertTo16Bit);

static const size_t MAX_FILENAME_LEN = 48;

static const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=_+&!(){}[]#";
static const size_t numValidChars = sizeof(validChars);

//======================================================================================================================
bool IsCharValid(char c)
{
    for (size_t i = 0 ; i != numValidChars-1 ; ++i)
    {
        if (c == validChars[i])
            return true;
    }
    return false;
}

//======================================================================================================================
void RemoveInvalidCharacters(std::string& text)
{
    char result[MAX_FILENAME_LEN+1] = "";
    const char* origText = text.c_str();

    size_t origLen = strlen(origText);
    if (origLen > MAX_FILENAME_LEN)
        origLen = MAX_FILENAME_LEN;

    for (size_t i = 0 ; i != origLen ; ++i)
    {
        result[i] = origText[i];
        if (!IsCharValid(result[i]))
            result[i] = '_';
    }
    result[origLen] = 0;
    text = result;
}

//======================================================================================================================
void RemoveExtension(char* filename, const char* extension)
{
    if (!extension || !filename)
        return;
    size_t extensionLen = strlen(extension);
    size_t filenameLen = strlen(filename);

    if (extensionLen > filenameLen)
        return;

    for (size_t i = extensionLen ; i-- != 0 ; )
    {
        char ext = extension[i];
        if (filename[filenameLen - extensionLen + i] != ext)
        {
            return;
        }
    }
    filename[(filenameLen - extensionLen)] = 0;
}

//======================================================================================================================
void RemovePath(char* filename)
{
    int origLen = strlen(filename);
    for (int i = origLen-1 ; i != -1 ; --i)
    {
        if (filename[i] == '/' || filename[i] == '\\')
        {
            if (i == 0)
                return;
            int j = 0;
            for (++i ; i != origLen + 1 ; ++i, ++j)
            {
                filename[j] = filename[i];
            }
            return;
        }
    }
}

//======================================================================================================================
struct FileMenuItem
{
    std::string fullPath;      // Full path for selection result
    std::string displayName;   // Filename without extension
    std::string title;         // Title from settings file (or displayName)
    std::string info;          // Description from settings file
    Texture* thumbnail;        // NULL for FILEMENUTYPE_FILE
    uint32 typeMask;           // For tab filtering (0xFFFFFFFF = all tabs)
    bool isFromUserPath;       // True if from UserSettings directory
};

//======================================================================================================================
class FileMenu
{
public:
    enum Mode { LOAD, SAVE, DELETE_MODE };  // DELETE is a Windows macro

    FileMenu(const GameSettings& gameSettings,
                      const char* systemPath, const char* userPath,
                      const char* extension, Mode mode, const char* title,
                      const char* tabTitles[], size_t numTabs, int initialTab,
                      const char* cancelButtonText, const char* altButtonText,
                      FileMenuType fileMenuType, float imagesPerScreen,
                      IncludeCallback* includeCallback);
    ~FileMenu();

    bool Update();  // Returns true when finished
    int GetSelectedTab() const { return mSelectedTab; }
    FileMenuResult GetResult(std::string& selectedPath) const;
    bool GetFinished() const { return mFinished; }

private:
    void LoadFilesFromDirectory(const char* path, bool isUserPath, bool useTitleFromFile);
    void Render();
    bool PassesTabFilter(const FileMenuItem& item) const;

    // Configuration
    const GameSettings& mGameSettings;
    std::string mSystemPath;
    std::string mUserPath;
    std::string mExtension;
    Mode mMode;
    std::string mTitle;
    std::vector<std::string> mTabTitles;
    std::string mCancelText;
    std::string mAltText;
    FileMenuType mFileMenuType;
    float mImagesPerScreen;
    IncludeCallback* mIncludeCallback;

    // State
    std::vector<FileMenuItem> mItems;
    int mSelectedTab;
    bool mFinished;
    FileMenuResult mResult;
    std::string mSelectedPath;
    std::vector<bool> mResetScroll;  // Reset scroll position per tab when menu opens
    bool mForceFirstTab;  // Force first tab selection on first render

    // For SAVE mode
    char mFilenameBuffer[MAX_FILENAME_LEN + 1];
};

//======================================================================================================================
FileMenu::FileMenu(const GameSettings& gameSettings,
                   const char* systemPath, const char* userPath,
                   const char* extension, Mode mode, const char* title,
                   const char* tabTitles[], size_t numTabs, int initialTab,
                   const char* cancelButtonText, const char* altButtonText,
                   FileMenuType fileMenuType, float imagesPerScreen,
                   IncludeCallback* includeCallback)
    : mGameSettings(gameSettings)
    , mSystemPath(systemPath ? systemPath : "")
    , mUserPath(userPath ? userPath : "")
    , mExtension(extension ? extension : "")
    , mMode(mode)
    , mTitle(title ? title : "")
    , mCancelText(cancelButtonText ? cancelButtonText : "")
    , mAltText(altButtonText ? altButtonText : "")
    , mFileMenuType(fileMenuType)
    , mImagesPerScreen(imagesPerScreen)
    , mIncludeCallback(includeCallback)
    , mSelectedTab(initialTab)
    , mFinished(false)
    , mResult(FILEMENURESULT_CANCEL)
    , mForceFirstTab(true)
{
    memset(mFilenameBuffer, 0, sizeof(mFilenameBuffer));

    // Copy tab titles
    if (tabTitles && numTabs > 0)
    {
        for (size_t i = 0; i < numTabs; ++i)
        {
            mTabTitles.push_back(tabTitles[i] ? tabTitles[i] : "");
        }
    }

    // Initialize reset scroll flags for all tabs
    size_t numScrollTabs = numTabs > 0 ? numTabs : 1;
    mResetScroll.resize(numScrollTabs, true);

    // Load files based on mode
    if (mMode == LOAD)
    {
        // System files first, then user files (both load metadata from file)
        if (!mSystemPath.empty())
            LoadFilesFromDirectory(mSystemPath.c_str(), false, true);
        if (!mUserPath.empty())
            LoadFilesFromDirectory(mUserPath.c_str(), true, true);
    }
    else
    {
        // SAVE and DELETE only show user files
        if (!mUserPath.empty())
            LoadFilesFromDirectory(mUserPath.c_str(), true, false);
    }
}

//======================================================================================================================
FileMenu::~FileMenu()
{
}

//======================================================================================================================
void FileMenu::LoadFilesFromDirectory(const char* path, bool isUserPath, bool useTitleFromFile)
{
    TRACE_FILE_IF(1) TRACE("FileMenu::LoadFilesFromDirectory: path='%s' isUserPath=%d", path, isUserPath);
    s3eFileList* fileList = s3eFileListDirectory(path);
    if (!fileList)
    {
        TRACE_FILE_IF(1) TRACE("FileMenu::LoadFilesFromDirectory: failed to open directory '%s'", path);
        return;
    }

    const int filenameLen = 512;
    char filename[filenameLen];

    while (s3eFileListNext(fileList, filename, filenameLen) == S3E_RESULT_SUCCESS)
    {
        std::string fullPath = std::string(path) + "/" + filename;

        // Apply include callback filter if provided
        if (mIncludeCallback && !mIncludeCallback->GetInclude(fullPath.c_str()))
            continue;

        FileMenuItem item;
        item.fullPath = fullPath;
        item.isFromUserPath = isUserPath;
        item.typeMask = 0xFFFFFFFF;  // All tabs by default
        item.thumbnail = nullptr;

        // Strip extension for display name
        RemoveExtension(filename, mExtension.c_str());
        item.displayName = filename;

        // Load metadata based on file type
        if (mFileMenuType != FILEMENUTYPE_FILE && useTitleFromFile)
        {
            switch (mFileMenuType)
            {
                case FILEMENUTYPE_AEROPLANE:
                {
                    AeroplaneSettings as;
                    as.LoadBasicsFromFile(fullPath);
                    item.title = as.mTitle;
                    item.info = as.mInfo;
                    item.typeMask = as.mType;
                    if (!as.mThumbnail.empty())
                        item.thumbnail = GetCachedTexture(as.mThumbnail, mGameSettings.mOptions.m16BitTextures);
                    break;
                }
                case FILEMENUTYPE_SCENERY:
                {
                    EnvironmentSettings es;
                    es.LoadBasicsFromFile(fullPath);
                    item.title = es.mTitle;
                    item.info = es.mInfo;
                    item.typeMask = es.mType;
                    if (!es.mThumbnail.empty())
                        item.thumbnail = GetCachedTexture(es.mThumbnail, mGameSettings.mOptions.m16BitTextures);
                    break;
                }
                case FILEMENUTYPE_CHALLENGE:
                {
                    ChallengeSettings cs;
                    cs.LoadBasicsFromFile(fullPath);
                    item.title = cs.mTitle;
                    item.info = cs.mInfo;
                    if (!cs.mThumbnail.empty())
                        item.thumbnail = GetCachedTexture(cs.mThumbnail, mGameSettings.mOptions.m16BitTextures);
                    break;
                }
                case FILEMENUTYPE_LIGHTING:
                {
                    LightingSettings ls;
                    ls.LoadBasicsFromFile(fullPath);
                    item.title = ls.mTitle;
                    item.info = ls.mInfo;
                    if (!ls.mThumbnail.empty())
                        item.thumbnail = GetCachedTexture(ls.mThumbnail, mGameSettings.mOptions.m16BitTextures);
                    break;
                }
                default:
                    item.title = item.displayName;
                    break;
            }
        }
        else
        {
            // Use filename as title
            item.title = item.displayName;
        }

        mItems.push_back(item);
    }
    s3eFileListClose(fileList);
}

//======================================================================================================================
bool FileMenu::PassesTabFilter(const FileMenuItem& item) const
{
    if (mSelectedTab == 0 || mTabTitles.empty())
        return true;  // Tab 0 = "All" or no tabs

    // Tab filtering uses bitmask: typeMask bits correspond to tabs (bit 0 = tab 1, bit 1 = tab 2, etc.)
    // Note: (1 << -1) produces 0xFFFFFFFF, so tab 0 ("All") matches everything
    return (item.typeMask & (1 << (mSelectedTab - 1))) != 0;
}

//======================================================================================================================
FileMenuResult FileMenu::GetResult(std::string& selectedPath) const
{
    selectedPath = mSelectedPath;
    return mResult;
}

//======================================================================================================================
bool FileMenu::Update()
{
    IwGxClear();
    Render();
    IwGxSwapBuffers();
    PollEvents();

    return mFinished;
}

//======================================================================================================================
void FileMenu::Render()
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
    ImGui::Begin("FileMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    float buttonH = 32.0f * scale;
    float padding = ImGui::GetStyle().WindowPadding.y;

    // === TOP ROW: Back button + Title + Tabs ===
    if (ImGui::Button(mCancelText.c_str(), ImVec2(0, buttonH)))
    {
        mResult = FILEMENURESULT_CANCEL;
        mFinished = true;
    }
    float backButtonRight = ImGui::GetItemRectMax().x;

    // Calculate tab strip position (right side, LOAD mode only if tabs provided)
    float tabsStartX = (float)width - padding;  // Default to right edge if no tabs
    float tabsWidth = 0;
    if (mMode == LOAD && !mTabTitles.empty())
    {
        // Account for text width + frame padding (10*2) + tab spacing/borders (~15 extra)
        for (const auto& tab : mTabTitles)
            tabsWidth += ImGui::CalcTextSize(tab.c_str()).x + 25.0f * scale;
        tabsStartX = (float)width - tabsWidth - padding;
    }

    // Title - centered between back button and tab strip
    float titleWidth = ImGui::CalcTextSize(mTitle.c_str()).x;
    float centerX = (backButtonRight + tabsStartX) * 0.5f;
    float titleX = centerX - titleWidth * 0.5f;
    ImGui::SameLine();
    ImGui::SetCursorPosX(titleX);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", mTitle.c_str());

    // Tab bar (right side, LOAD mode only if tabs provided)
    if (mMode == LOAD && !mTabTitles.empty())
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(tabsStartX);

        float fontSize = ImGui::GetFontSize();
        float tabPaddingY = (buttonH - fontSize) * 0.5f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f * scale, tabPaddingY));
        if (ImGui::BeginTabBar("FileTabs", ImGuiTabBarFlags_FittingPolicyScroll))
        {
            for (size_t i = 0; i < mTabTitles.size(); ++i)
            {
                // Force first tab to be selected when menu opens
                ImGuiTabItemFlags flags = 0;
                if (i == 0 && mForceFirstTab)
                    flags |= ImGuiTabItemFlags_SetSelected;

                if (ImGui::BeginTabItem(mTabTitles[i].c_str(), nullptr, flags))
                {
                    mSelectedTab = (int)i;
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
            mForceFirstTab = false;  // Only force on first frame
        }
        ImGui::PopStyleVar();
    }

    // === SAVE MODE: Filename input ===
    if (mMode == SAVE)
    {
        ImGui::Text("%s", GetPS(PS_FILENAME, language));
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##filename", mFilenameBuffer, sizeof(mFilenameBuffer));

        // Sanitize input
        std::string sanitized = mFilenameBuffer;
        RemoveInvalidCharacters(sanitized);
        strncpy(mFilenameBuffer, sanitized.c_str(), sizeof(mFilenameBuffer) - 1);
    }

    // === SCROLLABLE FILE LIST ===
    float topY = ImGui::GetCursorPosY();

    // Calculate bottom button area height
    float bottomAreaHeight = 0;
    if (mMode == SAVE)
        bottomAreaHeight = buttonH + padding;  // OK button
    else if (!mAltText.empty())
        bottomAreaHeight = buttonH + padding;  // Alt button

    float contentHeight = height - topY - bottomAreaHeight - padding;

    // Use per-tab child window IDs so each tab maintains its own scroll position
    char childId[32];
    snprintf(childId, sizeof(childId), "FileList##Tab%d", mSelectedTab);
    ImGui::BeginChild(childId, ImVec2(-1, contentHeight), true);

    // Reset scroll position for this tab if menu was just opened
    if (mSelectedTab < (int)mResetScroll.size() && mResetScroll[mSelectedTab])
    {
        ImGui::SetScrollY(0.0f);
        mResetScroll[mSelectedTab] = false;
    }

    float rowHeight = (float)height / mImagesPerScreen;
    bool hasUserFiles = false;
    bool userFilesLabelDrawn = false;

    // Check if we have any user files
    for (const auto& item : mItems)
    {
        if (item.isFromUserPath)
        {
            hasUserFiles = true;
            break;
        }
    }

    // Render file items
    for (size_t i = 0; i < mItems.size(); ++i)
    {
        const FileMenuItem& item = mItems[i];

        // Tab filtering (LOAD mode only)
        if (mMode == LOAD && !PassesTabFilter(item))
            continue;

        // Draw "User files:" label before first user file
        if (item.isFromUserPath && !userFilesLabelDrawn)
        {
            userFilesLabelDrawn = true;
            ImGui::TextUnformatted("User files:");
        }

        ImGui::PushID((int)i);

        // Determine row height - use fixed height for image menus, auto for text-only
        float imgSize = (mFileMenuType != FILEMENUTYPE_FILE) ? rowHeight - 8.0f : 0;

        // Thumbnail (if available and not FILE type)
        if (item.thumbnail && mFileMenuType != FILEMENUTYPE_FILE)
        {
            uint32 texW = item.thumbnail->GetWidth();
            uint32 texH = item.thumbnail->GetHeight();
            GLuint texID = item.thumbnail->GetTextureID();

            if (texW > 0 && texH > 0)
            {
                // Calculate display size: fill row height, extend width to maintain aspect ratio
                float ar = (float)texW / (float)texH;
                float displayH = imgSize;
                float displayW = imgSize * ar;

                ImGui::PushStyleColor(ImGuiCol_Button, PicaStyle::ImageButton::Transparent);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PicaStyle::ImageButton::HoverDark);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, PicaStyle::ImageButton::ActiveDark);

                if (ImGui::ImageButton("thumb", (ImTextureID)(intptr_t)texID,
                        ImVec2(displayW, displayH)))
                {
                    mSelectedPath = item.fullPath;
                    mResult = FILEMENURESULT_SELECTED;
                    mFinished = true;
                }
                ImGui::PopStyleColor(3);
                ImGui::SameLine();
            }
        }

        // Text button - user files show filename, system files show title from XML
        std::string buttonText = item.isFromUserPath ? item.displayName : item.title;
        if (!item.info.empty())
            buttonText += ": " + item.info;

        float buttonWidth = ImGui::GetContentRegionAvail().x;
        float buttonHeight = (mFileMenuType != FILEMENUTYPE_FILE && imgSize > 0) ? imgSize : buttonH;

        if (ImGui::Button(buttonText.c_str(), ImVec2(buttonWidth, buttonHeight)))
        {
            if (mMode == SAVE)
            {
                // In SAVE mode, clicking populates the filename input
                strncpy(mFilenameBuffer, item.displayName.c_str(), sizeof(mFilenameBuffer) - 1);
            }
            else if (mMode == DELETE_MODE)
            {
                // In DELETE mode, clicking selects for deletion
                mSelectedPath = item.fullPath;
                mResult = FILEMENURESULT_SELECTED;
                mFinished = true;
            }
            else
            {
                // LOAD mode - select the file
                mSelectedPath = item.fullPath;
                mResult = FILEMENURESULT_SELECTED;
                mFinished = true;
            }
        }

        ImGui::PopID();
    }

    // Show "No files found" in DELETE mode if no user files
    if (mMode == DELETE_MODE && !hasUserFiles)
    {
        ImGui::TextUnformatted("User files:");
        ImGui::TextUnformatted("No files found");
    }

    ImGui::EndChild();

    // === BOTTOM BUTTONS ===
    if (mMode == SAVE)
    {
        // OK button for SAVE mode
        ImGui::SetCursorPosY((float)height - buttonH - padding);
        if (ImGui::Button(GetPS(PS_OK, language), ImVec2(-1, buttonH)))
        {
            mSelectedPath = mFilenameBuffer;  // Just the filename, path built by caller
            mResult = FILEMENURESULT_SELECTED;
            mFinished = true;
        }
    }
    else if (!mAltText.empty())
    {
        // Alt button for LOAD mode (e.g., "Use default/previous")
        ImGui::SetCursorPosY((float)height - buttonH - padding);
        if (ImGui::Button(mAltText.c_str(), ImVec2(-1, buttonH)))
        {
            mResult = FILEMENURESULT_ALT;
            mFinished = true;
        }
    }

    ImGui::End();
    PicaStyle::PopMenuStyle();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//======================================================================================================================
FileMenuResult FileMenuLoad(
    std::string& result,
    const GameSettings& gameSettings,
    const char* systemPath,
    const char* userPath,
    const char* extension,
    const char* title,
    const char* tabTitles[],
    const size_t numTabs,
    const char* cancelButtonText,
    const char* altButtonText,
    FileMenuType fileMenuType,
    float imagesPerScreen,
    IncludeCallback* includeCallback)
{
    PrepareForIwGx(false);

    if (imagesPerScreen < 0.0f)
        imagesPerScreen = GetImagesPerLoadScreen(gameSettings);

    // With ImGui we filter tabs in-place, but keeping the loop structure
    // for compatibility with original behavior (menu recreation on tab change)
    int tab = 0;
    while (true)
    {
        FileMenu menu(gameSettings, systemPath, userPath, extension,
                      FileMenu::LOAD, title, tabTitles, numTabs, tab,
                      cancelButtonText, altButtonText, fileMenuType,
                      imagesPerScreen, includeCallback);

        while (!menu.Update())
        {
            if (CheckForQuitRequest() ||
                    (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) ||
                    (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED))
            {
                RecoverFromIwGx(false);
                return FILEMENURESULT_CANCEL;
            }
        }

        tab = menu.GetSelectedTab();
        FileMenuResult ret = menu.GetResult(result);

        // In original code, tab >= 0 means tab was changed (recreate menu)
        // With ImGui filtering in-place, we can just return directly
        RecoverFromIwGx(false);
        return ret;
    }
}

//======================================================================================================================
FileMenuResult FileMenuSave(
    std::string& selectedString,
    const GameSettings& gameSettings,
    const char* userPath,
    const char* extension,
    const char* title,
    FileMenuType fileMenuType)
{
    PrepareForIwGx(false);
    Language language = gameSettings.mOptions.mLanguage;

    FileMenu menu(gameSettings, nullptr, userPath, extension,
                  FileMenu::SAVE, title, nullptr, 0, 0,
                  GetPS(PS_BACK, language), nullptr, fileMenuType, 3.0f, nullptr);

    while (!menu.Update())
    {
        if (CheckForQuitRequest() ||
                (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) ||
                (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED))
        {
            RecoverFromIwGx(false);
            return FILEMENURESULT_CANCEL;
        }
    }

    FileMenuResult ret = menu.GetResult(selectedString);
    RecoverFromIwGx(false);

    if (selectedString.empty())
        return FILEMENURESULT_CANCEL;

    // Build full path: userPath/filename.extension
    selectedString = std::string(userPath) + "/" + selectedString + std::string(extension);
    return FILEMENURESULT_SELECTED;
}

//======================================================================================================================
void FileMenuDelete(
    const GameSettings& gameSettings,
    const char* userPath,
    const char* extension,
    const char* title,
    FileMenuType fileMenuType)
{
    PrepareForIwGx(false);
    Language language = gameSettings.mOptions.mLanguage;

    FileMenu menu(gameSettings, nullptr, userPath, extension,
                  FileMenu::DELETE_MODE, title, nullptr, 0, 0,
                  GetPS(PS_BACK, language), nullptr, fileMenuType, 3.0f, nullptr);

    while (!menu.Update())
    {
        if (CheckForQuitRequest() ||
                (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) ||
                (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED))
        {
            RecoverFromIwGx(false);
            return;
        }
    }

    std::string selectedPath;
    menu.GetResult(selectedPath);
    RecoverFromIwGx(false);

    if (!selectedPath.empty())
    {
        TRACE_FILE_IF(1) TRACE("Deleting %s", selectedPath.c_str());
        s3eFileDelete(selectedPath.c_str());
    }
}

//======================================================================================================================
// Higher-level selection functions (moved from SettingsMenu)
//======================================================================================================================

//======================================================================================================================
ScenarioResult SelectScenario(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText)
{
    TRACE_FUNCTION_ONLY(1);
    std::string file;
    FileMenuResult result = FileMenuLoad(file, gameSettings, "SystemSettings/Scenario", NULL, ".xml", title, 0, 0,
        cancelButtonText, altButtonText, FILEMENUTYPE_AEROPLANE, 3.7f);
    if (!file.empty() && result == FILEMENURESULT_SELECTED)
    {
        TRACE_FILE_IF(1) TRACE("Selected scenario settings %s", file.c_str());

        if (file.find("lider.xml") != std::string::npos)
            return SCENARIO_TRAINERGLIDER;
        else if (file.find("owered.xml") != std::string::npos)
            return SCENARIO_TRAINERPOWERED;
        else if (file.find("hoose.xml") != std::string::npos)
            return SCENARIO_CHOOSE;
    }
    else if (result == FILEMENURESULT_ALT)
    {
        return SCENARIO_DEFAULT;
    }
    return SCENARIO_CANCELLED;
}

//======================================================================================================================
SelectResult SelectAeroplane(std::string& file, GameSettings& gameSettings, const char* title,
    const char* cancelButtonText, const char* altButtonText, IncludeCallback* includeCallback)
{
    TRACE_FUNCTION_ONLY(1);
    Language language = gameSettings.mOptions.mLanguage;
    const char* aeroplaneTabTitles[] = {TXT(PS_ALL), TXT(PS_GLIDERS), TXT(PS_POWERED), TXT(PS_USER)};
    std::string userPath = Platform::GetUserSettingsPath() + "Aeroplane";
    FileMenuResult result = FileMenuLoad(
        file, gameSettings,
        "SystemSettings/Aeroplane", userPath.c_str(), ".xml",
        title, aeroplaneTabTitles, 4,
        cancelButtonText, altButtonText,
        FILEMENUTYPE_AEROPLANE, -1.0f, includeCallback);
    if (!file.empty() && result == FILEMENURESULT_SELECTED)
    {
        return SELECTRESULT_SELECTED;
    }
    else if (result == FILEMENURESULT_ALT)
    {
        return SELECTRESULT_SELECTED;
    }
    else
    {
        return SELECTRESULT_CANCELLED;
    }
}

//======================================================================================================================
SelectResult SelectAndLoadAeroplane(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText)
{
    TRACE_FUNCTION_ONLY(1);
    std::string file;
    Language language = gameSettings.mOptions.mLanguage;
    const char* aeroplaneTabTitles[] = {TXT(PS_ALL), TXT(PS_GLIDERS), TXT(PS_POWERED), TXT(PS_USER)};
    std::string userPath = Platform::GetUserSettingsPath() + "Aeroplane";
    FileMenuResult result = FileMenuLoad(
        file, gameSettings,
        "SystemSettings/Aeroplane", userPath.c_str(), ".xml",
        title, aeroplaneTabTitles, 4,
        cancelButtonText, altButtonText, FILEMENUTYPE_AEROPLANE);
    if (!file.empty() && result == FILEMENURESULT_SELECTED)
    {
        TRACE_FILE_IF(1) TRACE("Loading Aeroplane settings %s", file.c_str());
        bool loadResult = gameSettings.mAeroplaneSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, loadResult);
        TRACE_FILE_IF(1) TRACE(" %s\n", loadResult ? "success" : "failed");
        gameSettings.mStatistics.mLoadedAeroplane = true;

        if (
            loadResult &&
            gameSettings.mOptions.mUseAeroplanePreferredController &&
            !gameSettings.mAeroplaneSettings.mPreferredController.empty()
            )
        {
            TRACE_FILE_IF(1) TRACE("Loading Controller %s", gameSettings.mAeroplaneSettings.mPreferredController.c_str());
            bool controllerResult = gameSettings.mControllerSettings.LoadFromFile(gameSettings.mAeroplaneSettings.mPreferredController);
            TRACE_FILE_IF(1) TRACE(" %s\n", controllerResult ? "success" : "failed");
        }
        return SELECTRESULT_SELECTED;
    }
    else if (result == FILEMENURESULT_ALT)
    {
        return SELECTRESULT_SELECTED;
    }
    else
    {
        return SELECTRESULT_CANCELLED;
    }
}

//======================================================================================================================
SelectResult SelectAndLoadEnvironment(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText)
{
    TRACE_FUNCTION_ONLY(1);
    std::string file;
    Language language = gameSettings.mOptions.mLanguage;
    const char* environmentTabTitles[] = {TXT(PS_ALL), TXT(PS_SLOPE), TXT(PS_FLAT), TXT(PS_PANORAMIC), TXT(PS_3D), TXT(PS_USER)};
    std::string userPath = Platform::GetUserSettingsPath() + "Environment";
    FileMenuResult result = FileMenuLoad(
        file, gameSettings,
        "SystemSettings/Environment", userPath.c_str(), ".xml",
        title, environmentTabTitles, 6, cancelButtonText, altButtonText, FILEMENUTYPE_SCENERY);
    if (!file.empty() && result == FILEMENURESULT_SELECTED)
    {
        TRACE_FILE_IF(1) TRACE("Loading Environment %s - ", file.c_str());
        bool loadResult = gameSettings.mEnvironmentSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, loadResult);
        TRACE_FILE_IF(1) TRACE(" %s\n", loadResult ? "success" : "failed");
        bool objectsResult = gameSettings.mObjectsSettings.LoadFromFile(gameSettings.mEnvironmentSettings.mObjectsSettingsFile);
        IwAssert(ROWLHOUSE, objectsResult);
        gameSettings.mStatistics.mLoadedTerrain = true;
        return SELECTRESULT_SELECTED;
    }
    else if (result == FILEMENURESULT_ALT)
    {
        return SELECTRESULT_SELECTED;
    }
    else
    {
        return SELECTRESULT_CANCELLED;
    }
}
