#ifndef CHALLENGERACE_H
#define CHALLENGERACE_H

#include "Challenge.h"
#include "Gate.h"
#include "Scoreloop.h"
#include "Menus/ScoreloopMenu.h"
#include "GameSettings.h"

#include "Framework.h"

class ButtonOverlay;

//----------------------------------------------------------------------------------------------------------------------
class ChallengeRace : public Challenge, public RenderGxObject
{
public:
  /// This will ensure that the race is set up according to the challenge settings - 
  /// overriding any aeroplane/scenery settings etc
  ChallengeRace(struct GameSettings& gameSettings);
  ~ChallengeRace();

  void Init(class Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen) OVERRIDE;
  void Terminate() OVERRIDE;
  void Relaunched() OVERRIDE;
  void ReinitOverlays() OVERRIDE;

  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  ChallengeResult UpdateChallenge(float deltaTime) OVERRIDE;

  void OnScoreSubmitted(SC_Error_t completionStatus);
private:
  void UploadScore();
  float CalculateScore() const;
  float CalculateHeightMultiplier() const;

  class WindsockOverlay* mGatePointer;
  ButtonOverlay* mLeaderboardOverlay;

  typedef std::vector<GatePost> GatePosts;
  GatePosts mGatePosts;

  bool mNeedToCacheText;

  Vector3 mOldPos;
  float mRaceTime;
  size_t mTargetCheckpointsIndex; ///< Index into the gate order
  bool mRaceCompleted;
  bool mHaveDisplayedNetworkFailure;
  /// Accumulated height above ground * timestep - used for score multiplier.
  float mHeightTimesTime;

  Statistics::Score mHighScore;
  bool mGotHighScore;

  SC_ScoreController_h mScoreController;
  SC_Score_h mScoreloopScore;
  enum ScoreSubmissionStatus
  {
    SCORESUBMISSIONSTATUS_RACING,
    SCORESUBMISSIONSTATUS_SUBMITTED,
    SCORESUBMISSIONSTATUS_UPLOADED
  };
  ScoreSubmissionStatus mScoreSubmissionStatus;

  AudioManager::Sound* mSound;
  AudioManager::SoundChannel mSoundChannel;
};

#endif