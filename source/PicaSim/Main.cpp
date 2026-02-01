#include "PicaSim.h"
#include "GameSettings.h"
#include "ShaderManager.h"
#include "Menus/Menu.h"
#include "Menus/StartMenu.h"
#include "Menus/SettingsMenu.h"
#include "Menus/FileMenu.h"
#include "Menus/LoadingScreen.h"
#include "Menus/WhatsNewMenu.h"
#include "PicaStrings.h"
#include "VersionChecker.h"
#include "VersionInfo.h"
#include "PicaJoystick.h"
#include "Menus/PicaDialog.h"

#include "../Platform/S3ECompat.h"
#include "../Framework/Graphics.h"
#include "Platform.h"
#include <SDL.h>

#ifdef PICASIM_VR_SUPPORT
#include "../Platform/VRManager.h"
#include "../Platform/VRRuntime.h"
#endif

#define EXPLICIT_EGL_INITx

//======================================================================================================================
// Attempt to lock to a 60 frames per second
#define MS_PER_FRAME (1000 / 60)
#define CAP_FRAME_RATE

static float GetSurfaceDiagonalInches()
{
    int32 dpi = (int32)Platform::GetScreenDPI();
    if (dpi > 0)
    {
        int w = Platform::GetDisplayWidth();
        int h = Platform::GetDisplayHeight();
        float d = hypotf((float) w, (float) h);
        return d / dpi;
    }
    else
    {
        return 0.0f;
    }
}

static bool SelectChallenge(GameSettings& gameSettings)
{
    const Language language = gameSettings.mOptions.mLanguage;
    TRACE_FILE_IF(1) TRACE("Loading challenge from file");
    std::string file;
    std::string userChallengePath = Platform::GetUserSettingsPath() + "Challenge";
    FileMenuLoad(file, gameSettings, "SystemSettings/Challenge", userChallengePath.c_str(), ".xml", TXT(PS_SELECTRACE), 0, 0, TXT(PS_BACK), NULL, FILEMENUTYPE_CHALLENGE);
    if (!file.empty())
    {
        TRACE_FILE_IF(1) TRACE("Loading challenge %s", file.c_str());
        gameSettings.mChallengeSettings = ChallengeSettings();
        bool result = gameSettings.mChallengeSettings.LoadFromFile(file);

        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");

        gameSettings.mChallengeSettings.CalculateChecksum(file);
    }
    else
    {
        TRACE_FILE_IF(1) TRACE("Got cancel on challenge choice");
        return false;
    }
    return true;
}

//======================================================================================================================
// Returns true if options were set, false otherwise (e.g. cancel)
static bool InitialiseOptions(GameSettings& gameSettings)
{
    const Language language = gameSettings.mOptions.mLanguage;
    if (gameSettings.mOptions.mFrameworkSettings.isWindows())
    {
        bool result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/HighQuality-LargeScreen.xml");
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
        gameSettings.mStatistics.mLoadedOptions = true;
    }
    else
    {
        // Attempt to guess the screen size and CPU power
        float diagonalInches = GetSurfaceDiagonalInches();
        int32 numCores = Platform::GetCPUCount();
        TRACE_FILE_IF(1) TRACE("diagonalInches = %5.2f numCores = %d ", diagonalInches, numCores);

        if (diagonalInches > 0.0f && numCores > 0)
        {
            const char* settingsFile = 0;
            if (diagonalInches > 6.0f)
            {
                if (numCores <= 2)
                    settingsFile = "SystemSettings/Options/LowQuality-LargeScreen.xml";
                else if (numCores <= 4)
                    settingsFile = "SystemSettings/Options/StandardQuality-LargeScreen.xml";
                else
                    settingsFile = "SystemSettings/Options/HighQuality-LargeScreen.xml";
            }
            else
            {
                if (numCores <= 2)
                    settingsFile = "SystemSettings/Options/LowQuality-SmallScreen.xml";
                else if (numCores <= 4)
                    settingsFile = "SystemSettings/Options/StandardQuality-SmallScreen.xml";
                else
                    settingsFile = "SystemSettings/Options/HighQuality-SmallScreen.xml";
            }
            TRACE_FILE_IF(1) TRACE("Loading %s\n", settingsFile);
            bool result = gameSettings.mOptions.LoadFromFile(settingsFile);
            IwAssert(ROWLHOUSE, result);
            TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");

            gameSettings.mStatistics.mLoadedOptions = true;
        }
        else
        {
            TRACE_FILE_IF(1) TRACE("Forcing options choice");
            std::string file;
            std::string userOptionsPath = Platform::GetUserSettingsPath() + "Options";
            FileMenuLoad(file, gameSettings, "SystemSettings/Options", userOptionsPath.c_str(), ".xml", TXT(PS_SELECTOPTIONS), 0, 0, TXT(PS_BACK), NULL);
            if (!file.empty())
            {
                TRACE_FILE_IF(1) TRACE("Loading Options %s - ", file.c_str());
                bool result = gameSettings.mOptions.LoadFromFile(file);
                IwAssert(ROWLHOUSE, result);
                TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
                gameSettings.mStatistics.mLoadedOptions = true;
            }
            else
            {
                TRACE_FILE_IF(1) TRACE("Got cancel on options choice");
                return false;
            }
        }
    }

    int32 memoryMB = Platform::GetSystemRAM();
    TRACE_FILE_IF(1) TRACE("InitialiseOptions: reported memory = %d MB", memoryMB);
    if (memoryMB > 400 || memoryMB <= 0)
        gameSettings.mOptions.m16BitTextures = false;
    else
        gameSettings.mOptions.m16BitTextures = true;
    TRACE_FILE_IF(1) TRACE("Options: Using 16 bit textures = %d", gameSettings.mOptions.m16BitTextures);

        return true;
}

