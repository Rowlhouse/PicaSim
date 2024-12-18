#ifndef CONFIGURATIONANDSETTINGS_H
#define CONFIGURATIONANDSETTINGS_H

#include "Framework.h"

#include "Controller.h"
#include "PicaStrings.h"

#include <string>
#include <map>

bool GetFileChecksum(uint32& checksum, const char* file);

//---------------------------------------------------------------------------------------------------------------------
struct SettingsChangeActions
{
  SettingsChangeActions() : mReloadAeroplane(false), mReloadTerrain(false), mReloadAI(false), mResetAI(false), mRecalcWind(false), mRestartChallenge(false) {}
  bool mReloadAeroplane;
  bool mReloadTerrain;
  bool mReloadAI;
  bool mResetAI;
  bool mRecalcWind;
  bool mRestartChallenge;
};

//---------------------------------------------------------------------------------------------------------------------
struct Settings
{
  bool SaveToFile(const std::string& name) const;
  bool LoadFromFile(const std::string& name, bool readAll = true);
  bool LoadBasicsFromFile(const std::string& name);

  virtual bool WriteToDoc(TiXmlDocument& doc) const = 0;
  virtual bool ReadFromDoc(TiXmlDocument& doc, bool readAll) = 0;
  virtual bool ReadBasicsFromDoc(TiXmlDocument& doc) {return false;}
  virtual void Upgrade(TiXmlDocument& doc) = 0;
};

//---------------------------------------------------------------------------------------------------------------------
struct ControllerSettings : public Settings
{
  ControllerSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, ControllerSettings& settings) {return settingsChangeActions;}

  float mControllerStickDecayTime;
  enum ControllerControls
  {
    CONTROLLER_STICK_ROLL,
    CONTROLLER_STICK_PITCH,
    CONTROLLER_STICK_YAW,
    CONTROLLER_STICK_SPEED,
    CONTROLLER_ACCEL_HORIZONTAL,
    CONTROLLER_ACCEL_VERTICAL,
    CONTROLLER_ARROW_HORIZONTAL,
    CONTROLLER_ARROW_VERTICAL,
    CONTROLLER_CONSTANT,
    CONTROLLER_BUTTON0,
    CONTROLLER_BUTTON1,
    CONTROLLER_BUTTON2,
    CONTROLLER_NUM_CONTROLS
  };

  enum ControllerStick
  {
    CONTROLLER_RIGHT_HORIZONTAL,
    CONTROLLER_RIGHT_VERTICAL,
    CONTROLLER_LEFT_HORIZONTAL,
    CONTROLLER_LEFT_VERTICAL,
    CONTROLLER_NUM_STICKS
  };

  enum ControlClamp
  {
    CONTROL_CLAMP_NONE,
    CONTROL_CLAMP_POSITIVE,
    CONTROL_CLAMP_NEGATIVE,
    CONTROL_CLAMP_MAX
  };

  struct ControlSetting
  {
    // Note that each control input has a normal range of -1 to +1
    ControlSetting() : mAutoCentre(true), mClamp(CONTROL_CLAMP_NONE), mScale(1.0f), mExponential(1.0f), mTrim(0.0f) {}
    bool WriteToDoc(TiXmlDocument& doc, int i, int j) const;
    bool ReadFromDoc(TiXmlDocument& doc, int i, int j);

    bool mAutoCentre;
    ControlClamp mClamp;
    float mScale;
    float mExponential;
    float mTrim;
  };

  struct Mix
  {
    float mMixElevatorToFlaps;
    float mMixAileronToRudder;
    float mMixFlapsToElevator;
    float mMixBrakesToElevator;
    float mMixRudderToElevator;
    float mMixRudderToAileron;
  };

  static const int mCurrentVersion = 3;
  int mVersion;

  static const int CONTROLLER_MAX_NUM_ALT_SETTINGS=3;

  ControllerControls mControlPerChannel[Controller::MAX_CHANNELS];

  std::string mButtonNames[3];

  float mControllerAccelerometerOffsetAngle;
  float mControllerAccelerometerXSensitivity;
  float mControllerAccelerometerYSensitivity;

  bool mResetAltSettingOnLaunch;
  bool mTreatThrottleAsBrakes;
  int mNumAltSettings;
  int mCurrentAltSetting;
  ControlSetting& GetControlSetting(ControllerControls control, int alt) {return mControlSettings[alt % mNumAltSettings][control];}
  ControlSetting& GetControlSetting(ControllerControls control) {return mControlSettings[mCurrentAltSetting % mNumAltSettings][control];}
  ControlSetting* GetControlSettings() {return mControlSettings[mCurrentAltSetting];}
  const ControlSetting* GetControlSettings() const {return mControlSettings[mCurrentAltSetting];}
  Mix& GetMix(int alt) {return mMixes[alt];}
  Mix& GetCurrentMix() {return mMixes[mCurrentAltSetting];}
  const std::string& GetCurrentName() const {return mAltSettingNames[mCurrentAltSetting];}
  const std::string& GetName(int alt) const {return mAltSettingNames[alt];}
