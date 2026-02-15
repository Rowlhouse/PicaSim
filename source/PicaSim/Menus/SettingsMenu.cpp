#include "SettingsMenu.h"
#include "SettingsWidgets.h"
#include "FileMenu.h"
#include "UIHelpers.h"
#include "../GameSettings.h"
#include "../PicaSim.h"
#include "../Aeroplane.h"
#include "../AeroplanePhysics.h"
#include "../PicaJoystick.h"
#include "PicaDialog.h"
#include "../../Platform/S3ECompat.h"
#include "../../Platform/Input.h"
#include "../../Framework/Trace.h"
#include "Platform.h"
#include "HelpersXML.h"
#include "tinyxml.h"

#ifdef PICASIM_VR_SUPPORT
#include "../../Platform/VRManager.h"
#include "../../Framework/AudioManager.h"
#endif

#include "ScrollHelper.h"
#include "ScrollableTabStrip.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Forward declarations from Graphics.cpp
void IwGxClear();
void IwGxSwapBuffers();

// Forward declaration from Helpers.cpp
Texture* GetCachedTexture(std::string path, bool convertTo16Bit);

//======================================================================================================================
struct CheckForAIControlCallback : public IncludeCallback
{
    bool GetInclude(const char* filename) OVERRIDE
    {
        AIAeroplaneSettings aias;
        aias.LoadFromFile(filename);
        return aias.mAllowAIControl;
    }
};

//======================================================================================================================
struct CheckForTugCallback : public IncludeCallback
{
    bool GetInclude(const char* filename) OVERRIDE
    {
        AeroplaneSettings as;
        as.LoadFromFile(filename);
        return as.mAIAeroplaneSettings.mCanTow;
    }
};

//======================================================================================================================
enum SettingsStatus
{
    SETTINGS_UNSET,
    SETTINGS_BACK,
    SETTINGS_REFRESH,
    SETTINGS_LOAD_OPTIONS,
    SETTINGS_SAVE_OPTIONS,
    SETTINGS_DELETE_OPTIONS,
    SETTINGS_LOAD_ENVIRONMENT,
    SETTINGS_SAVE_ENVIRONMENT,
    SETTINGS_DELETE_ENVIRONMENT,
    SETTINGS_LOAD_OBJECTS,
    SETTINGS_SAVE_OBJECTS,
    SETTINGS_DELETE_OBJECTS,
    SETTINGS_LOAD_LIGHTING,
    SETTINGS_SAVE_LIGHTING,
    SETTINGS_DELETE_LIGHTING,
    SETTINGS_LOAD_CONTROLLER,
    SETTINGS_SAVE_CONTROLLER,
    SETTINGS_DELETE_CONTROLLER,
    SETTINGS_LOAD_JOYSTICK,
    SETTINGS_SAVE_JOYSTICK,
    SETTINGS_DELETE_JOYSTICK,
    SETTINGS_LOAD_AEROPLANE,
    SETTINGS_SAVE_AEROPLANE,
    SETTINGS_DELETE_AEROPLANE,
    SETTINGS_LOAD_AICONTROLLERS,
    SETTINGS_SAVE_AICONTROLLERS,
    SETTINGS_DELETE_AICONTROLLERS,
    SETTINGS_ADD_AICONTROLLER,
    SETTINGS_REMOVE_AICONTROLLERS,
    SETTINGS_LOAD_TUG,
    SETTINGS_SELECT_PANORAMA,
    SETTINGS_SELECT_FILE_TERRAIN,
    SETTINGS_SELECT_PREFERRED_CONTROLLER,
    SETTINGS_SELECT_OBJECTS_SETTINGS,
    SETTINGS_SELECT_LANGUAGE,
    SETTINGS_DELETE_LOCAL_HIGHSCORES,
    SETTINGS_RESET_OBJECTS,
    SETTINGS_CLEAR_JOYSTICK,
    SETTINGS_CALIBRATE_JOYSTICK,
    SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT
};

//======================================================================================================================
enum TabPanelEnum
{
    TAB_OPTIONS1,
    TAB_OPTIONS2,
    TAB_AEROPLANE,
    TAB_SCENERY,
    TAB_OBJECTS,
    TAB_LIGHTING,
    TAB_AICONTROLLERS,
    TAB_CONTROLLER,
    TAB_JOYSTICK,
    TAB_NUM_TABS
};

// Persistent state across menu instances
static int sSelectedTab = TAB_OPTIONS1;
static bool sAdvancedEnabled[TAB_NUM_TABS] = {false};

// Image button tracking for GetImageButtonInfo
static float sImageButtonX = 0, sImageButtonY = 0, sImageButtonW = 1, sImageButtonH = 1;

//======================================================================================================================
class SettingsMenu
{
public:
    SettingsMenu(GameSettings& gameSettings);

    SettingsStatus Update();
    void GetImageButtonInfo(int& x, int& y, int& w, int& h);

    // Render content without finalizing ImGui (for use as dialog background)
    void RenderContent();

private:
    void Render();

    // Tab rendering functions
    void RenderOptions1Tab();
    void RenderOptions2Tab();
    void RenderAeroplaneTab();
    void RenderSceneryTab();
    void RenderObjectsTab();
    void RenderLightingTab();
    void RenderAIControllersTab();
    void RenderControllerTab();
    void RenderJoystickTab();

    // Bottom buttons rendering (returns load/save/delete status if clicked)
    void RenderBottomButtons(SettingsStatus loadStatus, SettingsStatus saveStatus,
                             SettingsStatus deleteStatus);

    GameSettings& mGameSettings;
    SettingsStatus mStatus;
    bool mResetScroll[TAB_NUM_TABS];  // Reset scroll position per tab when menu opens

    // Smoke color HSV values (for color editing)
    Vector3 mSmokeHSVs[AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE];

    ScrollHelper mScrollHelper;
    ScrollableTabStrip mTabStrip;
};

//======================================================================================================================
SettingsMenu::SettingsMenu(GameSettings& gameSettings)
    : mGameSettings(gameSettings)
    , mStatus(SETTINGS_UNSET)
{
    UIHelpers::NotifyMenuTransition();

    // Reset scroll positions for all tabs when menu opens
    for (int i = 0; i < TAB_NUM_TABS; ++i)
        mResetScroll[i] = true;

    // Initialize smoke HSV values from current settings
    for (size_t i = 0; i < AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE; ++i)
    {
        mSmokeHSVs[i] = RGB2HSV(gameSettings.mAeroplaneSettings.mSmokeSources[i].mColour);
    }
}

//======================================================================================================================
void SettingsMenu::GetImageButtonInfo(int& x, int& y, int& w, int& h)
{
    x = (int)sImageButtonX;
    y = (int)sImageButtonY;
    w = (int)sImageButtonW;
    h = (int)sImageButtonH;
}

//======================================================================================================================
SettingsStatus SettingsMenu::Update()
{
    // Reset status each frame - only return a status if a button is clicked this frame
    mStatus = SETTINGS_UNSET;

    // Update input state
    UpdateJoystick(mGameSettings.mOptions.mJoystickID);

    int savedTab = sSelectedTab;

    IwGxClear();
    Render();
    IwGxSwapBuffers();
    PollEvents();

    // Suppress stale input from the previous menu's touch/click events
    if (UIHelpers::IsInputMuted())
    {
        mStatus = SETTINGS_UNSET;
        sSelectedTab = savedTab;
    }

    return mStatus;
}

