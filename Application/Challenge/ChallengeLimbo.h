#ifndef CHALLENGELIMBO_H
#define CHALLENGELIMBO_H

#include "Challenge.h"
#include "Gate.h"
#include "RenderManager.h"
#include "Scoreloop.h"
#include "AudioManager.h"
// #include "Gate.h"
// #include "Scoreloop.h"
// #include "Menus/ScoreloopMenu.h"
// #include "GameSettings.h"

class ButtonOverlay;

//----------------------------------------------------------------------------------------------------------------------
class ChallengeLimbo : public Challenge, public RenderGxObject
{
public:
  /// This will ensure that the limbo is set up according to the challenge settings - 
  /// overriding any aeroplane/scenery settings etc
  ChallengeLimbo(struct GameSettings& gameSettings);
  ~ChallengeLimbo();

  void Init(class Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen) OVERRIDE;
  void Terminate() OVERRIDE;
  void Relaunched() OVERRIDE;
  void ReinitOverlays() OVERRIDE;

  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  ChallengeResult UpdateChallenge(float deltaTime) OVERRIDE;

  void OnScoreSubmitted(SC_Error_t completionStatus);
private:
  void UploadScore();

  float CalculateScore(float challengeDuration, float difficultyMultiplier, float* timeBonus = 0) const;

  ButtonOverlay* mLeaderboardOverlay;

  PhysicalGate mGate;

  bool mNeedToCacheText;

  Vector3 mOldPos;
  float mLimboTime;
  size_t mLimboCount;
  float mLastGateTime;
  float mMaxAltitudeSinceLastGate;

  float mHeightScale;
  float mOriginalDifficulty;

  float mGateColourAmount;
  
  float mOnGroundTime;

  bool mHaveDisplayedNetworkFailure;

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