private:
  std::string mAltSettingNames[CONTROLLER_MAX_NUM_ALT_SETTINGS];
  ControlSetting mControlSettings[CONTROLLER_MAX_NUM_ALT_SETTINGS][CONTROLLER_NUM_CONTROLS]; // array this way round so we can extract the single active array
  Mix mMixes[CONTROLLER_MAX_NUM_ALT_SETTINGS];
};

//---------------------------------------------------------------------------------------------------------------------
struct JoystickSettings : public Settings
{
  JoystickSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  enum JoystickControls
  {
    JOYSTICK_0,
    JOYSTICK_1,
    JOYSTICK_2,
    JOYSTICK_3,
    JOYSTICK_4,
    JOYSTICK_5,
    JOYSTICK_6,
    JOYSTICK_7,
    JOYSTICK_NUM_CONTROLS,
  };

  struct JoystickAnalogueOverride
  {
    JoystickAnalogueOverride(ControllerSettings::ControllerControls control = ControllerSettings::CONTROLLER_NUM_CONTROLS, float scale = 1.0f, float deadZone = 0.0f) 
      : mControl(control), mScalePositive(scale), mScaleNegative(scale), mDeadZone(deadZone), mOffset(0.0f) {}

    ControllerSettings::ControllerControls mControl;
    float mScalePositive;
    float mScaleNegative;
    float mOffset;
    float mDeadZone;
  };

  struct JoystickButtonOverride
  {
    enum ButtonControl
    {
      CONTROL_BUTTON_NONE,
      CONTROL_BUTTON_RATES,
      CONTROL_BUTTON_RATESCYCLE,
      CONTROL_BUTTON_RELAUNCH,
      CONTROL_BUTTON_CAMERA,
      CONTROL_BUTTON_PAUSEPLAY,
      CONTROL_BUTTON_BUTTON0,
      CONTROL_BUTTON_BUTTON1,
      CONTROL_BUTTON_BUTTON2,
      CONTROL_BUTTON_BUTTON0TOGGLE,
      CONTROL_BUTTON_BUTTON1TOGGLE,
      CONTROL_BUTTON_BUTTON2TOGGLE,
      NUM_BUTTON_CONTROLS
    };

    JoystickButtonOverride(ButtonControl control = CONTROL_BUTTON_NONE) : mControl(control), mInvert(false) {}
    ButtonControl mControl;
    bool          mInvert;
  };

  static const size_t JOYSTICK_NUM_BUTTONS = 32;

  static const int mCurrentVersion = 3;
  int mVersion;
  bool mEnableJoystick;
  bool mAdjustForCircularSticks;
  JoystickAnalogueOverride mJoystickAnalogueOverrides[JOYSTICK_NUM_CONTROLS];
  JoystickButtonOverride mJoystickButtonOverrides[JOYSTICK_NUM_BUTTONS];
  JoystickButtonOverride mJoystickAButtonOverrides[JOYSTICK_NUM_CONTROLS];
};

