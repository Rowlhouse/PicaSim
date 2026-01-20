#include "ChallengeDuration.h"
#include "PicaSim.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "GameSettings.h"
#include "WindsockOverlay.h"
#include "ButtonOverlay.h"
#include "Menus/Menu.h"
#include "Menus/PicaDialog.h"

#include "../Platform/S3ECompat.h"
#include "../Platform/FontRenderer.h"

//======================================================================================================================
ChallengeDuration::ChallengeDuration(GameSettings& gameSettings) : Challenge(gameSettings), mFinalScore(0.0, 0.0, 0)
{
    TRACE_METHOD_ONLY(1);
    const ChallengeSettings& cs = gameSettings.mChallengeSettings;
    TRACE_FILE_IF(1) TRACE("ChallengeDuration::ChallengeDuration num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());
}

//======================================================================================================================
ChallengeDuration::~ChallengeDuration()
{
}

//======================================================================================================================
void ChallengeDuration::ReinitOverlays()
{
}

//======================================================================================================================
void ChallengeDuration::Init(Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const ChallengeSettings& cs = gs.mChallengeSettings;

    TRACE_FILE_IF(1) TRACE("ChallengeDuration::Init num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());

    mAeroplane = aeroplane;
    RenderManager::GetInstance().RegisterRenderGxObject(this, 0);

    mNeedToCacheText = true;

    mGotHighScore = false;

    if (cs.mDefaultToChaseView)
        PicaSim::GetInstance().SetMode(PicaSim::MODE_CHASE);

    Relaunched();
}

//======================================================================================================================
void ChallengeDuration::Terminate()
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
}

//======================================================================================================================
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

    Environment::GetInstance().GetThermalManager().SetSeed(cs.mThermalSeed);
    Environment::GetInstance().GetThermalManager().Repopulate();
    Environment::GetInstance().ResetTime();
}

//======================================================================================================================
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
        }
    }

    return CHALLENGE_CONTINUE;
}

//======================================================================================================================
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


//======================================================================================================================
void ChallengeDuration::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const ChallengeSettings& cs = gs.mChallengeSettings;

    if (!PicaSim::GetInstance().GetShowUI())
        return;

    FontRenderer& font = FontRenderer::GetInstance();
    uint32 origColour = font.GetColourABGR();

    font.SetColourABGR(0xffa0a0a0);
    uint16 fontHeight = font.GetFontHeight();
    font.SetAlignmentHor(FONT_ALIGN_CENTRE);

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
        char summaryText[256];
        sprintf(summaryText,
            "Time score = %.1f\n"
            "Distance penalty= %.2f",
            mFinalTimeScore, mFinalDistancePenalty);

        if (mGotHighScore)
        {
            numLines = 6;
            sprintf(txt, "%s\n\nNew local high score = %.1f Time = %s", summaryText, mFinalScore.mResult, timeText);
        }
        else
        {
            numLines = 6;
            char bestTimeText[64];
            sprintf(txt, "%s\n\nScore = %.1f Time = %s   Best = %.1f Time = %s",
                summaryText, mFinalScore.mResult, timeText, mHighScore.mResult, FormatTime(bestTimeText, (float) mHighScore.mMinorResult));
        }
    }

    font.SetRect(displayConfig.mLeft,(int16)(displayConfig.mBottom + displayConfig.mHeight - (0.5f * (numLines + 1.0f) * fontHeight * 1.2f)),(int16)displayConfig.mWidth, fontHeight);
    font.RenderText(txt);

    if (gs.mOptions.mDisplayFPS)
    {
        font.SetRect(displayConfig.mLeft,(int16)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4),(int16)displayConfig.mWidth,fontHeight);
        font.SetAlignmentHor(FONT_ALIGN_RIGHT);
        sprintf(txt, "%d", (int) (gs.mStatistics.mSmoothedFPS + 0.5f));
        font.SetColourABGR(0xff00ffff);
        font.RenderText(txt);
    }

    if (mNeedToCacheText)
    {
        font.SetColourABGR(0x00ffffff);
        font.RenderText("0123456789.");
        mNeedToCacheText = false;
    }

    font.SetColourABGR(origColour);
}
