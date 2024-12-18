#ifndef WHEEL_H
#define WHEEL_H

#include "Framework.h"
#include "Controller.h"

class TiXmlElement;
struct TurbulenceData;
struct AeroplaneSettings;
class Aeroplane;

class Wheel
{
public:
  Wheel();

  void Init(
    Aeroplane*                               aeroplane,
    TiXmlElement*                            wheelElement, 
    btRaycastVehicle*                        raycastVehicle, 
    const btRaycastVehicle::btVehicleTuning& vehicleTuning, 
    const int                                iWheel, 
    const Transform&                         offset,
    const AeroplaneSettings&                 as);

  void UpdatePrePhysics(const Controller* controller, float deltaTime);

  const std::string& GetName() const {return mName;}

  void Launched() {mRetraction = 0.0f;}

  /// This is the amount the wheel is retracted or rotated, if it's a retractable undercarriage
  float GetAngle() const {return mWheelAngle;}

  /// Returns the suspension force applied last update
  float GetSuspensionForce() const;
  /// Returns true if the last force was more than forceThresholdScale * mSuspensionCrashForce 
  bool GetCollapsed(float suspensionCrashForceScale) const;

  /// Forces the brake - use when crashed
  void SetBrake(float brake);

private:
  // Pretty much constant stuff
  class Aeroplane*  mAeroplane;
  std::string       mName;
  float             mAnglePerChannel[Controller::MAX_CHANNELS];
  float             mBrakePerChannel[Controller::MAX_CHANNELS];
  float             mRetractionPerChannel[Controller::MAX_CHANNELS];
  btRaycastVehicle* mRaycastVehicle;
  int               mWheel;
  float             mOriginalSuspensionLength;
  float             mUnretractedDrag;
  float             mBasicBrake;
  float             mDragOffsetMultiplier;
  float             mAngleForRetraction;
  float             mRotationMultiplier;
  float             mSuspensionCrashForce;
  // State
  float             mRetraction;
  float             mWheelAngle;
};

#endif