//---------------------------------------------------------------------------------------------------------------------
struct Options : public Settings
{
  Options();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, Options& settings);

  enum FreeFlyMode
  {
    FREEFLYMODE_TRAINERGLIDER,
    FREEFLYMODE_TRAINERPOWERED,
    FREEFLYMODE_MAX
  };
  enum Units
  {
    UNITS_MPS,
    UNITS_KPH,
    UNITS_MPH,
    UNITS_MAX
  };

  static const int mCurrentVersion = 3;
  int mVersion;

  FrameworkSettings mFrameworkSettings;
  Language mLanguage;
  FreeFlyMode mFreeFlyMode;
  bool  mFreeFlyOnStartup;
  bool  mEnableSocketController;
  bool  mSetWindDirectionOnWalkabout;
  bool  mUseBackButtonToExit;
  bool  mEnableWalkabout;
  bool  mEnableObjectEditing;
  bool  mUseAeroplanePreferredController;

  float mVolumeScale;
  float mVariometerVolume;
  float mWindVolume;
  float mOutsideAeroplaneVolume;
  float mInsideAeroplaneVolume;

  float mGroundViewAutoZoom;
  float mGroundViewAutoZoomScale;
  float mGroundViewFieldOfView;
  float mGroundViewHorizonAmount;
  float mGroundViewLag;
  float mGroundViewYawOffset;
  float mGroundViewPitchOffset;
  bool  mGroundViewFollow;
  float mAeroplaneViewFieldOfView;
  bool mEnableZoomView;
  bool mOnlyPlaneInZoomView;
  bool mSmokeOnlyInMainView;
  float mZoomViewSize;

  bool mEnableStereoscopy;
  float mStereoSeparation;

  float mMaxNearClipDistance;
  bool  mSeparateSpecular;

  bool mFreeFlightDisplayTime;
  bool mFreeFlightDisplaySpeed;
  bool mFreeFlightDisplayAirSpeed;
  bool mFreeFlightDisplayMaxSpeed;
  bool mFreeFlightDisplayAscentRate;
  bool mFreeFlightDisplayAltitude;
  bool mFreeFlightDisplayDistance;
  bool mFreeFlightColourText;
  bool mFreeFlightTextAtTop;
  float mFreeFlightTextBackgroundOpacity;
  float mFreeFlightTextBackgroundColour;
  Units mFreeFlightUnits;
  int   mFreeFlightMaxAI;

  bool mDisplayFPS;

  float mRaceVibrationAmount;
  float mRaceBeepVolume;

  static const float LIMBO_MAX_DIFFICULTY_MULTIPLIER;
  float mLimboDifficultyMultiplier;

  bool  mEnableScoreloop;

  float mWindArrowSize;
  int mWindsockOpacity;

  float mPauseButtonsSize;
  int   mPauseButtonOpacity;

  // Normal mode - 1
  int mControllerMode;
  bool mControllerBrakesForward;
  /// 0 - 1, where 1 means filling the space
  float mControllerSize;
  bool mControllerUseAbsolutePosition;
  bool mControllerSquare;
  bool mControllerStaggered;
  enum ControllerStyle
  {
    CONTROLLER_STYLE_CROSS,
    CONTROLLER_STYLE_BOX,
    CONTROLLER_STYLE_CROSS_AND_BOX
  };
  ControllerStyle mControllerStyle;
  int  mControllerAlpha;
  float mControllerHorOffset;
  float mControllerVerOffset;

  int  mControllerStickAlpha;
  bool mControllerStickCross;
  bool mControllerEnableTrim;
  float mControllerTrimSize;

  int mJoystickID;

  float mGroundViewTerrainLOD;
  float mAeroplaneViewTerrainLOD;
  bool  mGroundViewUpdateTerrainLOD;
  bool  mTerrainWireframe;
  enum DrawCoMType {COM_NONE, COM_CROSS, COM_BOX, COM_MAX};
  DrawCoMType  mDrawAeroplaneCoM;
  bool  mDrawSunPosition;
  enum ShadowType {NONE, BLOB, PROJECTED};
  ShadowType mControlledPlaneShadows;
  ShadowType mOtherShadows;
  int   mProjectedShadowDetail;
  bool  mDrawLaunchMarker;
  bool  mDrawGroundPosition;
  enum SkyGridOverlay {SKYGRID_NONE, SKYGRID_SPHERE, SKYGRID_BOX};
  SkyGridOverlay mSkyGridOverlay;
  enum SkyGridAlignment {SKYGRIDALIGN_ALONGWIND, SKYGRIDALIGN_CROSSWIND, SKYGRIDALIGN_ALONGRUNWAY, SKYGRIDALIGN_CROSSRUNWAY};
  SkyGridAlignment mSkyGridAlignment;
  float mSkyGridDistance;
  static const int MAX_MAX_MARKERS_PER_THERMAL = 4;
  int   mMaxMarkersPerThermal;
  bool  mDrawThermalWindField;
  float mGraphDuration;
  float mGraphFPS;
  float mGraphAltitude;
  float mGraphAirSpeed;
  float mGraphGroundSpeed;
  float mGraphClimbRate;
  float mGraphWindSpeed;
  float mGraphWindVerticalVelocity;
  int   mNumWindStreamers;
  float mWindStreamerTime;
  float mWindStreamerDeltaZ;
  bool mStallMarkers;
  float mTimeScale;

  int   mAerofoilInfo;
  float mAerofoilPlotAngleRange;
  float mAerofoilPlotCLMax;
  float mAerofoilPlotCDMax;
  float mAerofoilPlotCMMax;
  float mAerofoilPlotLDMax;
  bool  mAerofoilPlotReference;
  bool mAerofoilPlotPolar;

  enum RenderPreference
  {
    RENDER_PREFER_COMPONENTS,
    RENDER_PREFER_3DS,
    RENDER_PREFER_BOTH,
    RENDER_PREFER_MAX
  };
  RenderPreference mRenderPreference;
  bool m16BitTextures;
  float mAmbientLightingScale;
  float mDiffuseLightingScale;
  int mBasicTextureDetail; /// texture size is 2^mBasicTextureDetail
  int mMaxSkyboxDetail;

  bool mEnableSmoke;
  float mSmokeQuality;
};

