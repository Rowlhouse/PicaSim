#include "GameSettings.h"
#include "File.h"
#include "FonctionsMarmelade.h"
#include "HelpersXML.h"
#include "SimpleObject.h"
#include "Trace.h"
// #include "Scoreloop.h"
// #include "VersionChecker.h"
// #include "SimpleObject.h"
#include "PicaJoystick.h"

// #include <s3eFile.h>

const int Statistics::LATEST_PICASIM_SETTINGS_VERSION = 27;

const float Options::LIMBO_MAX_DIFFICULTY_MULTIPLIER = 3.0f;

#define COMPARE_FOR_WIND(var)  if (var != settings.var) {settingsChangeActions.mRecalcWind = true; TRACE_FILE_IF(1) TRACE("Recalc wind due to %s", #var);}
#define COMPARE_FOR_TERRAIN(var)  if (var != settings.var) {settingsChangeActions.mReloadTerrain = true; TRACE_FILE_IF(1) TRACE("Reload terrain due to %s", #var);}
#define COMPARE_FOR_AIRELOAD(var)  if (var != settings.var) {settingsChangeActions.mReloadAI = true; TRACE_FILE_IF(1) TRACE("Reload AI due to %s", #var);}
#define COMPARE_FOR_AIRESET(var)  if (var != settings.var) {settingsChangeActions.mResetAI = true; TRACE_FILE_IF(1) TRACE("Reset AI due to %s", #var);}
#define COMPARE_FOR_AEROPLANE(var)  if (var != settings.var) {settingsChangeActions.mReloadAeroplane = true; TRACE_FILE_IF(1) TRACE("Reload aeroplane due to %s", #var);}
#define COMPARE_FOR_CHALLENGE(var)  if (var != settings.var) {settingsChangeActions.mRestartChallenge = true; TRACE_FILE_IF(1) TRACE("Restart challenge due to %s", #var);}

const char* TerrainSettings::mTerrainTypes[NUM_TYPES] =
{
  "Midpoint displacement",
  "Ridge",
  "Panorama",
  "ImageFile",
  "Panorama3D"
};

//---------------------------------------------------------------------------------------------------------------------
bool Settings::SaveToFile(const std::string& fileName) const
{
  TiXmlDocument doc;
  TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
  doc.LinkEndChild(decl);
  if (WriteToDoc(doc))
  {
    bool result = doc.SaveFile(fileName.c_str());
    TRACE_FILE_IF(1) TRACE("Written to file %s = %d", fileName.c_str(), result);
    return result;
  }
  else
  {
    TRACE_FILE_IF(1) TRACE("Failed writing settings to xml doc");
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool Settings::LoadFromFile(const std::string& name, bool readAll)
{
  TiXmlDocument doc(name);
  if (!doc.LoadFile())
  {
    IwAssertMsg(ROWLHOUSE, false, ("Failed to open file"));
    TRACE_FILE_IF(1) TRACE("Failed to load settings from %s", name.c_str());
    return false;
  }
  return ReadFromDoc(doc, readAll);
}

//---------------------------------------------------------------------------------------------------------------------
bool Settings::LoadBasicsFromFile(const std::string& name)
{
  TiXmlDocument doc(name);
  if (!doc.LoadFile())
  {
    IwAssertMsg(ROWLHOUSE, false, ("Failed to open file"));
    TRACE_FILE_IF(1) TRACE("Failed to load settings from %s", name.c_str());
    return false;
  }
  return ReadBasicsFromDoc(doc);
}

//---------------------------------------------------------------------------------------------------------------------
AIControllersSettings::AIControllersSettings() :
  mVersion(mCurrentVersion),
  mEnableDebugDraw(false),
  mIncludeInCameraViews(false),
  mCreateMaxNumControllers(false),
  mRandomColourOffset(0.0f),
  mLaunchDirection(-1.0f),
  mLaunchSeparationDistance(4.0f)
{
  mAIControllers.clear();
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions AIControllersSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AIControllersSettings& settings)
{
  if (mAIControllers.size() != settings.mAIControllers.size())
  {
    settingsChangeActions.mReloadAI = true;
  }
  else
  {
    COMPARE_FOR_AIRELOAD(mCreateMaxNumControllers);
    COMPARE_FOR_AIRESET(mLaunchSeparationDistance);
    COMPARE_FOR_AIRELOAD(mRandomColourOffset);
    COMPARE_FOR_AIRESET(mLaunchDirection);
    for (size_t i = 0 ; i != mAIControllers.size() ; ++i)
    {
      COMPARE_FOR_AIRELOAD(mAIControllers[i].mAeroplaneFile);
      COMPARE_FOR_AIRELOAD(mAIControllers[i].mColourOffset);
      COMPARE_FOR_AIRESET(mAIControllers[i].mIncludeInCameraViews);
      COMPARE_FOR_AIRESET(mAIControllers[i].mEnableDebugDraw);
    }
  }
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
bool AIControllersSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "AIControllers" );
  doc.LinkEndChild( element );

  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mEnableDebugDraw);
  WRITE_ATTRIBUTE(mIncludeInCameraViews);
  WRITE_ATTRIBUTE(mCreateMaxNumControllers);
  WRITE_DOUBLE_ATTRIBUTE(mRandomColourOffset);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchDirection);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchSeparationDistance);
  size_t numAIControllers = mAIControllers.size();
  WRITE_ATTRIBUTE(numAIControllers); 

  char txt[256];
  for (size_t i = 0 ; i != numAIControllers ; ++i)
  {
    const AIControllerSetting& aic = mAIControllers[i];
    sprintf(txt, "file_%lu", (long unsigned) i);
    element->SetAttribute(txt, aic.mAeroplaneFile);
    sprintf(txt, "includeInCameraViews_%lu", (long unsigned) i);
    element->SetAttribute(txt, aic.mIncludeInCameraViews);
    sprintf(txt, "enableDebugDraw_%lu", (long unsigned) i);
    element->SetAttribute(txt, aic.mEnableDebugDraw);
    sprintf(txt, "colourShift_%lu", (long unsigned) i);
    element->SetDoubleAttribute(txt, aic.mColourOffset);
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void AIControllersSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool AIControllersSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "AIControllers" ).ToElement();
  if (!element)
    return false;

  *this = AIControllersSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mEnableDebugDraw);
  READ_ATTRIBUTE(mIncludeInCameraViews);
  READ_ATTRIBUTE(mCreateMaxNumControllers);
  READ_ATTRIBUTE(mRandomColourOffset);
  READ_ATTRIBUTE(mLaunchDirection);
  READ_ATTRIBUTE(mLaunchSeparationDistance);
  size_t numAIControllers = 0;
  READ_ATTRIBUTE(numAIControllers);

  mAIControllers.resize(numAIControllers);

  char txt[256];
  for (size_t i = 0 ; i != numAIControllers ; ++i)
  {
    AIControllerSetting& aic = mAIControllers[i];
    sprintf(txt, "file_%lu", (long unsigned) i);
    readFromXML(element, txt, aic.mAeroplaneFile);
    sprintf(txt, "enableDebugDraw_%lu", (long unsigned) i);
    readFromXML(element, txt, aic.mEnableDebugDraw);
    sprintf(txt, "colourShift_%lu", (long unsigned) i);
    readFromXML(element, txt, aic.mColourOffset);
  }

  Upgrade(doc);

  return true;
}
//---------------------------------------------------------------------------------------------------------------------
Options::Options() :
  mVersion(mCurrentVersion),
  mLanguage(LANG_EN),
  m16BitTextures(false),
  mFreeFlyMode(FREEFLYMODE_MAX),
  mFreeFlyOnStartup(false),
  mEnableSocketController(false),
  mSetWindDirectionOnWalkabout(true),
  mUseBackButtonToExit(false),
  mUseAeroplanePreferredController(true),
  mEnableWalkabout(false),
  mEnableObjectEditing(false),
  mVolumeScale(1.0f),
  mVariometerVolume(1.0f),
  mWindVolume(1.0f),
  mOutsideAeroplaneVolume(1.0f),
  mInsideAeroplaneVolume(0.3f),
  mTerrainWireframe(false),
  mGroundViewUpdateTerrainLOD(false),
  mGroundViewTerrainLOD(200.0f),
  mAeroplaneViewTerrainLOD(190.0f),
  mGroundViewAutoZoom(0.65f),
  mGroundViewAutoZoomScale(0.4f),
  mGroundViewFieldOfView(70.0f),
  mGroundViewHorizonAmount(0.6f),
  mGroundViewLag(0.1f),
  mGroundViewYawOffset(0.0f),
  mGroundViewPitchOffset(0.0f),
  mGroundViewFollow(true),
  mAeroplaneViewFieldOfView(60.0f),
  mEnableZoomView(false),
  mOnlyPlaneInZoomView(false),
  mSmokeOnlyInMainView(true),
  mZoomViewSize(0.2f),
  mEnableStereoscopy(false),
  mStereoSeparation(0.1f),
  mMaxNearClipDistance(1.0f),
  mSeparateSpecular(true),
  mWindArrowSize(0.12f),
  mWindsockOpacity(128),
  mFreeFlightDisplayTime(true),
  mFreeFlightDisplaySpeed(false),
  mFreeFlightDisplayAirSpeed(false),
  mFreeFlightDisplayMaxSpeed(false),
  mFreeFlightDisplayAscentRate(false),
  mFreeFlightDisplayAltitude(true),
  mFreeFlightDisplayDistance(false),
  mFreeFlightColourText(true),
  mFreeFlightTextAtTop(true),
  mFreeFlightTextBackgroundOpacity(0.0f),
  mFreeFlightTextBackgroundColour(1.0f),
  mFreeFlightUnits(UNITS_MPS),
  mFreeFlightMaxAI(5),
  mDisplayFPS(false),
  mRaceVibrationAmount(0.5f),
  mLimboDifficultyMultiplier(1.0f),
  mRaceBeepVolume(0.1f),
  mEnableScoreloop(false),
  mPauseButtonsSize(0.5f),
  mPauseButtonOpacity(128),
  mControllerMode(1),
  mControllerBrakesForward(true),
  mControllerSize(0.9f),
  mControllerUseAbsolutePosition(false),
  mControllerSquare(true),
  mControllerHorOffset(0.25f),
  mControllerVerOffset(0.5f),
  mJoystickID(0),
  mControllerStaggered(false),
  mControllerStyle(CONTROLLER_STYLE_CROSS),
  mControllerAlpha(255),
  mControllerStickAlpha(128),
  mControllerStickCross(false),
  mControllerEnableTrim(false),
  mControllerTrimSize(0.05f),
  mDrawAeroplaneCoM(COM_NONE),
  mDrawSunPosition(false),
  mControlledPlaneShadows(BLOB),
  mOtherShadows(BLOB),
  mProjectedShadowDetail(8),
  mDrawLaunchMarker(true),
  mDrawGroundPosition(false),
  mSkyGridOverlay(SKYGRID_NONE),
  mSkyGridAlignment(SKYGRIDALIGN_CROSSWIND),
  mSkyGridDistance(150.0f),
  mMaxMarkersPerThermal(2),
  mDrawThermalWindField(false),
  mGraphDuration(120.0f),
  mGraphFPS(0.0f),
  mGraphAirSpeed(0.0f),
  mGraphAltitude(0.0f),
  mGraphGroundSpeed(0.0f),
  mGraphClimbRate(0.0f),
  mGraphWindSpeed(0.0f),
  mGraphWindVerticalVelocity(0.0f),
  mNumWindStreamers(0),
  mWindStreamerTime(15.0f),
  mWindStreamerDeltaZ(2.5f),
  mStallMarkers(false),
  mTimeScale(1.0f),
  mAerofoilInfo(-1),
  mAerofoilPlotAngleRange(90.0f),
  mAerofoilPlotCDMax(2.0f),
  mAerofoilPlotCMMax(0.5f),
  mAerofoilPlotCLMax(1.5f),
  mAerofoilPlotLDMax(50.0f),
  mAerofoilPlotReference(true),
  mAerofoilPlotPolar(false),
  mRenderPreference(RENDER_PREFER_3DS),
  mAmbientLightingScale(1.0f),
  mDiffuseLightingScale(1.0f),
  mBasicTextureDetail(9),
  mMaxSkyboxDetail(1),
  mEnableSmoke(true),
  mSmokeQuality(1.0f)
{
  int32_t memoryKB = s3eDeviceGetInt(S3E_DEVICE_MEM_TOTAL);
  TRACE_FILE_IF(1) TRACE("Options: reported memory = %d KB", memoryKB);
  if (memoryKB > 400*1024 || memoryKB <= 0) // -1 on Windows?!
    m16BitTextures = false;
  else
    m16BitTextures = true;
  TRACE_FILE_IF(1) TRACE("Options: Using 16 bit textures = %d", m16BitTextures);

  int32_t language = s3eDeviceGetInt(S3E_DEVICE_LANGUAGE);
  if (language == S3E_DEVICE_LANGUAGE_GERMAN)
    mLanguage = LANG_DE;
  else if (language == S3E_DEVICE_LANGUAGE_PORTUGUESE)
    mLanguage = LANG_PT;
  else if (language == S3E_DEVICE_LANGUAGE_FRENCH)
    mLanguage = LANG_FR;
  else if (language == S3E_DEVICE_LANGUAGE_ENGLISH)
    mLanguage = LANG_EN;
  else
  {
    mLanguage = LANG_EN;
  }

  int os = s3eDeviceGetInt(S3E_DEVICE_OS);
  if (os == S3E_OS_ID_ANDROID)
    mJoystickID = 1;

  if (os == S3E_OS_ID_WINDOWS) //|| os == S3E_OS_ID_WIN10)
    mMaxSkyboxDetail = 2;
}

