#ifndef AI_CONTROLLERGLIDER_H
#define AI_CONTROLLERGLIDER_H

#include "AIController.h"
#include "GameSettings.h"

class AIControllerGlider : public AIController
{
public:
    AIControllerGlider(const AIControllersSettings::AIControllerSetting& aiControllerSetting, int AIControllerIndex);
    bool Init(LoadingScreenHelper* loadingScreen) OVERRIDE;
    void Terminate() OVERRIDE;
    void Reset() OVERRIDE;
    void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;
    void Relaunched() OVERRIDE;

private:
    void CheckForNewWaypoint(
        const AIEnvironmentSettings& aies,
        const AIAeroplaneSettings& aias,
        const Vector3& pos,
        const Vector3& windVel,
        Vector3& deltaToTarget, 
        Vector3& horDeltaToTarget);
    void ChooseNewWaypoint(
        const AIEnvironmentSettings& aies,
        const AIAeroplaneSettings& aias,
        const Vector3& windVel,
        const Vector3& pos,
        bool isLaunch);
    Vector3 GetLaunchPos() const;

    const AIControllersSettings::AIControllerSetting& mAIControllerSetting;
    Vector3 mTargetWaypoint;
    float   mWaypointTimer; ///< Time left heading towards this waypoint
};

#endif