//======================================================================================================================
void SettingsMenu::RenderContent()
{
    float scale = UIHelpers::GetFontScale();
    Language language = mGameSettings.mOptions.mLanguage;

    // Begin ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    UIHelpers::ApplyFontScale();
    SettingsWidgets::ResetFrameState();

    // Unified settings style
    PicaStyle::PushSettingsStyle();

    // Full-screen window with safe area insets
    ImVec2 winSize = UIHelpers::BeginFullscreenWindow("SettingsMenu",
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    float height = winSize.y;

    float buttonH = 32.0f * scale;
    float padding = ImGui::GetStyle().WindowPadding.y;

    // === TOP ROW: Back button + Tab bar ===
    if (ImGui::Button(TXT(PS_BACK), ImVec2(0, buttonH)))
    {
        mStatus = SETTINGS_BACK;
    }
    ImGui::SameLine();

    // Build tab labels dynamically (joystick tab is conditional)
    const char* tabLabels[TAB_NUM_TABS];
    int tabIndices[TAB_NUM_TABS];
    int tabCount = 0;

    tabLabels[tabCount] = TXT(PS_OPTIONS1);      tabIndices[tabCount] = TAB_OPTIONS1;      tabCount++;
    tabLabels[tabCount] = TXT(PS_OPTIONS2);      tabIndices[tabCount] = TAB_OPTIONS2;      tabCount++;
    tabLabels[tabCount] = TXT(PS_AEROPLANE);     tabIndices[tabCount] = TAB_AEROPLANE;     tabCount++;
    tabLabels[tabCount] = TXT(PS_SCENERY);       tabIndices[tabCount] = TAB_SCENERY;       tabCount++;
    tabLabels[tabCount] = TXT(PS_OBJECTS);       tabIndices[tabCount] = TAB_OBJECTS;        tabCount++;
    tabLabels[tabCount] = TXT(PS_LIGHTING);      tabIndices[tabCount] = TAB_LIGHTING;      tabCount++;
    tabLabels[tabCount] = TXT(PS_AICONTROLLERS); tabIndices[tabCount] = TAB_AICONTROLLERS;  tabCount++;
    tabLabels[tabCount] = TXT(PS_CONTROLLER);    tabIndices[tabCount] = TAB_CONTROLLER;    tabCount++;
    if (ShowJoystickInGame(mGameSettings))
    {
        tabLabels[tabCount] = TXT(PS_JOYSTICK);  tabIndices[tabCount] = TAB_JOYSTICK;      tabCount++;
    }

    // Map sSelectedTab to strip index for rendering
    int stripIndex = 0;
    for (int i = 0; i < tabCount; ++i)
    {
        if (tabIndices[i] == sSelectedTab)
        {
            stripIndex = i;
            break;
        }
    }

    mTabStrip.Render("SettingsTabs", tabLabels, tabCount, stripIndex, buttonH);
    sSelectedTab = tabIndices[stripIndex];

    // === SCROLLABLE CONTENT AREA ===
    float topY = ImGui::GetCursorPosY();
    float bottomButtonsHeight = buttonH + padding * 2;
    float contentHeight = height - topY - bottomButtonsHeight;

    // Use per-tab child window IDs so each tab maintains its own scroll position
    static const char* tabContentIds[TAB_NUM_TABS] = {
        "Content_Options1", "Content_Options2", "Content_Aeroplane", "Content_Scenery",
        "Content_Objects", "Content_Lighting", "Content_AIControllers", "Content_Controller",
        "Content_Joystick"
    };

    ImGui::BeginChild(tabContentIds[sSelectedTab], ImVec2(-1, contentHeight), true);
    mScrollHelper.ApplyDragScroll(tabContentIds[sSelectedTab]);

    // Reset scroll position for this tab if menu was just opened
    if (mResetScroll[sSelectedTab])
    {
        ImGui::SetScrollY(0.0f);
        mScrollHelper.ResetScroll(tabContentIds[sSelectedTab]);
        mResetScroll[sSelectedTab] = false;
    }

    // Render the appropriate tab content
    switch (sSelectedTab)
    {
        case TAB_OPTIONS1:      RenderOptions1Tab(); break;
        case TAB_OPTIONS2:      RenderOptions2Tab(); break;
        case TAB_AEROPLANE:     RenderAeroplaneTab(); break;
        case TAB_SCENERY:       RenderSceneryTab(); break;
        case TAB_OBJECTS:       RenderObjectsTab(); break;
        case TAB_LIGHTING:      RenderLightingTab(); break;
        case TAB_AICONTROLLERS: RenderAIControllersTab(); break;
        case TAB_CONTROLLER:    RenderControllerTab(); break;
        case TAB_JOYSTICK:      RenderJoystickTab(); break;
    }

    ImGui::EndChild();

    // === BOTTOM BUTTONS ===
    // Determine which load/save/delete status to use based on current tab
    SettingsStatus loadStatus = SETTINGS_UNSET;
    SettingsStatus saveStatus = SETTINGS_UNSET;
    SettingsStatus deleteStatus = SETTINGS_UNSET;

    switch (sSelectedTab)
    {
        case TAB_OPTIONS1:
        case TAB_OPTIONS2:
            loadStatus = SETTINGS_LOAD_OPTIONS;
            saveStatus = SETTINGS_SAVE_OPTIONS;
            deleteStatus = SETTINGS_DELETE_OPTIONS;
            break;
        case TAB_AEROPLANE:
            loadStatus = SETTINGS_LOAD_AEROPLANE;
            saveStatus = SETTINGS_SAVE_AEROPLANE;
            deleteStatus = SETTINGS_DELETE_AEROPLANE;
            break;
        case TAB_SCENERY:
            loadStatus = SETTINGS_LOAD_ENVIRONMENT;
            saveStatus = SETTINGS_SAVE_ENVIRONMENT;
            deleteStatus = SETTINGS_DELETE_ENVIRONMENT;
            break;
        case TAB_OBJECTS:
            loadStatus = SETTINGS_LOAD_OBJECTS;
            saveStatus = SETTINGS_SAVE_OBJECTS;
            deleteStatus = SETTINGS_DELETE_OBJECTS;
            break;
        case TAB_LIGHTING:
            // Only show buttons if not panorama and environment settings allowed
            {
                bool isPanorama = mGameSettings.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA;
                bool allowEnvironmentSettings = mGameSettings.mChallengeSettings.mAllowEnvironmentSettings;
                if (!isPanorama && allowEnvironmentSettings)
                {
                    loadStatus = SETTINGS_LOAD_LIGHTING;
                    saveStatus = SETTINGS_SAVE_LIGHTING;
                    deleteStatus = SETTINGS_DELETE_LIGHTING;
                }
            }
            break;
        case TAB_AICONTROLLERS:
            loadStatus = SETTINGS_LOAD_AICONTROLLERS;
            saveStatus = SETTINGS_SAVE_AICONTROLLERS;
            deleteStatus = SETTINGS_DELETE_AICONTROLLERS;
            break;
        case TAB_CONTROLLER:
            loadStatus = SETTINGS_LOAD_CONTROLLER;
            saveStatus = SETTINGS_SAVE_CONTROLLER;
            deleteStatus = SETTINGS_DELETE_CONTROLLER;
            break;
        case TAB_JOYSTICK:
            loadStatus = SETTINGS_LOAD_JOYSTICK;
            saveStatus = SETTINGS_SAVE_JOYSTICK;
            deleteStatus = SETTINGS_DELETE_JOYSTICK;
            break;
    }

    RenderBottomButtons(loadStatus, saveStatus, deleteStatus);

    ImGui::End();
    PicaStyle::PopSettingsStyle();
}

//======================================================================================================================
void SettingsMenu::Render()
{
    RenderContent();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//======================================================================================================================
void SettingsMenu::RenderBottomButtons(SettingsStatus loadStatus, SettingsStatus saveStatus,
                                       SettingsStatus deleteStatus)
{
    Language language = mGameSettings.mOptions.mLanguage;
    float scale = UIHelpers::GetFontScale();
    float buttonH = 32.0f * scale;
    int width = Platform::GetDisplayWidth();

    bool advanced = sAdvancedEnabled[sSelectedTab];
    int numButtons = advanced ? 4 : 3;
    float buttonWidth = (float)width / numButtons - ImGui::GetStyle().ItemSpacing.x;

    // Load button
    if (SettingsWidgets::ButtonSized(TXT(PS_LOAD), buttonWidth, buttonH))
    {
        mStatus = loadStatus;
    }
    ImGui::SameLine();

    // Save button
    if (SettingsWidgets::ButtonSized(TXT(PS_SAVE), buttonWidth, buttonH))
    {
        mStatus = saveStatus;
    }
    ImGui::SameLine();

    // Delete button (only in advanced mode)
    if (advanced)
    {
        if (SettingsWidgets::ButtonSized(TXT(PS_DELETE), buttonWidth, buttonH))
        {
            mStatus = deleteStatus;
        }
        ImGui::SameLine();
    }

    // Advanced/Simple toggle button
    const char* toggleText = advanced ? TXT(PS_SIMPLE) : TXT(PS_ADVANCED);
    if (SettingsWidgets::ButtonSized(toggleText, buttonWidth, buttonH))
    {
        sAdvancedEnabled[sSelectedTab] = !sAdvancedEnabled[sSelectedTab];
    }
}

//======================================================================================================================
void SettingsMenu::RenderOptions1Tab()
{
    Options& options = mGameSettings.mOptions;
    Language language = options.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_OPTIONS1];

    // Language settings
    SettingsWidgets::SectionHeader(TXT(PS_LANGUAGESETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        int lang = (int)options.mLanguage;
        if (SettingsWidgets::Combo(TXT(PS_CURRENTLANGUAGE), lang, gLanguageStrings, LANG_NUM_ACTIVE))
        {
            options.mLanguage = (Language)lang;
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Camera settings
    SettingsWidgets::SectionHeader(TXT(PS_CAMERASETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::Checkbox(TXT(PS_ZOOMVIEW), options.mEnableZoomView);
        SettingsWidgets::Checkbox(TXT(PS_PLANEONLYINZOOMVIEW), options.mOnlyPlaneInZoomView);
        SettingsWidgets::Checkbox(TXT(PS_SMOKEONLYINMAINVIEW), options.mSmokeOnlyInMainView);
        if (advanced)
            SettingsWidgets::SliderFloat(TXT(PS_ZOOMVIEWSIZE), options.mZoomViewSize, 0.1f, 0.5f);
        SettingsWidgets::SliderFloat(TXT(PS_GROUNDAUTOZOOM), options.mGroundViewAutoZoom, 0.0f, 1.0f);
        SettingsWidgets::SliderFloat(TXT(PS_GROUNDHORIZONAMOUNT), options.mGroundViewHorizonAmount, 0.0f, 1.0f);
        SettingsWidgets::SliderFloat(TXT(PS_GROUNDLAG), options.mGroundViewLag, 0.0f, 1.0f, "%.3f s");
        if (advanced)
        {
            SettingsWidgets::SliderFloat(TXT(PS_GROUNDAUTOZOOMSCALE), options.mGroundViewAutoZoomScale, 0.1f, 2.0f);
            SettingsWidgets::SliderFloat(TXT(PS_GROUNDFIELDOFVIEW), options.mGroundViewFieldOfView, 10.0f, 120.0f, "%.0f deg");
            SettingsWidgets::Checkbox(TXT(PS_GROUNDVIEWFOLLOW), options.mGroundViewFollow);
            SettingsWidgets::SliderFloat(TXT(PS_GROUNDVIEWYAWOFFSET), options.mGroundViewYawOffset, -180.0f, 180.0f, "%.0f deg");
            SettingsWidgets::SliderFloat(TXT(PS_GROUNDVIEWPITCHOFFSET), options.mGroundViewPitchOffset, -90.0f, 90.0f, "%.0f deg");
            SettingsWidgets::SliderFloat(TXT(PS_AEROPLANEFIELDOFVIEW), options.mAeroplaneViewFieldOfView, 10.0f, 120.0f, "%.0f deg");
        }
    }
    SettingsWidgets::EndSettingsBlock();

#ifdef PICASIM_VR_SUPPORT
    // VR Headset settings
    {
        SettingsWidgets::SectionHeader(TXT(PS_VRHEADSET));
        SettingsWidgets::BeginSettingsBlock();
        {
            if (VRManager::IsAvailable())
            {
                // VR enable checkbox
                bool wasEnabled = options.mEnableVR;
                SettingsWidgets::Checkbox(TXT(PS_ENABLEVRMODE), options.mEnableVR);

                // Handle enable/disable
                if (options.mEnableVR != wasEnabled)
                {
                    if (options.mEnableVR)
                    {
                        VRManager::GetInstance().SetMSAASamples(options.mVRMSAASamples);
                        VRManager::GetInstance().SetVRAudioDevice(options.mVRAudioDevice);

                        // Auto-detect VR audio device if not configured
                        std::string autoDevice = VRManager::GetInstance().AutoDetectVRAudioDevice();
                        if (!autoDevice.empty() && options.mVRAudioDevice.empty())
                        {
                            options.mVRAudioDevice = autoDevice;
                        }

                        VRManager::GetInstance().EnableVR();
                    }
                    else
                    {
                        VRManager::GetInstance().DisableVR();
                    }
                }

                if (VRManager::GetInstance().IsVREnabled())
                {
                    // Show runtime info
                    char infoStr[256];
                    snprintf(infoStr, sizeof(infoStr), "%s - %s",
                        VRManager::GetInstance().GetRuntimeName(),
                        VRManager::GetInstance().GetSystemName());
                    SettingsWidgets::InfoLabel(TXT(PS_HEADSET), infoStr);

                    // VR Desktop window display mode
                    static const char* vrDesktopDescs[] = { TXT(PS_NOTHING), TXT(PS_LEFTEYE), TXT(PS_RIGHTEYE), TXT(PS_BOTHEYES), TXT(PS_BOTHEYESCROSSED), TXT(PS_NORMALVIEW) };
                    int vrDesktopIndex = (int)options.mVRDesktopMode;
                    if (SettingsWidgets::Combo(TXT(PS_VRDESKTOP), vrDesktopIndex, vrDesktopDescs, 6))
                        options.mVRDesktopMode = (Options::VRDesktopMode)vrDesktopIndex;

                    if (advanced)
                    {
                        // VR Anti-aliasing
                        static const char* vrMsaaDescs[] = { TXT(PS_NONE), TXT(PS_ANTIALIASING_2X), TXT(PS_ANTIALIASING_4X), TXT(PS_ANTIALIASING_8X) };
                        static const int vrMsaaValues[] = { 0, 2, 4, 8 };
                        int vrMsaaIndex = 0;
                        for (int i = 0; i < 4; ++i)
                        {
                            if (vrMsaaValues[i] == options.mVRMSAASamples)
                            {
                                vrMsaaIndex = i;
                                break;
                            }
                        }
                        if (SettingsWidgets::Combo(TXT(PS_VRANTIALIASING), vrMsaaIndex, vrMsaaDescs, 4))
                        {
                            options.mVRMSAASamples = vrMsaaValues[vrMsaaIndex];
                            // Restart VR to apply new MSAA setting
                            VRManager::GetInstance().DisableVR();
                            VRManager::GetInstance().SetMSAASamples(options.mVRMSAASamples);
                            VRManager::GetInstance().EnableVR();
                        }

                        // VR Audio device selection
                        auto audioDevices = AudioManager::GetInstance().EnumerateAudioDevices();
                        if (!audioDevices.empty())
                        {
                            // Auto-select a matching device if none is configured
                            if (options.mVRAudioDevice.empty())
                            {
                                std::string autoDevice = VRManager::GetInstance().AutoDetectVRAudioDevice();
                                if (!autoDevice.empty())
                                {
                                    options.mVRAudioDevice = autoDevice;
                                }
                            }

                            // Build list with "None" option first
                            std::vector<std::string> deviceNames;
                            deviceNames.push_back(TXT(PS_NONEUSEDEFAULT));
                            for (const auto& dev : audioDevices)
                                deviceNames.push_back(dev);

                            // Find current selection
                            int audioDeviceIndex = 0;
                            for (size_t i = 0; i < audioDevices.size(); ++i)
                            {
                                if (audioDevices[i] == options.mVRAudioDevice)
                                {
                                    audioDeviceIndex = (int)(i + 1);  // +1 because "None" is at index 0
                                    break;
                                }
                            }

                            if (SettingsWidgets::Combo(TXT(PS_VRAUDIO), audioDeviceIndex, deviceNames))
                            {
                                if (audioDeviceIndex == 0)
                                    options.mVRAudioDevice.clear();
                                else
                                    options.mVRAudioDevice = audioDevices[audioDeviceIndex - 1];
                                // Update VRManager with new device
                                VRManager::GetInstance().SetVRAudioDevice(options.mVRAudioDevice);
                            }
                        }

                        // VR Panorama Depth - stereoscopic parallax for panoramic sceneries
                        SettingsWidgets::Checkbox(TXT(PS_VRPANORAMADEPTH), options.mVRPanoramaDepth);

                        // VR Panorama Extension - border expansion for tiled panoramas (helps with parallax sampling)
                        SettingsWidgets::SliderFloat(TXT(PS_VRPANORAMAEXTENSION), options.mVRPanoramaExtension, 0.0f, 0.1f);

                        SettingsWidgets::SliderFloat(TXT(PS_VROVERLAYDISTANCE), options.mVROverlayDistance, 0.1f, 0.5f);
                        SettingsWidgets::SliderFloat(TXT(PS_VROVERLAYSCALE), options.mVROverlayScale, 0.25f, 1.0f);
                    }
                }
            }
            else
            {
                SettingsWidgets::InfoLabel(TXT(PS_STATUS), TXT(PS_VRNOTAVAILABLE));
            }
        }
        SettingsWidgets::EndSettingsBlock();
    }
#endif

    // Simulation settings (if freefly or advanced)
    if (mGameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY || advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_SIMULATIONSETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
            if (mGameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
                SettingsWidgets::SliderFloat(TXT(PS_TIMESCALE), options.mTimeScale, 0.1f, 1.0f);
            SettingsWidgets::SliderInt(TXT(PS_PHYSICSACCURACY), options.mFrameworkSettings.mPhysicsSubsteps, 4, 12);
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Controller settings
    SettingsWidgets::SectionHeader(TXT(PS_CONTROLLERSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        static const char* modeDescs[] = {
            TXT(PS_MODE1DESCRIPTION),
            TXT(PS_MODE2DESCRIPTION),
            TXT(PS_MODE3DESCRIPTION),
            TXT(PS_MODE4DESCRIPTION)
        };
        int mode = options.mControllerMode;
        if (SettingsWidgets::Combo(TXT(PS_CONTROLLERMODE), mode, modeDescs, 4))
            options.mControllerMode = mode;

        SettingsWidgets::SliderFloat(TXT(PS_CONTROLLERSIZE), options.mControllerSize, 0.2f, 1.0f);
        SettingsWidgets::Checkbox(TXT(PS_BRAKESFORWARD), options.mControllerBrakesForward);
        SettingsWidgets::Checkbox(TXT(PS_USEABSOLUTECONTROLLERTOUCHPOSITION), options.mControllerUseAbsolutePosition);
        if (mGameSettings.mOptions.mFrameworkSettings.isAndroid())
        {
            SettingsWidgets::Checkbox(TXT(PS_STAGGERCONTROLLER), options.mControllerStaggered);
        }
        SettingsWidgets::Checkbox(TXT(PS_ENABLEELEVATORTRIM), options.mControllerEnableTrim);

        if (advanced)
        {
            SettingsWidgets::SliderFloat(TXT(PS_ELEVATORTRIMSIZE), options.mControllerTrimSize, 0.01f, 0.1f);
            SettingsWidgets::Checkbox(TXT(PS_SQUARECONTROLLERS), options.mControllerSquare);
            SettingsWidgets::SliderFloat(TXT(PS_HORIZONTALOFFSETFROMEDGE), options.mControllerHorOffset, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_VERTICALOFFSETFROMEDGE), options.mControllerVerOffset, 0.0f, 1.0f);

            static const char* styleDescs[] = { TXT(PS_CROSS), TXT(PS_BOX), TXT(PS_CROSSANDBOX) };
            int style = (int)options.mControllerStyle;
            if (SettingsWidgets::Combo(TXT(PS_STYLE), style, styleDescs, 3))
                options.mControllerStyle = (Options::ControllerStyle)style;

            SettingsWidgets::SliderInt(TXT(PS_SHAPEOPACITY), options.mControllerAlpha, 0, 255);
            SettingsWidgets::SliderInt(TXT(PS_STICKOPACITY), options.mControllerStickAlpha, 0, 255);
            SettingsWidgets::Checkbox(TXT(PS_STICKCROSS), options.mControllerStickCross);
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Audio settings
    SettingsWidgets::SectionHeader(TXT(PS_AUDIOSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::SliderFloat(TXT(PS_OVERALLVOLUME), options.mVolumeScale, 0.0f, 2.0f);
        SettingsWidgets::SliderFloat(TXT(PS_VARIOMETERVOLUME), options.mVariometerVolume, 0.0f, 2.0f);
        if (advanced)
        {
            SettingsWidgets::SliderFloat(TXT(PS_WINDVOLUME), options.mWindVolume, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_OUTSIDEAEROPLANEVOLUME), options.mOutsideAeroplaneVolume, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_INSIDEAEROPLANEVOLUME), options.mInsideAeroplaneVolume, 0.0f, 1.0f);
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // On-screen display settings (advanced)
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_ONSCREENDISPLAYSETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_WINDARROWSIZE), options.mWindArrowSize, 0.05f, 0.25f);
            SettingsWidgets::SliderInt(TXT(PS_WINDARROWOPACITY), options.mWindsockOpacity, 0, 255);
            SettingsWidgets::SliderFloat(TXT(PS_PAUSEBUTTONSSIZE), options.mPauseButtonsSize, 0.25f, 0.75f);
            SettingsWidgets::SliderInt(TXT(PS_PAUSEBUTTONOPACITY), options.mPauseButtonOpacity, 0, 255);
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Information settings
    SettingsWidgets::SectionHeader(TXT(PS_INFORMATIONSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::Checkbox(TXT(PS_GRAPHFPS), options.mDisplayFPS);
        SettingsWidgets::SliderInt(TXT(PS_MAXMARKERSPERTHERMAL), options.mMaxMarkersPerThermal, 0, Options::MAX_MAX_MARKERS_PER_THERMAL);
        if (advanced)
            SettingsWidgets::Checkbox(TXT(PS_THERMALWINDFIELD), options.mDrawThermalWindField);
        SettingsWidgets::Checkbox(TXT(PS_DRAWGROUNDPOSITION), options.mDrawGroundPosition);

        static const char* skyGridDescs[] = { TXT(PS_SKYGRID_NONE), TXT(PS_SKYGRID_SPHERE), TXT(PS_SKYGRID_BOX) };
        int skyGrid = (int)options.mSkyGridOverlay;
        if (SettingsWidgets::Combo(TXT(PS_SKYGRIDOVERLAY), skyGrid, skyGridDescs, 3))
            options.mSkyGridOverlay = (Options::SkyGridOverlay)skyGrid;

        static const char* skyGridAlignDescs[] = {
            TXT(PS_SKYGRIDALIGN_ALONGWIND), TXT(PS_SKYGRIDALIGN_CROSSWIND),
            TXT(PS_SKYGRIDALIGN_ALONGRUNWAY), TXT(PS_SKYGRIDALIGN_CROSSRUNWAY)
        };
        int skyGridAlign = (int)options.mSkyGridAlignment;
        if (SettingsWidgets::Combo(TXT(PS_SKYGRIDALIGNMENT), skyGridAlign, skyGridAlignDescs, 4))
            options.mSkyGridAlignment = (Options::SkyGridAlignment)skyGridAlign;

        if (advanced)
            SettingsWidgets::SliderFloat(TXT(PS_SKYGRIDDISTANCE), options.mSkyGridDistance, 10.0f, 200.0f, "%.0f m");
    }
    SettingsWidgets::EndSettingsBlock();

    // Advanced graph/debug settings
    if (advanced)
    {
        if (!PicaSim::IsCreated() || PicaSim::GetInstance().GetSettings().mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
        {
            SettingsWidgets::SectionHeader(TXT(PS_INFORMATIONSETTINGS));
            SettingsWidgets::BeginSettingsBlock();
            {
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHDURATION), options.mGraphDuration, 5.0f, 240.0f, "%.0f s");
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHALTITUDE), options.mGraphAltitude, 0, 500.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHAIRSPEED), options.mGraphAirSpeed, 0, 200.0f, "%.0f m/s");
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHGROUNDSPEED), options.mGraphGroundSpeed, 0, 200.0f, "%.0f m/s");
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHCLIMBRATE), options.mGraphClimbRate, 0, 10.0f, "%.1f m/s");
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHWINDSPEED), options.mGraphWindSpeed, 0, 30.0f, "%.0f m/s");
                SettingsWidgets::SliderFloat(TXT(PS_GRAPHWINDVERTICALVELOCITY), options.mGraphWindVerticalVelocity, 0, 15.0f, "%.1f m/s");
            }
            SettingsWidgets::EndSettingsBlock();
        }

        SettingsWidgets::SliderFloat(TXT(PS_GRAPHFPS), options.mGraphFPS, 0, 60.0f);
        SettingsWidgets::Checkbox(TXT(PS_STALLMARKERS), options.mStallMarkers);

        static const char* comDescs[] = { TXT(PS_NONE), TXT(PS_CROSS), TXT(PS_BOX) };
        int com = (int)options.mDrawAeroplaneCoM;
        if (SettingsWidgets::Combo(TXT(PS_DRAWAEROPLANECOM), com, comDescs, 3))
            options.mDrawAeroplaneCoM = (Options::DrawCoMType)com;

        SettingsWidgets::SliderInt(TXT(PS_NUMWINDSTREAMERS), options.mNumWindStreamers, 0, 64);
        SettingsWidgets::SliderFloat(TXT(PS_WINDSTREAMERTIME), options.mWindStreamerTime, 0.1f, 60.0f, "%.1f s");
        SettingsWidgets::SliderFloat(TXT(PS_WINDSTREAMERDELTAZ), options.mWindStreamerDeltaZ, 0.1f, 10.0f, "%.1f m");

        // Graphics settings
        SettingsWidgets::SectionHeader(TXT(PS_GRAPHICSSETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_GROUNDTERRAINLOD), options.mGroundViewTerrainLOD, 0.0f, 500.0f);
            SettingsWidgets::SliderFloat(TXT(PS_AEROPLANETERRAINLOD), options.mAeroplaneViewTerrainLOD, 0.0f, 500.0f);
            SettingsWidgets::Checkbox(TXT(PS_UPDATETERRAINLOD), options.mGroundViewUpdateTerrainLOD);

            static const char* renderDescs[] = { TXT(PS_COMPONENTS), TXT(PS_3DMODEL), TXT(PS_BOTH) };
            int render = (int)options.mRenderPreference;
            if (SettingsWidgets::Combo(TXT(PS_AEROPLANERENDER), render, renderDescs, 3))
                options.mRenderPreference = (Options::RenderPreference)render;

            static const char* shadowDescs[] = { TXT(PS_NONE), TXT(PS_BLOB), TXT(PS_PROJECTED) };
            int controlledShadow = (int)options.mControlledPlaneShadows;
            if (SettingsWidgets::Combo(TXT(PS_CONTROLLEDPLANESHADOWS), controlledShadow, shadowDescs, 3))
                options.mControlledPlaneShadows = (Options::ShadowType)controlledShadow;

            int otherShadow = (int)options.mOtherShadows;
            if (SettingsWidgets::Combo(TXT(PS_OTHERSHADOWS), otherShadow, shadowDescs, 3))
                options.mOtherShadows = (Options::ShadowType)otherShadow;

            SettingsWidgets::SliderInt(TXT(PS_PROJECTEDSHADOWDETAIL), options.mProjectedShadowDetail, 7, 10);
            SettingsWidgets::SliderFloat(TXT(PS_SHADOWBLURMULTIPLIER), options.mShadowBlurMultiplier, 0.0f, 2.0f);
            SettingsWidgets::Checkbox(TXT(PS_USE16BIT), options.m16BitTextures);
            SettingsWidgets::Checkbox(TXT(PS_SEPARATESPECULAR), options.mSeparateSpecular);
            SettingsWidgets::SliderFloat(TXT(PS_AMBIENTLIGHTINGSCALE), options.mAmbientLightingScale, 0.0f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_DIFFUSELIGHTINGSCALE), options.mDiffuseLightingScale, 0.0f, 5.0f);
            SettingsWidgets::SliderInt(TXT(PS_TERRAINTEXTUREDETAIL), options.mBasicTextureDetail, 8, 10);
            SettingsWidgets::SliderInt(TXT(PS_MAXSKYBOXDETAIL), options.mMaxSkyboxDetail, 1, 2);

            // Anti-aliasing (MSAA) - requires restart
            static const char* msaaDescs[] = { TXT(PS_NONE), TXT(PS_ANTIALIASING_2X), TXT(PS_ANTIALIASING_4X), TXT(PS_ANTIALIASING_8X) };
            static const int msaaValues[] = { 0, 2, 4, 8 };
            int msaaIndex = 0;
            for (int i = 0; i < 4; ++i)
            {
                if (msaaValues[i] == options.mMSAASamples)
                {
                    msaaIndex = i;
                    break;
                }
            }
            if (SettingsWidgets::Combo(TXT(PS_ANTIALIASING), msaaIndex, msaaDescs, 4))
                options.mMSAASamples = msaaValues[msaaIndex];
            SettingsWidgets::InfoText(TXT(PS_REQUIRESRESTART));
        }
        SettingsWidgets::EndSettingsBlock();

        // Misc settings
        SettingsWidgets::SectionHeader(TXT(PS_MISCSETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
            if (advanced)
            {
                if (SettingsWidgets::SliderInt("Log Level", options.mFrameworkSettings.mLogLevel, 1, 9))
                    SetTraceLevel(options.mFrameworkSettings.mLogLevel);
            }
            if (SettingsWidgets::Button(TXT(PS_CLEARALLSAVEDSETTINGSANDEXIT)))
            {
                mStatus = SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT;
            }
        }
        SettingsWidgets::EndSettingsBlock();
    }
}

