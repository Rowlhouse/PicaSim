#ifndef CHALLENGERACE_H
#define CHALLENGERACE_H

#include "Challenge.h"
#include "Gate.h"
#include "GameSettings.h"

#include "Framework.h"

//======================================================================================================================
class ChallengeRace : public Challenge, public RenderGxObject
{
public:
    /// This will ensure that the race is set up according to the challenge settings -
    /// overriding any aeroplane/scenery settings etc
    ChallengeRace(struct GameSettings& gameSettings);
    ~ChallengeRace();

    void Init(class Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen) OVERRIDE;
    void Terminate() OVERRIDE;
    void Relaunched() OVERRIDE;
    void ReinitOverlays() OVERRIDE;

    void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

    ChallengeResult UpdateChallenge(float deltaTime) OVERRIDE;

private:
    float CalculateScore() const;
    float CalculateHeightMultiplier() const;

    class WindsockOverlay* mGatePointer;

    typedef std::vector<GatePost> GatePosts;
    GatePosts mGatePosts;

    bool mNeedToCacheText;

    Vector3 mOldPos;
    float mRaceTime;
    size_t mTargetCheckpointsIndex; ///< Index into the gate order
    bool mRaceCompleted;
    /// Accumulated height above ground * timestep - used for score multiplier.
    float mHeightTimesTime;

    Statistics::Score mHighScore;
    bool mGotHighScore;

    AudioManager::Sound* mSound;
    AudioManager::SoundChannel mSoundChannel;
};

#endif
