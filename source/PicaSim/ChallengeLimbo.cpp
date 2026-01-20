#include "ChallengeLimbo.h"
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

static const Vector4 colourWrongWay(1.0f, 0.0f, 0.0f, 1.0f);
static const Vector4 colourNeedAltitude(0.0f, 1.0f, 0.0f, 1.0f);
static const Vector4 colourWaiting(0.0f, 0.5f, 1.0f, 1.0f);

//======================================================================================================================
ChallengeLimbo::ChallengeLimbo(GameSettings& gameSettings) : Challenge(gameSettings)
{
    TRACE_METHOD_ONLY(1);
    const ChallengeSettings& cs = gameSettings.mChallengeSettings;
    TRACE_FILE_IF(1) TRACE("ChallengeLimbo::ChallengeLimbo num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());
}

//======================================================================================================================
ChallengeLimbo::~ChallengeLimbo()
{
}

//======================================================================================================================
void ChallengeLimbo::ReinitOverlays()
{
}

//======================================================================================================================
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

//======================================================================================================================
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

    Relaunched();
}

//======================================================================================================================
void ChallengeLimbo::Terminate()
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
    mGate.Terminate();

    if (mSoundChannel != -1)
        AudioManager::GetInstance().ReleaseSoundChannel(mSoundChannel);
    if (mSound)
        AudioManager::GetInstance().UnloadSound(mSound);
}

//======================================================================================================================
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

    Environment::GetInstance().GetThermalManager().SetSeed(cs.mThermalSeed);
    Environment::GetInstance().GetThermalManager().Repopulate();
    Environment::GetInstance().ResetTime();
}

//======================================================================================================================
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
    else if (!mGotHighScore)
    {
        // Handle the local score when challenge ends
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
            mGotHighScore = true; // Set to true to prevent re-checking
        }
    }

    return CHALLENGE_CONTINUE;
}

//======================================================================================================================
void ChallengeLimbo::GxRender(int renderLevel, DisplayConfig& displayConfig)
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
    font.SetAlignmentVer(FONT_ALIGN_MIDDLE);

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
        Score displayScore(score, mLimboCount, cs.mChallengeID);

        char summaryText[256];
        sprintf(summaryText,
            "Basic score = %lu * 100\n"
            "Difficulty bonus = x %.2f\n"
            "Time bonus = %.1f\n",
            (long unsigned) mLimboCount, gs.mOptions.mLimboDifficultyMultiplier, timeBonus);
        numLines += 3;

        if (mGotHighScore && score >= mHighScore.mResult)
        {
            numLines = 6;
            sprintf(txt, "%s\n\nNew local high score = %.1f", summaryText, displayScore.mResult);
        }
        else
        {
            numLines = 6;
            sprintf(txt, "%s\n\nScore = %.1f   Best score = %.1f",
                summaryText, displayScore.mResult, mHighScore.mResult);
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
