#include "ChallengeDuration.h"
#include "AeroplanePhysics.h"
#include "ButtonOverlay.h"
#include "Menu.h"
#include "PicaDialog.h"
#include "Trace.h"
#include "PicaSim.h"
#include "Environment.h"
#include "ClassesFontMarmelade.h"
// #include "Aeroplane.h"
// #include "AeroplanePhysics.h"
// #include "GameSettings.h"
// #include "WindsockOverlay.h"
// #include "ButtonOverlay.h"
// #include "Menus/Menu.h"
// #include "Menus/ScoreloopMenu.h"
// #include "Menus/PicaDialog.h"

// #include <IwGx.h>
// #include <IwGxFont.h>
// #include <s3eVibra.h>

extern float numButtonSlots;

//----------------------------------------------------------------------------------------------------------------------
ChallengeDuration::ChallengeDuration(GameSettings& gameSettings) : Challenge(gameSettings), mFinalScore(0.0, 0.0, 0)
{
  TRACE_METHOD_ONLY(1);
  const ChallengeSettings& cs = gameSettings.mChallengeSettings;
  TRACE_FILE_IF(1) TRACE("ChallengeDuration::ChallengeDuration num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());
  float buttonSize = gameSettings.mOptions.mPauseButtonsSize / numButtonSlots;
  float buttonOffset = 1.0f / numButtonSlots;
  float paddingFraction = 0.25f;
  mLeaderboardOverlay = new ButtonOverlay("SystemData/Menu/Leaderboards.png", 
    buttonSize, paddingFraction, ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP,  buttonOffset*6, 1.0f, 255, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
ChallengeDuration::~ChallengeDuration()
{
  delete mLeaderboardOverlay;
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::OnScoreSubmitted(SC_Error_t completionStatus)
{
  if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_RACING)
    return;

  Language language = PicaSim::GetInstance().GetSettings().mOptions.mLanguage;

  if (completionStatus == SC_OK)
  {
    mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_UPLOADED;
  }
  else
  {
    if (!mHaveDisplayedNetworkFailure)
    {
      ShowDialog("PicaSim", TXT(PS_SCORESUBMISSIONFAILED), TXT(PS_OK));
      mHaveDisplayedNetworkFailure = true;
    }
    // Try again
    IwAssert(ROWLHOUSE, mScoreloopScore);
    SC_ScoreController_SubmitScore(mScoreController, mScoreloopScore);
  }
}

//---------------------------------------------------------------------------------------------------------------------
static void ScoreSubmissionCallback(void* userData, SC_Error_t completionStatus)
{
  TRACE_FILE_IF(1) TRACE("ScoreSubmissionCallback %d", completionStatus);
  IwAssert(ROWLHOUSE, userData);
  if (!userData)
    return;
  ((ChallengeDuration*) userData)->OnScoreSubmitted(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::UploadScore()
{
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score");
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  float distancePenalty, timeScore;
  Score score(CalculateScore(timeScore, distancePenalty), mDurationTime, cs.mChallengeID);

  SC_Score_SetResult(mScoreloopScore, score.mResult);
  SC_Score_SetMinorResult(mScoreloopScore, score.mMinorResult);
  SC_Score_SetMode(mScoreloopScore, score.mMode);

  int errorCode = SC_ScoreController_SubmitScore(mScoreController, mScoreloopScore);
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score - result = %d", errorCode);

  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_SUBMITTED;
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::ReinitOverlays() 
{
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::Init(Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen)
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  TRACE_FILE_IF(1) TRACE("ChallengeDuration::Init num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());

  mAeroplane = aeroplane;
  RenderManager::GetInstance().RegisterRenderGxObject(this, 0);

  mNeedToCacheText = true;
  mHaveDisplayedNetworkFailure = false;

  SC_Error_t errorCode = SC_Client_CreateScoreController(gScoreloopClient, &mScoreController, &ScoreSubmissionCallback, this);
  errorCode = SC_Client_CreateScore(gScoreloopClient, &mScoreloopScore);
  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_RACING;

  mGotHighScore = false;

  if (cs.mDefaultToChaseView)
    PicaSim::GetInstance().SetMode(PicaSim::MODE_CHASE);

  Validate();

  Relaunched();
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::Terminate()
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
  if (mScoreController)
    SC_ScoreController_Release(mScoreController);
  if (mScoreloopScore)
    SC_Score_Release(mScoreloopScore);
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::Relaunched()
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  mDurationTime = 0.0f;
  mOnGroundTime = 0.0f;
  mAttemptCompleted = false;
  mFinalTimeScore = 0.0f;
  mFinalDistancePenalty = 0.0f;
  mFinalScore = Score(0.0, 0.0, 0);

  SC_ScoreController_Cancel(mScoreController);
  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_RACING;
  Environment::GetInstance().GetThermalManager().SetSeed(cs.mThermalSeed);
  Environment::GetInstance().GetThermalManager().Repopulate();
  Environment::GetInstance().ResetTime();
}

//----------------------------------------------------------------------------------------------------------------------
Challenge::ChallengeResult ChallengeDuration::UpdateChallenge(float deltaTime)
{
  Challenge::UpdateChallenge(deltaTime);

  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  // Make sure of some settings
  gs.mOptions.mEnableWalkabout = false;
  gs.mOptions.mEnableObjectEditing = false;

  if (!mAttemptCompleted)
  {
    mDurationTime += deltaTime;

    float speed = mAeroplane->GetVelocity().GetLength();
    bool touching = mAeroplane->GetPhysics()->GetContactTime() > 0.0f;
    if (speed < 1.0f && touching)
      mOnGroundTime += deltaTime;
    else
      mOnGroundTime = 0.0f;

    if (mOnGroundTime > 3.0f)
    {
      mAttemptCompleted = true;

      Score score(CalculateScore(mFinalTimeScore, mFinalDistancePenalty), mDurationTime, cs.mChallengeID);
      mFinalScore = score;

      // Handle the local score
      Statistics::Scores::iterator it = gs.mStatistics.mHighScores.find(cs.mChallengeID);
      if (
        it == gs.mStatistics.mHighScores.end() ||
        it->second.mResult < score.mResult
        )
      {
        // New high score
        mHighScore = Statistics::Score(score.mResult, score.mMinorResult);
        gs.mStatistics.mHighScores[cs.mChallengeID] = mHighScore;
        mGotHighScore = true;
      }
      else
      {
        mGotHighScore = false;
        mHighScore = it->second;
      }

      if (
        mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_RACING && 
        gs.mOptions.mEnableScoreloop &&
        mValidated
        )
      {
        UploadScore();
      }
    }
  }

  if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_PAUSED && gs.mOptions.mEnableScoreloop && mValidated)
  {
    mLeaderboardOverlay->Enable(true);
    if (mLeaderboardOverlay->IsPressed())
    {
      DisplayScoreloopMenuLeaderboard(gs, 0);
      mNeedToCacheText = true;
      PicaSim::GetInstance().ReinitOverlays();
    }
  }
  else
  {
    mLeaderboardOverlay->Enable(false);
  }

  if (gs.mOptions.mEnableScoreloop && mValidated && mAttemptCompleted)
  {
    mLeaderboardOverlay->Enable(true);
    if (mLeaderboardOverlay->IsPressed())
    {
      if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_UPLOADED)
      {
        float distancePenalty, timeScore;
        Score score(CalculateScore(timeScore, distancePenalty), mDurationTime, cs.mChallengeID);
        DisplayScoreloopMenuLeaderboard(gs, &score);
        PicaSim::GetInstance().ReinitOverlays();
      }
      else
      {
        // Cancel just in case
        SC_ScoreController_Cancel(mScoreController);
        mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_UPLOADED;

        float distancePenalty, timeScore;
        Score score(CalculateScore(timeScore, distancePenalty), mDurationTime, cs.mChallengeID);
        UploadScoreloop(gs, score);
        PicaSim::GetInstance().ReinitOverlays();
      }
      mNeedToCacheText = true;
    }
  }

  return CHALLENGE_CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------
float ChallengeDuration::CalculateScore(float& timeScore, float& distancePenalty) const
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  timeScore = 1000.0f * mDurationTime / cs.mReferenceTime;
  Vector3 delta = mAeroplane->GetTransform().GetTrans() - PicaSim::GetInstance().GetObserver().GetTransform().GetTrans();
  delta.z = 0.0f;
  float distance = delta.GetLength();

  distancePenalty = distance * 10.0f;
  float score = timeScore - distancePenalty;
  if (score < 0.0f)
    score = 0.0f;

  return score;
}


//----------------------------------------------------------------------------------------------------------------------
void ChallengeDuration::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  if (!PicaSim::GetInstance().GetShowUI())
    return;

  CIwGxFont* font = GetFont(gs);
  IwGxFontSetFont(font);

  Colour origColour = IwGxFontGetCol();

  IwGxFontSetCol(0xffa0a0a0);
  IwGxLightingOn();
  uint16_t fontHeight = font->GetHeight();
  IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE);

  char txt[256];
  char timeText[64];
  FormatTime(timeText, mDurationTime);
  const size_t numGates = cs.mCheckpoints.size();
  int numLines = 1;
  if (!mAttemptCompleted)
  {
    sprintf(txt, "%s", timeText);
  }
  else
  {
    // Have finished
    const char* scoreloopText = 0;
    if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_SUBMITTED)
      scoreloopText = " (Uploading)";
    else
      scoreloopText = "";

    char summaryText[256];
    sprintf(summaryText, 
      "Time score = %.1f\n"
      "Distance penalty= %.2f",
      mFinalTimeScore, mFinalDistancePenalty);

    if (mGotHighScore)
    {
      numLines = 6;
      sprintf(txt, "%s\n\nNew local high score = %.1f Time = %s%s", summaryText, mFinalScore.mResult, timeText, scoreloopText);
    }
    else
    {
      numLines = 6;
      char bestTimeText[64];
      sprintf(txt, "%s\n\nScore = %.1f Time = %s%s   Best = %.1f Time = %s", 
        summaryText, mFinalScore.mResult, timeText, scoreloopText, mHighScore.mResult, FormatTime(bestTimeText, (float) mHighScore.mMinorResult));
    }
  }

  IwGxFontSetRect(CIwRect(displayConfig.mLeft,(int16_t)(displayConfig.mBottom + displayConfig.mHeight - (0.5f * (numLines + 1.0f) * fontHeight * 1.2f)),(int16_t)displayConfig.mWidth, fontHeight));
  IwGxFontDrawText(txt);

  if (gs.mOptions.mDisplayFPS)
  {
    IwGxFontSetRect(CIwRect(displayConfig.mLeft,(int16_t)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4),(int16_t)displayConfig.mWidth,fontHeight));
    IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_RIGHT);
    sprintf(txt, "%d", (int) (gs.mStatistics.mSmoothedFPS + 0.5f));
    IwGxFontSetCol(0xff00ffff);
    IwGxFontDrawText(txt);
  }

  if (mNeedToCacheText)
  {
    IwGxFontSetCol(0x00ffffff);
    IwGxFontDrawText("0123456789.");
    mNeedToCacheText = false;
  }

  IwGxFontSetCol(origColour);
  IwGxLightingOff();
}