//---------------------------------------------------------------------------------------------------------------------
bool Options::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "Options" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mFrameworkSettings.mPhysicsSubsteps); 
  WRITE_ATTRIBUTE(mFrameworkSettings.mUseMultiLights); 
  WRITE_DOUBLE_ATTRIBUTE(mMaxNearClipDistance); 
  WRITE_ATTRIBUTE(mSeparateSpecular);
  WRITE_DOUBLE_ATTRIBUTE(mFrameworkSettings.mFarClipPlaneDistance); 

  WRITE_ATTRIBUTE(mLanguage); 
  WRITE_DOUBLE_ATTRIBUTE(mVolumeScale); 
  WRITE_DOUBLE_ATTRIBUTE(mVariometerVolume); 
  WRITE_DOUBLE_ATTRIBUTE(mWindVolume); 
  WRITE_DOUBLE_ATTRIBUTE(mOutsideAeroplaneVolume);
  WRITE_DOUBLE_ATTRIBUTE(mInsideAeroplaneVolume);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewAutoZoom);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewAutoZoomScale);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewFieldOfView);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewHorizonAmount);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewLag);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewYawOffset);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewPitchOffset);
  WRITE_ATTRIBUTE(mGroundViewFollow);
  WRITE_DOUBLE_ATTRIBUTE(mAeroplaneViewFieldOfView);
  WRITE_ATTRIBUTE(mEnableZoomView);
  WRITE_ATTRIBUTE(mOnlyPlaneInZoomView);
  WRITE_ATTRIBUTE(mSmokeOnlyInMainView);
  WRITE_DOUBLE_ATTRIBUTE(mZoomViewSize);
  WRITE_ATTRIBUTE(mEnableStereoscopy);
  WRITE_DOUBLE_ATTRIBUTE(mStereoSeparation);
  WRITE_DOUBLE_ATTRIBUTE(mWindArrowSize);
  WRITE_ATTRIBUTE(mFreeFlightDisplayTime);
  WRITE_ATTRIBUTE(mFreeFlightDisplaySpeed);
  WRITE_ATTRIBUTE(mFreeFlightDisplayAirSpeed);
  WRITE_ATTRIBUTE(mFreeFlightDisplayMaxSpeed);
  WRITE_ATTRIBUTE(mFreeFlightDisplayAscentRate);
  WRITE_ATTRIBUTE(mFreeFlightDisplayAltitude);
  WRITE_ATTRIBUTE(mFreeFlightDisplayDistance);
  WRITE_ATTRIBUTE(mFreeFlightColourText);
  WRITE_ATTRIBUTE(mFreeFlightTextAtTop);
  WRITE_DOUBLE_ATTRIBUTE(mFreeFlightTextBackgroundOpacity);
  WRITE_DOUBLE_ATTRIBUTE(mFreeFlightTextBackgroundColour);
  WRITE_ATTRIBUTE(mFreeFlightUnits);
  WRITE_ATTRIBUTE(mFreeFlightMaxAI);
  WRITE_ATTRIBUTE(mDisplayFPS);
  WRITE_DOUBLE_ATTRIBUTE(mRaceVibrationAmount);
  WRITE_DOUBLE_ATTRIBUTE(mLimboDifficultyMultiplier);
  WRITE_DOUBLE_ATTRIBUTE(mRaceBeepVolume);
  WRITE_ATTRIBUTE(mEnableScoreloop);
  WRITE_ATTRIBUTE(mWindsockOpacity);
  WRITE_DOUBLE_ATTRIBUTE(mPauseButtonsSize);
  WRITE_ATTRIBUTE(mPauseButtonOpacity);
  WRITE_ATTRIBUTE(mControllerMode); 
  WRITE_ATTRIBUTE(mControllerBrakesForward); 
  WRITE_DOUBLE_ATTRIBUTE(mControllerSize); 
  WRITE_ATTRIBUTE(mControllerUseAbsolutePosition);
  WRITE_ATTRIBUTE(mControllerSquare);
  WRITE_DOUBLE_ATTRIBUTE(mControllerHorOffset);
  WRITE_DOUBLE_ATTRIBUTE(mControllerVerOffset);
  WRITE_ATTRIBUTE(mJoystickID);
  WRITE_ATTRIBUTE(mControllerStaggered);
  WRITE_ATTRIBUTE(mControllerStyle);
  WRITE_ATTRIBUTE(mControllerAlpha);
  WRITE_ATTRIBUTE(mControllerStickAlpha);
  WRITE_ATTRIBUTE(mControllerStickCross);
  WRITE_ATTRIBUTE(mControllerEnableTrim);
  WRITE_DOUBLE_ATTRIBUTE(mControllerTrimSize);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewUpdateTerrainLOD);
  WRITE_DOUBLE_ATTRIBUTE(mGroundViewTerrainLOD);
  WRITE_DOUBLE_ATTRIBUTE(mAeroplaneViewTerrainLOD);
  WRITE_ATTRIBUTE(m16BitTextures);
  WRITE_ATTRIBUTE(mFreeFlyMode);
  WRITE_ATTRIBUTE(mFreeFlyOnStartup);
  WRITE_ATTRIBUTE(mEnableSocketController);
  WRITE_ATTRIBUTE(mSetWindDirectionOnWalkabout);
  WRITE_ATTRIBUTE(mUseBackButtonToExit);
  WRITE_ATTRIBUTE(mEnableWalkabout);
  WRITE_ATTRIBUTE(mEnableObjectEditing);
  WRITE_ATTRIBUTE(mUseAeroplanePreferredController);
  WRITE_ATTRIBUTE(mTerrainWireframe);
  WRITE_ATTRIBUTE(mDrawAeroplaneCoM);
  WRITE_ATTRIBUTE(mDrawSunPosition);
  WRITE_ATTRIBUTE(mControlledPlaneShadows);
  WRITE_ATTRIBUTE(mOtherShadows);
  WRITE_ATTRIBUTE(mProjectedShadowDetail);
  WRITE_ATTRIBUTE(mDrawLaunchMarker);
  WRITE_ATTRIBUTE(mDrawGroundPosition);
  WRITE_ATTRIBUTE(mSkyGridOverlay);
  WRITE_ATTRIBUTE(mSkyGridAlignment);
  WRITE_DOUBLE_ATTRIBUTE(mSkyGridDistance);
  WRITE_ATTRIBUTE(mMaxMarkersPerThermal);
  WRITE_ATTRIBUTE(mDrawThermalWindField);
  WRITE_DOUBLE_ATTRIBUTE(mGraphDuration);
  WRITE_DOUBLE_ATTRIBUTE(mGraphFPS);
  WRITE_DOUBLE_ATTRIBUTE(mGraphAltitude);
  WRITE_DOUBLE_ATTRIBUTE(mGraphAirSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mGraphGroundSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mGraphClimbRate);
  WRITE_DOUBLE_ATTRIBUTE(mGraphWindSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mGraphWindVerticalVelocity);
  WRITE_ATTRIBUTE(mNumWindStreamers);
  WRITE_DOUBLE_ATTRIBUTE(mWindStreamerTime);
  WRITE_DOUBLE_ATTRIBUTE(mWindStreamerDeltaZ);
  WRITE_ATTRIBUTE(mStallMarkers);
  WRITE_DOUBLE_ATTRIBUTE(mTimeScale);
  WRITE_ATTRIBUTE(mAerofoilInfo);
  WRITE_DOUBLE_ATTRIBUTE(mAerofoilPlotAngleRange);
  WRITE_DOUBLE_ATTRIBUTE(mAerofoilPlotCLMax);
  WRITE_DOUBLE_ATTRIBUTE(mAerofoilPlotCDMax);
  WRITE_DOUBLE_ATTRIBUTE(mAerofoilPlotCMMax);
  WRITE_DOUBLE_ATTRIBUTE(mAerofoilPlotLDMax);
  WRITE_ATTRIBUTE(mAerofoilPlotReference);
  WRITE_ATTRIBUTE(mAerofoilPlotPolar);
  WRITE_ATTRIBUTE(mRenderPreference);
  WRITE_DOUBLE_ATTRIBUTE(mAmbientLightingScale);
  WRITE_DOUBLE_ATTRIBUTE(mDiffuseLightingScale);
  WRITE_ATTRIBUTE(mBasicTextureDetail); 
  WRITE_ATTRIBUTE(mMaxSkyboxDetail);
  WRITE_ATTRIBUTE(mEnableSmoke);
  WRITE_DOUBLE_ATTRIBUTE(mSmokeQuality);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void Options::Upgrade(TiXmlDocument& doc)
{
  if (mVersion < 3)
  {
    mGroundViewLag *= 0.5f;
  }
  mVersion = mCurrentVersion;
}
//---------------------------------------------------------------------------------------------------------------------
bool Options::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "Options" ).ToElement();
  if (!element)
    return false;

  *this = Options();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mFrameworkSettings.mPhysicsSubsteps);
  READ_ATTRIBUTE(mFrameworkSettings.mUseMultiLights);
  READ_ATTRIBUTE(mMaxNearClipDistance);
  READ_ATTRIBUTE(mSeparateSpecular);
  READ_ATTRIBUTE(mFrameworkSettings.mFarClipPlaneDistance);
  READ_ENUM_ATTRIBUTE(mLanguage);
  READ_ATTRIBUTE(mVolumeScale);
  READ_ATTRIBUTE(mVariometerVolume);
  READ_ATTRIBUTE(mWindVolume);
  READ_ATTRIBUTE(mOutsideAeroplaneVolume);
  READ_ATTRIBUTE(mInsideAeroplaneVolume);
  READ_ATTRIBUTE(mGroundViewAutoZoom);
  READ_ATTRIBUTE(mGroundViewAutoZoomScale);
  READ_ATTRIBUTE(mGroundViewFieldOfView);
  READ_ATTRIBUTE(mGroundViewHorizonAmount);
  READ_ATTRIBUTE(mGroundViewLag);
  READ_ATTRIBUTE(mGroundViewYawOffset);
  READ_ATTRIBUTE(mGroundViewPitchOffset);
  READ_ATTRIBUTE(mGroundViewFollow);
  READ_ATTRIBUTE(mAeroplaneViewFieldOfView);
  READ_ATTRIBUTE(mEnableZoomView);
  READ_ATTRIBUTE(mOnlyPlaneInZoomView);
  READ_ATTRIBUTE(mSmokeOnlyInMainView);
  READ_ATTRIBUTE(mZoomViewSize);
  READ_ATTRIBUTE(mStereoSeparation);
  READ_ATTRIBUTE(mWindArrowSize);
  READ_ATTRIBUTE(mFreeFlightDisplayTime);
  READ_ATTRIBUTE(mFreeFlightDisplaySpeed);
  READ_ATTRIBUTE(mFreeFlightDisplayAirSpeed);
  READ_ATTRIBUTE(mFreeFlightDisplayMaxSpeed);
  READ_ATTRIBUTE(mFreeFlightDisplayAscentRate);
  READ_ATTRIBUTE(mFreeFlightDisplayAltitude);
  READ_ATTRIBUTE(mFreeFlightDisplayDistance);
  READ_ATTRIBUTE(mFreeFlightColourText);
  READ_ATTRIBUTE(mFreeFlightTextAtTop);
  READ_ATTRIBUTE(mFreeFlightTextBackgroundOpacity);
  READ_ATTRIBUTE(mFreeFlightTextBackgroundColour);
  READ_ENUM_ATTRIBUTE(mFreeFlightUnits);
  READ_ATTRIBUTE(mFreeFlightMaxAI);
  READ_ATTRIBUTE(mDisplayFPS);
  READ_ATTRIBUTE(mRaceVibrationAmount);
  READ_ATTRIBUTE(mLimboDifficultyMultiplier);
  READ_ATTRIBUTE(mRaceBeepVolume);
  READ_ATTRIBUTE(mEnableScoreloop);
  READ_ATTRIBUTE(mWindsockOpacity);
  READ_ATTRIBUTE(mPauseButtonsSize);
  READ_ATTRIBUTE(mPauseButtonOpacity);
  READ_ATTRIBUTE(mControllerMode);
  READ_ATTRIBUTE(mControllerBrakesForward);
  READ_ATTRIBUTE(mControllerSize);
  READ_ATTRIBUTE(mControllerUseAbsolutePosition);
  READ_ATTRIBUTE(mControllerSquare);
  READ_ATTRIBUTE(mControllerHorOffset);
  READ_ATTRIBUTE(mControllerVerOffset);
  READ_ATTRIBUTE(mJoystickID);
  READ_ATTRIBUTE(mControllerStaggered);
  READ_ENUM_ATTRIBUTE(mControllerStyle);
  READ_ATTRIBUTE(mControllerAlpha);
  READ_ATTRIBUTE(mControllerStickAlpha);
  READ_ATTRIBUTE(mControllerStickCross);
  READ_ATTRIBUTE(mControllerEnableTrim);
  READ_ATTRIBUTE(mControllerTrimSize);
  READ_ATTRIBUTE(mGroundViewTerrainLOD);
  READ_ATTRIBUTE(mAeroplaneViewTerrainLOD);
  READ_ATTRIBUTE(mFreeFlyOnStartup);
  READ_ATTRIBUTE(mEnableSocketController);
  READ_ATTRIBUTE(mSetWindDirectionOnWalkabout);
  READ_ATTRIBUTE(m16BitTextures);
  READ_ENUM_ATTRIBUTE(mFreeFlyMode);
  READ_ATTRIBUTE(mUseBackButtonToExit);
  READ_ATTRIBUTE(mEnableWalkabout);
  READ_ATTRIBUTE(mUseAeroplanePreferredController);
  READ_ENUM_ATTRIBUTE(mControlledPlaneShadows);
  READ_ENUM_ATTRIBUTE(mOtherShadows);
  READ_ATTRIBUTE(mProjectedShadowDetail);
  READ_ATTRIBUTE(mDrawLaunchMarker);
  READ_ATTRIBUTE(mDrawGroundPosition);
  READ_ENUM_ATTRIBUTE(mSkyGridOverlay);
  READ_ENUM_ATTRIBUTE(mSkyGridAlignment);
  READ_ATTRIBUTE(mSkyGridDistance);
  READ_ATTRIBUTE(mMaxMarkersPerThermal);
  READ_ATTRIBUTE(mDrawThermalWindField);
  READ_ATTRIBUTE(mGraphDuration);
  READ_ATTRIBUTE(mWindStreamerTime);
  READ_ATTRIBUTE(mWindStreamerDeltaZ);
  READ_ATTRIBUTE(mAerofoilPlotAngleRange);
  READ_ATTRIBUTE(mAerofoilPlotCLMax);
  READ_ATTRIBUTE(mAerofoilPlotCDMax);
  READ_ATTRIBUTE(mAerofoilPlotCMMax);
  READ_ATTRIBUTE(mAerofoilPlotLDMax);
  READ_ATTRIBUTE(mAerofoilPlotReference);
  READ_ATTRIBUTE(mAerofoilPlotPolar);
  READ_ATTRIBUTE(mAmbientLightingScale);
  READ_ATTRIBUTE(mDiffuseLightingScale);
  READ_ATTRIBUTE(mBasicTextureDetail);
  READ_ATTRIBUTE(mMaxSkyboxDetail);
  READ_ATTRIBUTE(mEnableSmoke);
  READ_ATTRIBUTE(mSmokeQuality);

  if (readAll)
  {
    READ_ATTRIBUTE(mEnableStereoscopy);
    READ_ATTRIBUTE(mStallMarkers);
    READ_ATTRIBUTE(mEnableObjectEditing);
    READ_ATTRIBUTE(mTerrainWireframe);
    READ_ENUM_ATTRIBUTE(mDrawAeroplaneCoM);
    READ_ATTRIBUTE(mDrawSunPosition);
    READ_ATTRIBUTE(mGraphFPS);
    READ_ATTRIBUTE(mGraphAltitude);
    READ_ATTRIBUTE(mGraphAirSpeed);
    READ_ATTRIBUTE(mGraphGroundSpeed);
    READ_ATTRIBUTE(mGraphClimbRate);
    READ_ATTRIBUTE(mGraphWindSpeed);
    READ_ATTRIBUTE(mGraphWindVerticalVelocity);
    READ_ATTRIBUTE(mNumWindStreamers);
    READ_ATTRIBUTE(mTimeScale);
    READ_ATTRIBUTE(mAerofoilInfo);
    READ_ENUM_ATTRIBUTE(mRenderPreference);
  }

#ifndef ENABLE_SCORELOOP
  mEnableScoreloop = false;