//======================================================================================================================
void SettingsMenu::RenderOptions2Tab()
{
    Options& options = mGameSettings.mOptions;
    Language language = options.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_OPTIONS2];
    bool isFreefly = mGameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY;

    // Free flight settings
    SettingsWidgets::SectionHeader(TXT(PS_FREEFLYSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::Checkbox(TXT(PS_FREEFLYONSTARTUP), options.mFreeFlyOnStartup);
        SettingsWidgets::InfoTextWrapped(TXT(PS_TOOMANYAI));
        int maxAI = advanced ? 50 : 10;
        SettingsWidgets::SliderInt(TXT(PS_MAXNUMBERAI), options.mFreeFlightMaxAI, 0, maxAI);

        // Display settings
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYFLIGHTTIME), options.mFreeFlightDisplayTime);
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYSPEED), options.mFreeFlightDisplaySpeed);
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYAIRSPEED), options.mFreeFlightDisplayAirSpeed);
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYMAXSPEED), options.mFreeFlightDisplayMaxSpeed);
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYASCENTRATE), options.mFreeFlightDisplayAscentRate);
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYALTITUDE), options.mFreeFlightDisplayAltitude);
        SettingsWidgets::Checkbox(TXT(PS_DISPLAYDISTANCE), options.mFreeFlightDisplayDistance);
        SettingsWidgets::Checkbox(TXT(PS_COLOURTEXT), options.mFreeFlightColourText);

        if (advanced)
        {
            SettingsWidgets::Checkbox(TXT(PS_ENABLESOCKETCONTROLLER), options.mEnableSocketController);
            SettingsWidgets::Checkbox(TXT(PS_TEXTATTOP), options.mFreeFlightTextAtTop);
            SettingsWidgets::SliderFloat(TXT(PS_TEXTBACKGROUNDOPACITY), options.mFreeFlightTextBackgroundOpacity, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TEXTBACKGROUNDCOLOUR), options.mFreeFlightTextBackgroundColour, 0.0f, 1.0f);
        }

        // Units dropdown
        static const char* unitDescs[] = { "m/s & m", "km/h & m", "mph & ft" };
        int units = (int)options.mFreeFlightUnits;
        if (SettingsWidgets::Combo(TXT(PS_UNITS), units, unitDescs, Options::UNITS_MAX))
            options.mFreeFlightUnits = (Options::Units)units;
    }
    SettingsWidgets::EndSettingsBlock();

    // Walkabout settings (only in freefly mode)
    if (isFreefly)
    {
        SettingsWidgets::SectionHeader(TXT(PS_WALKABOUTSETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::Checkbox(TXT(PS_ENABLEWALKABOUTBUTTON), options.mEnableWalkabout);
            if (advanced)
            {
                SettingsWidgets::Checkbox(TXT(PS_SETWINDDIRONWALKABOUT), options.mSetWindDirectionOnWalkabout);
            }
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Race settings
    SettingsWidgets::SectionHeader(TXT(PS_RACESETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::SliderFloat(TXT(PS_LIMBODIFFICULTY), options.mLimboDifficultyMultiplier, 1.0f, Options::LIMBO_MAX_DIFFICULTY_MULTIPLIER);
        if (advanced)
        {
            SettingsWidgets::SliderFloat(TXT(PS_RACEVIBRATIONAMOUNT), options.mRaceVibrationAmount, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_RACEBEEPVOLUME), options.mRaceBeepVolume, 0.0f, 1.0f);

            if (SettingsWidgets::Button(TXT(PS_DELETELOCALHIGHSCORES)))
            {
                mStatus = SETTINGS_DELETE_LOCAL_HIGHSCORES;
            }
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Misc settings (advanced only)
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_MISCSETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
#ifdef __ANDROID__
            SettingsWidgets::Checkbox(TXT(PS_USEBACKBUTTON), options.mUseBackButtonToExit);
#endif
            SettingsWidgets::Checkbox(TXT(PS_DRAWLAUNCHMARKER), options.mDrawLaunchMarker);
            SettingsWidgets::Checkbox(TXT(PS_USEAEROPLANEPREFERREDCONTROLLER), options.mUseAeroplanePreferredController);
        }
        SettingsWidgets::EndSettingsBlock();

        // Testing/developer settings (Windows only)
        if (options.mFrameworkSettings.isWindows())
        {
            SettingsWidgets::SectionHeader(TXT(PS_TESTINGDEVELOPERSETTINGS));
            SettingsWidgets::BeginSettingsBlock();
            {
                SettingsWidgets::Checkbox(TXT(PS_WIREFRAMETERRAIN), options.mTerrainWireframe);
                SettingsWidgets::Checkbox(TXT(PS_DRAWSUNPOSITION), options.mDrawSunPosition);
            }
            SettingsWidgets::EndSettingsBlock();
        }
    }
}

//======================================================================================================================
void SettingsMenu::RenderAeroplaneTab()
{
    AeroplaneSettings& as = mGameSettings.mAeroplaneSettings;
    AIAeroplaneSettings& aias = as.mAIAeroplaneSettings;
    Language language = mGameSettings.mOptions.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_AEROPLANE];
    bool allowSettings = mGameSettings.mChallengeSettings.mAllowAeroplaneSettings;
    bool isGlider = !(as.mType & 2);  // Not powered
    bool isNotHeli = !(as.mType & 1); // Not helicopter/control line

    // Thumbnail button
    Texture* thumbnail = GetCachedTexture(as.mThumbnail, mGameSettings.mOptions.m16BitTextures);
    float thumbnailHeight = (float)Platform::GetDisplayHeight() * 0.3f;

    if (SettingsWidgets::ThumbnailButton(thumbnail, as.mTitle.c_str(), as.mInfo.c_str(),
        thumbnailHeight, &sImageButtonX, &sImageButtonY, &sImageButtonW, &sImageButtonH))
    {
        mStatus = SETTINGS_LOAD_AEROPLANE;
    }

    // Website button (optional - only shown if URL is set)
    if (!as.mWWW.empty())
    {
        if (SettingsWidgets::Button(as.mWWW.c_str()))
        {
            Platform::OpenURL(as.mWWW.c_str());
        }
    }

    // General settings
    SettingsWidgets::SectionHeader(TXT(PS_GENERALSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        // Colour scheme selector - only shown if multiple models exist
        {
            const int maxModels = 10;
            TiXmlDocument doc(as.mName + "/Aeroplane.xml");
            std::vector<std::string> modelNames;
            if (doc.LoadFile())
            {
                TiXmlHandle graphicsHandle = doc.FirstChild("Graphics");
                int iModel = 0;
                while (iModel < maxModels)
                {
                    TiXmlElement* dataElement = graphicsHandle.Child("Model", iModel).ToElement();
                    if (!dataElement)
                        break;
                    std::string name = readStringFromXML(dataElement, "name");
                    modelNames.push_back(name);
                    ++iModel;
                }

                if (modelNames.size() > 1)
                {
                    SettingsWidgets::Combo(TXT(PS_COLOURSCHEME), as.mColourScheme, modelNames);
                }
            }
        }
        SettingsWidgets::SliderFloat(TXT(PS_COLOUROFFSET), as.mColourOffset, 0.0f, 1.0f);
        SettingsWidgets::SliderFloat(TXT(PS_BALLAST), as.mExtraMassPerCent, 0.0f, 100.0f, "%.0f %%");
        SettingsWidgets::SliderFloat(TXT(PS_BALLASTFWD), as.mExtraMassOffset.x, -1.0f, 1.0f, "%.2f m");

        if (advanced && allowSettings)
        {
            SettingsWidgets::SliderFloat(TXT(PS_BALLASTLEFT), as.mExtraMassOffset.y, -1.0f, 1.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_BALLASTUP), as.mExtraMassOffset.z, -1.0f, 1.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_DRAGMULTIPLIER), as.mDragScale, 0.2f, 4.0f);
            SettingsWidgets::SliderFloat(TXT(PS_SIZEMULTIPLIER), as.mSizeScale, 0.2f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_MASSMULTIPLIER), as.mMassScale, 0.2f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_ENGINEMULTIPLIER), as.mEngineScale, 0.2f, 5.0f);
        }

        SettingsWidgets::Checkbox(TXT(PS_SHOWBUTTON1), as.mShowButton[0]);
        SettingsWidgets::Checkbox(TXT(PS_SHOWBUTTON2), as.mShowButton[1]);

        if (advanced && allowSettings)
        {
            static const char* varioTypes[] = { TXT(PS_NO), TXT(PS_AUTO), TXT(PS_YES) };
            if (SettingsWidgets::Combo(TXT(PS_HASVARIOMETER), as.mHasVariometer, varioTypes, 3))
            {
                // Value updated by Combo
            }
        }

        // Preferred controller (clickable button showing current selection)
        if (SettingsWidgets::LabelButton(TXT(PS_PREFERREDCONTROLLER), as.mPreferredController.c_str()))
        {
            mStatus = SETTINGS_SELECT_PREFERRED_CONTROLLER;
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Launch settings
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_LAUNCH));
        SettingsWidgets::BeginSettingsBlock();
        {
            if (allowSettings && isGlider)
            {
                static const char* launchMethods[] = { TXT(PS_HAND), TXT(PS_BUNGEE), TXT(PS_AEROTOW) };
                int launchMethod = (int)as.mFlatLaunchMethod;
                if (SettingsWidgets::Combo(TXT(PS_FLATLAUNCHMETHOD), launchMethod, launchMethods, 3))
                    as.mFlatLaunchMethod = (AeroplaneSettings::FlatLaunchMethod)launchMethod;
            }

            SettingsWidgets::SliderFloat(TXT(PS_LAUNCHSPEED), as.mLaunchSpeed, 0.0f, 40.0f, "%.1f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_LAUNCHANGLE), as.mLaunchAngleUp, -45.0f, 45.0f, "%.0f deg");
            SettingsWidgets::SliderFloat(TXT(PS_LAUNCHUP), as.mLaunchUp, 0.0f, 10.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_LAUNCHFORWARD), as.mLaunchForwards, -30.0f, 30.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_LAUNCHLEFT), as.mLaunchLeft, -30.0f, 30.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_LAUNCHOFFSETUP), as.mLaunchOffsetUp, -2.0f, 2.0f, "%.2f m");
            SettingsWidgets::Checkbox(TXT(PS_RELAUNCHWHENSTATIONARY), as.mRelaunchWhenStationary);
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Hooks (advanced only)
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_HOOKS));
        SettingsWidgets::BeginSettingsBlock();
        {
            if (isGlider)
            {
                SettingsWidgets::SliderFloat(TXT(PS_BELLYHOOKOFFSETFORWARD), as.mBellyHookOffset.x, -0.5f, 0.5f, "%.2f m");
                SettingsWidgets::SliderFloat(TXT(PS_BELLYHOOKOFFSETUP), as.mBellyHookOffset.z, -1.0f, 0.0f, "%.2f m");
                SettingsWidgets::SliderFloat(TXT(PS_NOSEHOOKOFFSETFORWARD), as.mNoseHookOffset.x, -0.5f, 1.0f, "%.2f m");
                SettingsWidgets::SliderFloat(TXT(PS_NOSEHOOKOFFSETUP), as.mNoseHookOffset.z, -1.0f, 1.0f, "%.2f m");
            }
            else
            {
                SettingsWidgets::SliderFloat(TXT(PS_TAILHOOKOFFSETFORWARD), as.mNoseHookOffset.x, -3.0f, 0.5f, "%.2f m");
                SettingsWidgets::SliderFloat(TXT(PS_TAILHOOKOFFSETUP), as.mNoseHookOffset.z, -1.0f, 1.0f, "%.2f m");
            }
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Bungee launch (advanced, glider only)
    if (advanced && isGlider)
    {
        SettingsWidgets::SectionHeader(TXT(PS_BUNGEELAUNCH));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_MAXBUNGEELENGTH), as.mMaxBungeeLength, 50.0f, 300.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_MAXBUNGEEACCEL), as.mMaxBungeeAcceleration, 0.0f, 100.0f, "%.0f m/s^2");
        }
        SettingsWidgets::EndSettingsBlock();

        // Aerotow launch
        SettingsWidgets::SectionHeader(TXT(PS_AEROTOWLAUNCH));
        SettingsWidgets::BeginSettingsBlock();
        {
            // Tug plane (clickable button showing current selection)
            if (SettingsWidgets::LabelButton(TXT(PS_TUGPLANE), as.mTugName.c_str()))
            {
                mStatus = SETTINGS_LOAD_TUG;
            }
            SettingsWidgets::SliderFloat(TXT(PS_TUGSIZESCALE), as.mTugSizeScale, 0.5f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TUGMASSSCALE), as.mTugMassScale, 0.5f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TUGENGINESCALE), as.mTugEngineScale, 0.5f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TUGMAXCLIMBSLOPE), as.mTugMaxClimbSlope, 0.0f, 0.3f);
            SettingsWidgets::SliderFloat(TXT(PS_TUGTARGETSPEED), as.mTugTargetSpeed, 2.0f, 40.0f, "%.1f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_AEROTOWROPELENGTH), as.mAeroTowRopeLength, 2.0f, 50.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_AEROTOWROPESTRENGTH), as.mAeroTowRopeStrength, 1.0f, 100.0f);
            SettingsWidgets::SliderFloat(TXT(PS_AEROTOWROPEMASSSCALE), as.mAeroTowRopeMassScale, 0.2f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_AEROTOWROPEDRAGSCALE), as.mAeroTowRopeDragScale, 0.2f, 5.0f);
            SettingsWidgets::SliderFloat(TXT(PS_AEROTOWHEIGHT), as.mAeroTowHeight, 2.0f, 400.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_AEROTOWCIRCUITSIZE), as.mAeroTowCircuitSize, 10.0f, 500.0f, "%.0f m");
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Crash detection (advanced only)
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_CRASHDETECTION));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_CRASHDELTAVELX), as.mCrashDeltaVel.x, 0.0f, 100.0f, "%.0f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRASHDELTAVELY), as.mCrashDeltaVel.y, 0.0f, 100.0f, "%.0f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRASHDELTAVELZ), as.mCrashDeltaVel.z, 0.0f, 100.0f, "%.0f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRASHDELTAANGVELX), as.mCrashDeltaAngVel.x, 0.0f, 10000.0f, "%.0f deg/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRASHDELTAANGVELY), as.mCrashDeltaAngVel.y, 0.0f, 10000.0f, "%.0f deg/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRASHDELTAANGVELZ), as.mCrashDeltaAngVel.z, 0.0f, 10000.0f, "%.0f deg/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRASHSUSPENSIONFORCESCALE), as.mCrashSuspensionForceScale, 0.1f, 100.0f);
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Tethering (advanced, not helicopter)
    if (advanced && isNotHeli)
    {
        SettingsWidgets::SectionHeader(TXT(PS_TETHERING));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderInt(TXT(PS_TETHERLINES), as.mTetherLines, 0, 2);
            SettingsWidgets::Checkbox(TXT(PS_TETHERREQUIRESTENSION), as.mTetherRequiresTension);
            SettingsWidgets::SliderFloat(TXT(PS_TETHERPHYSICSOFFSETFORWARD), as.mTetherPhysicsOffset.x, -2.0f, 2.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERPHYSICSOFFSETLEFT), as.mTetherPhysicsOffset.y, -5.0f, 5.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERPHYSICSOFFSETUP), as.mTetherPhysicsOffset.z, -5.0f, 5.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERVISUALOFFSETFORWARD), as.mTetherVisualOffset.x, -2.0f, 2.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERVISUALOFFSETLEFT), as.mTetherVisualOffset.y, -5.0f, 5.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERVISUALOFFSETUP), as.mTetherVisualOffset.z, -5.0f, 5.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERDISTANCELEFT), as.mTetherDistanceLeft, -30.0f, 30.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_TETHERCOLOURR), as.mTetherColour.x, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TETHERCOLOURG), as.mTetherColour.y, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TETHERCOLOURB), as.mTetherColour.z, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_TETHERCOLOURA), as.mTetherColour.w, 0.0f, 1.0f);
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Chase camera (advanced only)
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_CHASECAMERA));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_CAMERATARGETPOSFWD), as.mCameraTargetPosFwd, -2.0f, 2.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_CAMERATARGETPOSUP), as.mCameraTargetPosUp, -2.0f, 2.0f, "%.2f m");
            SettingsWidgets::SliderFloat(TXT(PS_DISTANCE), as.mChaseCamDistance, 1.0f, 10.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_HEIGHT), as.mChaseCamHeight, 0.0f, 3.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_VERTICALVELFRAC), as.mChaseCamVerticalVelMult, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_FLEXIBILITY), as.mChaseCamFlexibility, 0.0f, 1.0f);
        }
        SettingsWidgets::EndSettingsBlock();

        SettingsWidgets::SectionHeader(TXT(PS_COCKPITCAMERA));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_PITCH), as.mCockpitCamPitch, -90.0f, 90.0f, "%.0f deg");
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // AI Controller (advanced only)
    if (advanced)
    {
        SettingsWidgets::SectionHeader(TXT(PS_AICONTROLLER));
        SettingsWidgets::BeginSettingsBlock();
        {
            static const char* planeTypes[] = { TXT(PS_GLIDER), TXT(PS_POWERED), TXT(PS_HELI), TXT(PS_CONTROLLINE) };
            int planeType = (int)as.mAIType;
            if (SettingsWidgets::Combo(TXT(PS_PLANETYPE), planeType, planeTypes, 4))
                as.mAIType = (AeroplaneSettings::AIType)planeType;

            SettingsWidgets::Checkbox(TXT(PS_ALLOWAICONTROL), aias.mAllowAIControl);
            SettingsWidgets::Checkbox(TXT(PS_CANTOW), aias.mCanTow);
            SettingsWidgets::SliderFloat(TXT(PS_WAYPOINTTOLERANCE), aias.mWaypointTolerance, 0.1f, 50.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_MINSPEED), aias.mGliderControlMinSpeed, 1.0f, 40.0f, "%.1f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_CRUISESPEED), aias.mGliderControlCruiseSpeed, 1.0f, 50.0f, "%.1f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_MAXBANKANGLE), aias.mControlMaxBankAngle, 10.0f, 90.0f, "%.0f deg");
            SettingsWidgets::SliderFloat(TXT(PS_BANKANGLEPERHEADINGCHANGE), aias.mControlBankAnglePerHeadingChange, 0.0f, 4.0f);
            SettingsWidgets::SliderFloat(TXT(PS_SPEEDPERALTITUDECHANGE), aias.mGliderControlSpeedPerAltitudeChange, 0.0f, 4.0f, "%.2f s^-1");
            SettingsWidgets::SliderFloat(TXT(PS_GLIDESLOPEPEREXCESSSPEED), aias.mGliderControlSlopePerExcessSpeed, 0.0f, 0.2f, "%.3f s/m");
            SettingsWidgets::SliderFloat(TXT(PS_PITCHPERROLLANGLE), aias.mControlPitchControlPerRollAngle, 0.0f, 0.02f, "%.4f deg^-1");
            SettingsWidgets::SliderFloat(TXT(PS_HEADINGCHANGEFORNOSLOPE), aias.mGliderControlHeadingChangeForNoSlope, 0.0f, 360.0f, "%.0f deg");
            SettingsWidgets::SliderFloat(TXT(PS_MAXPITCHCONTROL), aias.mControlMaxPitchControl, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_MAXROLLCONTROL), aias.mControlMaxRollControl, 0.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_CONTROLPERROLLANGLE), aias.mControlRollControlPerRollAngle, 0.0f, 0.05f, "%.4f deg^-1");
            SettingsWidgets::SliderFloat(TXT(PS_PITCHCONTROLPERGLIDESLOPE), aias.mGliderControlPitchControlPerGlideSlope, 0.0f, 10.0f);
            SettingsWidgets::SliderFloat(TXT(PS_ROLLTIMESCALE), aias.mControlRollTimeScale, 0.0f, 0.5f, "%.2f s");
            SettingsWidgets::SliderFloat(TXT(PS_PITCHTIMESCALE), aias.mControlPitchTimeScale, 0.0f, 0.5f, "%.2f s");
        }
        SettingsWidgets::EndSettingsBlock();

        SettingsWidgets::SectionHeader(TXT(PS_AINAVIGATION));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_MINALTITUDE), aias.mGliderControlMinAltitude, -100.0f, 100.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMINUPWINDDISTANCE), aias.mSlopeMinUpwindDistance, -500.0f, 500.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMAXUPWINDDISTANCE), aias.mSlopeMaxUpwindDistance, -500.0f, 500.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMINLEFTDISTANCE), aias.mSlopeMinLeftDistance, -500.0f, 500.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMAXLEFTDISTANCE), aias.mSlopeMaxLeftDistance, -500.0f, 500.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMINUPDISTANCE), aias.mSlopeMinUpDistance, -100.0f, 100.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMAXUPDISTANCE), aias.mSlopeMaxUpDistance, -100.0f, 100.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_SLOPEMAXWAYPOINTTIME), aias.mSlopeMaxWaypointTime, 10.0f, 1000.0f, "%.0f s");
            SettingsWidgets::SliderFloat(TXT(PS_FLATMAXDISTANCE), aias.mFlatMaxDistance, 10.0f, 1000.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_FLATMAXWAYPOINTTIME), aias.mFlatMaxWaypointTime, 10.0f, 1000.0f, "%.0f s");
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Smoke settings
    for (int i = 0; i < AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE; ++i)
    {
        AeroplaneSettings::SmokeSource& smoke = as.mSmokeSources[i];
        char sectionLabel[64];
        sprintf(sectionLabel, TXT(PS_SMOKESOURCE), i + 1);

        // Use smoke color for section header background
        SettingsWidgets::SectionHeaderColored(sectionLabel, smoke.mColour.x, smoke.mColour.y, smoke.mColour.z);
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::Checkbox(TXT(PS_ENABLE), smoke.mEnable);

            if (smoke.mEnable)
            {
                char label[64];

                // HSV color sliders
                if (SettingsWidgets::SliderFloat(TXT(PS_COLOURH), mSmokeHSVs[i].x, 0.0f, 1.0f))
                    smoke.mColour = HSV2RGB(mSmokeHSVs[i]);

                if (SettingsWidgets::SliderFloat(TXT(PS_COLOURS), mSmokeHSVs[i].y, 0.0f, 1.0f))
                    smoke.mColour = HSV2RGB(mSmokeHSVs[i]);

                if (SettingsWidgets::SliderFloat(TXT(PS_COLOURV), mSmokeHSVs[i].z, 0.0f, 1.0f))
                    smoke.mColour = HSV2RGB(mSmokeHSVs[i]);

                if (advanced)
                {
                    static const char* channelDescs[Controller::MAX_CHANNELS + 1];
                    channelDescs[Controller::CHANNEL_AILERONS] = TXT(PS_AILERONS);
                    channelDescs[Controller::CHANNEL_ELEVATOR] = TXT(PS_ELEVATOR);
                    channelDescs[Controller::CHANNEL_RUDDER] = TXT(PS_RUDDER);
                    channelDescs[Controller::CHANNEL_THROTTLE] = TXT(PS_THROTTLE);
                    channelDescs[Controller::CHANNEL_LOOKYAW] = TXT(PS_LOOKYAW);
                    channelDescs[Controller::CHANNEL_LOOKPITCH] = TXT(PS_LOOKPITCH);
                    channelDescs[Controller::CHANNEL_AUX1] = TXT(PS_AUX1);
                    channelDescs[Controller::CHANNEL_SMOKE1] = TXT(PS_SMOKE1);
                    channelDescs[Controller::CHANNEL_SMOKE2] = TXT(PS_SMOKE2);
                    channelDescs[Controller::CHANNEL_HOOK] = TXT(PS_HOOK);
                    channelDescs[Controller::MAX_CHANNELS] = TXT(PS_CONSTANT);

                    SettingsWidgets::SliderFloat(TXT(PS_POSITIONX), smoke.mOffset.x, -5.0f, 5.0f, "%.2f m");
                    SettingsWidgets::SliderFloat(TXT(PS_POSITIONY), smoke.mOffset.y, -5.0f, 5.0f, "%.2f m");
                    SettingsWidgets::SliderFloat(TXT(PS_POSITIONZ), smoke.mOffset.z, -5.0f, 5.0f, "%.2f m");
                    SettingsWidgets::SliderFloat(TXT(PS_VELFWD), smoke.mVel.x, -20.0f, 20.0f, "%.1f m/s");
                    SettingsWidgets::SliderFloat(TXT(PS_VELLEFT), smoke.mVel.y, -20.0f, 20.0f, "%.1f m/s");
                    SettingsWidgets::SliderFloat(TXT(PS_VELUP), smoke.mVel.z, -20.0f, 20.0f, "%.1f m/s");
                    SettingsWidgets::SliderInt(TXT(PS_MAXPARTICLES), smoke.mMaxParticles, 100, 10000);

                    int channelForAlpha = (int)smoke.mChannelForAlpha;
                    if (SettingsWidgets::Combo(TXT(PS_CHANNELFOROPACITY), channelForAlpha, channelDescs, Controller::MAX_CHANNELS + 1))
                        smoke.mChannelForAlpha = (Controller::Channel)channelForAlpha;

                    SettingsWidgets::SliderFloat(TXT(PS_MINOPACITY), smoke.mMinAlpha, -1.0f, 1.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_MAXOPACITY), smoke.mMaxAlpha, -1.0f, 1.0f);

                    int channelForRate = (int)smoke.mChannelForRate;
                    if (SettingsWidgets::Combo(TXT(PS_CHANNELFORRATE), channelForRate, channelDescs, Controller::MAX_CHANNELS + 1))
                        smoke.mChannelForRate = (Controller::Channel)channelForRate;

                    SettingsWidgets::SliderFloat(TXT(PS_MINRATE), smoke.mMinRate, -90.0f, 90.0f, "%.0f Hz");
                    SettingsWidgets::SliderFloat(TXT(PS_MAXRATE), smoke.mMaxRate, -90.0f, 90.0f, "%.0f Hz");
                    SettingsWidgets::SliderFloat(TXT(PS_INITIALSIZE), smoke.mInitialSize, 0.0f, 1.0f, "%.2f m");
                    SettingsWidgets::SliderFloat(TXT(PS_FINALSIZE), smoke.mFinalSize, 0.0f, 10.0f, "%.1f m");
                    SettingsWidgets::SliderFloat(TXT(PS_LIFESPAN), smoke.mLifetime, 1.0f, 20.0f, "%.1f sec");
                    SettingsWidgets::SliderFloat(TXT(PS_DAMPINGTIME), smoke.mDampingTime, 0.0f, 2.0f, "%.2f sec");
                    SettingsWidgets::SliderFloat(TXT(PS_JITTER), smoke.mVelJitter, 0.0f, 1.0f, "%.2f m/s");
                    SettingsWidgets::SliderFloat(TXT(PS_ENGINEWASH), smoke.mEngineWash, 0.0f, 1.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_HUECYCLEFREQ), smoke.mHueCycleFreq, 0.0f, 2.0f, "%.2f Hz");
                }
            }
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Info section - show computed aeroplane properties
    if (PicaSim::IsCreated())
    {
        Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane(0);
        if (aeroplane)
        {
            const AeroplanePhysics* physics = aeroplane->GetPhysics();
            if (physics)
            {
                SettingsWidgets::SectionHeader(TXT(PS_INFO));
                SettingsWidgets::BeginSettingsBlock();
                {
                    char buf[128];

                    // Mass (always shown)
                    snprintf(buf, sizeof(buf), "%.3f kg", physics->GetMass());
                    SettingsWidgets::InfoLabel(TXT(PS_MASS), buf);

                    // Additional info in advanced mode
                    if (advanced)
                    {
                        // Inertia
                        Vector3 inertia = physics->GetInertiaLocal();
                        snprintf(buf, sizeof(buf), "%.3f, %.3f, %.3f", inertia.x, inertia.y, inertia.z);
                        SettingsWidgets::InfoLabel(TXT(PS_INERTIA), buf);

                        // Wing area (if available)
                        float wingArea = physics->GetTotalWingArea();
                        if (wingArea > 0.0f)
                        {
                            snprintf(buf, sizeof(buf), "%.2f m^2", wingArea);
                            SettingsWidgets::InfoLabel(TXT(PS_WINGAREA), buf);
                        }

                        // Extents (AABB size)
                        Vector3 aabbMin, aabbMax;
                        physics->getLocalAABB(aabbMin, aabbMax);
                        Vector3 extents = aabbMax - aabbMin;
                        snprintf(buf, sizeof(buf), "%.3f, %.3f, %.3f m", extents.x, extents.y, extents.z);
                        SettingsWidgets::InfoLabel(TXT(PS_EXTENTS), buf);

                        // Current position
                        Vector3 pos = aeroplane->GetTransform().GetTrans();
                        snprintf(buf, sizeof(buf), "%.1f, %.1f, %.1f m", pos.x, pos.y, pos.z);
                        SettingsWidgets::InfoLabel(TXT(PS_CURRENTPOSITION), buf);

                        // Current orientation (yaw, pitch, roll in degrees)
                        const Transform& tm = aeroplane->GetTransform();
                        Vector3 fwd = tm.RowX();
                        Vector3 up = tm.RowZ();
                        float pitch = asinf(ClampToRange(-fwd.z, -1.0f, 1.0f)) * 180.0f / 3.14159265f;
                        float yaw = atan2f(fwd.y, fwd.x) * 180.0f / 3.14159265f;
                        float roll = atan2f(tm.RowY().z, up.z) * 180.0f / 3.14159265f;
                        snprintf(buf, sizeof(buf), "%.1f, %.1f, %.1f deg", yaw, pitch, roll);
                        SettingsWidgets::InfoLabel(TXT(PS_ORIENTATION), buf);
                    }
                }
                SettingsWidgets::EndSettingsBlock();
            }
        }
    }
}

