#ifndef WING_H
#define WING_H

//#include "GameSettings.h"

#include "Controller.h"
#include "Vecteurs.h"
#include <cstdint>
#include <vector>
class Aerofoil;

class Wing
{
public:
  void Init(class TiXmlElement* wingElement, 
            class TiXmlHandle& aerodynamicsHandle, 
            class Aeroplane* aeroplane, uint32_t& checksum);
  void Terminate();

  float GetMass() const {return mMass;}
  const Transform& GetTMLocal() const {return mTMLocal;}
  const Vector3& GetExtents() const {return mExtents;}
  bool GetCollide() const {return mCollide;}
  const std::string& GetName() const {return mName;}
  float getControl() const {return mControl;}
  float getFlapAngle() const {return mFlapAngle + mSlopAngle;}

  /// Opportunity to apply forces
  void UpdatePrePhysics(float deltaTime, const struct TurbulenceData& turbulenceData);

  /// Updates the aerofoils to update their position etc after the physics step
  void UpdatePostPhysics(float deltaTime);

  /// Returns the last downwash - i.e. the disturbance to the global airflow
  Vector3 GetLastWash() const {return mLastWash;}

private:
  bool ReadFromXML(class TiXmlElement* wingElement, struct AerofoilData& aerofoilData);
  typedef std::vector<Aerofoil*> Aerofoils;

  class Aeroplane* mAeroplane;

  Aerofoils mAerofoils;

  std::string mName;
  float mMass;
  bool mCollide;
  bool mIsMainWing;
  // The position/orientation of the middle of the wing
  Transform mTMLocal;
  Vector3 mExtents;

  Vector3 mLastWash;
  Vector3 mLastForce;
  // Angles in degrees
  float   mFlapAngle;
  float   mSlopAngle;
  float   mSlopAngleRate;

  /// The actual control (servo position)
  float mControl;

  /// Constant data shared between aerofoils within a wing, but may be different between wings
  float mDegreesPerControl;
  float mTrimControl;
  float mCLPerDegree;
  float mCDPerDegree;
  float mCMPerDegree;
  float mFlapFraction;
  float mSlopDegreesPerNewton;

  float mControlPerChannel[Controller::MAX_CHANNELS];
  float mControlPerAbsChannel[Controller::MAX_CHANNELS];
  float mControlDifferentialPerChannel[Controller::MAX_CHANNELS];
  float mControlExpPerChannel[Controller::MAX_CHANNELS];

  float mControlRate;
  float mControlClamp;
};

#endif