//---------------------------------------------------------------------------------------------------------------------
struct LightingSettings : public Settings
{
  LightingSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  bool ReadBasicsFromDoc(TiXmlDocument& doc) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, LightingSettings& settings);

  static const int mCurrentVersion = 2;
  int mVersion;

  std::string mSkyboxName;
  std::string mThumbnail;
  std::string mTitle;
  std::string mInfo;

  int mSunBearingOffset; ///< degrees
  float mGamma;
  float mThermalActivity; // 0 to 1
};

//---------------------------------------------------------------------------------------------------------------------
struct TerrainSettings : public Settings
{
  TerrainSettings();
  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, TerrainSettings& settings);

  // Common settings
  enum Type
  {
    TYPE_MIDPOINT_DISPLACEMENT,
    TYPE_RIDGE,
    TYPE_PANORAMA,
    TYPE_FILE_TERRAIN,
    TYPE_PANORAMA_3D,
    NUM_TYPES
  };

  static const int mCurrentVersion = 2;
  int mVersion;

  static const char* mTerrainTypes[NUM_TYPES];

  Type  mType;

  // Generic settings

  // For the plain
  bool  mRenderPlain;
  bool  mCollideWithPlain;
  float mPlainInnerRadius;
  float mPlainFogDistance;
  float mPlainHeight;

  int   mHeightmapDetail; ///< 2^mHeightmapDetail
  float mTerrainSize; ///< Extents
  float mCoastEnhancement;
  bool  mSimplifyUnderPlain;

  float mSurfaceRoughness;
  float mFriction;

  float mTerrainTextureScaleX;
  float mTerrainTextureScaleY;
  float mTerrainDetailTextureScaleX;
  float mTerrainDetailTextureScaleY;
  float mPlainDetailTextureScaleX;
  float mPlainDetailTextureScaleY;

  float mBeachColourR;
  float mBeachColourG;
  float mBeachColourB;
  float mPlainColourR;
  float mPlainColourG;
  float mPlainColourB;

  std::string mBasicTexture;
  std::string mDetailTexture;

  // Midpoint settings
  float mMidpointDisplacementHeight;
  float mMidpointDisplacementRoughness;
  float mMidpointDisplacementEdgeHeight;
  float mMidpointDisplacementUpwardsBias;
  int   mMidpointDisplacementFilterIterations;
  int   mMidpointDisplacementSeed;

  // Ridge settings
  float mRidgeHeight;
  float mRidgeMaxHeightFraction;
  float mRidgeWidth;
  float mRidgeEdgeHeight;
  float mRidgeHorizontalVariationWavelength;
  float mRidgeHorizontalVariation;
  float mRidgeVerticalVariationFraction;

  // Panorama settings
  std::string mPanoramaName;

  // File terrain settings
  std::string mFileTerrainName;
  float mFileTerrainMinZ;
  float mFileTerrainMaxZ;
};