#endif

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions Options::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, Options& settings)
{
  COMPARE_FOR_TERRAIN(mProjectedShadowDetail);
  COMPARE_FOR_TERRAIN(m16BitTextures);
  COMPARE_FOR_TERRAIN(mAmbientLightingScale);
  COMPARE_FOR_TERRAIN(mDiffuseLightingScale);
  COMPARE_FOR_TERRAIN(mBasicTextureDetail);
  COMPARE_FOR_TERRAIN(mMaxSkyboxDetail);
  COMPARE_FOR_CHALLENGE(mLimboDifficultyMultiplier);
  COMPARE_FOR_AIRELOAD(mFreeFlightMaxAI);
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
TerrainSettings::TerrainSettings() :
  mVersion(mCurrentVersion),
  mType(TYPE_MIDPOINT_DISPLACEMENT),
  mRenderPlain(true),
  mCollideWithPlain(false),
  mPlainInnerRadius(5000.0f),
  mPlainFogDistance(20000.0f),
  mPlainHeight(25.0f),
  mHeightmapDetail(8),
  mTerrainSize(2560.0f),
  mCoastEnhancement(4.0f),
  mSimplifyUnderPlain(true),
  mSurfaceRoughness(0.0f),
  mFriction(1.0f),
  mTerrainTextureScaleX(2.0f),
  mTerrainTextureScaleY(2.0f),
  mTerrainDetailTextureScaleX(60.0f),
  mTerrainDetailTextureScaleY(60.0f),
  mPlainDetailTextureScaleX(20.0f),
  mPlainDetailTextureScaleY(60.0f),
  mBeachColourR(151/255.0f),
  mBeachColourG(122/255.0f),
  mBeachColourB(88/255.0f),
  mPlainColourR(112/255.0f),
  mPlainColourG(153/255.0f),
  mPlainColourB(204/255.0f),
  mBasicTexture("SystemData/Textures/TerrainTexture.jpg"),
  mDetailTexture("SystemData/Textures/CloudTexture.jpg"),
  // midpoint
  mMidpointDisplacementHeight(90.0f),
  mMidpointDisplacementRoughness(0.95f),
  mMidpointDisplacementEdgeHeight(-30.0f),
  mMidpointDisplacementUpwardsBias(0.4f),
  mMidpointDisplacementFilterIterations(1),
  mMidpointDisplacementSeed(300),
  // ridge
  mRidgeHeight(100.0f),
  mRidgeMaxHeightFraction(0.6f),
  mRidgeWidth(100.0f),
  mRidgeEdgeHeight(20.0f),
  mRidgeHorizontalVariation(10.0f),
  mRidgeHorizontalVariationWavelength(500.0f),
  mRidgeVerticalVariationFraction(0.2f),
  // panorama
  mPanoramaName("SystemData/Panoramas/Slope1"),
  // File terrain
  mFileTerrainName("SystemData/FileTerrains/PicaSim"),
  mFileTerrainMinZ(-46.0f),
  mFileTerrainMaxZ(133.0f)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool TerrainSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "TerrainSettings" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mType); 
  WRITE_ATTRIBUTE(mRenderPlain); 
  WRITE_ATTRIBUTE(mCollideWithPlain); 
  WRITE_DOUBLE_ATTRIBUTE(mPlainInnerRadius); 
  WRITE_DOUBLE_ATTRIBUTE(mPlainFogDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mPlainHeight); 
  WRITE_ATTRIBUTE(mHeightmapDetail); 
  WRITE_DOUBLE_ATTRIBUTE(mTerrainSize); 
  WRITE_DOUBLE_ATTRIBUTE(mCoastEnhancement); 
  WRITE_ATTRIBUTE(mSimplifyUnderPlain);
  WRITE_DOUBLE_ATTRIBUTE(mSurfaceRoughness);
  WRITE_DOUBLE_ATTRIBUTE(mFriction);
  WRITE_DOUBLE_ATTRIBUTE(mTerrainTextureScaleX);
  WRITE_DOUBLE_ATTRIBUTE(mTerrainTextureScaleY);
  WRITE_DOUBLE_ATTRIBUTE(mTerrainDetailTextureScaleX);
  WRITE_DOUBLE_ATTRIBUTE(mTerrainDetailTextureScaleY);
  WRITE_DOUBLE_ATTRIBUTE(mPlainDetailTextureScaleX);
  WRITE_DOUBLE_ATTRIBUTE(mPlainDetailTextureScaleY);
  WRITE_DOUBLE_ATTRIBUTE(mBeachColourR);
  WRITE_DOUBLE_ATTRIBUTE(mBeachColourG);
  WRITE_DOUBLE_ATTRIBUTE(mBeachColourB);
  WRITE_DOUBLE_ATTRIBUTE(mPlainColourR);
  WRITE_DOUBLE_ATTRIBUTE(mPlainColourG);
  WRITE_DOUBLE_ATTRIBUTE(mPlainColourB);
  WRITE_ATTRIBUTE(mBasicTexture); 
  WRITE_ATTRIBUTE(mDetailTexture); 
  // Midpoint
  WRITE_DOUBLE_ATTRIBUTE(mMidpointDisplacementHeight); 
  WRITE_DOUBLE_ATTRIBUTE(mMidpointDisplacementRoughness); 
  WRITE_DOUBLE_ATTRIBUTE(mMidpointDisplacementEdgeHeight); 
  WRITE_DOUBLE_ATTRIBUTE(mMidpointDisplacementUpwardsBias); 
  WRITE_ATTRIBUTE(mMidpointDisplacementFilterIterations); 
  WRITE_ATTRIBUTE(mMidpointDisplacementSeed); 
  // ridge
  WRITE_DOUBLE_ATTRIBUTE(mRidgeHeight);
  WRITE_DOUBLE_ATTRIBUTE(mRidgeMaxHeightFraction);
  WRITE_DOUBLE_ATTRIBUTE(mRidgeWidth);
  WRITE_DOUBLE_ATTRIBUTE(mRidgeEdgeHeight);
  WRITE_DOUBLE_ATTRIBUTE(mRidgeHorizontalVariation);
  WRITE_DOUBLE_ATTRIBUTE(mRidgeHorizontalVariationWavelength);
  WRITE_DOUBLE_ATTRIBUTE(mRidgeVerticalVariationFraction);
  // panorama
  WRITE_ATTRIBUTE(mPanoramaName);
  // Image
  WRITE_ATTRIBUTE(mFileTerrainName);
  WRITE_DOUBLE_ATTRIBUTE(mFileTerrainMinZ);
  WRITE_DOUBLE_ATTRIBUTE(mFileTerrainMaxZ);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void TerrainSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool TerrainSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "TerrainSettings" ).ToElement();
  if (!element)
    return false;

  *this = TerrainSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ENUM_ATTRIBUTE(mType);
  READ_ATTRIBUTE(mRenderPlain);
  READ_ATTRIBUTE(mCollideWithPlain);
  READ_ATTRIBUTE(mPlainInnerRadius);
  READ_ATTRIBUTE(mPlainFogDistance);
  READ_ATTRIBUTE(mPlainHeight);
  READ_ATTRIBUTE(mHeightmapDetail);
  if (mHeightmapDetail > 9)
    mHeightmapDetail = 9;
  READ_ATTRIBUTE(mTerrainSize);
  READ_ATTRIBUTE(mCoastEnhancement);
  READ_ATTRIBUTE(mSimplifyUnderPlain);
  READ_ATTRIBUTE(mSurfaceRoughness);
  READ_ATTRIBUTE(mFriction);
  READ_ATTRIBUTE(mTerrainTextureScaleX);
  READ_ATTRIBUTE(mTerrainTextureScaleY);
  READ_ATTRIBUTE(mTerrainDetailTextureScaleX);
  READ_ATTRIBUTE(mTerrainDetailTextureScaleY);
  READ_ATTRIBUTE(mPlainDetailTextureScaleX);
  READ_ATTRIBUTE(mPlainDetailTextureScaleY);
  READ_ATTRIBUTE(mBeachColourR);
  READ_ATTRIBUTE(mBeachColourG);
  READ_ATTRIBUTE(mBeachColourB);
  READ_ATTRIBUTE(mPlainColourR);
  READ_ATTRIBUTE(mPlainColourG);
  READ_ATTRIBUTE(mPlainColourB);
  READ_ATTRIBUTE(mBasicTexture);
  READ_ATTRIBUTE(mDetailTexture);
  // midpoint
  READ_ATTRIBUTE(mMidpointDisplacementHeight);
  READ_ATTRIBUTE(mMidpointDisplacementRoughness);
  READ_ATTRIBUTE(mMidpointDisplacementEdgeHeight);
  READ_ATTRIBUTE(mMidpointDisplacementUpwardsBias);
  READ_ATTRIBUTE(mMidpointDisplacementFilterIterations);
  READ_ATTRIBUTE(mMidpointDisplacementSeed);
  // ridge
  READ_ATTRIBUTE(mRidgeHeight);
  READ_ATTRIBUTE(mRidgeMaxHeightFraction);
  READ_ATTRIBUTE(mRidgeWidth);
  READ_ATTRIBUTE(mRidgeEdgeHeight);
  READ_ATTRIBUTE(mRidgeHorizontalVariation);
  READ_ATTRIBUTE(mRidgeHorizontalVariationWavelength);
  READ_ATTRIBUTE(mRidgeVerticalVariationFraction);
  // panorama
  READ_ATTRIBUTE(mPanoramaName);
  // Image
  READ_ATTRIBUTE(mFileTerrainName);
  READ_ATTRIBUTE(mFileTerrainMinZ);
  READ_ATTRIBUTE(mFileTerrainMaxZ);

  Upgrade(doc);

  return true;
}


//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions TerrainSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, TerrainSettings& settings)
{
  COMPARE_FOR_TERRAIN(mType);
  COMPARE_FOR_TERRAIN(mPlainHeight);
  COMPARE_FOR_TERRAIN(mCollideWithPlain);
  COMPARE_FOR_TERRAIN(mPlainInnerRadius);
  COMPARE_FOR_TERRAIN(mPlainFogDistance);
  COMPARE_FOR_TERRAIN(mHeightmapDetail);
  COMPARE_FOR_TERRAIN(mTerrainSize);
  COMPARE_FOR_TERRAIN(mCoastEnhancement);
  COMPARE_FOR_TERRAIN(mSimplifyUnderPlain);
  COMPARE_FOR_TERRAIN(mFriction);
  COMPARE_FOR_TERRAIN(mMidpointDisplacementHeight);
  COMPARE_FOR_TERRAIN(mMidpointDisplacementRoughness);
  COMPARE_FOR_TERRAIN(mMidpointDisplacementEdgeHeight);
  COMPARE_FOR_TERRAIN(mMidpointDisplacementUpwardsBias);
  COMPARE_FOR_TERRAIN(mMidpointDisplacementFilterIterations);
  COMPARE_FOR_TERRAIN(mMidpointDisplacementSeed);
  COMPARE_FOR_TERRAIN(mRidgeHeight);
  COMPARE_FOR_TERRAIN(mRidgeMaxHeightFraction);
  COMPARE_FOR_TERRAIN(mRidgeWidth);
  COMPARE_FOR_TERRAIN(mRidgeEdgeHeight);
  COMPARE_FOR_TERRAIN(mRidgeHorizontalVariation);
  COMPARE_FOR_TERRAIN(mRidgeHorizontalVariationWavelength);
  COMPARE_FOR_TERRAIN(mRidgeVerticalVariationFraction);
  COMPARE_FOR_TERRAIN(mPanoramaName);
  COMPARE_FOR_TERRAIN(mFileTerrainName);
  COMPARE_FOR_TERRAIN(mFileTerrainMinZ);
  COMPARE_FOR_TERRAIN(mFileTerrainMaxZ);
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
AIEnvironmentSettings::AIEnvironmentSettings() : 
  mVersion(mCurrentVersion),
  mSceneType(SCENETYPE_SLOPE)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool AIEnvironmentSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "AIEnvironmentSettings" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mSceneType);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void AIEnvironmentSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool AIEnvironmentSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "AIEnvironmentSettings" ).ToElement();
  if (!element)
    return false;

  *this = AIEnvironmentSettings();

  READ_ENUM_ATTRIBUTE(mSceneType);
  Upgrade(doc);
  return true;
}


//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions AIEnvironmentSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AIEnvironmentSettings& settings)
{
  COMPARE_FOR_AIRESET(mSceneType);
  return settingsChangeActions;
}


//---------------------------------------------------------------------------------------------------------------------
// Constructor will create if create is set
ObjectsSettings::Box::Box(const Vector3& extents, const Transform& tm, const Vector3& colour, const char* textureFile, float mass, bool visible, bool enableShadows, bool create)
{
  mExtents = extents;
  mTM = tm;
  mColour = colour;
  mTextureFile = textureFile ? textureFile : "";
  mMass = mass;
  mVisible = visible;
  mShadow = enableShadows;
  mObject = 0;

  if (create)
    Create();
}

//---------------------------------------------------------------------------------------------------------------------
void ObjectsSettings::Box::Create()
{
  if (mObject)
    delete mObject;
  mObject = new BoxObject(mExtents, mTM, mColour, mTextureFile.size() > 0 ? mTextureFile.c_str() : 0, mMass, true, true, mVisible, mShadow);
}

