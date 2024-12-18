#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "GameSettings.h"

#include <cstdio>

//---------------------------------------------------------------------------------------------------------------------
class Challenge
{
public:
  Challenge() {}
  /// This constructor overrides various things according to the challenge settings
  Challenge(GameSettings& gameSettings);
  virtual ~Challenge() {}
  virtual void Terminate() {}

  enum ChallengeResult
  {
    CHALLENGE_CONTINUE,
    CHALLENGE_RELAUNCH
  };
  virtual ChallengeResult UpdateChallenge(float deltaTime) = 0;

  virtual void ReinitOverlays() {}

  virtual void Init(class Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen) = 0;

  virtual void Reset() {}

  virtual void Relaunched() {}

  virtual void Validate();

protected:
  class Aeroplane* mAeroplane;
  bool mValidated;
};

//----------------------------------------------------------------------------------------------------------------------
inline char* FormatTime(char* text, float time)
{
  int minutes = (int) (time / 60.0f);
  float seconds = time - minutes * 60.0f;
  if (minutes > 0)
    sprintf(text, "%d:%05.2f", minutes, seconds);
  else
    sprintf(text, "%4.2f", seconds);
  return text;
}


#endif