#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

//#include "Controller.h"
//#include "GameSettings.h"

#include "Vecteurs.h"
#include <string>
class Accelerometer
{
public:
  Accelerometer();

  void Init(class TiXmlElement* accelerometerElement, class Aeroplane* aeroplane);
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
  Vector3 mOffset;
  float mMinAccel;
  float mMaxAccel;
  float mMinOutput;
  float mMaxOutput;
  float mSmoothTime;

  float mOutput;

  float mPrevDeltaTime;
  Vector3 mPrevVel;

  float mSmoothedAccel;
};

#endif