//---------------------------------------------------------------------------------------------------------------------
ObjectsSettings::Box::Box()
{
  mExtents = Vector3::g_Zero;
  mTM = Transform::g_Identity;
  mColour = Vector3(1,1,1) * 0.5f;
  mTextureFile = "";
  mMass = 0.0f;
  mVisible = false;
  mShadow = false;
  mObject = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void ObjectsSettings::Box::SetExtents(const Vector3& extents)
{
  mExtents = extents;
  if (mObject)
    mObject->SetExtents(extents);
}

//---------------------------------------------------------------------------------------------------------------------
void ObjectsSettings::Box::SetColour(const Vector3& colour)
{
  mColour = colour;
  if (mObject)
    mObject->SetColour(colour);
}

//---------------------------------------------------------------------------------------------------------------------
void ObjectsSettings::Box::SetInitialTM(const Transform& tm)
{
  mTM = tm;
  if (mObject)
    mObject->SetInitialTM(tm);
}

//---------------------------------------------------------------------------------------------------------------------
ObjectsSettings::ObjectsSettings() : 
  mVersion(mCurrentVersion),
  mResetCounter(0),
  mForceAllVisible(false)
{
  TRACE_METHOD_ONLY(1);
}

//---------------------------------------------------------------------------------------------------------------------
ObjectsSettings::~ObjectsSettings()
{
  TRACE_METHOD_ONLY(1);
}

//---------------------------------------------------------------------------------------------------------------------
void ObjectsSettings::GetStats(size_t& numObjects, size_t& numStaticVisible, size_t& numStaticInvisible, size_t& numDynamicVisible) const
{
  numObjects = mBoxes.size();
  numStaticInvisible = numStaticVisible = numDynamicVisible = 0;
  for (size_t i = 0 ; i != numObjects ; ++i)
  {
    const Box& box = mBoxes[i];
    if (box.mMass > 0.0f && box.mVisible)
      ++numDynamicVisible;
    if (box.mMass == 0.0f && box.mVisible)
      ++numStaticVisible;
    if (box.mMass == 0.0f && !box.mVisible)
      ++numStaticInvisible;
  }
}


//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions ObjectsSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, ObjectsSettings& settings) const
{
  TRACE_METHOD_ONLY(1);
  if (mBoxes.size() != settings.mBoxes.size())
  {
    settingsChangeActions.mReloadTerrain = true;
  }
  else
  {
    for (size_t i = 0 ; i != mBoxes.size() ; ++i)
    {
      COMPARE_FOR_TERRAIN(mBoxes[i].mTM);
      COMPARE_FOR_TERRAIN(mBoxes[i].mMass);
      COMPARE_FOR_TERRAIN(mBoxes[i].mShadow);
      COMPARE_FOR_TERRAIN(mBoxes[i].mExtents);
      COMPARE_FOR_TERRAIN(mBoxes[i].mColour);
      COMPARE_FOR_TERRAIN(mBoxes[i].mTextureFile);
      COMPARE_FOR_TERRAIN(mBoxes[i].mObject);
    }
  }
  COMPARE_FOR_TERRAIN(mResetCounter);
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
bool ObjectsSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TRACE_METHOD_ONLY(1);
  TiXmlElement* element = new TiXmlElement( "Objects" );
  doc.LinkEndChild( element );

  WRITE_ATTRIBUTE(mVersion);
  size_t numBoxes = mBoxes.size();
  WRITE_ATTRIBUTE(numBoxes); 
  WRITE_ATTRIBUTE(mForceAllVisible); 

  for (size_t i = 0 ; i != numBoxes ; ++i)
  {
    TiXmlElement* boxElement = new TiXmlElement( "Box" );
    doc.LinkEndChild(boxElement);
    const Box& box = mBoxes[i];
    writeToXML(i, boxElement, "ID");
    writeFloatToXML(box.mMass, boxElement, "mMass");
    writeToXML(box.mVisible, boxElement, "mVisible");
    writeToXML(box.mShadow, boxElement, "mShadow");
    writeTransformToXML(box.mTM, boxElement, "mTM");
    writeVector3ToXML(box.mExtents, boxElement, "mExtents");
    writeVector3ToXML(box.mColour, boxElement, "mColour");
    writeStringToXML(box.mTextureFile, boxElement, "mTextureFile");
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void ObjectsSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool ObjectsSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "Objects" ).ToElement();
  if (!element)
    return false;

  *this = ObjectsSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mForceAllVisible);
  if (mVersion < 3)
  {
    size_t numBoxes = 0;
    READ_ATTRIBUTE(numBoxes);
    mBoxes.resize(numBoxes);

    for (size_t i = 0 ; i != numBoxes ; ++i)
    {
      Box& box = mBoxes[i];
      box.mTM = readTransformFromXML(element, "mTM", &i);
      box.mMass = readFloatFromXML(element, "mMass", &i);
      box.mShadow = readBoolFromXML(element, "mShadow", &i);
      box.mVisible = readBoolFromXML(element, "mVisible", &i);
      box.mExtents = readVector3FromXML(element, "mExtents", &i);
      box.mColour = readVector3FromXML(element, "mColour", &i);
      box.mTextureFile = readStringFromXML(element, "mTextureFile", &i);
      box.mObject = 0;
    }
  }
  else
  {
    for (int iBox = 0 ; ; ++iBox)
    {
      TiXmlElement* boxElement = docHandle.Child("Box", iBox).ToElement();
      if (!boxElement)
        break;
      Box box;
      box.mTM = readTransformFromXML(boxElement, "mTM");
      box.mMass = readFloatFromXML(boxElement, "mMass");
      box.mShadow = readBoolFromXML(boxElement, "mShadow");
      box.mVisible = readBoolFromXML(boxElement, "mVisible");
      box.mExtents = readVector3FromXML(boxElement, "mExtents");
      box.mColour = readVector3FromXML(boxElement, "mColour");
      box.mTextureFile = readStringFromXML(boxElement, "mTextureFile");
      box.mObject = 0;
      mBoxes.push_back(box);
    }
  }

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
EnvironmentSettings::EnvironmentSettings() : 
  mVersion(mCurrentVersion),
  mAvailability(0),
  mType(9),
  mTitle("Hills"),
  mInfo("A hilly island - lots of lift on the steep slopes."),
  mWWW(""),
  mThumbnail("SystemSettings/Thumbnails/Hills.jpg"),
  mObjectsSettingsFile("SystemSettings/Objects/None.xml"),
  mObserverPosition(-209.0,264.5,2.0f),
  mWindSpeed(4.0f),
  mWindBearing(-90.0f),
  mWindLiftSmoothing(4.0f),
  mVerticalWindDecayDistance(50.0f),
  mWindGustTime(60.0f),
  mWindGustAmplitudeFraction(0.0f),
  mWindGustBearingAmplitude(0.0f),
  mSeparationTendency(1.0f),
  mRotorTendency(1.0f),
  mTurbulenceAmount(0.0f),
  mSurfaceTurbulence(1.0f),
  mShearTurbulence(1.0f),
  mBoundaryLayerDepth(100.0f),
  mDeadAirTurbulence(1.0f),
  mAllowBungeeLaunch(false),
  mThermalDensity(10.0f),
  mThermalRange(1000.0f),
  mThermalAverageLifeSpan(300.0f),
  mThermalAverageDepth(50.0f),
  mThermalAverageCoreRadius(40.0f),
  mThermalAverageDowndraftRadius(80.0f),
  mThermalAverageUpdraftSpeed(3.5f),
  mThermalAverageAscentRate(0.4f),
  mThermalExpansionOverLifespan(2.0f),
  mRunwayType(RUNWAY_NONE),
  mRunwayPosition(0.0f, 0.0f, 0.0f),
  mRunwayAngle(0.0f),
  mRunwayLength(40.0f),
  mRunwayWidth(15.0f)
{
  TRACE_METHOD_ONLY(1);
}

//---------------------------------------------------------------------------------------------------------------------
bool EnvironmentSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TRACE_METHOD_ONLY(1);
  TiXmlElement* element = new TiXmlElement( "EnvironmentSettings" );
  doc.LinkEndChild( element );

  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mAvailability);
  writeToXML(mType | 16, element, "mType"); // flag as user setting
  WRITE_ATTRIBUTE(mThumbnail);
  WRITE_ATTRIBUTE(mTitle);
  WRITE_ATTRIBUTE(mInfo);
  WRITE_ATTRIBUTE(mWWW);

  WRITE_ATTRIBUTE(mObjectsSettingsFile);

  WRITE_DOUBLE_ATTRIBUTE(mWindSpeed); 
  WRITE_DOUBLE_ATTRIBUTE(mObserverPosition.x);
  WRITE_DOUBLE_ATTRIBUTE(mObserverPosition.y);
  WRITE_DOUBLE_ATTRIBUTE(mObserverPosition.z);
  WRITE_DOUBLE_ATTRIBUTE(mWindSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mWindBearing);
  WRITE_DOUBLE_ATTRIBUTE(mWindLiftSmoothing);
  WRITE_DOUBLE_ATTRIBUTE(mVerticalWindDecayDistance);
  WRITE_DOUBLE_ATTRIBUTE(mWindGustTime);
  WRITE_DOUBLE_ATTRIBUTE(mWindGustAmplitudeFraction);
  WRITE_DOUBLE_ATTRIBUTE(mWindGustBearingAmplitude);
  WRITE_DOUBLE_ATTRIBUTE(mSeparationTendency);
  WRITE_DOUBLE_ATTRIBUTE(mRotorTendency);
  WRITE_DOUBLE_ATTRIBUTE(mTurbulenceAmount);
  WRITE_DOUBLE_ATTRIBUTE(mSurfaceTurbulence);
  WRITE_DOUBLE_ATTRIBUTE(mShearTurbulence);
  WRITE_DOUBLE_ATTRIBUTE(mBoundaryLayerDepth);
  WRITE_DOUBLE_ATTRIBUTE(mDeadAirTurbulence);

  WRITE_ATTRIBUTE(mAllowBungeeLaunch);

  WRITE_DOUBLE_ATTRIBUTE(mThermalDensity);
  WRITE_DOUBLE_ATTRIBUTE(mThermalRange);
  WRITE_DOUBLE_ATTRIBUTE(mThermalAverageLifeSpan);
  WRITE_DOUBLE_ATTRIBUTE(mThermalAverageDepth);
  WRITE_DOUBLE_ATTRIBUTE(mThermalAverageCoreRadius);
  WRITE_DOUBLE_ATTRIBUTE(mThermalAverageDowndraftRadius);
  WRITE_DOUBLE_ATTRIBUTE(mThermalAverageUpdraftSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mThermalAverageAscentRate);
  WRITE_DOUBLE_ATTRIBUTE(mThermalExpansionOverLifespan);

  WRITE_ATTRIBUTE(mRunwayType);
  WRITE_DOUBLE_ATTRIBUTE(mRunwayPosition.x);
  WRITE_DOUBLE_ATTRIBUTE(mRunwayPosition.y);
  WRITE_DOUBLE_ATTRIBUTE(mRunwayPosition.z);
  WRITE_DOUBLE_ATTRIBUTE(mRunwayAngle);
  WRITE_DOUBLE_ATTRIBUTE(mRunwayLength);
  WRITE_DOUBLE_ATTRIBUTE(mRunwayWidth);

  mTerrainSettings.WriteToDoc(doc);
  mAIEnvironmentSettings.WriteToDoc(doc);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool EnvironmentSettings::ReadBasicsFromDoc(TiXmlDocument& doc)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "EnvironmentSettings" ).ToElement();
  if (!element)
    return false;

  READ_ATTRIBUTE(mAvailability);
  READ_ATTRIBUTE(mType);
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void EnvironmentSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool EnvironmentSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "EnvironmentSettings" ).ToElement();
  if (!element)
    return false;

  *this = EnvironmentSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mAvailability);
  READ_ATTRIBUTE(mType);
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  READ_ATTRIBUTE(mWWW);

  READ_ATTRIBUTE(mObjectsSettingsFile);

  READ_ATTRIBUTE(mWindSpeed);
  READ_ATTRIBUTE(mObserverPosition.x);
  READ_ATTRIBUTE(mObserverPosition.y);
  READ_ATTRIBUTE(mObserverPosition.z);
  READ_ATTRIBUTE(mWindSpeed);
  READ_ATTRIBUTE(mWindBearing);
  READ_ATTRIBUTE(mWindLiftSmoothing);
  READ_ATTRIBUTE(mVerticalWindDecayDistance);
  READ_ATTRIBUTE(mWindGustTime);
  READ_ATTRIBUTE(mWindGustAmplitudeFraction);
  READ_ATTRIBUTE(mWindGustBearingAmplitude);
  READ_ATTRIBUTE(mSeparationTendency);
  READ_ATTRIBUTE(mRotorTendency);
  READ_ATTRIBUTE(mTurbulenceAmount);
  READ_ATTRIBUTE(mSurfaceTurbulence);
  READ_ATTRIBUTE(mShearTurbulence);
  READ_ATTRIBUTE(mBoundaryLayerDepth);
  READ_ATTRIBUTE(mDeadAirTurbulence);

  READ_ATTRIBUTE(mAllowBungeeLaunch);

  READ_ATTRIBUTE(mThermalDensity);
  READ_ATTRIBUTE(mThermalRange);
  READ_ATTRIBUTE(mThermalAverageLifeSpan);
  READ_ATTRIBUTE(mThermalAverageDepth);
  READ_ATTRIBUTE(mThermalAverageCoreRadius);
  READ_ATTRIBUTE(mThermalAverageDowndraftRadius);
  READ_ATTRIBUTE(mThermalAverageUpdraftSpeed);
  READ_ATTRIBUTE(mThermalAverageAscentRate);
  READ_ATTRIBUTE(mThermalExpansionOverLifespan);

  READ_ENUM_ATTRIBUTE(mRunwayType);
  READ_ATTRIBUTE(mRunwayPosition.x);
  READ_ATTRIBUTE(mRunwayPosition.y);
  READ_ATTRIBUTE(mRunwayPosition.z);
  READ_ATTRIBUTE(mRunwayAngle);
  READ_ATTRIBUTE(mRunwayLength);
  READ_ATTRIBUTE(mRunwayWidth);

  mTerrainSettings.ReadFromDoc(doc, readAll);
  mAIEnvironmentSettings.ReadFromDoc(doc, readAll);

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions EnvironmentSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, EnvironmentSettings& settings)
{
  COMPARE_FOR_WIND(mWindBearing);
  COMPARE_FOR_AIRESET(mWindBearing);
  COMPARE_FOR_WIND(mWindSpeed);
  COMPARE_FOR_WIND(mSeparationTendency);
  COMPARE_FOR_TERRAIN(mRunwayType);
  COMPARE_FOR_TERRAIN(mRunwayPosition);
  COMPARE_FOR_TERRAIN(mRunwayAngle);
  COMPARE_FOR_TERRAIN(mRunwayLength);
  COMPARE_FOR_TERRAIN(mRunwayWidth);
  COMPARE_FOR_TERRAIN(mObjectsSettingsFile);
  settingsChangeActions = mTerrainSettings.GetSettingsChangeActions(settingsChangeActions, settings.mTerrainSettings);
  settingsChangeActions = mAIEnvironmentSettings.GetSettingsChangeActions(settingsChangeActions, settings.mAIEnvironmentSettings);
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
ChallengeSettings::ChallengeSettings() :
  mVersion(mCurrentVersion),
  mTitle("Select race"),
  mInfo("You shouldn't be reading this."),
  mChallengeMode(CHALLENGE_FREEFLY),
  mChallengeID(0),
  mChecksum(0),
  mThermalSeed(0),
  mReferenceTime(60.0f),
  mPreparationTime(60.0f),
  mMaxHeightMultiplier(1.0f),
  mMaxHeightForBonus(20.0f),
  mLimboDuration(60.0f),
  mLimboRequiredAltitude(0.0f),
  mLimboRelaunchPenalty(30.0f),
  mLimboRelaunchWhenStationary(false),
  mWindSpeedOverride(-1.0f),
  mTurbulenceOverride(-1.0f),
  mThermalDensityOverride(-1.0f),
  mAllowAeroplaneSettings(true),
  mAllowEnvironmentSettings(true),
  mAllowWindStrengthSetting(true),
  mDefaultToChaseView(false)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool ChallengeSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "ChallengeSettings" );
  doc.LinkEndChild( element );
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool ChallengeSettings::ReadBasicsFromDoc(TiXmlDocument& doc)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "ChallengeSettings" ).ToElement();
  if (!element)
    return false;
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool ChallengeSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "ChallengeSettings" ).ToElement();
  if (!element)
    return false;
  *this = ChallengeSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  READ_ATTRIBUTE(mThumbnail);
  READ_ENUM_ATTRIBUTE(mChallengeMode);
  READ_ATTRIBUTE(mChallengeID);
  READ_ATTRIBUTE(mThermalSeed);
  READ_ATTRIBUTE(mReferenceTime);
  READ_ATTRIBUTE(mPreparationTime);
  READ_ATTRIBUTE(mMaxHeightMultiplier);
  READ_ATTRIBUTE(mMaxHeightForBonus);
  READ_ATTRIBUTE(mLimboDuration);
  READ_ATTRIBUTE(mLimboRequiredAltitude);
  READ_ATTRIBUTE(mLimboRelaunchPenalty);
  READ_ATTRIBUTE(mLimboRelaunchWhenStationary);
  READ_ATTRIBUTE(mWindSpeedOverride);
  READ_ATTRIBUTE(mTurbulenceOverride);
  READ_ATTRIBUTE(mThermalDensityOverride);
  READ_ATTRIBUTE(mAllowAeroplaneSettings);
  READ_ATTRIBUTE(mAllowEnvironmentSettings);
  READ_ATTRIBUTE(mAllowWindStrengthSetting);
  READ_ATTRIBUTE(mAeroplaneSettingsFile);
  READ_ATTRIBUTE(mEnvironmentSettingsFile);
  READ_ATTRIBUTE(mLightingSettingsFile);
  READ_ATTRIBUTE(mDefaultToChaseView);

  if (mAllowEnvironmentSettings)
    mAllowWindStrengthSetting = true;

  mGates.clear();
  TiXmlHandle gateHandle = docHandle.FirstChild("Gates");
  for (int iGate = 0 ; ; ++iGate)
  {
    TiXmlElement* dataElement = gateHandle.Child("Gate", iGate).ToElement();
    if (!dataElement)
      break;
    Gate gate;
    gate.mID = readIntFromXML(dataElement, "mID");
    gate.mType = (Gate::Type) readIntFromXML(dataElement, "mType");
    gate.mPos1 = readVector3FromXML(dataElement, "mPos1");
    gate.mPos2 = readVector3FromXML(dataElement, "mPos2");
    gate.mDraw1 = readBoolFromXML(dataElement, "mDraw1");
    gate.mDraw2 = readBoolFromXML(dataElement, "mDraw2");
    gate.mRadius = readFloatFromXML(dataElement, "mRadius");
    gate.mHeight = readFloatFromXML(dataElement, "mHeight");

    readFromXML(dataElement, "mColour1", gate.mColour1);
    readFromXML(dataElement, "mColour2", gate.mColour2);
    readFromXML(dataElement, "mTargetColour1", gate.mTargetColour1);
    readFromXML(dataElement, "mTargetColour2", gate.mTargetColour2);


    if (gate.mType == Gate::TYPE_GATE || gate.mType == Gate::TYPE_LIMBO)
    {
      gate.mLeftToRightDir = gate.mPos2 - gate.mPos1;
      gate.mLeftToRightDir.z = 0.0f;
      gate.mLeftToRightDir.Normalise();

      gate.mFwdDir = Vector3(0,0,1).Cross(gate.mLeftToRightDir);
    }
    else
    {
      gate.mLeftToRightDir = Vector3(0,0,0);
      gate.mFwdDir = Vector3(0,0,0);
      gate.mPos2 = gate.mPos1;
      gate.mDraw2 = false;
    }

    mGates.push_back(gate);
  }

  mCheckpoints.clear();
  TiXmlHandle gateOrderHandle = docHandle.FirstChild("Checkpoints");
  element = gateOrderHandle.ToElement();
  mCheckpointForTimer = 0;
  READ_ATTRIBUTE(mCheckpointForTimer);

  for (int iCheckpoints = 0 ; ; ++iCheckpoints)
  {
    TiXmlElement* dataElement = gateOrderHandle.Child("Checkpoint", iCheckpoints).ToElement();
    if (!dataElement)
      break;
    int id = readIntFromXML(dataElement, "gateID");
    size_t i;
    for (i = 0 ; i != mGates.size() ; ++i)
    {
      if (id == mGates[i].mID)
      {
        TRACE_FILE_IF(1) TRACE("Adding gate ID %d", id);
        mCheckpoints.push_back(i);
        break;
      }
    }
    if (i == mGates.size())
    {
      TRACE("Failed to find gate ID %d for checkpoint", id);
    }
  }

  TRACE_FILE_IF(1) TRACE("mCheckpoints.size = %d", mCheckpoints.size());

  mChecksum = 0;

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
uint32_t GetChecksum(uint8_t* pData, uint32_t dwLen)
{
  uint32_t crc=0xFFFFFFFF;
  for (uint32_t ct=0;ct<dwLen;ct++)
  {
    uint32_t i=(pData[ct]) ^ ((crc) & 0x000000FF);
    for(int j = 8; j > 0; j--)
    {
      if(i & 1)
        i = (i >> 1) ^ 0xEDB88320;
      else
        i >>= 1;
    }
    crc = ((crc) >> 8) ^ i;
  }
  return ~crc;
}

