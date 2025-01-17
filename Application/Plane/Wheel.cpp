#include "Wheel.h"
#include "Environment.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "DimensionalScaling.h"

//----------------------------------------------------------------------------------------------------------------------
Wheel::Wheel()
{
  mRetraction = 0.0f;
  mWheelAngle = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void Wheel::Init(
  Aeroplane*                               aeroplane,
  TiXmlElement*                            wheelElement, 
  btRaycastVehicle*                        raycastVehicle, 
  const btRaycastVehicle::btVehicleTuning& vehicleTuning, 
  const int                                iWheel, 
  const Transform&                         offset,
  const AeroplaneSettings&                 as)
{
  mAeroplane = aeroplane;
  mWheel = iWheel;
  mRaycastVehicle = raycastVehicle;

  float suspensionRestLength = 0.4f;
  float wheelRadius = 0.02f;
  float suspensionStiffness = 100.f;
  float suspensionDampingRatio = 0.1f;
  float suspensionCompressionDampingRatio = 0.1f;
  float rollInfluence = 0.0f;
  float wheelFriction = 1;
  float maxSuspensionTravelCm = 10.0f;
  float maxSuspensionForce = BT_LARGE_FLOAT;
  mBasicBrake = 0.004f;

  readFromXML(wheelElement, "name", mName);
  readFromXML(wheelElement, "suspensionRestLength", suspensionRestLength);
  readFromXML(wheelElement, "wheelRadius", wheelRadius);
  readFromXML(wheelElement, "suspensionStiffness", suspensionStiffness);
  readFromXML(wheelElement, "suspensionDampingRatio", suspensionDampingRatio);
  readFromXML(wheelElement, "suspensionCompressionDampingRatio", suspensionCompressionDampingRatio);
  readFromXML(wheelElement, "rollInfluence", rollInfluence);
  readFromXML(wheelElement, "wheelFriction", wheelFriction);
  readFromXML(wheelElement, "maxSuspensionTravelCm", maxSuspensionTravelCm);
  readFromXML(wheelElement, "maxSuspensionForce", maxSuspensionForce);
  readFromXML(wheelElement, "brake", mBasicBrake);

  Vector3 position = readVector3FromXML(wheelElement, "position");
  Vector3 direction = readVector3FromXML(wheelElement, "direction");
  Vector3 axle = readVector3FromXML(wheelElement, "axle");

  DimensionalScaling ds(as.mSizeScale, as.mMassScale, false);

  ds.ScaleLength(position);
  ds.ScaleLength(suspensionRestLength);
  ds.ScaleLength(wheelRadius);
  ds.ScaleLength(maxSuspensionTravelCm);

  ds.ScaleStiffness(suspensionStiffness);
  ds.ScaleForce(mBasicBrake);
  ds.ScaleForce(maxSuspensionForce);

  float suspensionDamping = suspensionDampingRatio * 2.0f * sqrtf(suspensionStiffness);
  float suspensionDampingCompression = suspensionCompressionDampingRatio * 2.0f * sqrtf(suspensionStiffness);
    
  position = offset.TransformVec(position);
  direction = offset.RotateVec(direction);
  axle = offset.RotateVec(axle);

  btWheelInfo& wheelInfo = mRaycastVehicle->addWheel(
    Vector3ToBulletVector3(position), Vector3ToBulletVector3(direction), Vector3ToBulletVector3(axle), 
    suspensionRestLength, wheelRadius, vehicleTuning);

  wheelInfo.m_suspensionStiffness = suspensionStiffness;
  wheelInfo.m_wheelsDampingRelaxation = suspensionDamping;
  wheelInfo.m_maxSuspensionTravelCm = maxSuspensionTravelCm;
  wheelInfo.m_wheelsDampingCompression = suspensionDampingCompression;
  wheelInfo.m_maxSuspensionForce = maxSuspensionForce;
  wheelInfo.m_frictionSlip = wheelFriction;
  wheelInfo.m_rollInfluence = rollInfluence;
  wheelInfo.m_brake = mBasicBrake; // force

  // Not quite sure which of these are needed, but clearing them is needed to stop bogus forces coming through
  wheelInfo.m_clippedInvContactDotSuspension = 0.0f;
  wheelInfo.m_suspensionRelativeVelocity = 0.0f;
  wheelInfo.m_wheelsSuspensionForce = 0.0f;
  wheelInfo.m_skidInfo = 0.0f;
  wheelInfo.m_raycastInfo.m_suspensionLength = suspensionRestLength;

  for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
  {
    mAnglePerChannel[i] = 0.0f;
    mBrakePerChannel[i] = 0.0f;
    mRetractionPerChannel[i] = 0.0f;
  }
  readFromXML(wheelElement, "anglePerChannel0", mAnglePerChannel[0]);
  readFromXML(wheelElement, "anglePerChannel1", mAnglePerChannel[1]);
  readFromXML(wheelElement, "anglePerChannel2", mAnglePerChannel[2]);
  readFromXML(wheelElement, "anglePerChannel3", mAnglePerChannel[3]);
  readFromXML(wheelElement, "anglePerChannel4", mAnglePerChannel[4]);
  readFromXML(wheelElement, "anglePerChannel5", mAnglePerChannel[5]);
  readFromXML(wheelElement, "anglePerChannel6", mAnglePerChannel[6]);

  readFromXML(wheelElement, "brakePerChannel0", mBrakePerChannel[0]);
  readFromXML(wheelElement, "brakePerChannel1", mBrakePerChannel[1]);
  readFromXML(wheelElement, "brakePerChannel2", mBrakePerChannel[2]);
  readFromXML(wheelElement, "brakePerChannel3", mBrakePerChannel[3]);
  readFromXML(wheelElement, "brakePerChannel4", mBrakePerChannel[4]);
  readFromXML(wheelElement, "brakePerChannel5", mBrakePerChannel[5]);
  readFromXML(wheelElement, "brakePerChannel6", mBrakePerChannel[6]);

  readFromXML(wheelElement, "retractionPerChannel0", mRetractionPerChannel[0]);
  readFromXML(wheelElement, "retractionPerChannel1", mRetractionPerChannel[1]);
  readFromXML(wheelElement, "retractionPerChannel2", mRetractionPerChannel[2]);
  readFromXML(wheelElement, "retractionPerChannel3", mRetractionPerChannel[3]);
  readFromXML(wheelElement, "retractionPerChannel4", mRetractionPerChannel[4]);
  readFromXML(wheelElement, "retractionPerChannel5", mRetractionPerChannel[5]);
  readFromXML(wheelElement, "retractionPerChannel6", mRetractionPerChannel[6]);

  mDragOffsetMultiplier = 1.0f;
  mUnretractedDrag = 0.0f;
  mAngleForRetraction = 0.0f;
  mRotationMultiplier = 0.0f;
  readFromXML(wheelElement, "dragOffsetMultiplier", mDragOffsetMultiplier);
  readFromXML(wheelElement, "unretractedDrag", mUnretractedDrag);
  readFromXML(wheelElement, "angleForRetraction", mAngleForRetraction);
  readFromXML(wheelElement, "rotationMultiplier", mRotationMultiplier);
  ds.ScaleArea(mUnretractedDrag);

  mSuspensionCrashForce = FLT_MAX;
  readFromXML(wheelElement, "suspensionCrashForce", mSuspensionCrashForce);
  ds.ScaleForce(mSuspensionCrashForce);

  mOriginalSuspensionLength = suspensionRestLength;
}

//----------------------------------------------------------------------------------------------------------------------
float Wheel::GetSuspensionForce() const
{
  const btWheelInfo& wheelInfo = mRaycastVehicle->getWheelInfo(mWheel);
  return wheelInfo.m_wheelsSuspensionForce;
}

//----------------------------------------------------------------------------------------------------------------------
void Wheel::SetBrake(float brake)
{
  mRaycastVehicle->setBrake(brake, mWheel);
}

//----------------------------------------------------------------------------------------------------------------------
bool Wheel::GetCollapsed(float suspensionCrashForceScale) const
{
  const btWheelInfo& wheelInfo = mRaycastVehicle->getWheelInfo(mWheel);
  float force = wheelInfo.m_wheelsSuspensionForce;
  return force > mSuspensionCrashForce * suspensionCrashForceScale;
}

//----------------------------------------------------------------------------------------------------------------------
void Wheel::UpdatePrePhysics(const Controller* controller, float deltaTime)
{
  float angle = 0.0f;
  float retraction = 0.0f;
  float extraBrake = 0.0f;

  if (controller)
  {
    for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    {
      float control = controller->GetControl((Controller::Channel) i);
      angle += mAnglePerChannel[i] * control;
      extraBrake += mBrakePerChannel[i] * control;
      retraction += mRetractionPerChannel[i] * control;
    }
    float maxRate = 1.0f;
    float delta = ClampToRange(retraction - mRetraction, -maxRate * deltaTime, maxRate * deltaTime);
    mRetraction = ClampToRange(mRetraction + delta, 0.0f, 1.0f);
  }

  float brake = Maximum(mBasicBrake * (1.0f + extraBrake), 0.0f);
  mRaycastVehicle->setSteeringValue(DegreesToRadians(angle), mWheel);
  mRaycastVehicle->setBrake(brake, mWheel);

  btWheelInfo& wheelInfo = mRaycastVehicle->getWheelInfo(mWheel);
  wheelInfo.m_suspensionRestLength1 = mOriginalSuspensionLength * (1.0f - mRetraction);

  wheelInfo.m_bEnable = mRetraction < 1.0f;

  float drag = (1.0f - mRetraction) * mUnretractedDrag;
  if (mUnretractedDrag > 0.0f)
  {
    Vector3 pos = BulletVector3ToVector3(
      wheelInfo.m_raycastInfo.m_hardPointWS + 
      wheelInfo.m_raycastInfo.m_wheelDirectionWS * (wheelInfo.m_raycastInfo.m_suspensionLength * mDragOffsetMultiplier));

    Vector3 globalWind = Environment::GetInstance().GetWindAtPosition(pos, Environment::WIND_TYPE_SMOOTH);
    Vector3 velocity = mAeroplane->GetVelocity();

    Vector3 airflow = globalWind - velocity;
    const float airDensity = mAeroplane->GetAirDensity();
    Vector3 dynamicPressure = 0.5f * airDensity * ComponentMultiply(airflow, Abs(airflow));

    Vector3 dragForce = dynamicPressure * drag;

    mAeroplane->GetPhysics()->ApplyWorldForceAtWorldPosition(dragForce, pos);
  }

  mWheelAngle = mRetraction * mAngleForRetraction + RadiansToDegrees(wheelInfo.m_rotation) * mRotationMultiplier;
}