//---------------------------------------------------------------------------------------------------------------------
struct ObjectsSettings : public Settings
{
  ObjectsSettings();
  ~ObjectsSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, ObjectsSettings& settings) const;
  void GetStats(size_t& numObjects, size_t& numStaticVisible, size_t& numStaticInvisible, size_t& numDynamicVisible) const;

  static const int mCurrentVersion = 3;
  int mVersion;

  struct Box
  {
    // Constructor will create if create is set
    Box(const Vector3& extents, const Transform& tm, const Vector3& colour, const char* textureFile, float mass, bool visible, bool enableShadows, bool create);
    Box();

    // This will create an object given the definition (deleting any original)
    void Create();

    void SetExtents(const Vector3& extents);
    void SetColour(const Vector3& colour);
    void SetInitialTM(const Transform& tm);
    void SetShadow(bool shadow);

    // Definition
    Transform mTM;
    Vector3 mExtents;
    Vector3 mColour;
    std::string mTextureFile;
    float mMass; // 0 if kinematic
    bool mShadow;
    bool mVisible;

    // The instance
    class BoxObject* mObject;
  };

  typedef std::vector<Box> Boxes;
  Boxes mBoxes;
  bool mForceAllVisible;
  int mResetCounter; // Used to detect when the objects should be reset
};

//---------------------------------------------------------------------------------------------------------------------
struct AIEnvironmentSettings : public Settings
{
  AIEnvironmentSettings();
  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AIEnvironmentSettings& settings);

  enum SceneType
  {
    SCENETYPE_FLAT,
    SCENETYPE_SLOPE,
  };
  static const int mCurrentVersion = 2;
  int mVersion;

  SceneType mSceneType;
};

//---------------------------------------------------------------------------------------------------------------------
struct EnvironmentSettings : public Settings
{
  EnvironmentSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  bool ReadBasicsFromDoc(TiXmlDocument& doc) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, EnvironmentSettings& settings);

  static const int mCurrentVersion = 2;
  int mVersion;

  int mAvailability;
  // 1 Slope
  // 2 Flat
  // 4 Panoramic
  // 8 3D
  uint32 mType;
  std::string mThumbnail;
  std::string mTitle;
  std::string mInfo;
  std::string mWWW;

  TerrainSettings       mTerrainSettings;
  AIEnvironmentSettings mAIEnvironmentSettings;
  std::string           mObjectsSettingsFile;

  Vector3 mObserverPosition;
  float mWindSpeed;
  float mWindBearing; ///< degrees
  float mWindLiftSmoothing; ///< Larger values result in a smoother (flatter) windfield at high levels
  float mVerticalWindDecayDistance;

  float mWindGustTime; // s
  float mWindGustAmplitudeFraction; // fraction of mWindSpeed
  float mWindGustBearingAmplitude; // degrees

  float mSeparationTendency;
  float mRotorTendency;

  float mTurbulenceAmount;
  float mSurfaceTurbulence;
  float mShearTurbulence;
  float mDeadAirTurbulence;
  float mBoundaryLayerDepth;

  bool mAllowBungeeLaunch;

  float mThermalDensity; // per km^2
  float mThermalRange;
  float mThermalAverageLifeSpan;
  float mThermalAverageDepth;
  float mThermalAverageCoreRadius;
  float mThermalAverageDowndraftRadius;
  float mThermalAverageUpdraftSpeed;
  float mThermalAverageAscentRate;
  float mThermalExpansionOverLifespan;

  enum RunwayType {RUNWAY_NONE, RUNWAY_RUNWAY, RUNWAY_CIRCLE};
  RunwayType mRunwayType;
  Vector3    mRunwayPosition;
  float      mRunwayAngle;
  float      mRunwayLength;
  float      mRunwayWidth;
};