//---------------------------------------------------------------------------------------------------------------------
bool GetFileChecksum(uint32_t& checksum, const char* file)
{
  if (!file)
    return false;

  s3eFile *fileHandle = s3eFileOpen(file, "rb");
  if (!fileHandle)
  {
    TRACE_FILE_IF(1) TRACE("Error opening %s", file);
    return false;
  }
  int32_t fileNumBytes = s3eFileGetSize(fileHandle);
  std::vector<uint8_t> data(fileNumBytes);
  int32_t result = s3eFileRead(&data[0], fileNumBytes, 1, fileHandle);
  if (result != 1)
  {
    s3eFileError error = s3eFileGetError();
    TRACE_FILE_IF(1) TRACE("Error loading %s error = %d", file, error);
    s3eFileClose(fileHandle);
    return false;
  }
  s3eFileClose(fileHandle);
  uint32_t crc = GetChecksum(&data[0], fileNumBytes);
  checksum += crc;
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeSettings::CalculateChecksum(const std::string& file)
{
  mChecksum = 34512334;
  GetFileChecksum(mChecksum, file.c_str());
  if (!mAllowAeroplaneSettings)
    GetFileChecksum(mChecksum, mAeroplaneSettingsFile.c_str());
  GetFileChecksum(mChecksum, mEnvironmentSettingsFile.c_str());
  GetFileChecksum(mChecksum, mLightingSettingsFile.c_str());
}


//---------------------------------------------------------------------------------------------------------------------
int ChallengeSettings::Gate::IsThrough(const Vector3& oldPos, const Vector3& pos, float heightScale) const
{
  if (mType == TYPE_GATE)
  {
    if ((pos - mPos1).Dot(mFwdDir) < 0.0f)
      return 0;
    if ((pos - mPos1).Dot(mLeftToRightDir) < 0.0f)
      return 0;
    if ((pos - mPos2).Dot(mLeftToRightDir) > 0.0f)
      return 0;
    return 1;
  }
  else if (mType == Gate::TYPE_LIMBO)
  {
    Vector3 midPos = (mPos1 + mPos2) * 0.5f;

    float h = pos.z - midPos.z;
    if (h > mHeight * heightScale)
      return 0;

    if ((pos - mPos1).Dot(mLeftToRightDir) < 0.0f)
      return 0;
    if ((pos - mPos2).Dot(mLeftToRightDir) > 0.0f)
      return 0;

    float fwd = (pos - midPos).Dot(mFwdDir);
    float oldFwd = (oldPos - midPos).Dot(mFwdDir);

    if (fwd > 0.0f && oldFwd <= 0.0f)
      return 1;

    if (fwd < 0.0f && oldFwd >= 0.0f)
      return -1;

    return 0;
  }
  else
  {
    Vector3 p1(pos.x, pos.y, 0.0f);
    Vector3 p2(mPos1.x, mPos2.y, 0.0f);
    if ((p1 - p2).GetLengthSquared() > Square(mRadius))
      return 0;
    else if (mHeight > 0.0f && pos.z > mPos1.z + mHeight)
      return 0;
    else
      return 1;
  }
}

//---------------------------------------------------------------------------------------------------------------------
ChallengeSettings::Gate::Gate()
{
  mColour1 = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
  mColour2 = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
  mTargetColour1 = Vector4(0.3f, 1.0f, 0.3f, 1.0f);
  mTargetColour2 = Vector4(0.3f, 1.0f, 0.3f, 1.0f);
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 ChallengeSettings::Gate::GetClosestPointOnGateToPosition(const Vector3& position) const
{
  if (mType == TYPE_GATE || mType == Gate::TYPE_LIMBO)
  {
    Vector3 pos = position;
    // project onto plane
    pos -= (pos - mPos1).Dot(mFwdDir) * mFwdDir;
    float d = (pos - mPos2).Dot(mLeftToRightDir);
    if (d > 0.0f)
      pos -= d * mLeftToRightDir;
    d = (mPos1 - pos).Dot(mLeftToRightDir);
    if (d > 0.0f)
      pos += d * mLeftToRightDir;
    return pos;
  }
  else
  {
    Vector3 pos(mPos1.x, mPos2.y, position.z);
    return pos;
  }
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions ChallengeSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, ChallengeSettings& settings)
{
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
LightingSettings::LightingSettings() : 
  mVersion(mCurrentVersion),
  mSkyboxName("SystemData/Skyboxes/CloudyDaytime"),
  mThumbnail("SystemSettings/Thumbnails/CloudyDaytime.jpg"),
  mTitle("Cloudy daytime"),
  mInfo(""),
  mSunBearingOffset(90),
  mThermalActivity(0.7f),
  mGamma(1.0f)
{
};

//---------------------------------------------------------------------------------------------------------------------
bool LightingSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "LightingSettings" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mSkyboxName);
  WRITE_ATTRIBUTE(mThumbnail);
  WRITE_ATTRIBUTE(mTitle);
  WRITE_ATTRIBUTE(mInfo);
  WRITE_ATTRIBUTE(mSunBearingOffset);
  WRITE_DOUBLE_ATTRIBUTE(mThermalActivity);
  WRITE_DOUBLE_ATTRIBUTE(mGamma); 
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool LightingSettings::ReadBasicsFromDoc(TiXmlDocument& doc)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "LightingSettings" ).ToElement();
  if (!element)
    return false;
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void LightingSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool LightingSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "LightingSettings" ).ToElement();
  if (!element)
    return false;
  *this = LightingSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mSkyboxName);
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  READ_ATTRIBUTE(mSunBearingOffset);
  READ_ATTRIBUTE(mThermalActivity);
  READ_ATTRIBUTE(mGamma);

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions LightingSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, LightingSettings& settings)
{
  COMPARE_FOR_TERRAIN(mSunBearingOffset);
  COMPARE_FOR_TERRAIN(mSkyboxName);
  COMPARE_FOR_TERRAIN(mGamma);
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
AIAeroplaneSettings::AIAeroplaneSettings() :
  mVersion(mCurrentVersion),
  mAllowAIControl(false),
  mCanTow(false),
  mWaypointTolerance(10.0f),

  mControlMaxBankAngle(70.0f),
  mControlBankAnglePerHeadingChange(0.5f),
  mControlPitchControlPerRollAngle(0.01f),
  mControlMaxPitchControl(1.0f),
  mControlMaxRollControl(1.0f),
  mControlRollControlPerRollAngle(0.01f),
  mControlRollTimeScale(0.1f),
  mControlPitchTimeScale(0.05f),

  mGliderControlMinSpeed(10.0f),
  mGliderControlCruiseSpeed(15.0f),
  mGliderControlSpeedPerAltitudeChange(1.0f),
  mGliderControlSlopePerExcessSpeed(0.05f),
  mGliderControlHeadingChangeForNoSlope(90.0f),
  mGliderControlPitchControlPerGlideSlope(1.0f),
  mGliderControlMinAltitude(15.0f),

  mPoweredControlMaxSlopeDown(0.1f),
  mPoweredControlMaxSlopeUp(0.05f),
  mPoweredControlPitchControlPerSlope(10.0f),
  mPoweredControlThrottleControlPerAltitude(0.05f),
  mPoweredControlThrottleControlPerSpeed(0.1f),
  mPoweredControlCruiseSpeed(10.0f),

  mPoweredTakeoffDistance(50.0f),
  mPoweredMaxWaypointTime(20.0f),
  mPoweredMaxDistance(150.0f),
  mPoweredMinHeight(10.0f),
  mPoweredMaxHeight(50.0f),
  mPoweredAltitudeForAux(10000.0f),

  mSlopeMinUpwindDistance(17.0f),
  mSlopeMaxUpwindDistance(40.0f),
  mSlopeMinLeftDistance(-100.0f),
  mSlopeMaxLeftDistance(100.0f),
  mSlopeMinUpDistance(-10.0f),
  mSlopeMaxUpDistance(20.0f),
  mSlopeMaxWaypointTime(60.0f),

  mFlatMaxDistance(150.0f),
  mFlatMaxWaypointTime(60.0f)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool AIAeroplaneSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "AIAeroplaneSettings" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mAllowAIControl);
  WRITE_ATTRIBUTE(mCanTow);
  WRITE_DOUBLE_ATTRIBUTE(mWaypointTolerance);

  WRITE_DOUBLE_ATTRIBUTE(mControlMaxBankAngle);
  WRITE_DOUBLE_ATTRIBUTE(mControlBankAnglePerHeadingChange);
  WRITE_DOUBLE_ATTRIBUTE(mControlPitchControlPerRollAngle);
  WRITE_DOUBLE_ATTRIBUTE(mControlMaxPitchControl);
  WRITE_DOUBLE_ATTRIBUTE(mControlMaxRollControl);
  WRITE_DOUBLE_ATTRIBUTE(mControlRollControlPerRollAngle);
  WRITE_DOUBLE_ATTRIBUTE(mControlRollTimeScale);
  WRITE_DOUBLE_ATTRIBUTE(mControlPitchTimeScale);

  WRITE_DOUBLE_ATTRIBUTE(mGliderControlMinSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mGliderControlCruiseSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mGliderControlSpeedPerAltitudeChange);
  WRITE_DOUBLE_ATTRIBUTE(mGliderControlSlopePerExcessSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mGliderControlHeadingChangeForNoSlope);
  WRITE_DOUBLE_ATTRIBUTE(mGliderControlPitchControlPerGlideSlope);
  WRITE_DOUBLE_ATTRIBUTE(mGliderControlMinAltitude); 

  WRITE_DOUBLE_ATTRIBUTE(mPoweredControlMaxSlopeDown);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredControlMaxSlopeUp);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredControlPitchControlPerSlope);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredControlThrottleControlPerAltitude);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredControlThrottleControlPerSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredControlCruiseSpeed);

  WRITE_DOUBLE_ATTRIBUTE(mPoweredTakeoffDistance);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredMaxWaypointTime);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredMaxDistance);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredMinHeight);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredMaxHeight);
  WRITE_DOUBLE_ATTRIBUTE(mPoweredAltitudeForAux);

  WRITE_DOUBLE_ATTRIBUTE(mSlopeMinUpwindDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mSlopeMaxUpwindDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mSlopeMinLeftDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mSlopeMaxLeftDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mSlopeMinUpDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mSlopeMaxUpDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mSlopeMaxWaypointTime); 

  WRITE_DOUBLE_ATTRIBUTE(mFlatMaxDistance); 
  WRITE_DOUBLE_ATTRIBUTE(mFlatMaxWaypointTime); 

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void AIAeroplaneSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool AIAeroplaneSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "AIAeroplaneSettings" ).ToElement();
  if (!element)
    return false;

  *this = AIAeroplaneSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mAllowAIControl);
  READ_ATTRIBUTE(mCanTow);
  READ_ATTRIBUTE(mWaypointTolerance);

  READ_ATTRIBUTE(mControlMaxBankAngle);
  READ_ATTRIBUTE(mControlBankAnglePerHeadingChange);
  READ_ATTRIBUTE(mControlPitchControlPerRollAngle);
  READ_ATTRIBUTE(mControlMaxPitchControl);
  READ_ATTRIBUTE(mControlMaxRollControl);
  READ_ATTRIBUTE(mControlRollControlPerRollAngle);
  READ_ATTRIBUTE(mControlRollTimeScale);
  READ_ATTRIBUTE(mControlPitchTimeScale);

  READ_ATTRIBUTE(mGliderControlMinSpeed);
  READ_ATTRIBUTE(mGliderControlCruiseSpeed);
  READ_ATTRIBUTE(mGliderControlSpeedPerAltitudeChange);
  READ_ATTRIBUTE(mGliderControlSlopePerExcessSpeed);
  READ_ATTRIBUTE(mGliderControlHeadingChangeForNoSlope);
  READ_ATTRIBUTE(mGliderControlPitchControlPerGlideSlope);
  READ_ATTRIBUTE(mGliderControlMinAltitude); 

  READ_ATTRIBUTE(mPoweredControlMaxSlopeDown);
  READ_ATTRIBUTE(mPoweredControlMaxSlopeUp);
  READ_ATTRIBUTE(mPoweredControlPitchControlPerSlope);
  READ_ATTRIBUTE(mPoweredControlThrottleControlPerAltitude);
  READ_ATTRIBUTE(mPoweredControlThrottleControlPerSpeed);
  READ_ATTRIBUTE(mPoweredControlCruiseSpeed);

  READ_ATTRIBUTE(mPoweredTakeoffDistance);
  READ_ATTRIBUTE(mPoweredMaxWaypointTime);
  READ_ATTRIBUTE(mPoweredMaxDistance);
  READ_ATTRIBUTE(mPoweredMinHeight);
  READ_ATTRIBUTE(mPoweredMaxHeight);
  READ_ATTRIBUTE(mPoweredAltitudeForAux);

  READ_ATTRIBUTE(mSlopeMinUpwindDistance); 
  READ_ATTRIBUTE(mSlopeMaxUpwindDistance); 
  READ_ATTRIBUTE(mSlopeMinLeftDistance); 
  READ_ATTRIBUTE(mSlopeMaxLeftDistance); 
  READ_ATTRIBUTE(mSlopeMinUpDistance); 
  READ_ATTRIBUTE(mSlopeMaxUpDistance); 
  READ_ATTRIBUTE(mSlopeMaxWaypointTime); 

  READ_ATTRIBUTE(mFlatMaxDistance); 
  READ_ATTRIBUTE(mFlatMaxWaypointTime); 

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions AIAeroplaneSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AIAeroplaneSettings& settings)
{
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions AeroplaneSettings::SmokeSource::GetSettingsChangeActions(
  SettingsChangeActions settingsChangeActions, const AeroplaneSettings::SmokeSource& settings) const
{
  COMPARE_FOR_AEROPLANE(mMaxParticles);
  COMPARE_FOR_AEROPLANE(mInitialSize);
  COMPARE_FOR_AEROPLANE(mFinalSize);
  COMPARE_FOR_AEROPLANE(mDampingTime);
  return settingsChangeActions;
}


//---------------------------------------------------------------------------------------------------------------------
AeroplaneSettings::SmokeSource::SmokeSource() 
{
  mEnable = false;
  mOffset = Vector3(0,0,0);
  mVel = Vector3(0,0,-2);
  mColour = Vector3(1,1,1);
  mChannelForRate = Controller::MAX_CHANNELS;
  mChannelForAlpha = Controller::MAX_CHANNELS;
  mMinAlpha = -0.3f;
  mMaxAlpha = 0.3f;
  mMinRate = -60.0f;
  mMaxRate = 60.0f;
  mMaxParticles = 10000;
  mInitialSize = 0.1f;
  mFinalSize = 5.0f;
  mLifetime = 4.0f;
  mDampingTime = 0.1f;
  mVelJitter = 0.1f;
  mEngineWash = 0.7f;
  mHueCycleFreq = 0.0f;
}

//---------------------------------------------------------------------------------------------------------------------
bool AeroplaneSettings::SmokeSource::WriteToDoc(TiXmlDocument& doc, int i) const
{
  char name[256];
  sprintf(name, "SmokeSource_%d", i);
  TiXmlElement* element = new TiXmlElement(name);
  doc.LinkEndChild( element );
  writeToXML(mEnable, element, "mEnable");
  writeVector3ToXML(mOffset, element, "mOffset");
  writeVector3ToXML(mVel, element, "mVel");
  writeVector3ToXML(mColour, element, "mColour");
  writeToXML(mChannelForAlpha, element, "mChannelForAlpha");
  writeToXML(mChannelForRate, element, "mChannelForRate");
  writeToXML(mMaxParticles, element, "mMaxParticles");
  writeFloatToXML(mMinRate, element, "mMinRate");
  writeFloatToXML(mMaxRate, element, "mMaxRate");
  writeFloatToXML(mMinAlpha, element, "mMinAlpha");
  writeFloatToXML(mMaxAlpha, element, "mMaxAlpha");
  writeFloatToXML(mInitialSize, element, "mInitialSize");
  writeFloatToXML(mFinalSize, element, "mFinalSize");
  writeFloatToXML(mLifetime, element, "mLifetime");
  writeFloatToXML(mDampingTime, element, "mDampingTime");
  writeFloatToXML(mVelJitter, element, "mVelJitter");
  writeFloatToXML(mEngineWash, element, "mEngineWash");
  writeFloatToXML(mHueCycleFreq, element, "mHueCycleFreq");
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool AeroplaneSettings::SmokeSource::ReadFromDoc(TiXmlDocument& doc, int i)
{
  TRACE_METHOD_ONLY(1);
  char name[256];
  sprintf(name, "SmokeSource_%d", i);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( name ).ToElement();
  if (!element)
    return false;
  *this = SmokeSource();

  mEnable = readBoolFromXML(element, "mEnable");
  mOffset = readVector3FromXML(element, "mOffset");
  mVel = readVector3FromXML(element, "mVel");
  mColour = readVector3FromXML(element, "mColour");
  mChannelForAlpha = (Controller::Channel) readIntFromXML(element, "mChannelForAlpha");
  mChannelForRate = (Controller::Channel) readIntFromXML(element, "mChannelForRate");
  mMaxParticles = readIntFromXML(element, "mMaxParticles");
  mMinRate = readFloatFromXML(element, "mMinRate");
  mMaxRate = readFloatFromXML(element, "mMaxRate");
  mMinAlpha = readFloatFromXML(element, "mMinAlpha");
  mMaxAlpha = readFloatFromXML(element, "mMaxAlpha");
  mInitialSize = readFloatFromXML(element, "mInitialSize");
  mFinalSize = readFloatFromXML(element, "mFinalSize");
  mLifetime = readFloatFromXML(element, "mLifetime");
  mDampingTime = readFloatFromXML(element, "mDampingTime");
  mVelJitter = readFloatFromXML(element, "mVelJitter");
  mEngineWash = readFloatFromXML(element, "mEngineWash");
  mHueCycleFreq = readFloatFromXML(element, "mHueCycleFreq");
  return true;
}



//---------------------------------------------------------------------------------------------------------------------
AeroplaneSettings::AeroplaneSettings() :
  mVersion(mCurrentVersion),
  mAvailability(0),
  mType(1),
  mAIType(AITYPE_GLIDER),
  mName("SystemData/Aeroplanes/Trainer"), 
  mTitle("Trainer glider"),
  mInfo("Two metre span glider with rudder and elevator controls - ideal for learning to fly."),
  mThumbnail("SystemSettings/Thumbnails/Trainer.jpg"),
  mWWW(""),
  mPreferredController("SystemSettings/Controller/SingleStick.xml"),
  mExtraMassPerCent(0.0f), 
  mExtraMassOffset(0,0,0),
  mColourScheme(0),
  mColourOffset(0.0f),
  mRelaunchWhenStationary(true),
  mRelaunchTime(3.0f),
  mCrashDeltaVel(5.0f, 5.0f, 10.0f),
  mCrashDeltaAngVel(500.0f, 500.0f, 500.0f),
  mCrashSuspensionForceScale(1.0f),
  mDragScale(1.0f),
  mSizeScale(1.0f),
  mMassScale(1.0f),
  mEngineScale(1.0f),
  mLaunchSpeed(6.0f),
  mLaunchAngleUp(-5.0f),
  mLaunchUp(1.0f),
  mLaunchForwards(1.0f),
  mLaunchLeft(-1.0f),
  mLaunchOffsetUp(0.0f),
  mBellyHookOffset(0,0,0),
  mNoseHookOffset(0,0,0),
  mTailHookOffset(0,0,0),
  mFlatLaunchMethod(FLAT_LAUNCH_BUNGEE),
  mMaxBungeeLength(140.0f),
  mMaxBungeeAcceleration(25.0f),
  mTugName("SystemSettings/Aeroplane/Jackdaw.xml"),
  mTugSizeScale(1.0f),
  mTugMassScale(1.0f),
  mTugEngineScale(1.0f),
  mTugTargetSpeed(10.0f),
  mTugMaxClimbSlope(0.05f),
  mAeroTowRopeLength(20.0f),
  mAeroTowRopeStrength(2.0f),
  mAeroTowRopeMassScale(1.0f),
  mAeroTowRopeDragScale(1.0f),
  mAeroTowHeight(100.0f),
  mAeroTowCircuitSize(100.0f),
  mHasVariometer(true),
  mTetherLines(0),
  mTetherRequiresTension(true),
  mTetherColour(1.0f, 1.0f, 1.0f, 0.2f),
  mTetherPhysicsOffset(0.0f, 0.0f, 0.0f),
  mTetherVisualOffset(0.0f, 0.0f, 0.0f),
  mTetherDistanceLeft(20.0f),
  mCameraTargetPosFwd(0.0f),
  mCameraTargetPosUp(0.0f),
  mChaseCamDistance(2.0f),
  mChaseCamHeight(0.5f),
  mChaseCamVerticalVelMult(0.5f),
  mChaseCamFlexibility(0.9f),
  mCockpitCamPitch(0.0f)
{
  mShowButton[0] = false;
  mShowButton[1] = false;
  for (size_t i = 0 ; i != MAX_NUM_SMOKES_PER_PLANE ; ++i)
  {
    mSmokeSources[i] = SmokeSource();
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool AeroplaneSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "AeroplaneSettings" );
  doc.LinkEndChild( element );

  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mAvailability);
  writeToXML(mType | 4, element, "mType"); // flag as user setting
  WRITE_ATTRIBUTE(mAIType);
  WRITE_ATTRIBUTE(mName);
  WRITE_ATTRIBUTE(mThumbnail);
  WRITE_ATTRIBUTE(mTitle);
  WRITE_ATTRIBUTE(mInfo);
  WRITE_ATTRIBUTE(mWWW);
  WRITE_ATTRIBUTE(mPreferredController);
  writeToXML(mShowButton[0], element, "mShowButton_0");
  writeToXML(mShowButton[1], element, "mShowButton_1");
  WRITE_DOUBLE_ATTRIBUTE(mExtraMassPerCent);
  WRITE_DOUBLE_ATTRIBUTE(mExtraMassOffset.x);
  WRITE_DOUBLE_ATTRIBUTE(mExtraMassOffset.y);
  WRITE_DOUBLE_ATTRIBUTE(mExtraMassOffset.z);
  WRITE_ATTRIBUTE(mColourScheme);
  WRITE_DOUBLE_ATTRIBUTE(mColourOffset);
  WRITE_ATTRIBUTE(mRelaunchWhenStationary);
  WRITE_DOUBLE_ATTRIBUTE(mCrashDeltaVel.x);
  WRITE_DOUBLE_ATTRIBUTE(mCrashDeltaVel.y);
  WRITE_DOUBLE_ATTRIBUTE(mCrashDeltaVel.z);
  WRITE_DOUBLE_ATTRIBUTE(mCrashDeltaAngVel.x);
  WRITE_DOUBLE_ATTRIBUTE(mCrashDeltaAngVel.y);
  WRITE_DOUBLE_ATTRIBUTE(mCrashDeltaAngVel.z);
  WRITE_DOUBLE_ATTRIBUTE(mCrashSuspensionForceScale);
  WRITE_DOUBLE_ATTRIBUTE(mDragScale);
  WRITE_DOUBLE_ATTRIBUTE(mSizeScale);
  WRITE_DOUBLE_ATTRIBUTE(mMassScale);
  WRITE_DOUBLE_ATTRIBUTE(mEngineScale);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchSpeed);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchAngleUp);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchUp);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchForwards);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchLeft);
  WRITE_DOUBLE_ATTRIBUTE(mLaunchOffsetUp);
  WRITE_DOUBLE_ATTRIBUTE(mBellyHookOffset.x);
  WRITE_DOUBLE_ATTRIBUTE(mBellyHookOffset.y);
  WRITE_DOUBLE_ATTRIBUTE(mBellyHookOffset.z);
  WRITE_DOUBLE_ATTRIBUTE(mNoseHookOffset.x);
  WRITE_DOUBLE_ATTRIBUTE(mNoseHookOffset.y);
  WRITE_DOUBLE_ATTRIBUTE(mNoseHookOffset.z);
  WRITE_DOUBLE_ATTRIBUTE(mTailHookOffset.x);
  WRITE_DOUBLE_ATTRIBUTE(mTailHookOffset.y);
  WRITE_DOUBLE_ATTRIBUTE(mTailHookOffset.z);
  WRITE_ATTRIBUTE(mFlatLaunchMethod);
  WRITE_DOUBLE_ATTRIBUTE(mMaxBungeeLength);
  WRITE_DOUBLE_ATTRIBUTE(mMaxBungeeAcceleration);
  WRITE_ATTRIBUTE(mTugName);
  WRITE_DOUBLE_ATTRIBUTE(mTugSizeScale);
  WRITE_DOUBLE_ATTRIBUTE(mTugMassScale);
  WRITE_DOUBLE_ATTRIBUTE(mTugEngineScale);
  WRITE_DOUBLE_ATTRIBUTE(mTugMaxClimbSlope);
  WRITE_DOUBLE_ATTRIBUTE(mAeroTowRopeLength);
  WRITE_DOUBLE_ATTRIBUTE(mAeroTowRopeStrength);
  WRITE_DOUBLE_ATTRIBUTE(mAeroTowRopeMassScale);
  WRITE_DOUBLE_ATTRIBUTE(mAeroTowRopeDragScale);
  WRITE_DOUBLE_ATTRIBUTE(mAeroTowHeight);
  WRITE_DOUBLE_ATTRIBUTE(mAeroTowCircuitSize);
  WRITE_DOUBLE_ATTRIBUTE(mTugTargetSpeed);
  WRITE_ATTRIBUTE(mHasVariometer);
  WRITE_ATTRIBUTE(mTetherLines);
  WRITE_ATTRIBUTE(mTetherRequiresTension);
  WRITE_DOUBLE_ATTRIBUTE(mTetherColour.x);
  WRITE_DOUBLE_ATTRIBUTE(mTetherColour.y);
  WRITE_DOUBLE_ATTRIBUTE(mTetherColour.z);
  WRITE_DOUBLE_ATTRIBUTE(mTetherColour.w);
  WRITE_DOUBLE_ATTRIBUTE(mTetherPhysicsOffset.x);
  WRITE_DOUBLE_ATTRIBUTE(mTetherPhysicsOffset.y);
  WRITE_DOUBLE_ATTRIBUTE(mTetherPhysicsOffset.z);
  WRITE_DOUBLE_ATTRIBUTE(mTetherVisualOffset.x);
  WRITE_DOUBLE_ATTRIBUTE(mTetherVisualOffset.y);
  WRITE_DOUBLE_ATTRIBUTE(mTetherVisualOffset.z);
  WRITE_DOUBLE_ATTRIBUTE(mTetherDistanceLeft);
  WRITE_DOUBLE_ATTRIBUTE(mCameraTargetPosFwd);
  WRITE_DOUBLE_ATTRIBUTE(mCameraTargetPosUp);
  WRITE_DOUBLE_ATTRIBUTE(mChaseCamDistance);
  WRITE_DOUBLE_ATTRIBUTE(mChaseCamHeight);
  WRITE_DOUBLE_ATTRIBUTE(mChaseCamVerticalVelMult);
  WRITE_DOUBLE_ATTRIBUTE(mChaseCamFlexibility);
  WRITE_DOUBLE_ATTRIBUTE(mCockpitCamPitch);

  for (size_t i = 0 ; i != MAX_NUM_SMOKES_PER_PLANE ; ++i)
  {
    mSmokeSources[i].WriteToDoc(doc, i);
  }
  mAIAeroplaneSettings.WriteToDoc(doc);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool AeroplaneSettings::ReadBasicsFromDoc(TiXmlDocument& doc)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "AeroplaneSettings" ).ToElement();
  if (!element)
    return false;

  READ_ATTRIBUTE(mAvailability);
  READ_ATTRIBUTE(mType);
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplaneSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool AeroplaneSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "AeroplaneSettings" ).ToElement();
  if (!element)
    return false;
  *this = AeroplaneSettings();
  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mAvailability);
  READ_ATTRIBUTE(mType);
  READ_ENUM_ATTRIBUTE(mAIType);
  READ_ATTRIBUTE(mName);
  READ_ATTRIBUTE(mThumbnail);
  READ_ATTRIBUTE(mTitle);
  READ_ATTRIBUTE(mInfo);
  READ_ATTRIBUTE(mWWW);
  READ_ATTRIBUTE(mPreferredController);
  mShowButton[0] = readBoolFromXML(element, "mShowButton_0");
  mShowButton[1] = readBoolFromXML(element, "mShowButton_1");
  READ_ATTRIBUTE(mExtraMassPerCent);
  READ_ATTRIBUTE(mExtraMassOffset.x);
  READ_ATTRIBUTE(mExtraMassOffset.y);
  READ_ATTRIBUTE(mExtraMassOffset.z);
  READ_ATTRIBUTE(mColourScheme);
  READ_ATTRIBUTE(mColourOffset);
  READ_ATTRIBUTE(mRelaunchWhenStationary);
  READ_ATTRIBUTE(mCrashDeltaVel.x);
  READ_ATTRIBUTE(mCrashDeltaVel.y);
  READ_ATTRIBUTE(mCrashDeltaVel.z);
  READ_ATTRIBUTE(mCrashDeltaAngVel.x);
  READ_ATTRIBUTE(mCrashDeltaAngVel.y);
  READ_ATTRIBUTE(mCrashDeltaAngVel.z);
  READ_ATTRIBUTE(mCrashSuspensionForceScale);
  READ_ATTRIBUTE(mDragScale);
  READ_ATTRIBUTE(mSizeScale);
  READ_ATTRIBUTE(mMassScale);
  READ_ATTRIBUTE(mEngineScale);
  READ_ATTRIBUTE(mLaunchSpeed);
  READ_ATTRIBUTE(mLaunchAngleUp);
  READ_ATTRIBUTE(mLaunchUp);
  READ_ATTRIBUTE(mLaunchForwards);
  READ_ATTRIBUTE(mLaunchLeft);
  READ_ATTRIBUTE(mLaunchOffsetUp);
  READ_ATTRIBUTE(mBellyHookOffset.x);
  READ_ATTRIBUTE(mBellyHookOffset.y);
  READ_ATTRIBUTE(mBellyHookOffset.z);
  READ_ATTRIBUTE(mNoseHookOffset.x);
  READ_ATTRIBUTE(mNoseHookOffset.y);
  READ_ATTRIBUTE(mNoseHookOffset.z);
  READ_ATTRIBUTE(mTailHookOffset.x);
  READ_ATTRIBUTE(mTailHookOffset.y);
  READ_ATTRIBUTE(mTailHookOffset.z);
  READ_ENUM_ATTRIBUTE(mFlatLaunchMethod);
  READ_ATTRIBUTE(mMaxBungeeLength);
  READ_ATTRIBUTE(mMaxBungeeAcceleration);
  READ_ATTRIBUTE(mTugName);
  READ_ATTRIBUTE(mTugSizeScale);
  READ_ATTRIBUTE(mTugMassScale);
  READ_ATTRIBUTE(mTugEngineScale);
  READ_ATTRIBUTE(mTugMaxClimbSlope);
  READ_ATTRIBUTE(mAeroTowRopeLength);
  READ_ATTRIBUTE(mAeroTowRopeStrength);
  READ_ATTRIBUTE(mAeroTowRopeMassScale);
  READ_ATTRIBUTE(mAeroTowRopeDragScale);
  READ_ATTRIBUTE(mAeroTowHeight);
  READ_ATTRIBUTE(mAeroTowCircuitSize);
  READ_ATTRIBUTE(mTugTargetSpeed);
  READ_ATTRIBUTE(mHasVariometer);
  READ_ATTRIBUTE(mTetherLines);
  READ_ATTRIBUTE(mTetherRequiresTension);
  READ_ATTRIBUTE(mTetherColour.x);
  READ_ATTRIBUTE(mTetherColour.y);
  READ_ATTRIBUTE(mTetherColour.z);
  READ_ATTRIBUTE(mTetherColour.w);
  READ_ATTRIBUTE(mTetherPhysicsOffset.x);
  READ_ATTRIBUTE(mTetherPhysicsOffset.y);
  READ_ATTRIBUTE(mTetherPhysicsOffset.z);
  READ_ATTRIBUTE(mTetherVisualOffset.x);
  READ_ATTRIBUTE(mTetherVisualOffset.y);
  READ_ATTRIBUTE(mTetherVisualOffset.z);
  READ_ATTRIBUTE(mTetherDistanceLeft);
  READ_ATTRIBUTE(mCameraTargetPosFwd);
  READ_ATTRIBUTE(mCameraTargetPosUp);
  READ_ATTRIBUTE(mChaseCamDistance);
  READ_ATTRIBUTE(mChaseCamHeight);
  READ_ATTRIBUTE(mChaseCamVerticalVelMult);
  READ_ATTRIBUTE(mChaseCamFlexibility);
  READ_ATTRIBUTE(mCockpitCamPitch);
  for (size_t i = 0 ; i != MAX_NUM_SMOKES_PER_PLANE ; ++i)
  {
    mSmokeSources[i].ReadFromDoc(doc, i);
  }
  mAIAeroplaneSettings.ReadFromDoc(doc, readAll);

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions AeroplaneSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AeroplaneSettings& settings) 
{
  COMPARE_FOR_AEROPLANE(mName);
  COMPARE_FOR_AEROPLANE(mExtraMassPerCent);
  COMPARE_FOR_AEROPLANE(mExtraMassOffset);
  COMPARE_FOR_AEROPLANE(mColourScheme);
  COMPARE_FOR_AEROPLANE(mColourOffset);
  COMPARE_FOR_AEROPLANE(mSizeScale);
  COMPARE_FOR_AEROPLANE(mMassScale);
  COMPARE_FOR_AEROPLANE(mEngineScale);
  COMPARE_FOR_AEROPLANE(mFlatLaunchMethod);
  COMPARE_FOR_AEROPLANE(mBellyHookOffset);
  COMPARE_FOR_AEROPLANE(mNoseHookOffset);
  COMPARE_FOR_AEROPLANE(mTailHookOffset);
  COMPARE_FOR_AEROPLANE(mTugName);
  COMPARE_FOR_AEROPLANE(mTugSizeScale);
  COMPARE_FOR_AEROPLANE(mTugMassScale);
  COMPARE_FOR_AEROPLANE(mTugEngineScale);
  COMPARE_FOR_AEROPLANE(mTugTargetSpeed);
  COMPARE_FOR_AEROPLANE(mTugMaxClimbSlope);
  COMPARE_FOR_AEROPLANE(mAeroTowRopeLength);
  COMPARE_FOR_AEROPLANE(mAeroTowRopeStrength);
  COMPARE_FOR_AEROPLANE(mAeroTowRopeMassScale);
  COMPARE_FOR_AEROPLANE(mAeroTowRopeDragScale);
  COMPARE_FOR_AEROPLANE(mAeroTowHeight);
  COMPARE_FOR_AEROPLANE(mAeroTowCircuitSize);
  settingsChangeActions = mAIAeroplaneSettings.GetSettingsChangeActions(settingsChangeActions, settings.mAIAeroplaneSettings);
  for (int i = 0 ; i != MAX_NUM_SMOKES_PER_PLANE ; ++i)
    settingsChangeActions = mSmokeSources[i].GetSettingsChangeActions(settingsChangeActions, settings.mSmokeSources[i]);
  return settingsChangeActions;
}