//======================================================================================================================
void SettingsMenu::RenderSceneryTab()
{
    EnvironmentSettings& es = mGameSettings.mEnvironmentSettings;
    TerrainSettings& ts = es.mTerrainSettings;
    AIEnvironmentSettings& aies = es.mAIEnvironmentSettings;
    Language language = mGameSettings.mOptions.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_SCENERY];

    // Thumbnail button
    Texture* thumbnail = GetCachedTexture(es.mThumbnail, mGameSettings.mOptions.m16BitTextures);
    float thumbnailHeight = (float)Platform::GetDisplayHeight() * 0.3f;

    if (SettingsWidgets::ThumbnailButton(thumbnail, es.mTitle.c_str(), es.mInfo.c_str(),
        thumbnailHeight, &sImageButtonX, &sImageButtonY, &sImageButtonW, &sImageButtonH))
    {
        mStatus = SETTINGS_LOAD_ENVIRONMENT;
    }

    // Website button (optional - only shown if URL is set)
    if (!es.mWWW.empty())
    {
        if (SettingsWidgets::Button(es.mWWW.c_str()))
        {
            Platform::OpenURL(es.mWWW.c_str());
        }
    }

    // Wind settings
    SettingsWidgets::SectionHeader(TXT(PS_WINDSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        if (mGameSettings.mChallengeSettings.mAllowWindStrengthSetting)
        {
            SettingsWidgets::SliderFloat(TXT(PS_WINDSPEED), es.mWindSpeed, 0.00001f, 20.0f, "%.2f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_WINDBEARING), es.mWindBearing, -180.0f, 180.0f, "%.0f deg");
        }
        else
        {
            char str[256];
            sprintf(str, "%.2f m/s", es.mWindSpeed);
            SettingsWidgets::InfoLabel(TXT(PS_WINDSPEED), str);
            sprintf(str, "%.0f deg", es.mWindBearing);
            SettingsWidgets::InfoLabel(TXT(PS_WINDBEARING), str);
        }

        if (mGameSettings.mChallengeSettings.mAllowEnvironmentSettings)
        {
            if (advanced)
            {
                SettingsWidgets::Checkbox(TXT(PS_ALLOWBUNGEE), es.mAllowBungeeLaunch);
                SettingsWidgets::SliderFloat(TXT(PS_WINDGUSTTIME), es.mWindGustTime, 15.0f, 300.0f, "%.1f s");
                SettingsWidgets::SliderFloat(TXT(PS_WINDGUSTFRACTION), es.mWindGustAmplitudeFraction, 0.0f, 1.0f);
                SettingsWidgets::SliderFloat(TXT(PS_WINDGUSTANGLE), es.mWindGustBearingAmplitude, 0.0f, 90.0f, "%.0f deg");
            }
            SettingsWidgets::SliderFloat(TXT(PS_TURBULENCEAMOUNT), es.mTurbulenceAmount, 0.0f, 5.0f);
            if (advanced)
            {
                SettingsWidgets::SliderFloat(TXT(PS_SURFACETURBULENCEAMOUNT), es.mSurfaceTurbulence, 0.0f, 2.0f);
                SettingsWidgets::SliderFloat(TXT(PS_SHEARTURBULENCEAMOUNT), es.mShearTurbulence, 0.0f, 2.0f);
                SettingsWidgets::SliderFloat(TXT(PS_DEADAIRTURBULENCE), es.mDeadAirTurbulence, 0.0f, 2.0f);
                SettingsWidgets::SliderFloat(TXT(PS_WINDLIFTSMOOTHING), es.mWindLiftSmoothing, 0.01f, 10.0f);
                SettingsWidgets::SliderFloat(TXT(PS_VERTICALWINDDECAYDISTANCE), es.mVerticalWindDecayDistance, 10.0f, 400.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_SEPARATIONTENDENCY), es.mSeparationTendency, 0.0f, 5.0f);
                SettingsWidgets::SliderFloat(TXT(PS_ROTORTENDENCY), es.mRotorTendency, 0.0f, 1.0f);
                SettingsWidgets::SliderFloat(TXT(PS_BOUNDARYLAYERDEPTH), es.mBoundaryLayerDepth, 10.0f, 500.0f, "%.0f m");
            }
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Early return if environment settings not allowed
    if (!mGameSettings.mChallengeSettings.mAllowEnvironmentSettings)
    {
        SettingsWidgets::InfoText(TXT(PS_CANNOTMODIFYSCENERY));
        return;
    }

    // Thermal settings
    SettingsWidgets::SectionHeader(TXT(PS_THERMALSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::SliderFloat(TXT(PS_DENSITY), es.mThermalDensity, 0.0f, 50.0f, "%.1f per km^2");
        if (advanced)
        {
            SettingsWidgets::SliderFloat(TXT(PS_RANGE), es.mThermalRange, 100.0f, 10000.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_LIFESPAN), es.mThermalAverageLifeSpan, 60.0f, 600.0f, "%.0f s");
            SettingsWidgets::SliderFloat(TXT(PS_DEPTH), es.mThermalAverageDepth, 10.0f, 500.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_CORERADIUS), es.mThermalAverageCoreRadius, 20.0f, 200.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_DOWNDRAFTEXTENT), es.mThermalAverageDowndraftRadius, 50.0f, 200.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_UPDRAFTSPEED), es.mThermalAverageUpdraftSpeed, 0.0f, 10.0f, "%.1f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_ASCENTRATE), es.mThermalAverageAscentRate, 0.0f, 5.0f, "%.1f m/s");
            SettingsWidgets::SliderFloat(TXT(PS_THERMALEXPANSIONOVERLIFESPAN), es.mThermalExpansionOverLifespan, 1.0f, 5.0f);
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Runway settings
    SettingsWidgets::SectionHeader(TXT(PS_RUNWAY));
    SettingsWidgets::BeginSettingsBlock();
    {
        static const char* runwayTypes[] = { TXT(PS_NONE), TXT(PS_RUNWAY), TXT(PS_CIRCLE) };
        int runwayType = (int)es.mRunwayType;
        if (SettingsWidgets::Combo(TXT(PS_RUNWAYTYPE), runwayType, runwayTypes, 3))
            es.mRunwayType = (EnvironmentSettings::RunwayType)runwayType;

        SettingsWidgets::SliderFloatPower(TXT(PS_RUNWAYLENGTH), es.mRunwayLength, 10.0f, 200.0f, 4.0f, "%.0f m");

        if (advanced)
        {
            SettingsWidgets::SliderFloat(TXT(PS_RUNWAYX), es.mRunwayPosition.x, -1000.0f, 1000.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_RUNWAYY), es.mRunwayPosition.y, -1000.0f, 1000.0f, "%.0f m");
            SettingsWidgets::SliderFloat(TXT(PS_RUNWAYHEIGHT), es.mRunwayPosition.z, -1.0f, 5.0f, "%.1f m");
            SettingsWidgets::SliderFloat(TXT(PS_RUNWAYANGLE), es.mRunwayAngle, 0.0f, 180.0f, "%.0f deg");
            SettingsWidgets::SliderFloat(TXT(PS_RUNWAYWIDTH), es.mRunwayWidth, 1.0f, 20.0f, "%.0f m");
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Advanced-only sections
    if (advanced)
    {
        // Surface settings
        SettingsWidgets::SectionHeader(TXT(PS_SURFACESETTINGS));
        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::SliderFloat(TXT(PS_SURFACEROUGHNESS), ts.mSurfaceRoughness, 0.0f, 0.05f, "%.3f m");
            SettingsWidgets::SliderFloat(TXT(PS_SURFACEFRICTION), ts.mFriction, 0.0f, 3.0f);
        }
        SettingsWidgets::EndSettingsBlock();

        // Terrain type-specific settings
        if (ts.mType == TerrainSettings::TYPE_MIDPOINT_DISPLACEMENT)
        {
            SettingsWidgets::SectionHeader(TXT(PS_RANDOMTERRAINSETTINGS));
            SettingsWidgets::BeginSettingsBlock();
            {
                SettingsWidgets::SliderInt(TXT(PS_RANDOMSEED), ts.mMidpointDisplacementSeed, 0, 500);
                SettingsWidgets::SliderFloat(TXT(PS_DISPLACEMENTHEIGHT), ts.mMidpointDisplacementHeight, 0.0f, 300.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_SMOOTHNESS), ts.mMidpointDisplacementRoughness, 0.9f, 1.3f);
                SettingsWidgets::SliderFloat(TXT(PS_EDGEHEIGHT), ts.mMidpointDisplacementEdgeHeight, -200.0f, 0.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_UPWARDSBIAS), ts.mMidpointDisplacementUpwardsBias, 0.0f, 1.0f);
                SettingsWidgets::SliderInt(TXT(PS_FILTERITERATIONS), ts.mMidpointDisplacementFilterIterations, 0, 5);

                SettingsWidgets::Checkbox(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
                SettingsWidgets::Checkbox(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);
                SettingsWidgets::SliderFloat(TXT(PS_PLAININNERRADIUS), ts.mPlainInnerRadius, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINFOGDISTANCE), ts.mPlainFogDistance, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINHEIGHT), ts.mPlainHeight, -200.0f, 200.0f, "%.0f m");

                SettingsWidgets::SliderFloat(TXT(PS_TERRAINSIZE), ts.mTerrainSize, 1000.0f, 10000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_COASTENHANCEMENT), ts.mCoastEnhancement, 0.0f, 10.0f);
                SettingsWidgets::SliderInt(TXT(PS_TERRAINDETAIL), ts.mHeightmapDetail, 6, 12);
            }
            SettingsWidgets::EndSettingsBlock();
        }
        else if (ts.mType == TerrainSettings::TYPE_RIDGE)
        {
            SettingsWidgets::SectionHeader(TXT(PS_RIDGETERRAINSETTINGS));
            SettingsWidgets::BeginSettingsBlock();
            {
                SettingsWidgets::SliderFloat(TXT(PS_HEIGHT), ts.mRidgeHeight, 0.0f, 500.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_MAXHEIGHTFRACTION), ts.mRidgeMaxHeightFraction, 0.0f, 1.0f);
                SettingsWidgets::SliderFloat(TXT(PS_WIDTH), ts.mRidgeWidth, 0.0f, 500.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_HEIGHTOFFSET), ts.mRidgeEdgeHeight, -50.0f, 50.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_HORIZONTALVARIATION), ts.mRidgeHorizontalVariation, -50.0f, 50.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_HORIZONTALWAVELENGTH), ts.mRidgeHorizontalVariationWavelength, 50.0f, 2000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_VERTICALVARIATION), ts.mRidgeVerticalVariationFraction, 0.0f, 1.0f);

                SettingsWidgets::Checkbox(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
                SettingsWidgets::Checkbox(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);
                SettingsWidgets::SliderFloat(TXT(PS_PLAININNERRADIUS), ts.mPlainInnerRadius, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINFOGDISTANCE), ts.mPlainFogDistance, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINHEIGHT), ts.mPlainHeight, -200.0f, 200.0f, "%.0f m");

                SettingsWidgets::SliderFloat(TXT(PS_TERRAINSIZE), ts.mTerrainSize, 1000.0f, 10000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_COASTENHANCEMENT), ts.mCoastEnhancement, 0.0f, 10.0f);
                SettingsWidgets::SliderInt(TXT(PS_TERRAINDETAIL), ts.mHeightmapDetail, 6, 12);
            }
            SettingsWidgets::EndSettingsBlock();
        }
        else if (ts.mType == TerrainSettings::TYPE_PANORAMA_3D)
        {
            SettingsWidgets::SectionHeader(TXT(PS_PANORAMA3DSETTINGS));
            SettingsWidgets::BeginSettingsBlock();
            {
                SettingsWidgets::Checkbox(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
                SettingsWidgets::Checkbox(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);
                SettingsWidgets::SliderFloat(TXT(PS_PLAININNERRADIUS), ts.mPlainInnerRadius, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINFOGDISTANCE), ts.mPlainFogDistance, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINHEIGHT), ts.mPlainHeight, -200.0f, 200.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_COASTENHANCEMENT), ts.mCoastEnhancement, 0.0f, 10.0f);
            }
            SettingsWidgets::EndSettingsBlock();
        }
        else if (ts.mType == TerrainSettings::TYPE_FILE_TERRAIN)
        {
            SettingsWidgets::SectionHeader(TXT(PS_HEIGHTFIELDSETTINGS));
            SettingsWidgets::BeginSettingsBlock();
            {
                SettingsWidgets::SliderFloat(TXT(PS_MINHEIGHT), ts.mFileTerrainMinZ, -100.0f, 500.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_MAXHEIGHT), ts.mFileTerrainMaxZ, -100.0f, 500.0f, "%.0f m");

                SettingsWidgets::Checkbox(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
                SettingsWidgets::Checkbox(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);
                SettingsWidgets::SliderFloat(TXT(PS_PLAININNERRADIUS), ts.mPlainInnerRadius, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINFOGDISTANCE), ts.mPlainFogDistance, 1000.0f, 50000.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_PLAINHEIGHT), ts.mPlainHeight, -200.0f, 200.0f, "%.0f m");
                SettingsWidgets::SliderFloat(TXT(PS_COASTENHANCEMENT), ts.mCoastEnhancement, 0.0f, 10.0f);
            }
            SettingsWidgets::EndSettingsBlock();
        }

        // AI scenery settings
        SettingsWidgets::SectionHeader(TXT(PS_AISCENERY));
        SettingsWidgets::BeginSettingsBlock();
        {
            static const char* sceneTypes[] = { TXT(PS_FLAT), TXT(PS_SLOPE) };
            int sceneType = (int)aies.mSceneType;
            if (SettingsWidgets::Combo(TXT(PS_SCENETYPE), sceneType, sceneTypes, 2))
                aies.mSceneType = (AIEnvironmentSettings::SceneType)sceneType;
        }
        SettingsWidgets::EndSettingsBlock();

        // Objects settings
        SettingsWidgets::SectionHeader(TXT(PS_OBJECTS));
        SettingsWidgets::BeginSettingsBlock();
        {
            // Display filename without path and extension
            std::string objectsFile = es.mObjectsSettingsFile;
            size_t lastSlash = objectsFile.find_last_of("/\\");
            if (lastSlash != std::string::npos)
                objectsFile = objectsFile.substr(lastSlash + 1);
            size_t lastDot = objectsFile.find_last_of('.');
            if (lastDot != std::string::npos)
                objectsFile = objectsFile.substr(0, lastDot);

            if (SettingsWidgets::LabelButton(TXT(PS_OBJECTSSETTINGS), objectsFile.c_str()))
            {
                mStatus = SETTINGS_SELECT_OBJECTS_SETTINGS;
            }
        }
        SettingsWidgets::EndSettingsBlock();

        // Info section
        SettingsWidgets::SectionHeader(TXT(PS_INFO));
        SettingsWidgets::BeginSettingsBlock();
        {
            char posString[256];
            sprintf(posString, "%.2f, %.2f, %.2f", es.mObserverPosition.x, es.mObserverPosition.y, es.mObserverPosition.z);
            SettingsWidgets::InfoLabel(TXT(PS_CURRENTVIEWPOSITION), posString);
            sprintf(posString, "%.1f deg", es.mWindBearing);
            SettingsWidgets::InfoLabel(TXT(PS_WINDBEARING), posString);
        }
        SettingsWidgets::EndSettingsBlock();
    }
}

