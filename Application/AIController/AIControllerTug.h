#ifndef AI_CONTROLLERTUG_H
#define AI_CONTROLLERTUG_H

// #include "AIController.h"
// #include "GameSettings.h"

#include "AIController.h"
struct TugAeroplaneModifiers
{
  TugAeroplaneModifiers();
  float mSizeScale;
  float mMassScale;
  float mEngineScale;
};

class AIControllerTug : public AIController
{
public:
  AIControllerTug(const AIControllersSettings::AIControllerSetting& aiController, const TugAeroplaneModifiers& modifiers, int AIControllerIndex);
  bool Init(LoadingScreenHelper* loadingScreen) OVERRIDE;
  void Terminate() OVERRIDE;
  void Reset() OVERRIDE;
  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;
  void Relaunched() OVERRIDE;

  bool CanTow() const;
  bool IsTowing() const;
  void AttachToGlider(const Aeroplane* glider) {mGlider = glider;}
  void ChooseNewWaypoint(bool isLaunch);
private:
  void CheckForNewWaypoint(
    const AIEnvironmentSettings& aies,
    const AIAeroplaneSettings& aias,
    const Vector3& pos,
    Vector3& deltaToTarget, 
    Vector3& horDeltaToTarget);
  Vector3 GetLaunchPos() const;

  const Aeroplane* mGlider;
  const AIControllersSettings::AIControllerSetting& mAIControllerSetting;
  const TugAeroplaneModifiers mAeroplaneModifiers;
  Vector3 mTargetWaypoint;
  float   mWaypointTimer; ///< Time left heading towards this waypoint
};

#endif