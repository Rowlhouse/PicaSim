#include "FileMenu.h"
#include "FonctionsMarmelade.h"
#include "Graphics.h"
#include "MemoryCheck.h"
#include "PicaSim.h"
// #include "Scoreloop.h"
#include "ShaderManager.h"
#include "Menu.h"
#include "StartMenu.h"
#include "SettingsMenu.h"
#include "FileMenu.h"
#include "LoadingScreen.h"
#include "WhatsNewMenu.h"
#include "PicaStrings.h"
// #include "Profile.h"
#include "VersionChecker.h"
#include "AuthorisationChecker.h"
#include "PicaJoystick.h"
#include "WindowsBilling.h"
#include "PicaDialog.h"

// #include <IwGL.h>
// #include <IwGx.h>
// #include <IwUI.h>
// #include <Iw2D.h>

// #include <s3e.h>
// #include <s3eWindow.h>
// #include <s3eIOSBackgroundAudio.h>

// #include "gamepad.h"
// #include "dpiInfo.h"

#include "GameSettings.h"
#include "Trace.h"
#include <cstdint>
#define EXPLICIT_EGL_INITx

#define DO_AUTHORISATION_CHECKx

//---------------------------------------------------------------------------------------------------------------------
// Attempt to lock to a 60 frames per second
#define MS_PER_FRAME (1000 / 60)
#define CAP_FRAME_RATE

