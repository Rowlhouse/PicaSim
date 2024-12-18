#ifndef AI_CONTROLLERPOWERED_H
#define AI_CONTROLLERPOWERED_H

#include "AIController.h"
#include "GameSettings.h"

class AIControllerPowered : public AIController
{
public:
  AIControllerPowered(const AIControllersSettings::AIControllerSetting& aiController, int AIControllerIndex);
  bool Init(LoadingScreenHelper* loadingScreen) OVERRIDE;
  void Terminate() OVERRIDE;
  void Reset() OVERRIDE;
  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;
  void Relaunched() OVERRIDE;

  bool CanTow() const;
  bool IsTowing() const;
private:
  void CheckForNewWaypoint(
    const AIEnvironmentSettings& aies,
    const AIAeroplaneSettings& aias,
    const Vector3& pos,
    Vector3& deltaToTarget, 
    Vector3& horDeltaToTarget);
  void ChooseNewWaypoint(
    const AIEnvironmentSettings& aies,
    const AIAeroplaneSettings& aias,
    bool isLaunch);
  Vector3 GetLaunchPos() const;

  const AIControllersSettings::AIControllerSetting& mAIControllerSetting;
  Vector3 mTargetWaypoint;
  float   mWaypointTimer; ///< Time left heading towards this waypoint
  bool    mIsTowing;
};

#endif