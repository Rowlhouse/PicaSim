#include "ChallengeRace.h"
#include "FonctionsMarmelade.h"
#include "Menu.h"
#include "Trace.h"
#include "PicaSim.h"
#include "Environment.h"
// #include "Aeroplane.h"
// #include "AeroplanePhysics.h"
// #include "GameSettings.h"
#include "ButtonOverlay.h"
#include "ScoreloopMenu.h"
#include "PicaDialog.h"
#include "Viewport.h"
#include "WindsockOverlay.h"

// #include <IwGx.h>
// #include <IwGxFont.h>
// #include <s3eVibra.h>

extern float numButtonSlots;

//----------------------------------------------------------------------------------------------------------------------
ChallengeRace::ChallengeRace(GameSettings& gameSettings) : Challenge(gameSettings)
{
  TRACE_METHOD_ONLY(1);
  const ChallengeSettings& cs = gameSettings.mChallengeSettings;
  TRACE_FILE_IF(1) TRACE("ChallengeRace::ChallengeRace num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());
  float buttonSize = gameSettings.mOptions.mPauseButtonsSize / numButtonSlots;
  float buttonOffset = 1.0f / numButtonSlots;
  float paddingFraction = 0.25f;
  mLeaderboardOverlay = new ButtonOverlay("SystemData/Menu/Leaderboards.png", 
    buttonSize, paddingFraction, ButtonOverlay::ANCHOR_H_RIGHT, ButtonOverlay::ANCHOR_V_TOP,  buttonOffset*6, 1.0f, 255, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
ChallengeRace::~ChallengeRace()
{
  delete mLeaderboardOverlay;
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeRace::OnScoreSubmitted(SC_Error_t completionStatus)
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
  ((ChallengeRace*) userData)->OnScoreSubmitted(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
void ChallengeRace::UploadScore()
{
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score");
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  Score score(CalculateScore(), mRaceTime, cs.mChallengeID);

  SC_Score_SetResult(mScoreloopScore, score.mResult);
  SC_Score_SetMinorResult(mScoreloopScore, score.mMinorResult);
  SC_Score_SetMode(mScoreloopScore, score.mMode);

  int errorCode = SC_ScoreController_SubmitScore(mScoreController, mScoreloopScore);
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score - result = %d", errorCode);

  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_SUBMITTED;
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeRace::ReinitOverlays() 
{
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeRace::Init(Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen)
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  TRACE_FILE_IF(1) TRACE("ChallengeRace::Init num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());

  mAeroplane = aeroplane;
  RenderManager::GetInstance().RegisterRenderGxObject(this, 0);

  mNeedToCacheText = true;
  mRaceTime = -cs.mPreparationTime;
  mHeightTimesTime = 0.0f;
  mTargetCheckpointsIndex = 0;
  mRaceCompleted = false;
  mHaveDisplayedNetworkFailure = false;

  for (size_t i = 0 ; i != gs.mChallengeSettings.mGates.size() ; ++i)
  {
    mGatePosts.push_back(GatePost());
    mGatePosts.push_back(GatePost());
  }

  for (size_t i = 0 ; i != gs.mChallengeSettings.mGates.size() ; ++i)
  {
    size_t j = i*2;
    mGatePosts[j].Init(
      gs.mChallengeSettings.mGates[i].mPos1, 
      gs.mChallengeSettings.mGates[i].mDraw1, 
      gs.mChallengeSettings.mGates[i].mColour1,
      gs.mChallengeSettings.mGates[i].mHeight,
      gs.mChallengeSettings.mGates[i].mTargetColour1);
    ++j;
    mGatePosts[j].Init(
      gs.mChallengeSettings.mGates[i].mPos2, 
      gs.mChallengeSettings.mGates[i].mDraw2, 
      gs.mChallengeSettings.mGates[i].mColour2, 
      gs.mChallengeSettings.mGates[i].mHeight,
      gs.mChallengeSettings.mGates[i].mTargetColour2);
  }

  mGatePointer = new WindsockOverlay("SystemData/Menu/GatePointer.png", gs.mOptions.mWindArrowSize * 0.75f, 0.5f, gs.mOptions.mWindArrowSize, 0, 0.0f);

  mScoreController = 0;
  mScoreloopScore = 0;
  SC_Error_t errorCode = SC_Client_CreateScoreController(gScoreloopClient, &mScoreController, &ScoreSubmissionCallback, this);
  errorCode = SC_Client_CreateScore(gScoreloopClient, &mScoreloopScore);
  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_RACING;

  mGotHighScore = false;

  mSound = AudioManager::GetInstance().LoadSound("SystemData/Audio/Beep22050Mono.raw", 22050, false, false, false);
  mSoundChannel = -1;
  if (mSound)
  {
    mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(1.0f, false);
    TRACE_FILE_IF(1) TRACE("Allocated sound channel %d for ChallengeRace", mSoundChannel);
    if (mSoundChannel == -1)
      TRACE_FILE_IF(1) TRACE("Failed to allocate sound channel for race");
  }

  if (cs.mDefaultToChaseView)
    PicaSim::GetInstance().SetMode(PicaSim::MODE_CHASE);

  Validate();

  Relaunched();
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeRace::Terminate()
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
  for (size_t i = 0 ; i != mGatePosts.size() ; ++i)
  {
    mGatePosts[i].Terminate();
  }
  mGatePosts.clear();
  delete mGatePointer;
  mGatePointer = 0;
  if (mScoreController)
  {
    SC_ScoreController_Release(mScoreController);
    mScoreController = 0;
  }
  if (mScoreloopScore)
  {
    SC_Score_Release(mScoreloopScore);
    mScoreloopScore = 0;
  }

  if (mSoundChannel != -1)
  {
    AudioManager::GetInstance().ReleaseSoundChannel(mSoundChannel);
    mSoundChannel = -1;
  }
  if (mSound)
  {
    AudioManager::GetInstance().UnloadSound(mSound);
    mSound = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeRace::Relaunched()
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  mRaceTime = -cs.mPreparationTime;
  mHeightTimesTime = 0.0f;
  mTargetCheckpointsIndex = 0;
  mRaceCompleted = false;
  mOldPos = mAeroplane->GetTransform().GetTrans();

  SC_ScoreController_Cancel(mScoreController);
  mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_RACING;
  Environment::GetInstance().GetThermalManager().SetSeed(cs.mThermalSeed);
  Environment::GetInstance().GetThermalManager().Repopulate();
  Environment::GetInstance().ResetTime();
}

//----------------------------------------------------------------------------------------------------------------------
float ChallengeRace::CalculateHeightMultiplier() const
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;
  if (mRaceTime <= 0.0f || cs.mMaxHeightMultiplier <= 1.0f)
    return 1.0f;

  float averageHeight = mHeightTimesTime / mRaceTime;
  float heightMultiplier = 1.0f + (cs.mMaxHeightMultiplier - 1.0f) * ClampToRange(1.0f - averageHeight/cs.mMaxHeightForBonus, 0.0f, 1.0f);
  return heightMultiplier;
}

//----------------------------------------------------------------------------------------------------------------------
float ChallengeRace::CalculateScore() const
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;
  if (mRaceTime <= 0.0f)
    return 0.0f;

  float heightMultiplier = CalculateHeightMultiplier();

  float score = heightMultiplier * 1000.0f * cs.mReferenceTime / mRaceTime;
  return score;
}

//----------------------------------------------------------------------------------------------------------------------
Challenge::ChallengeResult ChallengeRace::UpdateChallenge(float deltaTime)
{
  Challenge::UpdateChallenge(deltaTime);

  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const ChallengeSettings& cs = gs.mChallengeSettings;

  Vector3 pos = mAeroplane->GetTransform().GetTrans();

  // Make sure of some settings
  gs.mOptions.mEnableWalkabout = false;
  gs.mOptions.mEnableObjectEditing = false;

  if (!mRaceCompleted)
  {
    if (mTargetCheckpointsIndex < cs.mCheckpoints.size())
    {
      if (mRaceTime < 0.0f && (int) mTargetCheckpointsIndex > cs.mCheckpointForTimer)
        mRaceTime = 0.0f;
      else
        mRaceTime += deltaTime;
      // Height multiplier
      if (mRaceTime > 0.0f)
      {
        float height = pos.z - Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true);
        mHeightTimesTime += height * deltaTime;
      }
      // evaluate if we're through the gate
      size_t gateIndex = cs.mCheckpoints[mTargetCheckpointsIndex];
      IwAssert(ROWLHOUSE, gateIndex < cs.mGates.size());
      int isThrough = cs.mGates[gateIndex].IsThrough(mOldPos, mAeroplane->GetTransform().GetTrans(), 1.0f);
      mOldPos = pos;
      if (isThrough == 1)
      {
        TRACE_FILE_IF(1) TRACE("Through gate order index %d", mTargetCheckpointsIndex);
        ++mTargetCheckpointsIndex;
        
        // Play sound and vibrate
        int amount = (int) (gs.mOptions.mRaceVibrationAmount * 255);
        if (amount > 0)
          s3eVibraVibrate(ClampToRange(amount, 0, 255), 20);

        if (gs.mOptions.mRaceBeepVolume > 0.0f && mSoundChannel != -1)
        {
          AudioManager::GetInstance().StartSoundOnChannel(mSoundChannel, mSound, false);
          AudioManager::GetInstance().SetChannelVolumeScale(mSoundChannel, Square(gs.mOptions.mRaceBeepVolume));
        }
        const size_t numGates = cs.mCheckpoints.size();
        if (mTargetCheckpointsIndex >= numGates)
        {
          mRaceCompleted = true;

          // Handle the local score
          Score score(CalculateScore(), mRaceTime, cs.mChallengeID);
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
    }
  }

  int targetGate = mTargetCheckpointsIndex < cs.mCheckpoints.size() ? cs.mCheckpoints[mTargetCheckpointsIndex] : -1;

  for (size_t i = 0 ; i != cs.mGates.size() ; ++i)
  {
    size_t j = i*2;
    mGatePosts[j].SetIsTarget(targetGate == (int) i);
    ++j;
    mGatePosts[j].SetIsTarget(targetGate == (int) i);
  }

  if (targetGate != -1)
  {
    const Transform& cameraTM = PicaSim::GetInstance().GetMainViewport().GetCamera()->GetTransform();

    Vector3 gatePos = cs.mGates[targetGate].GetClosestPointOnGateToPosition(pos);
    Vector3 dirToGate = gatePos - cameraTM.GetTrans();

    float gateAngle = 270.0f-RadiansToDegrees(atan2f(dirToGate.Dot(cameraTM.RowX()), dirToGate.Dot(cameraTM.RowY())));

    mGatePointer->SetAngle(gateAngle);
    mGatePointer->SetAlpha(255);
    float size = gs.mOptions.mWindArrowSize * 0.75f;
    mGatePointer->SetSize(size);
    mGatePointer->SetPosition(0.5f, gs.mOptions.mWindArrowSize + size);

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
    mGatePointer->SetAlpha(0);
    if (gs.mOptions.mEnableScoreloop && mValidated)
    {
      mLeaderboardOverlay->Enable(true);
      if (mLeaderboardOverlay->IsPressed())
      {
        if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_UPLOADED)
        {
          Score score(CalculateScore(), mRaceTime, cs.mChallengeID);
          DisplayScoreloopMenuLeaderboard(gs, &score);
          PicaSim::GetInstance().ReinitOverlays();
        }
        else
        {
          // Cancel just in case
          SC_ScoreController_Cancel(mScoreController);
          mScoreSubmissionStatus = SCORESUBMISSIONSTATUS_UPLOADED;

          Score score(CalculateScore(), mRaceTime, cs.mChallengeID);
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
void ChallengeRace::GxRender(int renderLevel, DisplayConfig& displayConfig)
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
  FormatTime(timeText, mRaceTime);
  const size_t numGates = cs.mCheckpoints.size();
  int numLines = 1;
  if (mTargetCheckpointsIndex < numGates)
  {
    if (mRaceTime < 0.0f)
      IwGxFontSetCol(0xff5050ff);

    if (cs.mMaxHeightMultiplier > 1.0f)
      sprintf(txt, "Gate %lu/%lu : Time %s : Bonus x %5.2f", (long unsigned) mTargetCheckpointsIndex, (long unsigned) numGates, timeText, CalculateHeightMultiplier());
    else
      sprintf(txt, "Gate %lu/%lu : %s", (long unsigned) mTargetCheckpointsIndex, (long unsigned) numGates, timeText);
  }
  else
  {
    // Have finished
    Score score(CalculateScore(), mRaceTime, cs.mChallengeID);

    const char* scoreloopText = 0;
    if (mScoreSubmissionStatus == SCORESUBMISSIONSTATUS_SUBMITTED)
      scoreloopText = " (Uploading)";
    else
      scoreloopText = "";

    if (mGotHighScore)
    {
      if (cs.mMaxHeightMultiplier > 1.0f)
        sprintf(txt, "New local high score = %.1f Time = %s Bonus x %5.2f%s", score.mResult, timeText, CalculateHeightMultiplier(), scoreloopText);
      else
        sprintf(txt, "New local high score = %.1f Time = %s%s", score.mResult, timeText, scoreloopText);
    }
    else
    {
      ++numLines;
      char bestTimeText[64];
      if (cs.mMaxHeightMultiplier > 1.0f)
        sprintf(txt, "Score = %.1f Time = %s Bonus x %5.2f%s\nBest = %.1f Time = %s", 
          score.mResult, timeText, CalculateHeightMultiplier(), scoreloopText, mHighScore.mResult, FormatTime(bestTimeText, (float) mHighScore.mMinorResult));
      else
        sprintf(txt, "Score = %.1f Time = %s%s\nBest = %.1f Time = %s", 
          score.mResult, timeText, scoreloopText, mHighScore.mResult, FormatTime(bestTimeText, (float) mHighScore.mMinorResult));
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