static float GetSurfaceDiagonalInches()
{
  int32_t dpi = DPI::dpiGetScreenDPI();
  if (dpi > 0)
  {
    int w = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
    int h = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
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
  FileMenuLoad(file, gameSettings, "SystemSettings/Challenge", "UserSettings/Challenge", ".xml", TXT(PS_SELECTRACE), 0, 0, TXT(PS_BACK), NULL, FILEMENUTYPE_CHALLENGE);
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

//---------------------------------------------------------------------------------------------------------------------
// Returns true if options were set, false otherwise (e.g. cancel)
static bool InitialiseOptions(GameSettings& gameSettings)
{
  const Language language = gameSettings.mOptions.mLanguage;
  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
  {
    bool result = false;
    const char* deviceName = s3eDeviceGetString(S3E_DEVICE_ID);
    TRACE_FILE_IF(1) TRACE("iOS Device ID = %s", deviceName);
    if (strncmp(deviceName, "iPad", 4) == 0)
    {
      if (deviceName[4] == '1' || deviceName[4] == '2')
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/LowQuality-LargeScreen.xml"); // ipad 1 & 2
      else if (deviceName[4] == '3' || deviceName[4] == '4')
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/StandardQuality-LargeScreen.xml"); // ipad 3, 4 and air
      else
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/HighQuality-LargeScreen.xml"); // ipad 5? and above
    }
    else if (strncmp(deviceName, "iPod", 4) == 0)
    {
      if (deviceName[4] == '1' || deviceName[4] == '2' || deviceName[4] == '3' || deviceName[4] == '4')
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/LowQuality-SmallScreen.xml"); // ipod touch gen 1-4
      else if (deviceName[4] == '5')
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/StandardQuality-SmallScreen.xml"); // ipod touch 5
      else
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/HighQuality-SmallScreen.xml"); // ipod touch 6
    }
    else if (strncmp(deviceName, "iPhone", 6) == 0)
    {
      if (deviceName[6] == '1' || deviceName[6] == '2' || deviceName[6] == '3' || deviceName[6] == '4')
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/LowQuality-SmallScreen.xml"); // iphone 1, 3G, 3GS, 4, 4s
      else if (deviceName[6] == '5' || deviceName[6] == '6')
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/StandardQuality-SmallScreen.xml"); // 5, 5s
      else
        result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/HighQuality-SmallScreen.xml"); // iphone 6 and above
    }
    else
    {
      // New device?!
      result = gameSettings.mOptions.LoadFromFile("SystemSettings/Options/StandardQuality-LargeScreen.xml");
    }
    IwAssert(ROWLHOUSE, result);
    TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
    gameSettings.mStatistics.mLoadedOptions = true;

    gameSettings.mOptions.mControlledPlaneShadows = Options::PROJECTED;
  }
  else if (gameSettings.mOptions.mFrameworkSettings.isWindows())
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
    int32_t numCores = s3eDeviceGetInt(S3E_DEVICE_NUM_CPU_CORES);
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
      FileMenuLoad(file, gameSettings, "SystemSettings/Options", "UserSettings/Options", ".xml", TXT(PS_SELECTOPTIONS), 0, 0, TXT(PS_BACK), NULL);
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

  int32_t memoryKB = s3eDeviceGetInt(S3E_DEVICE_MEM_TOTAL);
  TRACE_FILE_IF(1) TRACE("InitialiseOptions: reported memory = %d KB", memoryKB);
  if (memoryKB > 400*1024 || memoryKB <= 0) // -1 on Windows?!
    gameSettings.mOptions.m16BitTextures = false;
  else
    gameSettings.mOptions.m16BitTextures = true;
  TRACE_FILE_IF(1) TRACE("Options: Using 16 bit textures = %d", gameSettings.mOptions.m16BitTextures);

#ifndef IW_DEBUG
    if (gameSettings.mOptions.mFrameworkSettings.isWindows())
      gameSettings.mOptions.mEnableScoreloop = false;
#endif
#ifndef ENABLE_SCORELOOP
    gameSettings.mOptions.mEnableScoreloop = false;
#endif
    return true;
}

#if 0
int32 suspendCallback(void *systemData, void *userData)
{
  TRACE_FILE_IF(1) TRACE("Calling eglTerm");
  eglTerm(false);
  TRACE_FILE_IF(1) TRACE("Called eglTerm");
  return 0;
}

int32 resumeCallback(void *systemData, void *userData)
{
  TRACE_FILE_IF(1) TRACE("Calling eglInit");
  eglInit(false);
  TRACE_FILE_IF(1) TRACE("Called eglInit");
  return 0;
}
#endif

int32_t pauseCallback(void *systemData, void *userData)
{
  TRACE_FILE_IF(1) TRACE("PicaSim pause start");
  if (PicaSim::IsCreated())
  {
    PicaSim::GetInstance().SetStatus(PicaSim::STATUS_PAUSED);
  }
  TRACE_FILE_IF(1) TRACE("PicaSim pause end");
  return 0;
}

int32_t unpauseCallback(void *systemData, void *userData)
{
  TRACE_FILE_IF(1) TRACE("PicaSim unpause start");
  if (PicaSim::IsCreated())
  {
    PicaSim::GetInstance().SetStatus(PicaSim::STATUS_PAUSED);
  }
  TRACE_FILE_IF(1) TRACE("PicaSim unpause end");
  return 0;
}

static void BufferOverrunTest()
{
    char* buffer = new char[20];
    buffer[20] = 0; // wrote to the memory AFTER buffer
    delete[] buffer;
}

//---------------------------------------------------------------------------------------------------------------------
int main()
{
  SetTraceLevel(1);

  InitMemoryOverrunCheck();
  MEMTEST();

  s3eConfigGetInt("S3E", "SysGlesVersion", &gGLVersion);

  // Disable the PVRVFrame window
  void (*PVRVFrameEnableControlWindow)(bool)  = (void(*)(bool))eglGetProcAddress("PVRVFrameEnableControlWindow");
  if (PVRVFrameEnableControlWindow)
  {
    int enable = 0;
    s3eConfigGetInt("PICASIM", "EnablePVRVFrame", &enable);
    TRACE_FILE_IF(1) TRACE("Setting PVRVFrame window to %d", enable);
    PVRVFrameEnableControlWindow(enable != 0); 
  }

#if 0
  s3eGLRegister(S3E_GL_SUSPEND, suspendCallback, 0);
  s3eGLRegister(S3E_GL_RESUME, resumeCallback, 0);
#endif

  s3eDeviceRegister(S3E_DEVICE_PAUSE, pauseCallback, 0);
  s3eDeviceRegister(S3E_DEVICE_UNPAUSE, unpauseCallback, 0);

#ifdef EXPLICIT_EGL_INIT
  // This is no good as Marmalade recreates the surface badly on suspend/resume
  TRACE_FILE_IF(1) TRACE("Calling eglInit");
  eglInit(true);
  TRACE_FILE_IF(1) TRACE("eglInit has been called");
#endif

  // Since we request HW rasterisation this will initialise GL too
  TRACE_FILE_IF(1) TRACE("Calling IwUIInit");
  IwUIInit();
  TRACE_FILE_IF(1) TRACE("IwUIInit has been called");

  TRACE_FILE_IF(1) TRACE("Calling Iw2DInit");
  Iw2DInit();
  TRACE_FILE_IF(1) TRACE("Iw2DInit has been called");

  TRACE_FILE_IF(1) TRACE("Calling dpiInit");
  DPI::dpiInit();
  TRACE("dpi = %d so physical diagonal = %5.2f inches", DPI::dpiGetScreenDPI(), GetSurfaceDiagonalInches());
  TRACE_FILE_IF(1) TRACE("dpiInit has been called");

#if 0
  s3eWindowDisplayMode modes[32];
  int numModes = 32;
  s3eResult result = s3eWindowGetDisplayModes(modes, &numModes);
  result = s3eWindowSetFullscreen(&modes[numModes-1]);
#endif

  if (s3eIOSBackgroundAudioAvailable() == S3E_TRUE)
  {
    s3eIOSBackgroundAudioSetMix(S3E_TRUE);
  }

  TRACE_FILE_IF(1) TRACE("Calling AudioManager::Init()");
  AudioManager::Init();
  TRACE_FILE_IF(1) TRACE("AudioManager::Init() has been called");

  InitPicaStrings();

  srand(time(0));

  MEMTEST();
  // Make sure everything goes out of scope before we close down Marmalade
  {
    GameSettings gameSettings;

    TRACE_FILE_IF(1) TRACE("Calling MenuInit");
    MenuInit(gameSettings);
    TRACE_FILE_IF(1) TRACE("MenuInit has been called");

    // Make the user settings directory if necessary
    TRACE_FILE_IF(1) TRACE("Making settings directories if necessary");
    s3eFileMakeDirectory("UserSettings");
    s3eFileMakeDirectory("UserSettings/Options");
    s3eFileMakeDirectory("UserSettings/Aeroplane");
    s3eFileMakeDirectory("UserSettings/Environment");
    s3eFileMakeDirectory("UserSettings/Objects");
    s3eFileMakeDirectory("UserSettings/AIControllers");
    s3eFileMakeDirectory("UserSettings/Lighting");
    s3eFileMakeDirectory("UserSettings/Controller");
    s3eFileMakeDirectory("UserSettings/Joystick");
    s3eFileMakeDirectory("UserSettings/Challenge");

    s3eFileMakeDirectory("UserData");
    s3eFileMakeDirectory("UserData/Aerofoils");
    s3eFileMakeDirectory("UserData/Aeroplanes");
    s3eFileMakeDirectory("UserData/Audio");
    s3eFileMakeDirectory("UserData/Menu");
    s3eFileMakeDirectory("UserData/Panoramas");
    s3eFileMakeDirectory("UserData/FileTerrains");
    s3eFileMakeDirectory("UserData/Syboxes");
    s3eFileMakeDirectory("UserData/Textures");

    TRACE_FILE_IF(1) TRACE("Loading game settings");
    gameSettings.LoadFromFile("UserSettings/settings.xml", false);

    if (gameSettings.mStatistics.mPicaSimBuildNumber < GetBuildNumber() && gameSettings.mStatistics.mPicaSimBuildNumber != 0)
    {
      DisplayWhatsNewMenu(gameSettings);
      gameSettings.mStatistics.mPicaSimBuildNumber = GetBuildNumber();
      TRACE_FILE_IF(1) TRACE("Saving settings to remember the build number");
      gameSettings.SaveToFile("Usersettings/Settings.xml");
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

    LoadingScreen* initialLoadingScreen = new LoadingScreen(GetPS(PS_LOADING, gameSettings.mOptions.mLanguage), gameSettings, false, false, false);

    GLint depthBits = 0;
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    TRACE_FILE_IF(1) TRACE("Depth buffer = %d bits", depthBits);
    if (depthBits == 0)
    {
      const Language language = gameSettings.mOptions.mLanguage;
      ShowDialog("PicaSim", "Failed to get a depth buffer - graphics will be incorrect. Please try running PicaSim on a different device, or wait for a workaround. Sorry.", TXT(PS_OK));
    }
    gameSettings.mStatistics.mNumDepthBits = depthBits;

    ShaderManager::Init(initialLoadingScreen);

    if (s3eFileCheckExists("SystemData/Menu/Amazon.png"))
    {
      gameSettings.mStatistics.mIsAmazon = true;
    }
    if (s3eFileCheckExists("SystemData/Menu/Thanks.png"))
    {
      gameSettings.mStatistics.mAppType = Statistics::APP_FULL;
    }
    else
    {
      if (s3eFileCheckExists("SystemData/Menu/Free.png"))
        gameSettings.mStatistics.mAppType = Statistics::APP_FREE;
      else
        gameSettings.mStatistics.mAppType = Statistics::APP_DEMO;
    }
    if (gameSettings.mStatistics.mIsInTrialMode)
    {
      gameSettings.mStatistics.mAppType = Statistics::APP_DEMO;
    }
    TRACE_FILE_IF(1) TRACE("Calling InitWindowsBilling()");
    InitWindowsBilling(gameSettings);
    TRACE_FILE_IF(1) TRACE("InitWindowsBilling() has been called");


#ifndef IW_DEBUG
    if (gameSettings.mOptions.mFrameworkSettings.isWindows())
      gameSettings.mOptions.mEnableScoreloop = false;
#endif

    // Cache the thumbnails
    TRACE_FILE_IF(1) TRACE("Caching thumbnails");
    CacheThumbnailsFromDir("SystemSettings/Thumbnails", 
      gameSettings.mOptions.m16BitTextures, initialLoadingScreen, GetPS(PS_LOADING, gameSettings.mOptions.mLanguage));

#ifdef ENABLE_SCORELOOP
    // Initialise Scoreloop
    InitScoreloop();
#else
    gameSettings.mOptions.mEnableScoreloop = false;
#endif

    // Prompt version check if on windows
    if (gameSettings.mOptions.mFrameworkSettings.isWindowsDesktop())
      InitVersionChecker();

#ifdef DO_AUTHORISATION_CHECK
    InitAuthorisationChecker();
#endif

    delete initialLoadingScreen;
    initialLoadingScreen = 0;

#ifdef IW_USE_PROFILE
    createDebugMenu();
#endif

    {
      bool doDefaultFreeFly = gameSettings.mOptions.mFreeFlyOnStartup;

      while (1)
      {
        MEMTEST();

        // Reset the challenge settings so that by default we're not doing a challenge
        gameSettings.mChallengeSettings = ChallengeSettings();

        gameSettings.mOptions.mFrameworkSettings.UpdateScreenDimensions();

        if (s3eDeviceCheckQuitRequest())
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
          if (gameSettings.mStatistics.mAppType != Statistics::APP_FULL)
          {
            TRACE_FILE_IF(1) TRACE("Using default race in the free version");

            int button = ShowDialog("PicaSim", TXT(PS_ONERACE), TXT(PS_NOTNOW), TXT(PS_CANCEL), TXT(PS_PURCHASE));
            if (button == 1)
              continue;
            else if (button == 2)
              BuyFullVersion(gameSettings);

            std::string challengeFile("SystemSettings/Challenge/Race-Cliff-Banana.xml");
            bool result = gameSettings.mChallengeSettings.LoadFromFile(challengeFile);
            IwAssert(ROWLHOUSE, result);
            TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
            if (!result)
              continue;
            gameSettings.mChallengeSettings.CalculateChecksum(challengeFile);
          }
          else
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

          if (
            gameSettings.mStatistics.mAppType != Statistics::APP_FULL &&
            gameSettings.mChallengeSettings.mChallengeID != 98
            )
          {
            ShowDialog("PicaSim", TXT(PS_INVALIDRACE), TXT(PS_OK));
            gameSettings.mChallengeSettings.mChecksum = 0;
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

        int64_t lastTimeMs = s3eTimerGetMs();

        PicaSim::UpdateResult updateResult = PicaSim::UPDATE_CONTINUE;

        TRACE_FILE_IF(1) TRACE("Starting main loop");
        while (updateResult == PicaSim::UPDATE_CONTINUE)
        {
          MEMTEST();

          IW_PROFILE_NEWFRAME();
          gameSettings.mOptions.mFrameworkSettings.UpdateScreenDimensions();

          s3eDeviceYield(0);
          int64_t currentTimeMs = s3eTimerGetMs();

          updateResult = PicaSim::GetInstance().Update(currentTimeMs - lastTimeMs);
#ifdef CAP_FRAME_RATE
          // Attempt constant frame rate
          while ((s3eTimerGetMs() - currentTimeMs) < MS_PER_FRAME)
          {
            int32_t yield = (int32_t) (MS_PER_FRAME - (s3eTimerGetMs() - currentTimeMs));
            if (yield<0)
              break;
            else if (yield < 2)
              yield = 2; // always yield by at least 1ms for audio
            s3eDeviceYield(yield-1);
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
    gameSettings.SaveToFile("Usersettings/Settings.xml");

    TerminateVersionChecker();
    TerminateAuthorisationChecker();

#ifdef ENABLE_SCORELOOP
    //TermScoreloop();
#endif
    TRACE_FILE_IF(1) TRACE("MenuTerminate");
    MenuTerminate();

  }

  TRACE_FILE_IF(1) TRACE("AudioManager::Terminate()");
  AudioManager::Terminate();

  TRACE_FILE_IF(1) TRACE("TerminateWindowsBilling()");
  TerminateWindowsBilling();

  TRACE_FILE_IF(1) TRACE("Calling dpiTerminate");
  DPI::dpiTerminate();

  TRACE_FILE_IF(1) TRACE("Iw2DTerminate");
  Iw2DTerminate();

  TRACE_FILE_IF(1) TRACE("IwUITerminate");
  IwUITerminate();

#ifdef EXPLICIT_EGL_INIT
  TRACE_FILE_IF(1) TRACE("eglTerm");
  eglTerm(true);
#endif

  return 0;
}