//======================================================================================================================
void SettingsMenu::RenderObjectsTab()
{
    ObjectsSettings& os = mGameSettings.mObjectsSettings;
    Options& options = mGameSettings.mOptions;
    Language language = options.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_OBJECTS];

    // Check if environment settings are allowed
    if (!mGameSettings.mChallengeSettings.mAllowEnvironmentSettings)
    {
        SettingsWidgets::InfoText(TXT(PS_CANNOTMODIFYSCENERY));
        return;
    }

    char txt[256];

    // General Settings section
    SettingsWidgets::SectionHeader(TXT(PS_GENERALSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::Checkbox(TXT(PS_ENABLEOBJECTEDITING), options.mEnableObjectEditing);
        SettingsWidgets::Checkbox(TXT(PS_FORCEALLVISIBLE), os.mForceAllVisible);

        // Reset Objects button - only shown if there are objects and PicaSim is created
        if (!os.mBoxes.empty() && PicaSim::IsCreated())
        {
            if (SettingsWidgets::Button(TXT(PS_RESETOBJECTS)))
            {
                mStatus = SETTINGS_RESET_OBJECTS;
            }
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Summary section
    SettingsWidgets::SectionHeader(TXT(PS_SUMMARY));
    SettingsWidgets::BeginSettingsBlock();
    {
        if (os.mBoxes.empty())
        {
            SettingsWidgets::InfoText(" ");
            SettingsWidgets::InfoTextWrapped(TXT(PS_NOOBJECTS));
            SettingsWidgets::InfoText(" ");
        }
        else
        {
            size_t numObjects, numStaticVisible, numStaticInvisible, numDynamicVisible;
            os.GetStats(numObjects, numStaticVisible, numStaticInvisible, numDynamicVisible);

            sprintf(txt, "%lu", (unsigned long)numObjects);
            SettingsWidgets::InfoLabel(TXT(PS_OBJECTSTOTAL), txt);

            sprintf(txt, "%lu", (unsigned long)numStaticVisible);
            SettingsWidgets::InfoLabel(TXT(PS_OBJECTSSTATICVISIBLE), txt);

            sprintf(txt, "%lu", (unsigned long)numStaticInvisible);
            SettingsWidgets::InfoLabel(TXT(PS_OBJECTSSTATICINVISIBLE), txt);

            sprintf(txt, "%lu", (unsigned long)numDynamicVisible);
            SettingsWidgets::InfoLabel(TXT(PS_OBJECTSDYNAMICVISIBLE), txt);

            // Per-object settings
            for (size_t iObject = 0; iObject != numObjects; ++iObject)
            {
                ObjectsSettings::Box& box = os.mBoxes[iObject];
                sprintf(txt, TXT(PS_OBJECTNUMBER), (int)iObject);
                SettingsWidgets::SectionHeader(txt);
                SettingsWidgets::BeginSettingsBlock();
                {
                    if (advanced)
                    {
                        // Advanced mode: individual sliders for dimensions, position, color
                        SettingsWidgets::SliderFloatPower(TXT(PS_WIDTH), box.mExtents.x, 0.01f, 500.0f, 4.0f, "%.2f m");
                        SettingsWidgets::SliderFloatPower(TXT(PS_DEPTH), box.mExtents.y, 0.01f, 500.0f, 4.0f, "%.2f m");
                        SettingsWidgets::SliderFloatPower(TXT(PS_HEIGHT), box.mExtents.z, 0.01f, 500.0f, 4.0f, "%.2f m");

                        SettingsWidgets::SliderFloatPower(TXT(PS_POSITIONX), box.mTM.t.x, -1000.0f, 1000.0f, 4.0f, "%.2f m");
                        SettingsWidgets::SliderFloatPower(TXT(PS_POSITIONY), box.mTM.t.y, -1000.0f, 1000.0f, 4.0f, "%.2f m");
                        SettingsWidgets::SliderFloatPower(TXT(PS_POSITIONZ), box.mTM.t.z, -1000.0f, 1000.0f, 4.0f, "%.2f m");

                        SettingsWidgets::SliderFloat(TXT(PS_COLOURR), box.mColour.x, 0.0f, 1.0f);
                        SettingsWidgets::SliderFloat(TXT(PS_COLOURG), box.mColour.y, 0.0f, 1.0f);
                        SettingsWidgets::SliderFloat(TXT(PS_COLOURB), box.mColour.z, 0.0f, 1.0f);
                    }
                    else
                    {
                        // Simple mode: info text for position, size, color
                        sprintf(txt, "%5.2f, %5.2f, %5.2f", box.mTM.t.x, box.mTM.t.y, box.mTM.t.z);
                        SettingsWidgets::InfoLabel(TXT(PS_POSITION), txt);
                        sprintf(txt, "%5.2f, %5.2f, %5.2f", box.mExtents.x, box.mExtents.y, box.mExtents.z);
                        SettingsWidgets::InfoLabel(TXT(PS_SIZE), txt);
                        sprintf(txt, "%5.2f, %5.2f, %5.2f", box.mColour.x, box.mColour.y, box.mColour.z);
                        SettingsWidgets::InfoLabel(TXT(PS_COLOUR), txt);
                    }

                    // Always shown: mass, visible, shadow
                    SettingsWidgets::SliderFloatPower(TXT(PS_MASS), box.mMass, 0.0f, 100.0f, 4.0f, "%.2f kg");
                    SettingsWidgets::Checkbox(TXT(PS_VISIBLE), box.mVisible);
                    SettingsWidgets::Checkbox(TXT(PS_SHADOW), box.mShadow);
                }
                SettingsWidgets::EndSettingsBlock();
            }
        }
    }
    SettingsWidgets::EndSettingsBlock();
}
//======================================================================================================================
void SettingsMenu::RenderLightingTab()
{
    LightingSettings& ls = mGameSettings.mLightingSettings;
    Language language = mGameSettings.mOptions.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_LIGHTING];
    bool isPanorama = mGameSettings.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA;
    bool allowEnvironmentSettings = mGameSettings.mChallengeSettings.mAllowEnvironmentSettings;

    // If terrain is panorama, show message and return
    if (isPanorama)
    {
        SettingsWidgets::InfoText(" ");
        SettingsWidgets::InfoTextWrapped(TXT(PS_NOLIGHTINGSETTINGS));
        SettingsWidgets::InfoText(" ");
        return;
    }

    // Thumbnail button - only clickable if environment settings are allowed
    Texture* thumbnail = GetCachedTexture(ls.mThumbnail, mGameSettings.mOptions.m16BitTextures);
    float thumbnailHeight = (float)Platform::GetDisplayHeight() * 0.3f;

    if (allowEnvironmentSettings)
    {
        if (SettingsWidgets::ThumbnailButton(thumbnail, ls.mTitle.c_str(), ls.mInfo.c_str(),
            thumbnailHeight, &sImageButtonX, &sImageButtonY, &sImageButtonW, &sImageButtonH))
        {
            mStatus = SETTINGS_LOAD_LIGHTING;
        }
    }
    else
    {
        // Display thumbnail as non-clickable info
        SettingsWidgets::ThumbnailButton(thumbnail, ls.mTitle.c_str(), ls.mInfo.c_str(),
            thumbnailHeight, &sImageButtonX, &sImageButtonY, &sImageButtonW, &sImageButtonH);
    }

    // Lighting settings
    SettingsWidgets::SectionHeader(TXT(PS_SETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        // Sun bearing and thermal activity only if environment settings allowed
        if (allowEnvironmentSettings)
        {
            SettingsWidgets::SliderInt(TXT(PS_SUNBEARING), ls.mSunBearingOffset, 0, 360);
            SettingsWidgets::SliderFloat(TXT(PS_THERMALACTIVITY), ls.mThermalActivity, 0.0f, 1.0f);
        }
        // Terrain darkness always shown for non-panorama
        SettingsWidgets::SliderFloat(TXT(PS_TERRAINDARKNESS), ls.mGamma, 0.3f, 1.0f);
    }
    SettingsWidgets::EndSettingsBlock();
}

//======================================================================================================================
void SettingsMenu::RenderAIControllersTab()
{
    AIControllersSettings& acs = mGameSettings.mAIControllersSettings;
    Language language = mGameSettings.mOptions.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_AICONTROLLERS];

    // General settings section
    SettingsWidgets::SectionHeader(TXT(PS_GENERALSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        // Max AI info
        char maxAIText[256];
        sprintf(maxAIText, TXT(PS_MAXAI), mGameSettings.mOptions.mFreeFlightMaxAI);
        SettingsWidgets::InfoTextWrapped(maxAIText);

        // Global settings only shown if there are AI controllers
        if (acs.mAIControllers.size() > 0)
        {
            SettingsWidgets::Checkbox(TXT(PS_INCLUDEINCAMERAVIEWS), acs.mIncludeInCameraViews);
            SettingsWidgets::Checkbox(TXT(PS_CREATEMAXNUMCONTROLLERS), acs.mCreateMaxNumControllers);

            if (advanced)
            {
                SettingsWidgets::SliderFloat(TXT(PS_LAUNCHDIRECTION), acs.mLaunchDirection, -1.0f, 1.0f);
                SettingsWidgets::SliderFloat(TXT(PS_RANDOMCOLOUROFFSET), acs.mRandomColourOffset, 0.0f, 1.0f);
                SettingsWidgets::SliderFloat(TXT(PS_LAUNCHSEPARATIONDISTANCE), acs.mLaunchSeparationDistance, 1.0f, 10.0f, "%.1f m");
                SettingsWidgets::Checkbox(TXT(PS_ENABLEDEBUGDRAW), acs.mEnableDebugDraw);
            }
        }

        // Add button always shows
        if (SettingsWidgets::Button(TXT(PS_ADDNEWAICONTROLLER)))
        {
            mStatus = SETTINGS_ADD_AICONTROLLER;
        }

        // Remove all button only shows if there are AI controllers
        if (acs.mAIControllers.size() > 0)
        {
            if (SettingsWidgets::Button(TXT(PS_REMOVEAICONTROLLERS)))
            {
                mStatus = SETTINGS_REMOVE_AICONTROLLERS;
            }
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Per-AI controller settings
    for (size_t i = 0; i < acs.mAIControllers.size(); ++i)
    {
        AIControllersSettings::AIControllerSetting& ai = acs.mAIControllers[i];

        // Section header is the aeroplane file name
        SettingsWidgets::SectionHeader(ai.mAeroplaneFile.c_str());
        SettingsWidgets::BeginSettingsBlock();
        {
            // Check if the aeroplane is available
            AeroplaneSettings as;
            if (as.LoadBasicsFromFile(ai.mAeroplaneFile.c_str(), true))  // disableLogging=true
            {
                SettingsWidgets::SliderFloat(TXT(PS_COLOUROFFSET), ai.mColourOffset, 0.0f, 1.0f);

                if (advanced)
                {
                    SettingsWidgets::Checkbox(TXT(PS_INCLUDEINCAMERAVIEWS), ai.mIncludeInCameraViews);
                    SettingsWidgets::Checkbox(TXT(PS_ENABLEDEBUGDRAW), ai.mEnableDebugDraw);

                    ImGui::PushID((int)i);
                    if (SettingsWidgets::Button(TXT(PS_DUPLICATE)))
                    {
                        // Copy this AI controller
                        AIControllersSettings::AIControllerSetting copy = ai;
                        acs.mAIControllers.push_back(copy);
                    }
                    ImGui::PopID();
                }
            }

            // Remove button for each AI controller
            ImGui::PushID((int)i + 1000);  // Different ID range from copy button
            if (SettingsWidgets::Button(TXT(PS_REMOVE)))
            {
                acs.mAIControllers.erase(acs.mAIControllers.begin() + i);
                ImGui::PopID();
                break;  // Exit loop since we modified the vector
            }
            ImGui::PopID();
        }
        SettingsWidgets::EndSettingsBlock();
    }
}
//======================================================================================================================
void SettingsMenu::RenderControllerTab()
{
    ControllerSettings& cs = mGameSettings.mControllerSettings;
    Language language = mGameSettings.mOptions.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_CONTROLLER];

    // Control descriptions for combos
    static const char* controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS + 1];
    controlDescs[ControllerSettings::CONTROLLER_STICK_ROLL] = TXT(PS_ROLLSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_PITCH] = TXT(PS_PITCHSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_YAW] = TXT(PS_YAWSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_SPEED] = TXT(PS_SPEEDSTICK);
    controlDescs[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = TXT(PS_TILTHORIZONTAL);
    controlDescs[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = TXT(PS_TILTVERTICAL);
    controlDescs[ControllerSettings::CONTROLLER_ARROW_HORIZONTAL] = TXT(PS_ARROWSHORIZONTAL);
    controlDescs[ControllerSettings::CONTROLLER_ARROW_VERTICAL] = TXT(PS_ARROWSVERTICAL);
    controlDescs[ControllerSettings::CONTROLLER_CONSTANT] = TXT(PS_CONSTANT);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON0] = TXT(PS_BUTTON0);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON1] = TXT(PS_BUTTON1);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON2] = TXT(PS_BUTTON2);
    controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS] = TXT(PS_NONE);

    static const char* channelNames[Controller::MAX_CHANNELS];
    channelNames[Controller::CHANNEL_AILERONS] = TXT(PS_AILERONS);
    channelNames[Controller::CHANNEL_ELEVATOR] = TXT(PS_ELEVATOR);
    channelNames[Controller::CHANNEL_RUDDER] = TXT(PS_RUDDER);
    channelNames[Controller::CHANNEL_THROTTLE] = TXT(PS_THROTTLE);
    channelNames[Controller::CHANNEL_LOOKYAW] = TXT(PS_LOOKYAW);
    channelNames[Controller::CHANNEL_LOOKPITCH] = TXT(PS_LOOKPITCH);
    channelNames[Controller::CHANNEL_AUX1] = TXT(PS_AUX1);
    channelNames[Controller::CHANNEL_SMOKE1] = TXT(PS_SMOKE1);
    channelNames[Controller::CHANNEL_SMOKE2] = TXT(PS_SMOKE2);
    channelNames[Controller::CHANNEL_HOOK] = TXT(PS_HOOK);

    static const char* clampDescs[ControllerSettings::CONTROL_CLAMP_MAX];
    clampDescs[ControllerSettings::CONTROL_CLAMP_NONE] = TXT(PS_NONE);
    clampDescs[ControllerSettings::CONTROL_CLAMP_POSITIVE] = TXT(PS_POSITIVE);
    clampDescs[ControllerSettings::CONTROL_CLAMP_NEGATIVE] = TXT(PS_NEGATIVE);

    // General settings section
    SettingsWidgets::SectionHeader(TXT(PS_GENERALSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        SettingsWidgets::Checkbox(TXT(PS_RESETALTSETTINGONLAUNCH), cs.mResetAltSettingOnLaunch);
        SettingsWidgets::Checkbox(TXT(PS_TREATTHROTTLEASBRAKES), cs.mTreatThrottleAsBrakes);
        SettingsWidgets::SliderInt(TXT(PS_NUMCONFIGURATIONS), cs.mNumAltSettings, 1, ControllerSettings::CONTROLLER_MAX_NUM_ALT_SETTINGS);
    }
    SettingsWidgets::EndSettingsBlock();

    // Control sources section - always show all 10 channels
    SettingsWidgets::SectionHeader(TXT(PS_CONTROLSOURCES));
    SettingsWidgets::BeginSettingsBlock();
    {
        for (int ch = 0; ch < Controller::MAX_CHANNELS; ++ch)
        {
            int control = (int)cs.mControlPerChannel[ch];
            if (SettingsWidgets::Combo(channelNames[ch], control, controlDescs, ControllerSettings::CONTROLLER_NUM_CONTROLS + 1))
                cs.mControlPerChannel[ch] = (ControllerSettings::ControllerControls)control;
        }
    }
    SettingsWidgets::EndSettingsBlock();

    // Build array of which controllers are in use
    bool haveController[ControllerSettings::CONTROLLER_NUM_CONTROLS];
    for (int i = 0; i < ControllerSettings::CONTROLLER_NUM_CONTROLS; ++i)
        haveController[i] = false;
    for (int ch = 0; ch < Controller::MAX_CHANNELS; ++ch)
    {
        if (cs.mControlPerChannel[ch] < ControllerSettings::CONTROLLER_NUM_CONTROLS)
            haveController[cs.mControlPerChannel[ch]] = true;
    }

    // Loop through each alt setting
    for (int iAlt = 0; iAlt < cs.mNumAltSettings; ++iAlt)
    {
        ImGui::PushID(iAlt);

        // Show header if multiple configurations
        if (cs.mNumAltSettings > 1)
        {
            ImGui::Spacing();
            char txt[256];
            sprintf(txt, TXT(PS_SETTINGSFORCONTROLLER), iAlt + 1, cs.GetName(iAlt).c_str());
            SettingsWidgets::UberSectionHeader(txt);
        }

        if (!advanced)
        {
            // Simple view: Show trim settings section
            bool haveAnyController =
                haveController[ControllerSettings::CONTROLLER_STICK_ROLL] ||
                haveController[ControllerSettings::CONTROLLER_STICK_PITCH] ||
                haveController[ControllerSettings::CONTROLLER_STICK_YAW] ||
                haveController[ControllerSettings::CONTROLLER_STICK_SPEED] ||
                haveController[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] ||
                haveController[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] ||
                haveController[ControllerSettings::CONTROLLER_CONSTANT] ||
                haveController[ControllerSettings::CONTROLLER_BUTTON0] ||
                haveController[ControllerSettings::CONTROLLER_BUTTON1] ||
                haveController[ControllerSettings::CONTROLLER_BUTTON2];

            if (haveAnyController)
            {
                SettingsWidgets::SectionHeader(TXT(PS_TRIMSETTINGS));
                SettingsWidgets::BeginSettingsBlock();
                {
                    if (haveController[ControllerSettings::CONTROLLER_STICK_ROLL])
                        SettingsWidgets::SliderFloat(TXT(PS_ROLLSTICK), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mTrim, -1.0f, 1.0f);
                    if (haveController[ControllerSettings::CONTROLLER_STICK_PITCH])
                        SettingsWidgets::SliderFloat(TXT(PS_PITCHSTICK), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mTrim, -1.0f, 1.0f);
                    if (haveController[ControllerSettings::CONTROLLER_STICK_YAW])
                        SettingsWidgets::SliderFloat(TXT(PS_YAWSTICK), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mTrim, -1.0f, 1.0f);
                    if (haveController[ControllerSettings::CONTROLLER_STICK_SPEED])
                        SettingsWidgets::SliderFloat(TXT(PS_SPEEDSTICK), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mTrim, -1.0f, 1.0f);
                    if (haveController[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL])
                        SettingsWidgets::SliderFloat(TXT(PS_ACCELEROMETERROLL), cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt).mTrim, -1.0f, 1.0f);
                    if (haveController[ControllerSettings::CONTROLLER_ACCEL_VERTICAL])
                        SettingsWidgets::SliderFloat(TXT(PS_ACCELEROMETERPITCH), cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt).mTrim, -1.0f, 1.0f);
                    if (haveController[ControllerSettings::CONTROLLER_CONSTANT])
                        SettingsWidgets::SliderFloat(TXT(PS_CONSTANT), cs.GetControlSetting(ControllerSettings::CONTROLLER_CONSTANT, iAlt).mTrim, -1.0f, 1.0f);
                }
                SettingsWidgets::EndSettingsBlock();
            }
            else
            {
                SettingsWidgets::CenteredLabel(TXT(PS_NOSIMPLESETTINGS));
            }
        }
        else
        {
            // Advanced view: Show per-controller sections with all parameters

            // Roll Stick Movement
            if (haveController[ControllerSettings::CONTROLLER_STICK_ROLL])
            {
                SettingsWidgets::SectionHeader(TXT(PS_ROLLSTICKMOVEMENT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt);
                    int clamp = (int)setting.mClamp;
                    if (SettingsWidgets::Combo(TXT(PS_CLAMPING), clamp, clampDescs, ControllerSettings::CONTROL_CLAMP_MAX))
                        setting.mClamp = (ControllerSettings::ControlClamp)clamp;
                    SettingsWidgets::SliderFloat(TXT(PS_EXPONENTIAL), setting.mExponential, 0.5f, 3.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_SCALE), setting.mScale, -2.0f, 2.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -1.0f, 1.0f);
                    SettingsWidgets::Checkbox(TXT(PS_SPRING), setting.mAutoCentre);
                }
                SettingsWidgets::EndSettingsBlock();
            }

            // Pitch Stick Movement
            if (haveController[ControllerSettings::CONTROLLER_STICK_PITCH])
            {
                SettingsWidgets::SectionHeader(TXT(PS_PITCHSTICKMOVEMENT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt);
                    int clamp = (int)setting.mClamp;
                    if (SettingsWidgets::Combo(TXT(PS_CLAMPING), clamp, clampDescs, ControllerSettings::CONTROL_CLAMP_MAX))
                        setting.mClamp = (ControllerSettings::ControlClamp)clamp;
                    SettingsWidgets::SliderFloat(TXT(PS_EXPONENTIAL), setting.mExponential, 0.5f, 3.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_SCALE), setting.mScale, -2.0f, 2.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -1.0f, 1.0f);
                    SettingsWidgets::Checkbox(TXT(PS_SPRING), setting.mAutoCentre);
                }
                SettingsWidgets::EndSettingsBlock();
            }

            // Yaw Stick Movement
            if (haveController[ControllerSettings::CONTROLLER_STICK_YAW])
            {
                SettingsWidgets::SectionHeader(TXT(PS_YAWSTICKMOVEMENT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt);
                    int clamp = (int)setting.mClamp;
                    if (SettingsWidgets::Combo(TXT(PS_CLAMPING), clamp, clampDescs, ControllerSettings::CONTROL_CLAMP_MAX))
                        setting.mClamp = (ControllerSettings::ControlClamp)clamp;
                    SettingsWidgets::SliderFloat(TXT(PS_EXPONENTIAL), setting.mExponential, 0.5f, 3.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_SCALE), setting.mScale, -2.0f, 2.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -1.0f, 1.0f);
                    SettingsWidgets::Checkbox(TXT(PS_SPRING), setting.mAutoCentre);
                }
                SettingsWidgets::EndSettingsBlock();
            }

            // Speed Stick Movement
            if (haveController[ControllerSettings::CONTROLLER_STICK_SPEED])
            {
                SettingsWidgets::SectionHeader(TXT(PS_SPEEDSTICKMOVEMENT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt);
                    int clamp = (int)setting.mClamp;
                    if (SettingsWidgets::Combo(TXT(PS_CLAMPING), clamp, clampDescs, ControllerSettings::CONTROL_CLAMP_MAX))
                        setting.mClamp = (ControllerSettings::ControlClamp)clamp;
                    SettingsWidgets::SliderFloat(TXT(PS_EXPONENTIAL), setting.mExponential, 0.5f, 3.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_SCALE), setting.mScale, -2.0f, 2.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -1.0f, 1.0f);
                    SettingsWidgets::Checkbox(TXT(PS_SPRING), setting.mAutoCentre);
                }
                SettingsWidgets::EndSettingsBlock();
            }

            // Accelerometer Roll Movement
            if (haveController[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL])
            {
                SettingsWidgets::SectionHeader(TXT(PS_ACCELEROMETERROLLMOVEMENT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt);
                    int clamp = (int)setting.mClamp;
                    if (SettingsWidgets::Combo(TXT(PS_CLAMPING), clamp, clampDescs, ControllerSettings::CONTROL_CLAMP_MAX))
                        setting.mClamp = (ControllerSettings::ControlClamp)clamp;
                    SettingsWidgets::SliderFloat(TXT(PS_EXPONENTIAL), setting.mExponential, 0.5f, 3.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_SCALE), setting.mScale, -2.0f, 2.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -0.5f, 0.5f);
                    SettingsWidgets::SliderFloat(TXT(PS_TILTROLLSENSITIVITY), cs.mControllerAccelerometerXSensitivity, 0.0f, 1.0f);
                }
                SettingsWidgets::EndSettingsBlock();
            }

            // Accelerometer Pitch Movement
            if (haveController[ControllerSettings::CONTROLLER_ACCEL_VERTICAL])
            {
                SettingsWidgets::SectionHeader(TXT(PS_ACCELEROMETERPITCHMOVEMENT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt);
                    int clamp = (int)setting.mClamp;
                    if (SettingsWidgets::Combo(TXT(PS_CLAMPING), clamp, clampDescs, ControllerSettings::CONTROL_CLAMP_MAX))
                        setting.mClamp = (ControllerSettings::ControlClamp)clamp;
                    SettingsWidgets::SliderFloat(TXT(PS_EXPONENTIAL), setting.mExponential, 0.5f, 3.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_SCALE), setting.mScale, -2.0f, 2.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -1.0f, 1.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TILTPITCHSENSITIVITY), cs.mControllerAccelerometerYSensitivity, 0.0f, 1.0f);
                    SettingsWidgets::SliderFloat(TXT(PS_TILTNEUTRALANGLE), cs.mControllerAccelerometerOffsetAngle, 0.0f, 90.0f, "%.0f deg");
                }
                SettingsWidgets::EndSettingsBlock();
            }

            // Constant
            if (haveController[ControllerSettings::CONTROLLER_CONSTANT])
            {
                SettingsWidgets::SectionHeader(TXT(PS_CONSTANT));
                SettingsWidgets::BeginSettingsBlock();
                {
                    ControllerSettings::ControlSetting& setting = cs.GetControlSetting(ControllerSettings::CONTROLLER_CONSTANT, iAlt);
                    SettingsWidgets::SliderFloat(TXT(PS_TRIM), setting.mTrim, -1.0f, 1.0f);
                }
                SettingsWidgets::EndSettingsBlock();
            }
        }

        // Mixes section - shown in both simple and advanced views
        SettingsWidgets::SectionHeader(TXT(PS_MIXES));
        SettingsWidgets::BeginSettingsBlock();
        {
            ControllerSettings::Mix& mix = cs.GetMix(iAlt);
            SettingsWidgets::SliderFloat(TXT(PS_ELEVATORTOFLAPS), mix.mMixElevatorToFlaps, -3.0f, 3.0f);
            SettingsWidgets::SliderFloat(TXT(PS_AILERONTORUDDER), mix.mMixAileronToRudder, -1.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_FLAPSTOELEVATOR), mix.mMixFlapsToElevator, -1.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_BRAKESTOELEVATOR), mix.mMixBrakesToElevator, -1.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_RUDDERTOELEVATOR), mix.mMixRudderToElevator, -1.0f, 1.0f);
            SettingsWidgets::SliderFloat(TXT(PS_RUDDERTOAILERON), mix.mMixRudderToAileron, -1.0f, 1.0f);
        }
        SettingsWidgets::EndSettingsBlock();

        ImGui::PopID();
    }

    // Footer label
    SettingsWidgets::CenteredLabel(TXT(PS_CONTROLLERSEESETTINGS));
}

