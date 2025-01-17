#ifndef GYRO_H
#define GYRO_H

//#include "Controller.h"
//#include "GameSettings.h"

#include "Controller.h"
#include "Vecteurs.h"
#include <string>
class Gyro
{
public:
  Gyro();

  void Init(class TiXmlElement* gyroElement, class Aeroplane* aeroplane);
  void Terminate();
  void Launched();
  void UpdatePrePhysics(float deltaTime);

  const std::string& GetName() const {return mName;}

  float GetOutput() const {return mOutput;}

private:
  class Aeroplane* mAeroplane;
  // Read from config
  std::string mName;
  Vector3 mAxis;
  float mGain;
  int mChannelForMode;
  float mRotationRatePerChannel[Controller::MAX_CHANNELS];
  float mPassThroughPerChannel[Controller::MAX_CHANNELS];
  float mSpeedPerChannel[Controller::MAX_CHANNELS];
  float mAnglePerChannel[Controller::MAX_CHANNELS];
  float mHeightRatePerChannel[Controller::MAX_CHANNELS];
  float mTiltPerSpeed;
  float mMaxTilt;

  enum Type {TYPE_AXIS, TYPE_HEIGHT};
  Type mType;

  enum Mode {MODE_PASS_THROUGH, MODE_ROTATION_RATE, MODE_ORIENTATION, MODE_STABILISED_SPEED, MODE_STABILISED_HEIGHT};
  Mode mMode;

  // For height mode
  float mMaxDeltaAltitude;
  float mHeightStrength;
  float mHeightDamping;

  float mTargetAltitude;
  float mOutput;
};

#endif