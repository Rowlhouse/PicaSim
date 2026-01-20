#ifndef CHALLENGEDURATION_H
#define CHALLENGEDURATION_H

#include "Challenge.h"
#include "Gate.h"
#include "GameSettings.h"

#include "Framework.h"

//======================================================================================================================
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

private:
    float CalculateScore(float& timeScore, float& distancePenalty) const;

    bool mNeedToCacheText;

    float mDurationTime;
    float mOnGroundTime;
    bool mAttemptCompleted;

    Score mFinalScore;
    float mFinalTimeScore;
    float mFinalDistancePenalty;

    Statistics::Score mHighScore;
    bool mGotHighScore;
};

#endif