//---------------------------------------------------------------------------------------------------------------------
struct ChallengeSettings : public Settings
{
  ChallengeSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  bool ReadBasicsFromDoc(TiXmlDocument& doc) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  void CalculateChecksum(const std::string& file);

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, ChallengeSettings& settings);

  enum ChallengeMode
  {
    CHALLENGE_FREEFLY,
    CHALLENGE_RACE,
    CHALLENGE_LIMBO,
    CHALLENGE_DURATION
  };

  // General settings
  static const int mCurrentVersion = 2;
  int mVersion;

  ChallengeMode mChallengeMode;
  uint32 mChallengeID;

  uint32 mChecksum;

  std::string mTitle;
  std::string mInfo;
  std::string mThumbnail;

  bool mAllowAeroplaneSettings;
  bool mAllowWindStrengthSetting;
  bool mAllowEnvironmentSettings;

  std::string mAeroplaneSettingsFile;
  std::string mEnvironmentSettingsFile;
  std::string mLightingSettingsFile;

  bool mDefaultToChaseView;

  // FreeFly settings

  // Race settings. To go through a gate mPos1 is on the left and mPos2 on the right
  float mReferenceTime; ///< Time for a "good" score
  float mPreparationTime;
  float mMaxHeightMultiplier;
  float mMaxHeightForBonus;

  // Limbo settings
  float mLimboDuration;
  float mLimboRequiredAltitude;
  float mLimboRelaunchPenalty;
  bool  mLimboRelaunchWhenStationary;

  // Generic settings
  float mWindSpeedOverride; //< < 0 to ignore
  float mTurbulenceOverride; //< < 0 to ignore
  float mThermalDensityOverride; // <0 to ignore 
  int mThermalSeed;

  struct Gate
  {
    Vector3 mPos1;
    Vector3 mPos2;
    Vector3 mLeftToRightDir;
    Vector3 mFwdDir;
    enum Type
    {
      TYPE_GATE,
      TYPE_POST,
      TYPE_LIMBO,
    };
    Type mType;
    float mRadius;
    float mHeight;
    int mID;
    bool mDraw1;
    bool mDraw2;

    Vector4 mColour1;
    Vector4 mColour2;
    Vector4 mTargetColour1;
    Vector4 mTargetColour2;

    /// Returns 1 for a pass through, 0 for no pass, -1 for a pass the wrong way!
    int IsThrough(const Vector3& oldPos, const Vector3& pos, float heightScale) const;
    Vector3 GetClosestPointOnGateToPosition(const Vector3& position) const;

    Gate();
  };
  typedef CIwArray<Gate> Gates;
  Gates mGates;

  typedef CIwArray<int> Checkpoints;
  // These are indices into the Gates array
  Checkpoints mCheckpoints;

  // This is the index into the Checkpoints array
  int mCheckpointForTimer;
};