//---------------------------------------------------------------------------------------------------------------------
ControllerSettings::ControllerSettings() :
  mVersion(mCurrentVersion),
  mControllerStickDecayTime(0.1f),
  mControllerAccelerometerOffsetAngle(30.0f),
  mControllerAccelerometerXSensitivity(0.5f),
  mControllerAccelerometerYSensitivity(0.5f),
  mResetAltSettingOnLaunch(false),
  mTreatThrottleAsBrakes(false),
  mNumAltSettings(1),
  mCurrentAltSetting(0)
{
  mControlPerChannel[Controller::CHANNEL_AILERONS ] = CONTROLLER_STICK_ROLL;
  mControlPerChannel[Controller::CHANNEL_ELEVATOR ] = CONTROLLER_STICK_PITCH;
  mControlPerChannel[Controller::CHANNEL_RUDDER   ] = CONTROLLER_NUM_CONTROLS;
  mControlPerChannel[Controller::CHANNEL_THROTTLE ] = CONTROLLER_CONSTANT;
  mControlPerChannel[Controller::CHANNEL_LOOKYAW  ] = CONTROLLER_NUM_CONTROLS;
  mControlPerChannel[Controller::CHANNEL_LOOKPITCH] = CONTROLLER_NUM_CONTROLS;
  mControlPerChannel[Controller::CHANNEL_AUX1     ] = CONTROLLER_NUM_CONTROLS;
  mControlPerChannel[Controller::CHANNEL_SMOKE1   ] = CONTROLLER_NUM_CONTROLS;
  mControlPerChannel[Controller::CHANNEL_SMOKE2   ] = CONTROLLER_NUM_CONTROLS;
  mControlPerChannel[Controller::CHANNEL_HOOK     ] = CONTROLLER_NUM_CONTROLS;
  IwAssert(ROWLHOUSE, Controller::MAX_CHANNELS == 10);

  mButtonNames[0] = "Button 1";
  mButtonNames[1] = "Button 2";
  mButtonNames[2] = "Button 3";

  for (int i = 0 ; i != CONTROLLER_MAX_NUM_ALT_SETTINGS ; ++i)
  {
    mMixes[i].mMixElevatorToFlaps = 0.0f;
    mMixes[i].mMixAileronToRudder = 0.0f;
    mMixes[i].mMixFlapsToElevator = 0.0f;
    mMixes[i].mMixBrakesToElevator = 0.0f;
    mMixes[i].mMixRudderToElevator = 0.0f;
    mMixes[i].mMixRudderToAileron = 0.0f;

    char name[32];
    sprintf(name, "Control %d", i);
    mAltSettingNames[i] = name;

    mControlSettings[i][ControllerSettings::CONTROLLER_STICK_SPEED].mAutoCentre = true;
    mControlSettings[i][ControllerSettings::CONTROLLER_STICK_SPEED].mScale = 2.0f;
    mControlSettings[i][ControllerSettings::CONTROLLER_STICK_SPEED].mExponential = 1.0f;
    mControlSettings[i][ControllerSettings::CONTROLLER_STICK_SPEED].mTrim = -1.0f;
    mControlSettings[i][ControllerSettings::CONTROLLER_STICK_SPEED].mClamp = ControllerSettings::CONTROL_CLAMP_NONE;

    mControlSettings[i][ControllerSettings::CONTROLLER_CONSTANT].mAutoCentre = true;
    mControlSettings[i][ControllerSettings::CONTROLLER_CONSTANT].mScale = 1.0f;
    mControlSettings[i][ControllerSettings::CONTROLLER_CONSTANT].mExponential = 1.0f;
    mControlSettings[i][ControllerSettings::CONTROLLER_CONSTANT].mTrim = -1.0f;
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool ControllerSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "ControllerSettings" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  WRITE_DOUBLE_ATTRIBUTE(mControllerStickDecayTime);
  WRITE_DOUBLE_ATTRIBUTE(mControllerAccelerometerOffsetAngle);
  WRITE_DOUBLE_ATTRIBUTE(mControllerAccelerometerXSensitivity);
  WRITE_DOUBLE_ATTRIBUTE(mControllerAccelerometerYSensitivity);
  WRITE_ATTRIBUTE(mResetAltSettingOnLaunch);
  WRITE_ATTRIBUTE(mTreatThrottleAsBrakes);
  WRITE_ATTRIBUTE(mNumAltSettings);
  WRITE_ATTRIBUTE(mCurrentAltSetting);
  element->SetAttribute("mControlPerChannel_0", mControlPerChannel[Controller::CHANNEL_AILERONS ]);
  element->SetAttribute("mControlPerChannel_1", mControlPerChannel[Controller::CHANNEL_ELEVATOR ]);
  element->SetAttribute("mControlPerChannel_2", mControlPerChannel[Controller::CHANNEL_RUDDER   ]);
  element->SetAttribute("mControlPerChannel_3", mControlPerChannel[Controller::CHANNEL_THROTTLE ]);
  element->SetAttribute("mControlPerChannel_4", mControlPerChannel[Controller::CHANNEL_LOOKYAW  ]);
  element->SetAttribute("mControlPerChannel_5", mControlPerChannel[Controller::CHANNEL_LOOKPITCH]);
  element->SetAttribute("mControlPerChannel_6", mControlPerChannel[Controller::CHANNEL_AUX1     ]);
  element->SetAttribute("mControlPerChannel_7", mControlPerChannel[Controller::CHANNEL_SMOKE1   ]);
  element->SetAttribute("mControlPerChannel_8", mControlPerChannel[Controller::CHANNEL_SMOKE2   ]);
  element->SetAttribute("mControlPerChannel_9", mControlPerChannel[Controller::CHANNEL_HOOK     ]);

  for (int i = 0 ; i != CONTROLLER_NUM_CONTROLS ; ++i)
  {
    for (int j = 0 ; j != CONTROLLER_MAX_NUM_ALT_SETTINGS ; ++j)
    {
      mControlSettings[j][i].WriteToDoc(doc, i, j);
    }
  }

  element->SetAttribute("ButtonName_0", mButtonNames[0].c_str());
  element->SetAttribute("ButtonName_1", mButtonNames[1].c_str());
  element->SetAttribute("ButtonName_2", mButtonNames[2].c_str());

  for (size_t j = 0 ; j != CONTROLLER_MAX_NUM_ALT_SETTINGS ; ++j)
  {
    char name[256];
    sprintf(name, "AltSettingName_%lu", (long unsigned) j);
    TiXmlElement* element = new TiXmlElement(name);
    doc.LinkEndChild( element );
    element->SetAttribute("name", mAltSettingNames[j]);

    const Mix& mix = mMixes[j];
    WRITE_DOUBLE_ATTRIBUTE_INDEX(mix.mMixElevatorToFlaps, j);
    WRITE_DOUBLE_ATTRIBUTE_INDEX(mix.mMixAileronToRudder, j);
    WRITE_DOUBLE_ATTRIBUTE_INDEX(mix.mMixFlapsToElevator, j);
    WRITE_DOUBLE_ATTRIBUTE_INDEX(mix.mMixBrakesToElevator, j);
    WRITE_DOUBLE_ATTRIBUTE_INDEX(mix.mMixRudderToElevator, j);
    WRITE_DOUBLE_ATTRIBUTE_INDEX(mix.mMixRudderToAileron, j);
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void ControllerSettings::Upgrade(TiXmlDocument& doc)
{
  if (mVersion < 2)
  {
    for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    {
      if (mControlPerChannel[i] == 9)
        mControlPerChannel[i] = CONTROLLER_NUM_CONTROLS;
    }
  }
  if (mVersion < 3)
  {
    for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    {
      if (mControlPerChannel[i] == 11)
        mControlPerChannel[i] = CONTROLLER_NUM_CONTROLS;
    }
  }
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool ControllerSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "ControllerSettings" ).ToElement();
  if (!element)
    return false;
  *this = ControllerSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mControllerStickDecayTime);
  READ_ATTRIBUTE(mControllerAccelerometerOffsetAngle);
  READ_ATTRIBUTE(mControllerAccelerometerXSensitivity);
  READ_ATTRIBUTE(mControllerAccelerometerYSensitivity);
  READ_ATTRIBUTE(mResetAltSettingOnLaunch);
  READ_ATTRIBUTE(mTreatThrottleAsBrakes);
  READ_ATTRIBUTE(mNumAltSettings);
  READ_ATTRIBUTE(mCurrentAltSetting);
  readFromXML(element, "mControlPerChannel_0", (int&) mControlPerChannel[Controller::CHANNEL_AILERONS ]);
  readFromXML(element, "mControlPerChannel_1", (int&) mControlPerChannel[Controller::CHANNEL_ELEVATOR ]);
  readFromXML(element, "mControlPerChannel_2", (int&) mControlPerChannel[Controller::CHANNEL_RUDDER   ]);
  readFromXML(element, "mControlPerChannel_3", (int&) mControlPerChannel[Controller::CHANNEL_THROTTLE ]);
  readFromXML(element, "mControlPerChannel_4", (int&) mControlPerChannel[Controller::CHANNEL_LOOKYAW  ]);
  readFromXML(element, "mControlPerChannel_5", (int&) mControlPerChannel[Controller::CHANNEL_LOOKPITCH]);
  readFromXML(element, "mControlPerChannel_6", (int&) mControlPerChannel[Controller::CHANNEL_AUX1     ]);
  readFromXML(element, "mControlPerChannel_7", (int&) mControlPerChannel[Controller::CHANNEL_SMOKE1   ]);
  readFromXML(element, "mControlPerChannel_8", (int&) mControlPerChannel[Controller::CHANNEL_SMOKE2   ]);
  readFromXML(element, "mControlPerChannel_9", (int&) mControlPerChannel[Controller::CHANNEL_HOOK     ]);

  for (int i = 0 ; i != CONTROLLER_NUM_CONTROLS ; ++i)
  {
    for (int j = 0 ; j != CONTROLLER_MAX_NUM_ALT_SETTINGS ; ++j)
    {
      mControlSettings[j][i].ReadFromDoc(doc, i, j);
    }
  }

  readFromXML(element, "ButtonName_0", mButtonNames[0]);
  readFromXML(element, "ButtonName_1", mButtonNames[1]);
  readFromXML(element, "ButtonName_2", mButtonNames[2]);

  for (size_t j = 0 ; j != CONTROLLER_MAX_NUM_ALT_SETTINGS ; ++j)
  {
    char name[256];
    sprintf(name, "AltSettingName_%lu", (long unsigned) j);
    TiXmlHandle docHandle( &doc );
    TiXmlElement* element = docHandle.FirstChild( name ).ToElement();
    if (element)
    {
      readFromXML(element, "name", mAltSettingNames[j]);

      Mix& mix = mMixes[j];
      READ_ATTRIBUTE_INDEX(mix.mMixElevatorToFlaps, j);
      READ_ATTRIBUTE_INDEX(mix.mMixAileronToRudder, j);
      READ_ATTRIBUTE_INDEX(mix.mMixFlapsToElevator, j);
      READ_ATTRIBUTE_INDEX(mix.mMixBrakesToElevator,j);
      READ_ATTRIBUTE_INDEX(mix.mMixRudderToElevator,j);
      READ_ATTRIBUTE_INDEX(mix.mMixRudderToAileron, j);
    }
  }

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool ControllerSettings::ControlSetting::WriteToDoc(TiXmlDocument& doc, int i, int j) const
{
  char name[256];
  sprintf(name, "ControllerControlSettings_%d_%d", i, j);
  TiXmlElement* element = new TiXmlElement(name);
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mClamp);
  WRITE_ATTRIBUTE(mAutoCentre);
  WRITE_DOUBLE_ATTRIBUTE(mScale);
  WRITE_DOUBLE_ATTRIBUTE(mExponential);
  WRITE_DOUBLE_ATTRIBUTE(mTrim);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool ControllerSettings::ControlSetting::ReadFromDoc(TiXmlDocument& doc, int i, int j)
{
  TRACE_METHOD_ONLY(1);
  char name[256];
  sprintf(name, "ControllerControlSettings_%d_%d", i, j);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( name ).ToElement();
  if (!element)
    return false;
  *this = ControlSetting();

  READ_ENUM_ATTRIBUTE(mClamp);
  READ_ATTRIBUTE(mAutoCentre);
  READ_ATTRIBUTE(mScale);
  READ_ATTRIBUTE(mExponential);
  READ_ATTRIBUTE(mTrim);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
JoystickSettings::JoystickSettings() :
  mVersion(mCurrentVersion),
  mEnableJoystick(false), mAdjustForCircularSticks(false)
{
  for (size_t i = 0 ; i != JOYSTICK_NUM_CONTROLS ; ++i)
    mJoystickAnalogueOverrides[i] = JoystickAnalogueOverride(ControllerSettings::CONTROLLER_NUM_CONTROLS, 1.0f, 0.0f);
  for (size_t i = 0 ; i != JOYSTICK_NUM_BUTTONS ; ++i)
    mJoystickButtonOverrides[i] = JoystickButtonOverride(JoystickButtonOverride::CONTROL_BUTTON_NONE);
  for (size_t i = 0 ; i != JOYSTICK_NUM_CONTROLS ; ++i)
    mJoystickAButtonOverrides[i] = JoystickButtonOverride(JoystickButtonOverride::CONTROL_BUTTON_NONE);

  int os = s3eDeviceGetInt(S3E_DEVICE_OS);
  if (os == S3E_OS_ID_WIN10)
  {
    LoadFromFile("SystemSettings/Joystick/Win10-XBox360.xml", true);
    UpdateJoystick(0);
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool JoystickSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "JoystickSettings" );
  doc.LinkEndChild( element );

  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mEnableJoystick);
  WRITE_ATTRIBUTE(mAdjustForCircularSticks);

  char txt[256];
  for (uint32_t i = 0 ; i != JOYSTICK_NUM_CONTROLS ; ++i)
  {
    sprintf(txt, "JoystickAnalogue%d", i);
    element->SetAttribute(txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mControl);
    sprintf(txt, "JoystickAnalogueScalePositive%d", i);
    element->SetDoubleAttribute(txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mScalePositive);
    sprintf(txt, "JoystickAnalogueScaleNegative%d", i);
    element->SetDoubleAttribute(txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mScaleNegative);
    sprintf(txt, "JoystickAnalogueOffset%d", i);
    element->SetDoubleAttribute(txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mOffset);
    sprintf(txt, "JoystickAnalogueDeadZone%d", i);
    element->SetDoubleAttribute(txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mDeadZone);

    sprintf(txt, "JoystickAButton%d", i);
    element->SetAttribute(txt, mJoystickAButtonOverrides[i].mControl);
  }

  for (uint32_t i = 0 ; i != JOYSTICK_NUM_BUTTONS ; ++i)
  {
    sprintf(txt, "JoystickButton%d", i);
    element->SetAttribute(txt, mJoystickButtonOverrides[i].mControl);
  }

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void JoystickSettings::Upgrade(TiXmlDocument& doc)
{
  if (mVersion < 2)
  {
    for (uint32_t i = 0 ; i != JOYSTICK_NUM_CONTROLS ; ++i)
    {
      if (mJoystickAnalogueOverrides[JOYSTICK_0+i].mControl == 9)
        mJoystickAnalogueOverrides[JOYSTICK_0+i].mControl = ControllerSettings::CONTROLLER_NUM_CONTROLS;
    }
  }
  if (mVersion < 3)
  {
    for (uint32_t i = 0 ; i != JOYSTICK_NUM_CONTROLS ; ++i)
    {
      if (mJoystickAnalogueOverrides[JOYSTICK_0+i].mControl == 11)
        mJoystickAnalogueOverrides[JOYSTICK_0+i].mControl = ControllerSettings::CONTROLLER_NUM_CONTROLS;
    }
  }
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool JoystickSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "JoystickSettings" ).ToElement();
  if (!element)
    return false;
  *this = JoystickSettings();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mEnableJoystick);
  READ_ATTRIBUTE(mAdjustForCircularSticks);

  char txt[256];
  for (uint32_t i = 0 ; i != 8 ; ++i)
  {
    sprintf(txt, "JoystickAnalogue%d", i);
    readFromXML(element, txt, (int&) mJoystickAnalogueOverrides[JOYSTICK_0+i].mControl);
    // Legacy support
    sprintf(txt, "JoystickAnalogueScale%d", i);
    readFromXML(element, txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mScalePositive);
    readFromXML(element, txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mScaleNegative);
    mJoystickAnalogueOverrides[JOYSTICK_0+i].mOffset = 0.0f;

    sprintf(txt, "JoystickAnalogueScalePositive%d", i);
    readFromXML(element, txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mScalePositive);
    sprintf(txt, "JoystickAnalogueScaleNegative%d", i);
    readFromXML(element, txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mScaleNegative);
    sprintf(txt, "JoystickAnalogueOffset%d", i);
    readFromXML(element, txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mOffset);

    sprintf(txt, "JoystickAnalogueDeadZone%d", i);
    readFromXML(element, txt, mJoystickAnalogueOverrides[JOYSTICK_0+i].mDeadZone);
  }

  for (uint32_t i = 0 ; i != JOYSTICK_NUM_BUTTONS ; ++i)
  {
    sprintf(txt, "JoystickButton%d", i);
    readFromXML(element, txt, (int&) mJoystickButtonOverrides[i].mControl);
  }

  for (uint32_t i = 0 ; i != JOYSTICK_NUM_CONTROLS ; ++i)
  {
    sprintf(txt, "JoystickAButton%d", i);
    readFromXML(element, txt, (int&) mJoystickAButtonOverrides[i].mControl);
  }

  Upgrade(doc);

  return true;
}


//---------------------------------------------------------------------------------------------------------------------
Statistics::Statistics() : 
  mVersion(mCurrentVersion),
  mPicaSimSettingsVersion(LATEST_PICASIM_SETTINGS_VERSION), 
  mPicaSimBuildNumber(0),
  mSmoothedFPS(0.0f), 
  mFPS(0.0f),
  mNumDepthBits(0), 
  mAppType(APP_DEMO),
  mIsAmazon(false),
  mIsInTrialMode(false),
  mMaxFlightTime(0.0f),
  mTotalFlightTime(0.0f),
  mLoadCounter(0), 
  mLoadedAeroplane(false), 
  mLoadedTerrain(false),
  mLoadedOptions(false),
  mLastScoreloopUsername("Fetching username")
{
  int os = s3eDeviceGetInt(S3E_DEVICE_OS);
  if (os == S3E_OS_ID_WIN10)
    mIsInTrialMode = true;
}

//---------------------------------------------------------------------------------------------------------------------
bool Statistics::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "Statistics" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  WRITE_ATTRIBUTE(mPicaSimSettingsVersion);
  WRITE_ATTRIBUTE(mPicaSimBuildNumber);
  WRITE_DOUBLE_ATTRIBUTE(mSmoothedFPS); 
  WRITE_ATTRIBUTE(mNumDepthBits);
  WRITE_ATTRIBUTE(mAppType);
  WRITE_ATTRIBUTE(mIsAmazon);
  WRITE_ATTRIBUTE(mIsInTrialMode);
  WRITE_DOUBLE_ATTRIBUTE(mMaxFlightTime);
  WRITE_DOUBLE_ATTRIBUTE(mTotalFlightTime);
  WRITE_ATTRIBUTE(mLoadCounter);
  WRITE_ATTRIBUTE(mLoadedAeroplane);
  WRITE_ATTRIBUTE(mLoadedTerrain);
  WRITE_ATTRIBUTE(mLoadedOptions);
  WRITE_ATTRIBUTE(mLastScoreloopUsername);

  TiXmlElement* highScoresElement = new TiXmlElement("HighScores");
  doc.LinkEndChild( highScoresElement );

  for (Scores::const_iterator it = mHighScores.begin() ; it != mHighScores.end() ; ++it)
  {
    element = new TiXmlElement("Score");
    highScoresElement->LinkEndChild( element );

    uint32_t mode = it->first;
    Score score = it->second;
    WRITE_ATTRIBUTE(mode);
    WRITE_DOUBLE_ATTRIBUTE(score.mResult);
    WRITE_DOUBLE_ATTRIBUTE(score.mMinorResult);
  }


  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void Statistics::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool Statistics::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "Statistics" ).ToElement();
  if (!element)
    return false;
  *this = Statistics();

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  READ_ATTRIBUTE(mPicaSimSettingsVersion);
  READ_ATTRIBUTE(mPicaSimBuildNumber);
  READ_ATTRIBUTE(mIsInTrialMode);
  READ_ATTRIBUTE(mMaxFlightTime);
  READ_ATTRIBUTE(mTotalFlightTime);
  READ_ATTRIBUTE(mLoadCounter);
  READ_ATTRIBUTE(mLoadedAeroplane);
  READ_ATTRIBUTE(mLoadedTerrain);
  READ_ATTRIBUTE(mLoadedOptions);
  READ_ATTRIBUTE(mLastScoreloopUsername);

  // Fix up some historical bug
  int buildNumber = 0;
  READ_ATTRIBUTE(buildNumber);
  if (buildNumber > mPicaSimBuildNumber)
    mPicaSimBuildNumber = buildNumber;

  mHighScores.clear();
  TiXmlHandle highScoresHandle = docHandle.FirstChild("HighScores");
  for (int iScore = 0 ; ; ++iScore)
  {
    TiXmlElement* dataElement = highScoresHandle.Child("Score", iScore).ToElement();
    if (!dataElement)
      break;
    Score score;
    score.mResult = readFloatFromXML(dataElement, "score.mResult");
    score.mMinorResult = readFloatFromXML(dataElement, "score.mMinorResult");
    uint32_t mode = readIntFromXML(dataElement, "mode");

    mHighScores[mode] = score;
  }

  Upgrade(doc);

  return true;
}
//---------------------------------------------------------------------------------------------------------------------
GameSettings::GameSettings() : 
  mVersion(mCurrentVersion)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool GameSettings::WriteToDoc(TiXmlDocument& doc) const
{
  TiXmlElement* element = new TiXmlElement( "GameSettings" );
  doc.LinkEndChild( element );
  WRITE_ATTRIBUTE(mVersion);
  mOptions.WriteToDoc(doc);
  mAeroplaneSettings.WriteToDoc(doc);
  mEnvironmentSettings.WriteToDoc(doc);
  mObjectsSettings.WriteToDoc(doc);
  mAIControllersSettings.WriteToDoc(doc);
  mControllerSettings.WriteToDoc(doc);
  mJoystickSettings.WriteToDoc(doc);
  mChallengeSettings.WriteToDoc(doc);
  mLightingSettings.WriteToDoc(doc);
  mStatistics.WriteToDoc(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void GameSettings::Upgrade(TiXmlDocument& doc)
{
  mVersion = mCurrentVersion;
}

//---------------------------------------------------------------------------------------------------------------------
bool GameSettings::ReadFromDoc(TiXmlDocument& doc, bool readAll)
{
  TRACE_METHOD_ONLY(1);
  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "GameSettings" ).ToElement();
  if (!element)
    return false;

  if (!READ_ATTRIBUTE(mVersion))
    mVersion = 1;

  mOptions.ReadFromDoc(doc, readAll);
  mAeroplaneSettings.ReadFromDoc(doc, readAll);
  mEnvironmentSettings.ReadFromDoc(doc, readAll);
  mObjectsSettings.ReadFromDoc(doc, readAll);
  mAIControllersSettings.ReadFromDoc(doc, readAll);
  mControllerSettings.ReadFromDoc(doc, readAll);
  mJoystickSettings.ReadFromDoc(doc, readAll);
  mChallengeSettings.ReadFromDoc(doc, readAll);
  mLightingSettings.ReadFromDoc(doc, readAll);
  mStatistics.ReadFromDoc(doc, readAll);

  Upgrade(doc);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
SettingsChangeActions GameSettings::GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, GameSettings& settings)
{
  settingsChangeActions = mOptions.GetSettingsChangeActions(settingsChangeActions, settings.mOptions);
  settingsChangeActions = mAeroplaneSettings.GetSettingsChangeActions(settingsChangeActions, settings.mAeroplaneSettings);
  settingsChangeActions = mEnvironmentSettings.GetSettingsChangeActions(settingsChangeActions, settings.mEnvironmentSettings);
  settingsChangeActions = mObjectsSettings.GetSettingsChangeActions(settingsChangeActions, settings.mObjectsSettings);
  settingsChangeActions = mAIControllersSettings.GetSettingsChangeActions(settingsChangeActions, settings.mAIControllersSettings);
  settingsChangeActions = mControllerSettings.GetSettingsChangeActions(settingsChangeActions, settings.mControllerSettings);
  settingsChangeActions = mChallengeSettings.GetSettingsChangeActions(settingsChangeActions, settings.mChallengeSettings);
  settingsChangeActions = mLightingSettings.GetSettingsChangeActions(settingsChangeActions, settings.mLightingSettings);
  return settingsChangeActions;
}


