#ifndef CHALLENGELIMBO_H
#define CHALLENGELIMBO_H

#include "Challenge.h"
#include "Gate.h"
#include "GameSettings.h"

#include "Framework.h"

//======================================================================================================================
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

private:
    float CalculateScore(float challengeDuration, float difficultyMultiplier, float* timeBonus = 0) const;

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

    Statistics::Score mHighScore;
    bool mGotHighScore;

    AudioManager::Sound* mSound;
    AudioManager::SoundChannel mSoundChannel;
};

#endif