int32 pauseCallback(void *systemData, void *userData)
{
    TRACE_FILE_IF(1) TRACE("PicaSim pause start");
    if (PicaSim::IsCreated())
    {
        PicaSim::GetInstance().SetStatus(PicaSim::STATUS_PAUSED);
    }
    TRACE_FILE_IF(1) TRACE("PicaSim pause end");
    return 0;
}

int32 unpauseCallback(void *systemData, void *userData)
{
    TRACE_FILE_IF(1) TRACE("PicaSim unpause start");
    if (PicaSim::IsCreated())
    {
        PicaSim::GetInstance().SetStatus(PicaSim::STATUS_PAUSED);
    }
    TRACE_FILE_IF(1) TRACE("PicaSim unpause end");
    return 0;
}

//======================================================================================================================
int main()
{
    SetTraceLevel(1);

    InitMemoryOverrunCheck();
    MEMTEST();

    // Read GL version setting early (before renderer initialization)
    std::string settingsPath = Platform::GetUserSettingsPath() + "settings.xml";
    gGLVersion = ReadGLVersionFromSettings(settingsPath.c_str());
    TRACE_FILE_IF(1) TRACE("GL version from settings: %d", gGLVersion);

#if 0
    s3eGLRegister(S3E_GL_SUSPEND, suspendCallback, 0);
    s3eGLRegister(S3E_GL_RESUME, resumeCallback, 0);
#endif

#ifdef EXPLICIT_EGL_INIT
    // This is no good as Marmalade recreates the surface badly on suspend/resume
    TRACE_FILE_IF(1) TRACE("Calling eglInit");
    eglInit(true);
    TRACE_FILE_IF(1) TRACE("eglInit has been called");
#endif

    // Read MSAA setting early (before window creation)
    int msaaSamples = ReadMSAASamplesFromSettings(settingsPath.c_str());
    TRACE_FILE_IF(1) TRACE("MSAA samples from settings: %d", msaaSamples);

    // Create window and initialise OpenGL
    TRACE_FILE_IF(1) TRACE("Calling eglInit with MSAA=%d", msaaSamples);
    eglInit(true, msaaSamples);
    TRACE_FILE_IF(1) TRACE("eglInit has been called");

#ifdef PICASIM_VR_SUPPORT
    // Initialize VR manager (requires OpenGL context to be created)
    TRACE_FILE_IF(1) TRACE("Initializing VRManager");
    if (VRManager::Init())
    {
        TRACE_FILE_IF(1) TRACE("VRManager initialized successfully");
    }
    else
    {
        TRACE_FILE_IF(1) TRACE("VRManager initialization failed or no headset connected");
    }
#endif

    TRACE("dpi = %d so physical diagonal = %5.2f inches", (int)Platform::GetScreenDPI(), GetSurfaceDiagonalInches());

#if 0
    s3eWindowDisplayMode modes[32];
    int numModes = 32;
    s3eResult result = s3eWindowGetDisplayModes(modes, &numModes);
    result = s3eWindowSetFullscreen(&modes[numModes-1]);
#endif

    TRACE_FILE_IF(1) TRACE("Calling AudioManager::Init()");
    AudioManager::Init();
    TRACE_FILE_IF(1) TRACE("AudioManager::Init() has been called");

    InitPicaStrings();
    VersionInfo::Init();

    srand(time(0));

    MEMTEST();
    // Make sure everything goes out of scope before we close down Marmalade
    {
        GameSettings gameSettings;

        TRACE_FILE_IF(1) TRACE("Calling MenuInit");
        MenuInit(gameSettings);
        TRACE_FILE_IF(1) TRACE("MenuInit has been called");

        // Make the user settings directory if necessary (in user-writable location)
        TRACE_FILE_IF(1) TRACE("Making settings directories if necessary in %s", Platform::GetUserSettingsPath().c_str());
        std::string userSettingsBase = Platform::GetUserSettingsPath();
        std::string userDataBase = Platform::GetUserDataPath();

        FileSystem::MakeDirectory(userSettingsBase);
        FileSystem::MakeDirectory(userSettingsBase + "Options");
        FileSystem::MakeDirectory(userSettingsBase + "Aeroplane");
        FileSystem::MakeDirectory(userSettingsBase + "Environment");
        FileSystem::MakeDirectory(userSettingsBase + "Objects");
        FileSystem::MakeDirectory(userSettingsBase + "AIControllers");
        FileSystem::MakeDirectory(userSettingsBase + "Lighting");
        FileSystem::MakeDirectory(userSettingsBase + "Controller");
        FileSystem::MakeDirectory(userSettingsBase + "Joystick");
        FileSystem::MakeDirectory(userSettingsBase + "Challenge");

        FileSystem::MakeDirectory(userDataBase);
        FileSystem::MakeDirectory(userDataBase + "Aerofoils");
        FileSystem::MakeDirectory(userDataBase + "Aeroplanes");
        FileSystem::MakeDirectory(userDataBase + "Audio");
        FileSystem::MakeDirectory(userDataBase + "Menu");
        FileSystem::MakeDirectory(userDataBase + "Panoramas");
        FileSystem::MakeDirectory(userDataBase + "FileTerrains");
        FileSystem::MakeDirectory(userDataBase + "Syboxes");
        FileSystem::MakeDirectory(userDataBase + "Textures");

        TRACE_FILE_IF(1) TRACE("Loading game settings");
        std::string userSettingsFile = userSettingsBase + "settings.xml";
        gameSettings.LoadFromFile(userSettingsFile, false);

        if (gameSettings.mStatistics.mPicaSimBuildNumber < GetBuildNumber() && gameSettings.mStatistics.mPicaSimBuildNumber != 0)
        {
            DisplayWhatsNewMenu(gameSettings);
            gameSettings.mStatistics.mPicaSimBuildNumber = GetBuildNumber();
            TRACE_FILE_IF(1) TRACE("Saving settings to remember the build number");
            gameSettings.SaveToFile(userSettingsFile);
        }
        else
        {
            gameSettings.mStatistics.mPicaSimBuildNumber = GetBuildNumber();
        }

        if (gameSettings.mStatistics.mPicaSimSettingsVersion < Statistics::LATEST_PICASIM_SETTINGS_VERSION)
        {
            TRACE_FILE_IF(1) TRACE("Using default settings due to version update");
            // Preserve just a few things.
            Statistics origStatistics = gameSettings.mStatistics;

            gameSettings = GameSettings();
            InitialiseOptions(gameSettings);

            gameSettings.mStatistics = origStatistics;
            gameSettings.mStatistics.mPicaSimSettingsVersion = Statistics::LATEST_PICASIM_SETTINGS_VERSION;
        }

        // Force options to be loaded at least once
        if (!gameSettings.mStatistics.mLoadedOptions)
        {
            TRACE_FILE_IF(1) TRACE("Forcing options loading");
            InitialiseOptions(gameSettings);
        }

        LoadingScreen* initialLoadingScreen = new LoadingScreen(GetPS(PS_LOADING, gameSettings.mOptions.mLanguage), gameSettings, true, false, true);

        GLint depthBits = 0;
        // AI generated fix
        // Query depth buffer size - use Core Profile method if available
        #if defined(PICASIM_MACOS) || defined(PICASIM_LINUX)
        // OpenGL 3.3+ Core Profile: query framebuffer attachment
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Ensure default framebuffer is bound
        // Query default framebuffer depth attachment size; use GL_DEPTH_ATTACHMENT to avoid invalid enum
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depthBits);
        #else
        // Legacy/compatibility mode
        glGetIntegerv(GL_DEPTH_BITS, &depthBits);
        #endif
        TRACE_FILE_IF(1) TRACE("Depth buffer = %d bits", depthBits);
        if (depthBits == 0)
        {
            const Language language = gameSettings.mOptions.mLanguage;
            ShowDialog("PicaSim",
                "Failed to get a depth buffer - graphics will be incorrect. "
                "Please try running PicaSim on a different device, or wait for a workaround. Sorry.",
                TXT(PS_OK));
        }
        gameSettings.mStatistics.mNumDepthBits = depthBits;

        ShaderManager::Init(initialLoadingScreen);

        // Cache the thumbnails
        TRACE_FILE_IF(1) TRACE("Caching thumbnails");
        CacheThumbnailsFromDir("SystemSettings/Thumbnails",
            gameSettings.mOptions.m16BitTextures, initialLoadingScreen, GetPS(PS_LOADING, gameSettings.mOptions.mLanguage));

        // Prompt version check if on windows
        if (gameSettings.mOptions.mFrameworkSettings.isWindows())
            InitVersionChecker();

        delete initialLoadingScreen;
        initialLoadingScreen = 0;

        {
            bool doDefaultFreeFly = gameSettings.mOptions.mFreeFlyOnStartup;

            while (1)
            {
                MEMTEST();

                // Reset the challenge settings so that by default we're not doing a challenge
                gameSettings.mChallengeSettings = ChallengeSettings();

                gameSettings.mOptions.mFrameworkSettings.UpdateScreenDimensions();

                if (CheckForQuitRequest())
                    break;

                StartMenuResult startMenuResult;

                if (doDefaultFreeFly)
                {
                    TRACE_FILE_IF(1) TRACE("Doing default Free Fly");
                    startMenuResult = STARTMENU_FLY;
                }
                else
                {
                    TRACE_FILE_IF(1) TRACE("Displaying start menu");
                    startMenuResult = DisplayStartMenu(gameSettings);
                }

                if (startMenuResult == STARTMENU_QUIT)
                {
                    TRACE_FILE_IF(1) TRACE("Got start menu quit");
                    break;
                }

                // Force options to be loaded at least once
                if (!gameSettings.mStatistics.mLoadedOptions)
                {
                    TRACE_FILE_IF(1) TRACE("Forcing options loading");
                    if (!InitialiseOptions(gameSettings))
                        continue;
                }

                const Language language = gameSettings.mOptions.mLanguage;

                // Prompt for the challenge
                if (startMenuResult == STARTMENU_CHALLENGE)
                {
                    TRACE_FILE_IF(1) TRACE("Selecting aeroplane");
                    if (!SelectChallenge(gameSettings))
                        continue;
                    if (gameSettings.mChallengeSettings.mAllowAeroplaneSettings)
                    {
                        if (SelectAndLoadAeroplane(gameSettings, TXT(PS_SELECTAEROPLANE), TXT(PS_BACK), TXT(PS_USEDEFAULT)) == SELECTRESULT_CANCELLED)
                            continue;
                    }
                }
                else
                {
SelectScenario:
                    ScenarioResult scenarioResult;
                    if (doDefaultFreeFly)
                        scenarioResult = SCENARIO_DEFAULT;
                    else
                        scenarioResult = SelectScenario(gameSettings, TXT(PS_SELECTSCENARIO), TXT(PS_BACK), TXT(PS_USEDEFAULTPREVIOUS));

                    gameSettings.mOptions.mFreeFlyMode = Options::FREEFLYMODE_MAX;
                    // Select plane and scenery
                    if (scenarioResult == SCENARIO_CHOOSE)
                    {
SelectPlane:
                        TRACE_FILE_IF(1) TRACE("Selecting plane");
                        if (SelectAndLoadAeroplane(gameSettings, TXT(PS_SELECTAEROPLANE), TXT(PS_BACK), TXT(PS_USEDEFAULTPREVIOUS)) == SELECTRESULT_CANCELLED)
                            goto SelectScenario;
                        TRACE_FILE_IF(1) TRACE("Selecting scenery");
                        if (SelectAndLoadEnvironment(gameSettings, TXT(PS_SELECTSCENERY), TXT(PS_BACK), TXT(PS_USEDEFAULTPREVIOUS)) == SELECTRESULT_CANCELLED)
                            goto SelectPlane;
                        bool objectsResult = gameSettings.mObjectsSettings.LoadFromFile(gameSettings.mEnvironmentSettings.mObjectsSettingsFile);
                        IwAssert(ROWLHOUSE, objectsResult);
                    }
                    else if (scenarioResult == SCENARIO_TRAINERGLIDER)
                    {
                        gameSettings.mOptions.mFreeFlyMode = Options::FREEFLYMODE_TRAINERGLIDER;
                        TRACE_FILE_IF(1) TRACE("Using default/learner plane and environment settings");
                        bool result = gameSettings.mAeroplaneSettings.LoadFromFile("SystemSettings/Aeroplane/Trainer.xml");
                        IwAssert(ROWLHOUSE, result);
                        bool controllerResult = gameSettings.mControllerSettings.LoadFromFile("SystemSettings/Controller/SingleStick.xml");
                        IwAssert(ROWLHOUSE, controllerResult);
                        bool environmentResult = gameSettings.mEnvironmentSettings.LoadFromFile("SystemSettings/Environment/Hills.xml");
                        IwAssert(ROWLHOUSE, environmentResult);
                        bool lightingResult = gameSettings.mLightingSettings.LoadFromFile("SystemSettings/Lighting/CloudyDaytime.xml");
                        IwAssert(ROWLHOUSE, lightingResult);
                        // Tweak some things
                        gameSettings.mEnvironmentSettings.mThermalDensity = 0.0f;
                        gameSettings.mEnvironmentSettings.mWindSpeed = 4.0f;
                        gameSettings.mEnvironmentSettings.mTurbulenceAmount = 0.5f;
                        gameSettings.mEnvironmentSettings.mSurfaceTurbulence = 1.0f;
                        gameSettings.mEnvironmentSettings.mShearTurbulence = 1.0f;
                        gameSettings.mEnvironmentSettings.mDeadAirTurbulence = 0.0f;
                        gameSettings.mAeroplaneSettings.mLaunchUp = 1.0f;
                        gameSettings.mAeroplaneSettings.mLaunchLeft = -1.0f;
                        gameSettings.mAeroplaneSettings.mLaunchOffsetUp = 0.0f;

                        bool objectsResult = gameSettings.mObjectsSettings.LoadFromFile(gameSettings.mEnvironmentSettings.mObjectsSettingsFile);
                        IwAssert(ROWLHOUSE, objectsResult);
                    }
                    else if (scenarioResult == SCENARIO_TRAINERPOWERED)
                    {
                        gameSettings.mOptions.mFreeFlyMode = Options::FREEFLYMODE_TRAINERPOWERED;
                        bool result = gameSettings.mAeroplaneSettings.LoadFromFile("SystemSettings/Aeroplane/Jackdaw.xml");
                        IwAssert(ROWLHOUSE, result);
                        bool controllerResult = gameSettings.mControllerSettings.LoadFromFile("SystemSettings/Controller/TwoSticksWithThrottle.xml");
                        IwAssert(ROWLHOUSE, controllerResult);
                        bool environmentResult = gameSettings.mEnvironmentSettings.LoadFromFile("SystemSettings/Environment/RecreationGroundPanoramic.xml");
                        IwAssert(ROWLHOUSE, environmentResult);
                        gameSettings.mEnvironmentSettings.mThermalDensity = 0.0f;
                        bool objectsResult = gameSettings.mObjectsSettings.LoadFromFile(gameSettings.mEnvironmentSettings.mObjectsSettingsFile);
                        IwAssert(ROWLHOUSE, objectsResult);
                    }
                    else if (scenarioResult != SCENARIO_DEFAULT)
                    {
                        continue;
                    }
                }

                doDefaultFreeFly = false;

                {
                    TRACE_FILE_IF(1) TRACE("Setting up loading screen");
                    LoadingScreen loadingScreen(TXT(PS_LOADING), gameSettings, true, true, true);
                    TRACE_FILE_IF(1) TRACE("Initialising PicaSim");
                    if (!PicaSim::Init(gameSettings, &loadingScreen))
                    {
                        // Message box should have been shown from the source of the error
                        continue;
                    }
                }

                int64 lastTimeMs = Timer::GetMilliseconds();

                PicaSim::UpdateResult updateResult = PicaSim::UPDATE_CONTINUE;

                TRACE_FILE_IF(1) TRACE("Starting main loop");
                while (updateResult == PicaSim::UPDATE_CONTINUE)
                {
                    MEMTEST();

                    gameSettings.mOptions.mFrameworkSettings.UpdateScreenDimensions();

                    PollEvents();
                    int64 currentTimeMs = Timer::GetMilliseconds();

#ifdef PICASIM_VR_SUPPORT
                    // VR frame handling
                    bool inVRFrame = false;
                    VRFrameInfo vrFrameInfo;
                    if (VRManager::IsAvailable() && VRManager::GetInstance().IsVREnabled())
                    {
                        // Always poll events to detect headset reconnection
                        VRManager::GetInstance().PollEvents();

                        // Only start VR frames when headset is actually active
                        if (VRManager::GetInstance().IsVRReady())
                        {
                            inVRFrame = VRManager::GetInstance().BeginVRFrame(vrFrameInfo);
                        }
                    }
#endif

                    updateResult = PicaSim::GetInstance().Update(currentTimeMs - lastTimeMs);

#ifdef PICASIM_VR_SUPPORT
                    if (inVRFrame)
                    {
                        VRManager::GetInstance().EndVRFrame(vrFrameInfo);
                    }
#endif

#ifdef CAP_FRAME_RATE
#ifdef PICASIM_VR_SUPPORT
                    // Skip frame rate capping when in VR (VR runtime handles timing)
                    if (!inVRFrame)
#endif
                    {
                        // Attempt constant frame rate
                        while ((Timer::GetMilliseconds() - currentTimeMs) < MS_PER_FRAME)
                        {
                            int32 yield = (int32) (MS_PER_FRAME - (Timer::GetMilliseconds() - currentTimeMs));
                            if (yield<0)
                                break;
                            else if (yield < 2)
                                yield = 2; // always yield by at least 1ms for audio
                            SDL_Delay(yield-1);
                        }
                    }
#endif
                    lastTimeMs = currentTimeMs;
                }
                TRACE_FILE_IF(1) TRACE("Finished main loop. Terminating PicaSim ready to start again");

                PicaSim::Terminate();

                if (updateResult == PicaSim::UPDATE_QUIT)
                    break;
            }
            TRACE_FILE_IF(1) TRACE("PicaSim requested quit - terminating audio");
        }

        ShaderManager::Terminate();

#ifdef IW_USE_PROFILE
        destroyDebugMenu();
#endif

        TRACE_FILE_IF(1) TRACE("Saving settings");
        gameSettings.SaveToFile(userSettingsFile);

        TerminateVersionChecker();

        TRACE_FILE_IF(1) TRACE("MenuTerminate");
        MenuTerminate();

    }

    TRACE_FILE_IF(1) TRACE("AudioManager::Terminate()");
    AudioManager::Terminate();

#ifdef PICASIM_VR_SUPPORT
    TRACE_FILE_IF(1) TRACE("VRManager::Terminate()");
    VRManager::Terminate();
#endif

#ifdef EXPLICIT_EGL_INIT
    TRACE_FILE_IF(1) TRACE("eglTerm");
    eglTerm(true);
#endif

    return 0;
}
