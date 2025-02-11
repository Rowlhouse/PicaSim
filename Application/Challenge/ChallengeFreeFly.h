#ifndef CHALLENGEFREEFLY_H
#define CHALLENGEFREEFLY_H

#include "Challenge.h"
#include "AIController.h"


#include "AIController.h"
#include "Challenge.h"
#include "ClassesUIMarmelade.h"
#include "RenderManager.h"
#include "Helpers.h"

class ChallengeFreeFly : public Challenge, public RenderGxObject
{
public:
  void Init(class Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen) OVERRIDE;
  void Terminate() OVERRIDE;
  void Reset() OVERRIDE;

  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  ChallengeResult UpdateChallenge(float deltaTime) OVERRIDE;

  void Relaunched() OVERRIDE;

  void Validate() OVERRIDE {}

private:
  float mOnGroundTime;
  float mMaxSpeed;
  float mSmoothedAscentRate;

  bool mNeedToCacheText;

  CIwSVec2 mTextBackgroundXY, mTextBackgroundWH;
  CIwMaterial* mTextBackgroundMaterial;

  AIControllers mAIControllers;
};

#endif