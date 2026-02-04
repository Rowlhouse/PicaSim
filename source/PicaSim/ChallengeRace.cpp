#include "ChallengeRace.h"
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
ChallengeRace::ChallengeRace(GameSettings& gameSettings) : Challenge(gameSettings)
{
    TRACE_METHOD_ONLY(1);
    const ChallengeSettings& cs = gameSettings.mChallengeSettings;
    TRACE_FILE_IF(1) TRACE("ChallengeRace::ChallengeRace num gates = %d num checkpoints = %d", cs.mGates.size(), cs.mCheckpoints.size());
}

//======================================================================================================================
ChallengeRace::~ChallengeRace()
{
}

//======================================================================================================================
void ChallengeRace::ReinitOverlays()
{
}

//======================================================================================================================
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

    mGatePointer = std::make_unique<WindsockOverlay>("SystemData/Menu/GatePointer.png", gs.mOptions.mWindArrowSize * 0.75f, 0.5f, gs.mOptions.mWindArrowSize, 0, 0.0f);

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

    Relaunched();
}

//======================================================================================================================
void ChallengeRace::Terminate()
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
    for (size_t i = 0 ; i != mGatePosts.size() ; ++i)
    {
        mGatePosts[i].Terminate();
    }
    mGatePosts.clear();
    mGatePointer.reset();

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

//======================================================================================================================
void ChallengeRace::Relaunched()
{
    GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const ChallengeSettings& cs = gs.mChallengeSettings;

    mRaceTime = -cs.mPreparationTime;
    mHeightTimesTime = 0.0f;
    mTargetCheckpointsIndex = 0;
    mRaceCompleted = false;
    mOldPos = mAeroplane->GetTransform().GetTrans();

    Environment::GetInstance().GetThermalManager().SetSeed(cs.mThermalSeed);
    Environment::GetInstance().GetThermalManager().Repopulate();
    Environment::GetInstance().ResetTime();
}

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
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
    }
    else
    {
        // Reached the end
        mGatePointer->SetAlpha(0);
    }

    return CHALLENGE_CONTINUE;
}

//======================================================================================================================
void ChallengeRace::GxRender(int renderLevel, DisplayConfig& displayConfig)
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
    FormatTime(timeText, mRaceTime);
    const size_t numGates = cs.mCheckpoints.size();
    int numLines = 1;
    if (mTargetCheckpointsIndex < numGates)
    {
        if (mRaceTime < 0.0f)
            font.SetColourABGR(0xff5050ff);

        if (cs.mMaxHeightMultiplier > 1.0f)
            sprintf(txt, "Gate %lu/%lu : Time %s : Bonus x %5.2f", (long unsigned) mTargetCheckpointsIndex, (long unsigned) numGates, timeText, CalculateHeightMultiplier());
        else
            sprintf(txt, "Gate %lu/%lu : %s", (long unsigned) mTargetCheckpointsIndex, (long unsigned) numGates, timeText);
    }
    else
    {
        // Have finished
        Score score(CalculateScore(), mRaceTime, cs.mChallengeID);

        if (mGotHighScore)
        {
            if (cs.mMaxHeightMultiplier > 1.0f)
                sprintf(txt, "New local high score = %.1f Time = %s Bonus x %5.2f", score.mResult, timeText, CalculateHeightMultiplier());
            else
                sprintf(txt, "New local high score = %.1f Time = %s", score.mResult, timeText);
        }
        else
        {
            ++numLines;
            char bestTimeText[64];
            if (cs.mMaxHeightMultiplier > 1.0f)
                sprintf(txt, "Score = %.1f Time = %s Bonus x %5.2f\nBest = %.1f Time = %s",
                    score.mResult, timeText, CalculateHeightMultiplier(), mHighScore.mResult, FormatTime(bestTimeText, (float) mHighScore.mMinorResult));
            else
                sprintf(txt, "Score = %.1f Time = %s\nBest = %.1f Time = %s",
                    score.mResult, timeText, mHighScore.mResult, FormatTime(bestTimeText, (float) mHighScore.mMinorResult));
        }
    }

    font.SetRect(displayConfig.mLeft,(int16)(displayConfig.mBottom + displayConfig.mHeight - (0.5f * (numLines + 1.0f) * fontHeight * 1.2f)),(int16)displayConfig.mWidth, fontHeight);
    font.RenderText(txt);

    if (gs.mOptions.mDisplayFPS)
    {
        font.SetRect(displayConfig.mLeft,(int16)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4),(int16)displayConfig.mWidth,fontHeight);
        font.SetAlignmentHor(FONT_ALIGN_RIGHT);
        sprintf(txt, "%ld", std::lround(gs.mStatistics.mSmoothedFPS));
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
