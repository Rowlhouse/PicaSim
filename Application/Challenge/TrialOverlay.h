#ifndef TRIAL_OVERLAY_H
#define TRIAL_OVERLAY_H

#include "ClassesFontMarmelade.h"
#include "RenderManager.h"
//#include "Graphics.h"
#include "GameSettings.h"


class TrialOverlay : public RenderGxObject
{
public:
  /// Registers with render manager on creation
  TrialOverlay(const GameSettings& gameSettings, float timeRemaining);
  /// Deregisters on deletion
  ~TrialOverlay();

  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  void SetTimeRemaining(float timeRemaining) {mTimeRemaining = timeRemaining;}

private:
  CIwGxFont* mFont;
  float mTimeRemaining;
};

#endif