//---------------------------------------------------------------------------------------------------------------------
struct AIAeroplaneSettings : public Settings
{
  AIAeroplaneSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AIAeroplaneSettings& settings);

  // These are generic
  static const int mCurrentVersion = 2;
  int mVersion;
  bool  mAllowAIControl;
  float mWaypointTolerance;
  bool  mCanTow;

  // These controls are shared across types
  float mControlMaxBankAngle;
  float mControlBankAnglePerHeadingChange;
  float mControlRollControlPerRollAngle;
  float mControlPitchControlPerRollAngle;
  float mControlMaxRollControl;
  float mControlMaxPitchControl;
  float mControlRollTimeScale;
  float mControlPitchTimeScale;

  // These are specific to glider control
  float mGliderControlMinSpeed;
  float mGliderControlCruiseSpeed;
  float mGliderControlSpeedPerAltitudeChange;
  float mGliderControlSlopePerExcessSpeed;
  float mGliderControlHeadingChangeForNoSlope;
  float mGliderControlPitchControlPerGlideSlope;
  float mGliderControlMinAltitude;

  // Specific to powered control
  float mPoweredControlMaxSlopeDown;
  float mPoweredControlMaxSlopeUp;
  float mPoweredControlPitchControlPerSlope;
  float mPoweredControlThrottleControlPerAltitude;
  float mPoweredControlThrottleControlPerSpeed;
  float mPoweredControlCruiseSpeed;

  // Specific to powered flying
  float mPoweredTakeoffDistance;
  float mPoweredMaxWaypointTime;
  float mPoweredMaxDistance;
  float mPoweredMinHeight;
  float mPoweredMaxHeight;
  float mPoweredAltitudeForAux;

  // These are specific to slope soaring
  float mSlopeMinUpwindDistance;
  float mSlopeMaxUpwindDistance;
  float mSlopeMinLeftDistance;
  float mSlopeMaxLeftDistance;
  float mSlopeMinUpDistance;
  float mSlopeMaxUpDistance;
  float mSlopeMaxWaypointTime;

  // These are specific to flat soaring
  float mFlatMaxDistance;
  float mFlatMaxWaypointTime;
};

//---------------------------------------------------------------------------------------------------------------------
struct AeroplaneSettings : public Settings
{
  AeroplaneSettings();
  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AeroplaneSettings& settings);

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  bool ReadBasicsFromDoc(TiXmlDocument& doc) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  AIAeroplaneSettings mAIAeroplaneSettings;

  static const int mCurrentVersion = 2;
  int mVersion;

  int mAvailability;
  // 1 Glider
  // 2 Powered
  uint32 mType;
  enum AIType
  {
    AITYPE_GLIDER,
    AITYPE_POWERED,
    AITYPE_HELI,
    AITYPE_CONTROLLINE,
  };
  AIType mAIType;
  std::string mName;
  std::string mThumbnail;
  std::string mTitle;
  std::string mInfo;
  std::string mWWW;
  std::string mPreferredController;
  bool mShowButton[2];
  float mExtraMassPerCent;
  Vector3 mExtraMassOffset; // Offset relative to CoM, in frame of aircraft

  int mColourScheme;
  float mColourOffset;
  bool mRelaunchWhenStationary;
  float mRelaunchTime;

  enum FlatLaunchMethod
  {
    FLAT_LAUNCH_HAND,
    FLAT_LAUNCH_BUNGEE,
    FLAT_LAUNCH_AEROTOW
  };
  FlatLaunchMethod mFlatLaunchMethod;

  Vector3 mCrashDeltaVel;
  Vector3 mCrashDeltaAngVel;
  float   mCrashSuspensionForceScale;

  float mDragScale;
  float mSizeScale;
  float mMassScale;
  float mEngineScale;

  float mLaunchSpeed;
  float mLaunchAngleUp; // degrees
  float mLaunchUp;
  float mLaunchForwards;
  float mLaunchLeft;
  float mLaunchOffsetUp;

  Vector3 mBellyHookOffset;
  Vector3 mNoseHookOffset;
  Vector3 mTailHookOffset;

  float mMaxBungeeLength;
  float mMaxBungeeAcceleration;

  std::string mTugName;
  float mTugSizeScale;
  float mTugMassScale;
  float mTugEngineScale;
  float mTugTargetSpeed;
  float mTugMaxClimbSlope;
  float mAeroTowRopeLength;
  float mAeroTowRopeStrength;
  float mAeroTowRopeMassScale;
  float mAeroTowRopeDragScale;
  float mAeroTowHeight;
  float mAeroTowCircuitSize;


  int mHasVariometer;

  int mTetherLines;
  bool mTetherRequiresTension;
  Vector4 mTetherColour;
  Vector3 mTetherPhysicsOffset;
  Vector3 mTetherVisualOffset;
  float mTetherDistanceLeft;

  float mCameraTargetPosFwd;
  float mCameraTargetPosUp;
  float mChaseCamDistance;
  float mChaseCamHeight;
  float mChaseCamVerticalVelMult;
  float mChaseCamFlexibility;
  float mCockpitCamPitch;

  enum {MAX_NUM_SMOKES_PER_PLANE = 3};
  struct SmokeSource
  {
    SmokeSource();
    bool WriteToDoc(TiXmlDocument& doc, int i) const;
    bool ReadFromDoc(TiXmlDocument& doc, int i);
    SettingsChangeActions GetSettingsChangeActions(
      SettingsChangeActions settingsChangeActions, const SmokeSource& settings) const;

    bool                mEnable;
    Vector3             mOffset;
    Vector3             mVel;
    Vector3             mColour;
    Controller::Channel mChannelForAlpha;
    Controller::Channel mChannelForRate;
    float               mMinAlpha;
    float               mMaxAlpha;
    float               mMinRate;
    float               mMaxRate;
    int                 mMaxParticles;
    float               mInitialSize;
    float               mFinalSize;
    float               mLifetime;
    float               mDampingTime;
    float               mVelJitter;
    float               mEngineWash;
    float               mHueCycleFreq;
  };
  SmokeSource mSmokeSources[MAX_NUM_SMOKES_PER_PLANE];
};


