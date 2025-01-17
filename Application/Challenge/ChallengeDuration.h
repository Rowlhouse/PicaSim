#ifndef CHALLENGEDURATION_H
#define CHALLENGEDURATION_H

// #include "Challenge.h"
// #include "Gate.h"
// #include "Scoreloop.h"
// #include "Menus/ScoreloopMenu.h"
// #include "GameSettings.h"

#include "Challenge.h"
#include "RenderManager.h"
#include "Scoreloop.h"
#include "ScoreloopMenu.h"
class ButtonOverlay;

//----------------------------------------------------------------------------------------------------------------------
class ChallengeDuration : public Challenge, public RenderGxObject
{
public:
  /// This will ensure that the race is set up according to the challenge settings - 
  /// overriding any aeroplane/scenery settings etc
  ChallengeDuration(struct GameSettings& gameSettings);
  ~ChallengeDuration();

  void Init(class Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen) OVERRIDE;
  void Terminate() OVERRIDE;
  void Relaunched() OVERRIDE;
  void ReinitOverlays() OVERRIDE;

  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  ChallengeResult UpdateChallenge(float deltaTime) OVERRIDE;

  void OnScoreSubmitted(SC_Error_t completionStatus);
private:
  void UploadScore();
  float CalculateScore(float& timeScore, float& distancePenalty) const;

  ButtonOverlay* mLeaderboardOverlay;

  bool mNeedToCacheText;

  float mDurationTime;
  float mOnGroundTime;
  bool mAttemptCompleted;

  bool mHaveDisplayedNetworkFailure;

  Score mFinalScore;
  float mFinalTimeScore;
  float mFinalDistancePenalty;

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
};

#endif