#ifndef PICASIM_H
#define PICASIM_H

#include "Environment.h"
#include "Aeroplane.h"
#include "HumanController.h"
#include "Observer.h"
#include "GameSettings.h"

#include "Framework.h"

#include <vector>
#include <string>

#include <s3eTypes.h>

enum GraphIDs
{
  GRAPH_FPS,
  GRAPH_AIR_SPEED,
  GRAPH_GROUND_SPEED,
  GRAPH_CLIMB_RATE,
  GRAPH_WIND_SPEED,
  GRAPH_WIND_VERTICAL_VELOCITY,
  GRAPH_ALTITUDE,
  GRAPH_TOWFORCE,
};

enum CameraID
{
  CAMERA_AEROPLANE,
  CAMERA_CHASE,
  CAMERA_GROUND,
  CAMERA_ZOOM
};

class PicaSim
{
public:
  enum Mode
  {
    MODE_GROUND,
    MODE_AEROPLANE,
    MODE_CHASE,
    MODE_WALK,
    MODE_MAX
  };

  enum Status
  {
    STATUS_FLYING,
    STATUS_PAUSED
  };

  enum UpdateResult
  {
    UPDATE_CONTINUE,
    UPDATE_START,
    UPDATE_QUIT
  };

  /// Creates the singleton. Returns false if it fails, and will tidy up
  static bool Init(GameSettings& gameSettings, LoadingScreenHelper* loadingScreen);

  /// Destroys the singleton
  static void Terminate();

  static PicaSim& GetInstance() {return *mInstance;}

  static bool IsCreated() {return mInstance != 0;}

  // Returns when it's time to quit
  UpdateResult Update(int64 deltaTimeMs);

  Mode GetMode() const {return mMode;}
  void SetMode(Mode mode) {mMode = mode;}

  Status GetStatus() const {return mStatus;}
  void SetStatus(Status status) {mStatus = status;}

  GameSettings& GetSettings() {return mGameSettings;}
  const GameSettings& GetSettings() const {return mGameSettings;}

  Aeroplane* GetPlayerAeroplane() {return mPlayerAeroplane;}
  const Aeroplane* GetPlayerAeroplane() const {return mPlayerAeroplane;}

  /// This returns the unclamped total timestep delta
  float GetCurrentUpdateDeltaTime() const {return mCurrentDeltaTime;}

  const Viewport& GetMainViewport() const {return *mViewport;}

  const Observer& GetObserver() const {return *mObserver;}
  Observer& GetObserver() {return *mObserver;}

  void ReinitOverlays();

  float GetTimeScale() const {return mActualTimeScale;}

  /// Adds the Aeroplane as a potential camera target
  void AddCameraTarget(Aeroplane* cameraAeroplane);
  void RemoveCameraTarget(Aeroplane* cameraAeroplane);
  void AddRemoveCameraTarget(Aeroplane* cameraAeroplane, bool add);

  void AddAeroplane(Aeroplane* aeroplane);
  void RemoveAeroplane(Aeroplane* aeroplane);
  size_t GetNumAeroplanes() const {return mAeroplanes.size();}
  Aeroplane* GetAeroplane(size_t iPlane) {return iPlane < mAeroplanes.size() ? mAeroplanes[iPlane] : 0;}
  const Aeroplane* GetAeroplane(size_t iPlane) const {return iPlane < mAeroplanes.size() ? mAeroplanes[iPlane] : 0;}

  bool GetShowUI() const {return mShowUI;}

  ParticleEngine& GetParticleEngine() {return mParticleEngine;}

  class Challenge* GetChallenge() {return mChallenge;}
  const class Challenge* GetChallenge() const {return mChallenge;}

private:
  typedef std::vector<class BoxObject*> BoxObjects;

  PicaSim(GameSettings& gameSettings);

  void HandleMode();
  void UpdateJoystickToggles(bool& joystickRelaunch, bool& joystickChangeView, bool& joystickPausePlay);
  void HandleJoystickToggle(const JoystickSettings::JoystickButtonOverride& j, float buttonDown,
    bool& joystickRelaunch, bool& joystickChangeView, bool& joystickPausePlay);

  void ShowHelpOverlays();

  int ShowInGameDialog(float width, float height, const char* title, const char* text, const char* button0, const char* button1 = 0, const char* button2 = 0);

  static PicaSim* mInstance;

  Viewport* mViewport;
  Viewport* mZoomViewport;

  class Challenge* mChallenge;

  BoxObjects mBoxObjects;

  Aeroplane* mPlayerAeroplane;
  HumanController* mPlayerController;

  Aeroplanes mAeroplanes;
  Aeroplanes mCameraAeroplanes;
  size_t mCameraAeroplaneIndex;

  Observer* mObserver;
  ParticleEngine mParticleEngine;

  class ButtonOverlay* mPauseOverlay;
  class ButtonOverlay* mHelpOverlay;
  class ButtonOverlay* mStartMenuOverlay;
  class ButtonOverlay* mResumeOverlay;
  class ButtonOverlay* mSettingsMenuOverlay;
  class ButtonOverlay* mRelaunchOverlay;
  class ButtonOverlay* mChangeViewOverlay;
  class ButtonOverlay* mWalkaboutOverlay;
  class ButtonOverlay* mControllerOverlay;

  class WindsockOverlay* mWindsockOverlay;
  class TrialOverlay* mTrialOverlay;

  bool mShouldExit;
  Mode mMode;
  Status mStatus;
  float mCurrentDeltaTime;
  float mTimeSinceEnabled;
  float mActualTimeScale;
  float mControllerOverlayTextOpacity;
  float mRemainingTrialTime;
  bool mShowUI;
  bool mShowHelpAfterLoading; // Show the basic help after loading

  uint32 mUpdateCounter;

  GameSettings& mGameSettings;

  AudioManager::Sound* mSound;
  AudioManager::SoundChannel mSoundChannel;

  class ConnectionListener* mConnectionListener;

  // For when using the joystick
  bool mPrevJoystickRelaunch;
  bool mPrevJoystickCamera;
  bool mPrevJoystickPausePlay;
  bool mPrevJoystickRatesCycle;
  bool mPrevJoystickButton0Cycle;
  bool mPrevJoystickButton1Cycle;
  bool mPrevJoystickButton2Cycle;
};

#endif