//---------------------------------------------------------------------------------------------------------------------
struct Statistics : public Settings
{
  Statistics();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  static const int mCurrentVersion = 2;
  int mVersion;

  static const int LATEST_PICASIM_SETTINGS_VERSION;
  int mPicaSimSettingsVersion;
  int mPicaSimBuildNumber;

  float mSmoothedFPS;
  float mFPS;
  int mNumDepthBits;
  enum AppType {APP_DEMO, APP_FREE, APP_FULL};
  AppType mAppType;
  bool mIsAmazon;
  bool mIsInTrialMode;

  float mMaxFlightTime;
  float mTotalFlightTime;

  struct Score
  {
    Score(double result, double minorResult) : mResult(result), mMinorResult(minorResult) {}
    Score() : mResult(0.0f), mMinorResult(0.0f) {}
    double mResult;
    double mMinorResult;
  };
  // Store best scores per mode
  typedef std::map<uint32, Score> Scores;
  Scores mHighScores;

  std::string mLastScoreloopUsername;

  mutable uint32 mLoadCounter;
  mutable bool mLoadedAeroplane;
  mutable bool mLoadedTerrain;
  mutable bool mLoadedOptions;
};

//---------------------------------------------------------------------------------------------------------------------
struct AIControllersSettings : public Settings
{
  AIControllersSettings();
  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, AIControllersSettings& settings);

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  struct AIControllerSetting
  {
    AIControllerSetting(const std::string& file = "") : 
      mAeroplaneFile(file), mColourOffset(0.0f), mEnableDebugDraw(true), mIncludeInCameraViews(true) {}
    std::string mAeroplaneFile;
    float mColourOffset;
    bool  mEnableDebugDraw;
    bool  mIncludeInCameraViews;
  };

  static const int mCurrentVersion = 2;
  int mVersion;
  typedef std::vector<AIControllerSetting> AIControllers;
  AIControllers mAIControllers;
  bool          mEnableDebugDraw;
  bool          mIncludeInCameraViews;
  bool          mCreateMaxNumControllers;
  float         mRandomColourOffset;
  float         mLaunchDirection;
  float         mLaunchSeparationDistance;
};

//---------------------------------------------------------------------------------------------------------------------
struct GameSettings : public Settings
{
  GameSettings();

  bool WriteToDoc(TiXmlDocument& doc) const OVERRIDE;
  bool ReadFromDoc(TiXmlDocument& doc, bool readAll) OVERRIDE;
  void Upgrade(TiXmlDocument& doc) OVERRIDE;

  SettingsChangeActions GetSettingsChangeActions(SettingsChangeActions settingsChangeActions, GameSettings& settings);

  static const int mCurrentVersion = 2;
  int mVersion;

  Options             mOptions;
  AeroplaneSettings   mAeroplaneSettings;
  EnvironmentSettings mEnvironmentSettings;
  ObjectsSettings     mObjectsSettings;
  LightingSettings mLightingSettings;
  AIControllersSettings mAIControllersSettings;
  ControllerSettings mControllerSettings;
  JoystickSettings mJoystickSettings;
  ChallengeSettings mChallengeSettings;

  Statistics mStatistics;
};

#endif
