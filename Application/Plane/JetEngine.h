#ifndef JETENGINE_H
#define JETENGINE_H

#include "Controller.h"
// #include "GameSettings.h"
#include "AudioManager.h"
#include "Engine.h"
#include "Helpers.h"

class JetEngine : public Engine
{
public:
  void Init(class TiXmlElement* engineElement, class Aeroplane* aeroplane);
  void Terminate();

  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  /// Opportunity to apply forces
  void UpdatePrePhysics(float deltaTime, const struct TurbulenceData& turbulenceData) OVERRIDE;

  /// Update position etc after the physics step
  void UpdatePostPhysics(float deltaTime) OVERRIDE;

  void Launched() OVERRIDE;

  Transform GetTM() const OVERRIDE {return mTM;}

  virtual float GetRadius() const {return FLT_MAX;}

private:
  class Aeroplane* mAeroplane;

  // The position/orientation of the middle of the engine
  Transform mTMLocal;
  Transform mTM;
  Vector3 mVel;

  float mMaxForce;
  float mMinSpeed;
  float mMaxSpeed;
  float mControl;
  float mControlExp;
  float mControlRate;

  float mControlPerChannel[Controller::MAX_CHANNELS];

  struct SoundSetting
  {
    SoundSetting() : mSound(0), mSoundChannel(-1), mRadius(1.0f), mMinVolume(0), mMaxVolume(0), mMinFreqScale(1), mMaxFreqScale(1) {}
    AudioManager::Sound* mSound;
    AudioManager::SoundChannel mSoundChannel;

    float mRadius;
    float mMinVolume;
    float mMaxVolume;
    float mMinFreqScale;
    float mMaxFreqScale;
  };

  typedef std::vector<SoundSetting> SoundSettings;
  SoundSettings mSoundSettings;
};

#endif