//======================================================================================================================
void SettingsMenu::RenderJoystickTab()
{
    JoystickSettings& js = mGameSettings.mJoystickSettings;
    Language language = mGameSettings.mOptions.mLanguage;
    bool advanced = sAdvancedEnabled[TAB_JOYSTICK];

    bool joystickAvailable = JoystickAvailable();

    if (!joystickAvailable)
    {
        SettingsWidgets::InfoLabel(TXT(PS_NOJOYSTICK), "");
        return;
    }

    // Get joystick status
    JoystickData joystick;
    bool hasJoystick = S3E_RESULT_SUCCESS == GetJoystickStatus(joystick, mGameSettings.mOptions.mJoystickID);

    // Joystick settings
    SettingsWidgets::SectionHeader(TXT(PS_EXTERNALJOYSTICKSETTINGS));
    SettingsWidgets::BeginSettingsBlock();
    {
        if (hasJoystick)
        {
            SettingsWidgets::InfoLabel(TXT(PS_JOYSTICKINFO), "");
            SettingsWidgets::InfoLabel(TXT(PS_NAME), joystick.mName);
        }
        else
        {
            SettingsWidgets::InfoLabel(TXT(PS_JOYSTICKINFO), TXT(PS_NOJOYSTICKWITHID));
        }

        SettingsWidgets::SliderInt(TXT(PS_JOYSTICKID), mGameSettings.mOptions.mJoystickID, 0, 4);
        SettingsWidgets::Checkbox(TXT(PS_ENABLEJOYSTICK), js.mEnableJoystick);
        SettingsWidgets::Checkbox(TXT(PS_ADJUSTFORCIRCULARSTICKMOVEMENT), js.mAdjustForCircularSticks);

        if (SettingsWidgets::Button(TXT(PS_CLEARJOYSTICKSETTINGS)))
        {
            mStatus = SETTINGS_CLEAR_JOYSTICK;
        }

        if (mGameSettings.mOptions.mFrameworkSettings.isWindows())
        {
            if (SettingsWidgets::Button(TXT(PS_CALIBRATEJOYSTICK)))
            {
                mStatus = SETTINGS_CALIBRATE_JOYSTICK;
            }
        }
    }
    SettingsWidgets::EndSettingsBlock();

    if (!hasJoystick)
        return;

    // Control descriptions for combos
    static const char* controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS + 1];
    controlDescs[ControllerSettings::CONTROLLER_STICK_ROLL] = TXT(PS_ROLLSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_PITCH] = TXT(PS_PITCHSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_YAW] = TXT(PS_YAWSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_SPEED] = TXT(PS_SPEEDSTICK);
    controlDescs[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = TXT(PS_TILTHORIZONTAL);
    controlDescs[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = TXT(PS_TILTVERTICAL);
    controlDescs[ControllerSettings::CONTROLLER_ARROW_HORIZONTAL] = TXT(PS_ARROWSHORIZONTAL);
    controlDescs[ControllerSettings::CONTROLLER_ARROW_VERTICAL] = TXT(PS_ARROWSVERTICAL);
    controlDescs[ControllerSettings::CONTROLLER_CONSTANT] = TXT(PS_CONSTANT);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON0] = TXT(PS_BUTTON0);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON1] = TXT(PS_BUTTON1);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON2] = TXT(PS_BUTTON2);
    controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS] = TXT(PS_NONE);

    // Per-axis settings
    for (int i = 0; i < JoystickSettings::JOYSTICK_NUM_CONTROLS; ++i)
    {
        JoystickSettings::JoystickAnalogueOverride& j = js.mJoystickAnalogueOverrides[i];

        // Calculate current values
        float input = -1.0f + 2.0f * joystick.mAnalogueInputs[i] / 65535.0f;
        float output = input + j.mOffset;
        if (output > -j.mDeadZone && output < j.mDeadZone)
            output = 0.0f;
        else if (output > j.mDeadZone)
            output = (output - j.mDeadZone) / (1.0f - j.mDeadZone);
        else
            output = (output + j.mDeadZone) / (1.0f - j.mDeadZone);
        float finalOutput = output * (output > 0.0f ? j.mScalePositive : j.mScaleNegative);

        char header[64];
        sprintf(header, TXT(PS_JOYSTICKLABEL), i, input, finalOutput);
        SettingsWidgets::SectionHeader(header);
        SettingsWidgets::BeginSettingsBlock();
        {
            int control = (int)j.mControl;
            if (SettingsWidgets::Combo(TXT(PS_MAPTO), control, controlDescs, ControllerSettings::CONTROLLER_NUM_CONTROLS + 1))
                j.mControl = (ControllerSettings::ControllerControls)control;

            // Calibration buttons with live values
            char offsetStr[32];
            sprintf(offsetStr, " %5.2f", j.mOffset);
            if (SettingsWidgets::LabelValueButton(TXT(PS_OFFSET), offsetStr, TXT(PS_PRESSWHENCENTRED)))
            {
                j.mOffset = -input;
            }

            char scaleNegStr[32];
            sprintf(scaleNegStr, " %5.2f", j.mScaleNegative);
            if (SettingsWidgets::LabelValueButton(TXT(PS_SCALENEGATIVE), scaleNegStr, TXT(PS_PRESSWHENLEFTORDOWN)))
            {
                if (output < 0.0f)
                    j.mScaleNegative = -1.0f / output;
                else if (output > 0.0f)
                    j.mScalePositive = -1.0f / output;
            }

            char scalePosStr[32];
            sprintf(scalePosStr, " %5.2f", j.mScalePositive);
            if (SettingsWidgets::LabelValueButton(TXT(PS_SCALEPOSITIVE), scalePosStr, TXT(PS_PRESSWHENRIGHTORUP)))
            {
                if (output > 0.0f)
                    j.mScalePositive = 1.0f / output;
                else if (output < 0.0f)
                    j.mScaleNegative = 1.0f / output;
            }

            SettingsWidgets::SliderFloat(TXT(PS_DEADZONE), j.mDeadZone, 0.0f, 0.5f);
        }
        SettingsWidgets::EndSettingsBlock();
    }

    // Per-button settings - button overrides use ButtonControl enum, not ControllerControls
    static const char* buttonControlDescs[JoystickSettings::JoystickButtonOverride::NUM_BUTTON_CONTROLS];
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_NONE] = TXT(PS_NONE);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RATES] = TXT(PS_RATESBUTTON);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RATESCYCLE] = TXT(PS_RATESCYCLEBUTTON);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RELAUNCH] = TXT(PS_RELAUNCHBUTTON);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_CAMERA] = TXT(PS_CAMERABUTTON);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_PAUSEPLAY] = TXT(PS_PAUSEPLAYBUTTON);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON0] = TXT(PS_BUTTON0);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON1] = TXT(PS_BUTTON1);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON2] = TXT(PS_BUTTON2);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON0TOGGLE] = TXT(PS_BUTTON0TOGGLE);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON1TOGGLE] = TXT(PS_BUTTON1TOGGLE);
    buttonControlDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON2TOGGLE] = TXT(PS_BUTTON2TOGGLE);

    SettingsWidgets::SectionHeader(TXT(PS_BUTTONMAPPINGS));
    for (int i = 0; i < (int)JoystickSettings::JOYSTICK_NUM_BUTTONS; ++i)
    {
        JoystickSettings::JoystickButtonOverride& j = js.mJoystickButtonOverrides[i];

        float input = joystick.mButtons[i] / 128.0f;
        bool buttonDown = joystick.mButtons[i] > 64;
        buttonDown = j.mInvert ? !buttonDown : buttonDown;

        char label[64];
        sprintf(label, TXT(PS_JOYSTICKBUTTONLABEL), i, input, buttonDown ? 1 : 0);

        SettingsWidgets::BeginSettingsBlock();
        {
            SettingsWidgets::InfoLabel(label, "");

            int control = (int)j.mControl;
            if (SettingsWidgets::Combo(TXT(PS_MAPTO), control, buttonControlDescs, JoystickSettings::JoystickButtonOverride::NUM_BUTTON_CONTROLS))
                j.mControl = (JoystickSettings::JoystickButtonOverride::ButtonControl)control;

            SettingsWidgets::Checkbox(TXT(PS_INVERT), j.mInvert);
        }
        SettingsWidgets::EndSettingsBlock();
    }
}

