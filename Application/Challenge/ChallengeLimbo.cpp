#include "ChallengeLimbo.h"
#include "AeroplanePhysics.h"
// #include "ButtonOverlay.h"
// #include "ClassesFontMarmelade.h"
#include "FonctionsMarmelade.h"
#include "Menu.h"
#include "PicaDialog.h"
#include "Trace.h"
#include "PicaSim.h"
#include "Environment.h"
// #include "Aeroplane.h"
// #include "GameSettings.h"
// #include "WindsockOverlay.h"
#include "ScoreloopMenu.h"

// #include <IwGx.h>
// #include <IwGxFont.h>
// #include <s3eVibra.h>

extern float numButtonSlots;

static const Vector4 colourWrongWay(1.0f, 0.0f, 0.0f, 1.0f);
static const Vector4 colourNeedAltitude(0.0f, 1.0f, 0.0f, 1.0f);
static const Vector4 colourWaiting(0.0f, 0.5f, 1.0f, 1.0f);

//----------------------------------------------------------------------------------------------------------------------
ChallengeLimbo::ChallengeLimbo(GameSettings& gameSettings) : Challenge(gameSettings)
{
  TRACE_METHOD_ONLY(1);
  const ChallengeSettings& cs = gameSettings.mChallengeSettings;
  TRACE_FILE_IF(1) TRACE("ChallengeLimbo::ChallengeLimbo num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());
  float buttonSize = gameSettings.mOptions.mPauseButtonsSize / numButtonSlots;
  float buttonOffset = 1.0f / numButtonSlots;
  float paddingFraction = 0.25f;
  mLeaderboardOverlay = new ButtonOverlay("SystemData/Menu/Leaderboards.png", 
    buttonSize, paddingFraction, ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP,  buttonOffset*6, 1.0f, 255, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
ChallengeLimbo::~ChallengeLimbo()
{
  delete mLeaderboardOverlay;
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::OnScoreSubmitted(SC_Error_t completionStatus)
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
  ((ChallengeLimbo*) userData)->OnScoreSubmitted(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::UploadScore()
{
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score");
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  float score = CalculateScore(cs.mLimboDuration, gs.mOptions.mLimboDifficultyMultiplier);
  Score scoreloopScore(score, mLimboCount, cs.mChallengeID);

  SC_Score_SetResult(mScoreloopScore, scoreloopScore.mResult);
  SC_Score_SetMinorResult(mScoreloopScore, scoreloopScore.mMinorResult);
  SC_Score_SetMode(mScoreloopScore, scoreloopScore.mMode);

  int errorCode = SC_ScoreController_SubmitScore(mScoreController, mScoreloopScore);
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score - result = %d", errorCode);

  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_SUBMITTED;
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::ReinitOverlays() 
{
}

//----------------------------------------------------------------------------------------------------------------------
float ChallengeLimbo::CalculateScore(float challengeDuration, float difficultyMultiplier, float* timeBonus) const
{
  float score = mLimboCount * 100.0f;
  score *= difficultyMultiplier;

  if (mLastGateTime > 0.0f)
  {
    float t = 100.0f * (challengeDuration - mLastGateTime) / challengeDuration;
    if (timeBonus)
      *timeBonus = t;
    score += t;
  }

  return score;
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::Init(Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen)
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  TRACE_FILE_IF(1) TRACE("ChallengeLimbo::Init num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());

  gs.mOptions.mLimboDifficultyMultiplier = ClampToRange(gs.mOptions.mLimboDifficultyMultiplier, 1.0f, Options::LIMBO_MAX_DIFFICULTY_MULTIPLIER);

  mAeroplane = aeroplane;
  RenderManager::GetInstance().RegisterRenderGxObject(this, 0);

  mNeedToCacheText = true;
  mLimboTime = 0.0f;
  mLimboCount = 0;

  mOriginalDifficulty = -1.0f;

  mHaveDisplayedNetworkFailure = false;

  SC_Error_t errorCode = SC_Client_CreateScoreController(gScoreloopClient, &mScoreController, &ScoreSubmissionCallback, this);
  errorCode = SC_Client_CreateScore(gScoreloopClient, &mScoreloopScore);
  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_RACING;

  mSound = AudioManager::GetInstance().LoadSound("SystemData/Audio/Beep22050Mono.raw", 22050, false, false, false);
  mSoundChannel = -1;
  if (mSound)
  {
    mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(1.0f, false);
    TRACE_FILE_IF(1) TRACE("Allocated sound channel %d for ChallengeLimbo", mSoundChannel);
    if (mSoundChannel == -1)
      TRACE_FILE_IF(1) TRACE("Failed to allocate sound channel for limbo");
  }

  if (cs.mDefaultToChaseView)
    PicaSim::GetInstance().SetMode(PicaSim::MODE_CHASE);

  Validate();

  Relaunched();
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::Terminate()
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
  mGate.Terminate();
  if (mScoreController)
    SC_ScoreController_Release(mScoreController);
  if (mScoreloopScore)
    SC_Score_Release(mScoreloopScore);

  if (mSoundChannel != -1)
    AudioManager::GetInstance().ReleaseSoundChannel(mSoundChannel);
  if (mSound)
    AudioManager::GetInstance().UnloadSound(mSound);
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::Relaunched()
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  mLimboTime = 0.0f;
  mLimboCount = 0;
  mOnGroundTime = 0.0f;
  mGateColourAmount = 0.0f;
  mMaxAltitudeSinceLastGate = 0.0f;
  mLastGateTime = -1.0f;
  mOldPos = mAeroplane->GetTransform().GetTrans();
  mGotHighScore = false;

  if (mOriginalDifficulty != gs.mOptions.mLimboDifficultyMultiplier)
  {
    float difficulty = gs.mOptions.mLimboDifficultyMultiplier;
    mHeightScale = 1.0f / difficulty;
    if (gs.mChallengeSettings.mGates.size() == 1)
    {
      ChallengeSettings::Gate& gate = gs.mChallengeSettings.mGates[0];
      if (mOriginalDifficulty >= 0.0f)
        mGate.Terminate();
      mGate.Init(gate.mPos1, gate.mPos2, gate.mHeight * mHeightScale, colourWaiting);
    }
    else
    {
      TRACE("Invalid limbo gate count");
    }
    mOriginalDifficulty = difficulty;
  }

  SC_ScoreController_Cancel(mScoreController);
  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_RACING;
  Environment::GetInstance().GetThermalManager().SetSeed(cs.mThermalSeed);
  Environment::GetInstance().GetThermalManager().Repopulate();
  Environment::GetInstance().ResetTime();
}

//----------------------------------------------------------------------------------------------------------------------
Challenge::ChallengeResult ChallengeLimbo::UpdateChallenge(float deltaTime)
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  // Make sure of some settings
  gs.mOptions.mEnableWalkabout = false;
  gs.mOptions.mEnableObjectEditing = false;

  if (cs.mGates.size() != 1)
    return Challenge::CHALLENGE_CONTINUE;

  Vector3 aeroplanePos = mAeroplane->GetTransform().GetTrans();
  float altitude = aeroplanePos.z - Environment::GetTerrain().GetTerrainHeightFast(aeroplanePos.x, aeroplanePos.y, true);
  if (altitude > mMaxAltitudeSinceLastGate)
    mMaxAltitudeSinceLastGate = altitude;

  if (mMaxAltitudeSinceLastGate < cs.mLimboRequiredAltitude)
  {
    mGate.SetBlendColour(colourNeedAltitude, 1.0f);
    mGateColourAmount = 1.0f;
  }
  else
  {
    mGateColourAmount -= deltaTime;
    if (mGateColourAmount < 0.0f)
      mGateColourAmount = 0.0f;
    mGate.SetBlendAmount(mGateColourAmount);
  }

  // Check for auto relaunch if during the challenge
  if (mLimboTime < cs.mLimboDuration)
  {
    float speed = mAeroplane->GetVelocity().GetLength();
    bool touching = mAeroplane->GetPhysics()->GetContactTime() > 0.0f;

    if (speed < 1.0f && touching)
      mOnGroundTime += deltaTime;
    else
      mOnGroundTime = 0.0f;

    if (mOnGroundTime > 3.0f)
    {
      if (cs.mLimboRelaunchWhenStationary || mAeroplane->GetCrashed() )
      {
        mLimboTime += cs.mLimboRelaunchPenalty;
        mOnGroundTime = 0.0f;
        return CHALLENGE_RELAUNCH;
      }
    }
  }

  if (mLimboTime < cs.mLimboDuration)
  {
    mLimboTime += deltaTime;

    // evaluate if we're through the gate
    int isThrough = cs.mGates[0].IsThrough(mOldPos, aeroplanePos, mHeightScale);
    mOldPos = aeroplanePos;
    if (isThrough && mMaxAltitudeSinceLastGate > cs.mLimboRequiredAltitude)
    {
      mGateColourAmount = 1.0f;
      if (isThrough > 0)
      {
        TRACE_FILE_IF(1) TRACE("Through limbo gate");
        mGate.SetBlendColour(colourNeedAltitude, mGateColourAmount);
        ++mLimboCount;
        mMaxAltitudeSinceLastGate = 0.0f;
        mLastGateTime = mLimboTime;
      }
      else
      {
        TRACE_FILE_IF(1) TRACE("Through limbo gate BACKWARDS");
        mGate.SetBlendColour(colourWrongWay, mGateColourAmount);
        if (mLimboCount > 0)
          --mLimboCount;
      }

      // Play sound and vibrate
      int amount = (int) (gs.mOptions.mRaceVibrationAmount * 255);
      if (amount > 0)
        s3eVibraVibrate(ClampToRange(amount, 0, 255), 20);

      if (gs.mOptions.mRaceBeepVolume > 0.0f && mSoundChannel != -1)
      {
        AudioManager::GetInstance().StartSoundOnChannel(mSoundChannel, mSound, false);
        AudioManager::GetInstance().SetChannelVolumeScale(mSoundChannel, Square(gs.mOptions.mRaceBeepVolume));
      }
    }
  }
  else if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_RACING)
  {
      // Handle the local score
    float s = CalculateScore(cs.mLimboDuration, gs.mOptions.mLimboDifficultyMultiplier);
    Score score(s, mLimboCount, cs.mChallengeID);
    Statistics::Scores::iterator it = gs.mStatistics.mHighScores.find(cs.mChallengeID);
    if (
      it == gs.mStatistics.mHighScores.end() ||
      score.mResult > it->second.mResult
      )
    {
      // New high score
      mHighScore = Statistics::Score(score.mResult, score.mMinorResult);
      gs.mStatistics.mHighScores[cs.mChallengeID] = mHighScore;
      mGotHighScore = true;
    }
    else
    {
      mHighScore = it->second;
    }

    if (gs.mOptions.mEnableScoreloop && mValidated)
    {
      UploadScore();
    }
  }

  if (mLimboTime < cs.mLimboDuration)
  {
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
  }
  else
  {
    // Reached the end
    if (gs.mOptions.mEnableScoreloop && mValidated)
    {
      mLeaderboardOverlay->Enable(true);
      if (mLeaderboardOverlay->IsPressed())
      {
        if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_UPLOADED)
        {
          float s = CalculateScore(cs.mLimboDuration, gs.mOptions.mLimboDifficultyMultiplier);
          Score score(s, mLimboCount, cs.mChallengeID);
          DisplayScoreloopMenuLeaderboard(gs, &score);
          PicaSim::GetInstance().ReinitOverlays();
        }
        else
        {
          // Cancel just in case
          SC_ScoreController_Cancel(mScoreController);
          mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_UPLOADED;

          float s = CalculateScore(cs.mLimboDuration, gs.mOptions.mLimboDifficultyMultiplier);
          Score score(s, mLimboCount, cs.mChallengeID);
          UploadScoreloop(gs, score);
          PicaSim::GetInstance().ReinitOverlays();
        }
        mNeedToCacheText = true;
      }
    }
  }

  return CHALLENGE_CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeLimbo::GxRender(int renderLevel, DisplayConfig& displayConfig)
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
  IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE);

  char txt[256];
  char timeText[64];
  FormatTime(timeText, cs.mLimboDuration - mLimboTime);
  const size_t numGates = cs.mCheckpoints.size();
  int numLines = 0;
  if (mLimboTime < cs.mLimboDuration)
  {
    numLines = 1;
    sprintf(txt, "Through %lu Time %s", (long unsigned) mLimboCount, timeText);
  }
  else
  {
    float timeBonus = 0.0f;
    float score = CalculateScore(cs.mLimboDuration, gs.mOptions.mLimboDifficultyMultiplier, &timeBonus);

    // Have finished
    Score scoreloopScore(score, mLimboCount, cs.mChallengeID);

    const char* scoreloopText = 0;
    if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_SUBMITTED)
      scoreloopText = " (Uploading)";
    else
      scoreloopText = "";

    char summaryText[256];
    sprintf(summaryText, 
      "Basic score = %lu * 100\n"
      "Difficulty bonus = x %.2f\n"
      "Time bonus = %.1f\n",
      (long unsigned) mLimboCount, gs.mOptions.mLimboDifficultyMultiplier, timeBonus);
    numLines += 3;

    if (mGotHighScore || score > mHighScore.mResult)
    {
      numLines = 6;
      sprintf(txt, "%s\n\nNew local high score = %.1f%s", summaryText, scoreloopScore.mResult, scoreloopText);
    }
    else
    {
      numLines = 6;
      sprintf(txt, "%s\n\nScore = %.1f%s   Best score = %.1f", 
        summaryText, scoreloopScore.mResult, scoreloopText, mHighScore.mResult);
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