//=====================================================================================================================
// Main entry point
//=====================================================================================================================
void DisplaySettingsMenu(GameSettings& gameSettings, SettingsChangeActions& actions)
{
    TRACE_FUNCTION_ONLY(ONCE_1);

    AudioManager::GetInstance().SetAllChannelsToZeroVolume();

    GameSettings origSettings = gameSettings;
    SettingsStatus settingsStatus = SETTINGS_UNSET;

    while (true)
    {
        {
            SettingsMenu settingsMenu(gameSettings);

            while (true)
            {
                settingsStatus = settingsMenu.Update();

                settingsMenu.GetImageButtonInfo(
                    (int&)sImageButtonX, (int&)sImageButtonY,
                    (int&)sImageButtonW, (int&)sImageButtonH);

                if (CheckForQuitRequest())
                {

                    return;
                }

                if (settingsStatus == SETTINGS_BACK ||
                    (Input::GetInstance().GetKeyState(SDLK_AC_BACK) & KEY_STATE_PRESSED) ||
                    (Input::GetInstance().GetKeyState(SDLK_ESCAPE) & KEY_STATE_PRESSED))
                {
                    actions = gameSettings.GetSettingsChangeActions(actions, origSettings);

                    return;
                }

                // Handle clear settings dialog here while settingsMenu is still in scope
                if (settingsStatus == SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT)
                {
                    // Show confirmation dialog with settings menu as background
                    Language language = gameSettings.mOptions.mLanguage;
                    auto bgCallback = [&settingsMenu]() { settingsMenu.RenderContent(); };
                    if (ShowDialog(TXT(PS_CLEARALLSAVEDSETTINGSANDEXIT), TXT(PS_CONFIRMCLEARALLSETTINGS),
                        TXT(PS_YES), TXT(PS_NO), nullptr, bgCallback) == 0)
                    {
                        // Delete the main settings file (from user-writable location)
                        std::string settingsFile = Platform::GetUserSettingsPath() + "settings.xml";
                        ::remove(settingsFile.c_str());

                        // Exit the application
                        exit(0);
                    }
                    settingsStatus = SETTINGS_UNSET;
                }

                if (settingsStatus != SETTINGS_UNSET)
                {
                    break;
                }

                AudioManager::GetInstance().Update(1.0f / 30.0f);
            }
        }

        Language language = gameSettings.mOptions.mLanguage;

        // User-writable paths for settings
        std::string userOptions = Platform::GetUserSettingsPath() + "Options";
        std::string userEnvironment = Platform::GetUserSettingsPath() + "Environment";
        std::string userObjects = Platform::GetUserSettingsPath() + "Objects";
        std::string userLighting = Platform::GetUserSettingsPath() + "Lighting";
        std::string userController = Platform::GetUserSettingsPath() + "Controller";
        std::string userJoystick = Platform::GetUserSettingsPath() + "Joystick";
        std::string userAeroplane = Platform::GetUserSettingsPath() + "Aeroplane";
        std::string userAIControllers = Platform::GetUserSettingsPath() + "AIControllers";
        std::string userPanoramas = Platform::GetUserDataPath() + "Panoramas";
        std::string userFileTerrains = Platform::GetUserDataPath() + "FileTerrains";

        // Handle the status
        if (settingsStatus == SETTINGS_LOAD_OPTIONS)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Options", userOptions.c_str(), ".xml",
                TXT(PS_LOADOPTIONS), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Loading Options %s", file.c_str());
                gameSettings.mOptions.LoadFromFile(file);
                gameSettings.mStatistics.mLoadedOptions = true;
            }
        }
        else if (settingsStatus == SETTINGS_SAVE_OPTIONS)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userOptions.c_str(), ".xml", TXT(PS_SAVEOPTIONS));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Options %s", file.c_str());
                gameSettings.mOptions.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_OPTIONS)
        {
            FileMenuDelete(gameSettings, userOptions.c_str(), ".xml", TXT(PS_DELETEOPTIONS));
        }
        else if (settingsStatus == SETTINGS_LOAD_ENVIRONMENT)
        {
            SelectAndLoadEnvironment(gameSettings, TXT(PS_LOADSCENERY), TXT(PS_BACK), NULL);
        }
        else if (settingsStatus == SETTINGS_SAVE_ENVIRONMENT)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userEnvironment.c_str(), ".xml", TXT(PS_SAVESCENERY));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Environment %s", file.c_str());
                gameSettings.mEnvironmentSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_ENVIRONMENT)
        {
            FileMenuDelete(gameSettings, userEnvironment.c_str(), ".xml", TXT(PS_DELETESCENERY));
        }
        else if (settingsStatus == SETTINGS_LOAD_OBJECTS)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Objects", userObjects.c_str(), ".xml",
                TXT(PS_LOADOBJECTS), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Loading Objects %s", file.c_str());
                gameSettings.mObjectsSettings.LoadFromFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_SAVE_OBJECTS)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userObjects.c_str(), ".xml", TXT(PS_SAVEOBJECTS));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Objects %s", file.c_str());
                gameSettings.mObjectsSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_OBJECTS)
        {
            FileMenuDelete(gameSettings, userObjects.c_str(), ".xml", TXT(PS_DELETEOBJECTS));
        }
        else if (settingsStatus == SETTINGS_LOAD_LIGHTING)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Lighting", userLighting.c_str(), ".xml",
                TXT(PS_LOADLIGHTING), 0, 0, TXT(PS_BACK), NULL, FILEMENUTYPE_LIGHTING);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Loading Lighting %s", file.c_str());
                gameSettings.mLightingSettings.LoadFromFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_SAVE_LIGHTING)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userLighting.c_str(), ".xml", TXT(PS_SAVELIGHTING));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Lighting %s", file.c_str());
                gameSettings.mLightingSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_LIGHTING)
        {
            FileMenuDelete(gameSettings, userLighting.c_str(), ".xml", TXT(PS_DELETELIGHTING));
        }
        else if (settingsStatus == SETTINGS_LOAD_CONTROLLER)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Controller", userController.c_str(), ".xml",
                TXT(PS_LOADCONTROLLER), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Loading Controller %s", file.c_str());
                gameSettings.mControllerSettings.LoadFromFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_SAVE_CONTROLLER)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userController.c_str(), ".xml", TXT(PS_SAVECONTROLLER));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Controller %s", file.c_str());
                gameSettings.mControllerSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_CONTROLLER)
        {
            FileMenuDelete(gameSettings, userController.c_str(), ".xml", TXT(PS_DELETECONTROLLER));
        }
        else if (settingsStatus == SETTINGS_LOAD_JOYSTICK)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Joystick", userJoystick.c_str(), ".xml",
                TXT(PS_LOADJOYSTICK), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Loading Joystick %s", file.c_str());
                gameSettings.mJoystickSettings.LoadFromFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_SAVE_JOYSTICK)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userJoystick.c_str(), ".xml", TXT(PS_SAVEJOYSTICK));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Joystick %s", file.c_str());
                gameSettings.mJoystickSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_JOYSTICK)
        {
            FileMenuDelete(gameSettings, userJoystick.c_str(), ".xml", TXT(PS_DELETEJOYSTICK));
        }
        else if (settingsStatus == SETTINGS_LOAD_AEROPLANE)
        {
            SelectAndLoadAeroplane(gameSettings, TXT(PS_LOADAEROPLANE), TXT(PS_BACK), NULL);
        }
        else if (settingsStatus == SETTINGS_SAVE_AEROPLANE)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userAeroplane.c_str(), ".xml", TXT(PS_SAVEAEROPLANE));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving Aeroplane %s", file.c_str());
                gameSettings.mAeroplaneSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_AEROPLANE)
        {
            FileMenuDelete(gameSettings, userAeroplane.c_str(), ".xml", TXT(PS_DELETEAEROPLANE));
        }
        else if (settingsStatus == SETTINGS_LOAD_AICONTROLLERS)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/AIControllers", userAIControllers.c_str(), ".xml",
                TXT(PS_LOADAICONTROLLERS), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Loading AI Controllers %s", file.c_str());
                gameSettings.mAIControllersSettings.LoadFromFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_SAVE_AICONTROLLERS)
        {
            std::string file;
            FileMenuSave(file, gameSettings, userAIControllers.c_str(), ".xml", TXT(PS_SAVEAICONTROLLERS));
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Saving AI Controllers %s", file.c_str());
                gameSettings.mAIControllersSettings.SaveToFile(file);
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_AICONTROLLERS)
        {
            FileMenuDelete(gameSettings, userAIControllers.c_str(), ".xml", TXT(PS_DELETEAICONTROLLERS));
        }
        else if (settingsStatus == SETTINGS_ADD_AICONTROLLER)
        {
            // Add a new AI controller by selecting an aeroplane
            CheckForAIControlCallback callback;
            std::string file;
            SelectResult result = SelectAeroplane(file, gameSettings, TXT(PS_ADDNEWAICONTROLLER),
                TXT(PS_BACK), NULL, &callback);
            if (result == SELECTRESULT_SELECTED && !file.empty())
            {
                AIControllersSettings::AIControllerSetting newAI;
                newAI.mAeroplaneFile = file;
                gameSettings.mAIControllersSettings.mAIControllers.push_back(newAI);
            }
        }
        else if (settingsStatus == SETTINGS_REMOVE_AICONTROLLERS)
        {
            gameSettings.mAIControllersSettings.mAIControllers.clear();
        }
        else if (settingsStatus == SETTINGS_LOAD_TUG)
        {
            CheckForTugCallback callback;
            std::string file;
            SelectResult result = SelectAeroplane(file, gameSettings, TXT(PS_TUGPLANE),
                TXT(PS_BACK), NULL, &callback);
            if (result == SELECTRESULT_SELECTED && !file.empty())
            {
                gameSettings.mAeroplaneSettings.mTugName = file;
            }
        }
        else if (settingsStatus == SETTINGS_SELECT_PANORAMA)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemData/Panoramas", userPanoramas.c_str(), "",
                TXT(PS_SELECTPANORAMA), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Selected Panorama %s", file.c_str());
                gameSettings.mEnvironmentSettings.mTerrainSettings.mPanoramaName = file;
            }
        }
        else if (settingsStatus == SETTINGS_SELECT_FILE_TERRAIN)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemData/FileTerrains", userFileTerrains.c_str(), "",
                TXT(PS_SELECTTERRAINFILE), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Selected terrain file %s", file.c_str());
                gameSettings.mEnvironmentSettings.mTerrainSettings.mFileTerrainName = file;
            }
        }
        else if (settingsStatus == SETTINGS_SELECT_PREFERRED_CONTROLLER)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Controller", userController.c_str(), ".xml",
                TXT(PS_SELECTPREFERREDCONTROLLER), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Selected preferred controller %s", file.c_str());
                gameSettings.mAeroplaneSettings.mPreferredController = file;
            }
        }
        else if (settingsStatus == SETTINGS_SELECT_OBJECTS_SETTINGS)
        {
            std::string file;
            FileMenuLoad(file, gameSettings, "SystemSettings/Objects", userObjects.c_str(), ".xml",
                TXT(PS_SELECTOBJECTSSETTINGS), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Selected objects settings %s", file.c_str());
                gameSettings.mEnvironmentSettings.mObjectsSettingsFile = file;
            }
        }
        else if (settingsStatus == SETTINGS_DELETE_LOCAL_HIGHSCORES)
        {
            // TODO: Implement local highscore deletion
        }
        else if (settingsStatus == SETTINGS_RESET_OBJECTS)
        {
            // Reload objects from the environment's objects settings file
            gameSettings.mObjectsSettings.LoadFromFile(
                gameSettings.mEnvironmentSettings.mObjectsSettingsFile);
        }
        else if (settingsStatus == SETTINGS_CLEAR_JOYSTICK)
        {
            // Clear all joystick overrides
            JoystickSettings& js = gameSettings.mJoystickSettings;
            for (int i = 0; i < JoystickSettings::JOYSTICK_NUM_CONTROLS; ++i)
            {
                js.mJoystickAnalogueOverrides[i] = JoystickSettings::JoystickAnalogueOverride();
            }
            for (int i = 0; i < JoystickSettings::JOYSTICK_NUM_BUTTONS; ++i)
            {
                js.mJoystickButtonOverrides[i] = JoystickSettings::JoystickButtonOverride();
            }
        }
        else if (settingsStatus == SETTINGS_CALIBRATE_JOYSTICK)
        {
            // Open Windows joystick calibration
            s3eOSExecExecute("control joy.cpl", false);
        }
        else if (settingsStatus == SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT)
        {
            // Handled in inner loop while settingsMenu is in scope
        }
        else if (settingsStatus == SETTINGS_REFRESH)
        {
            // Just loop back to recreate the menu
        }

        // Reset status for next iteration
        settingsStatus = SETTINGS_UNSET;
    }
}
