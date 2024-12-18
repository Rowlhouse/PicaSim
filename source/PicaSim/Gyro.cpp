#include "Gyro.h"

#include "Aerofoil.h"
#include "AerofoilParameters.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "Controller.h"
#include "Environment.h"
#include "PicaSim.h"

//----------------------------------------------------------------------------------------------------------------------
Gyro::Gyro()
{
}

//----------------------------------------------------------------------------------------------------------------------
void Gyro::Init(class TiXmlElement* gyroElement, class Aeroplane* aeroplane)
{
  TRACE_METHOD_ONLY(1);
  mAeroplane = aeroplane;
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();

  for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
  {
    mPassThroughPerChannel[i] = 0.0f;
    mRotationRatePerChannel[i] = 0.0f;
    mAnglePerChannel[i] = 0.0f;
    mSpeedPerChannel[i] = 0.0f;
    mHeightRatePerChannel[i] = 0.0f;
  }
  mChannelForMode = -1;

  mAxis = Vector3(0,0,0);
  mGain = 0.0f;
  mTiltPerSpeed = 0.0f;
  mMaxTilt = 0.0f;
  mType = TYPE_AXIS;
  mMode = MODE_ROTATION_RATE;

  // Read data for this engine
  mName = readStringFromXML(gyroElement, "name");
  readFromXML(gyroElement, "passThroughPerChannel0", mPassThroughPerChannel[0]);
  readFromXML(gyroElement, "passThroughPerChannel1", mPassThroughPerChannel[1]);
  readFromXML(gyroElement, "passThroughPerChannel2", mPassThroughPerChannel[2]);
  readFromXML(gyroElement, "passThroughPerChannel3", mPassThroughPerChannel[3]);
  readFromXML(gyroElement, "passThroughPerChannel4", mPassThroughPerChannel[4]);
  readFromXML(gyroElement, "passThroughPerChannel5", mPassThroughPerChannel[5]);
  readFromXML(gyroElement, "passThroughPerChannel6", mPassThroughPerChannel[6]);

  readFromXML(gyroElement, "rotationRatePerChannel0", mRotationRatePerChannel[0]);
  readFromXML(gyroElement, "rotationRatePerChannel1", mRotationRatePerChannel[1]);
  readFromXML(gyroElement, "rotationRatePerChannel2", mRotationRatePerChannel[2]);
  readFromXML(gyroElement, "rotationRatePerChannel3", mRotationRatePerChannel[3]);
  readFromXML(gyroElement, "rotationRatePerChannel4", mRotationRatePerChannel[4]);
  readFromXML(gyroElement, "rotationRatePerChannel5", mRotationRatePerChannel[5]);
  readFromXML(gyroElement, "rotationRatePerChannel6", mRotationRatePerChannel[6]);

  readFromXML(gyroElement, "anglePerChannel0", mAnglePerChannel[0]);
  readFromXML(gyroElement, "anglePerChannel1", mAnglePerChannel[1]);
  readFromXML(gyroElement, "anglePerChannel2", mAnglePerChannel[2]);
  readFromXML(gyroElement, "anglePerChannel3", mAnglePerChannel[3]);
  readFromXML(gyroElement, "anglePerChannel4", mAnglePerChannel[4]);
  readFromXML(gyroElement, "anglePerChannel5", mAnglePerChannel[5]);
  readFromXML(gyroElement, "anglePerChannel6", mAnglePerChannel[6]);

  readFromXML(gyroElement, "speedPerChannel0", mSpeedPerChannel[0]);
  readFromXML(gyroElement, "speedPerChannel1", mSpeedPerChannel[1]);
  readFromXML(gyroElement, "speedPerChannel2", mSpeedPerChannel[2]);
  readFromXML(gyroElement, "speedPerChannel3", mSpeedPerChannel[3]);
  readFromXML(gyroElement, "speedPerChannel4", mSpeedPerChannel[4]);
  readFromXML(gyroElement, "speedPerChannel5", mSpeedPerChannel[5]);
  readFromXML(gyroElement, "speedPerChannel6", mSpeedPerChannel[6]);

  readFromXML(gyroElement, "heightRatePerChannel0", mHeightRatePerChannel[0]);
  readFromXML(gyroElement, "heightRatePerChannel1", mHeightRatePerChannel[1]);
  readFromXML(gyroElement, "heightRatePerChannel2", mHeightRatePerChannel[2]);
  readFromXML(gyroElement, "heightRatePerChannel3", mHeightRatePerChannel[3]);
  readFromXML(gyroElement, "heightRatePerChannel4", mHeightRatePerChannel[4]);
  readFromXML(gyroElement, "heightRatePerChannel5", mHeightRatePerChannel[5]);
  readFromXML(gyroElement, "heightRatePerChannel6", mHeightRatePerChannel[6]);

  readFromXML(gyroElement, "channelForMode", mChannelForMode);

  readFromXML(gyroElement, "tiltPerSpeed", mTiltPerSpeed);
  readFromXML(gyroElement, "maxTilt", mMaxTilt);

  mTiltPerSpeed = DegreesToRadians(mTiltPerSpeed);
  mMaxTilt = DegreesToRadians(mMaxTilt);
  for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    mAnglePerChannel[i] = DegreesToRadians(mAnglePerChannel[i]);

  std::string type("axis");
  readFromXML(gyroElement, "type", type);
  if (type == "height")
    mType = TYPE_HEIGHT;

  std::string initialMode("axis");
  readFromXML(gyroElement, "initialMode", initialMode);
  if (initialMode == "axis")
    mMode = MODE_ROTATION_RATE;
  else if (initialMode == "passThrough")
    mMode = MODE_PASS_THROUGH;
  else if (initialMode == "stabilisedSpeed")
    mMode = MODE_STABILISED_SPEED;
  else if (initialMode == "stabilisedHeight")
    mMode = MODE_STABILISED_HEIGHT;

  mAxis = readVector3FromXML(gyroElement, "axis");
  mGain = readFloatFromXML(gyroElement, "gain");

  mMaxDeltaAltitude = 4.0f;
  mHeightStrength = 1.0f;
  mHeightDamping = 0.5f;

  // Scale
  float massScale = as.mMassScale;
  float speedScale = as.mSizeScale * sqrtf(massScale);
  mMaxDeltaAltitude *= as.mSizeScale;
  mTiltPerSpeed /= speedScale;
  for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
  {
    mSpeedPerChannel[i] *= speedScale;
    mHeightRatePerChannel[i] *= speedScale;
  }


  // Initial values
  Launched();
}

//----------------------------------------------------------------------------------------------------------------------
void Gyro::Terminate()
{
  TRACE_METHOD_ONLY(1);
}

//---------------------------------------------------------------------------------------------------------------------
void Gyro::Launched()
{
  Vector3 pos = mAeroplane->GetTransform().GetTrans();
  float currentAltitude = pos.z;
  mTargetAltitude = currentAltitude;

  mOutput = 0.0f;
}

//---------------------------------------------------------------------------------------------------------------------
void Gyro::UpdatePrePhysics(float deltaTime)
{
  const Controller& controller = mAeroplane->GetController();

  const Transform& tm = mAeroplane->GetTransform();
  const Vector3 axis = tm.RotateVec(mAxis);
  Vector3 vel = mAeroplane->GetVelocity();
  const Vector3 up(0,0,1);

  if (mChannelForMode != -1)
  {
    float input = controller.GetControl((Controller::Channel) mChannelForMode);
    if (input < -0.5f)
      if (mType == TYPE_AXIS)
        mMode = MODE_ORIENTATION;
      else
        mMode = MODE_STABILISED_HEIGHT;
    else if (input < 0.5f)
      if (mType == TYPE_AXIS)
        mMode = MODE_ROTATION_RATE;
      else
        mMode = MODE_PASS_THROUGH;
    else if (input < 1.5f)
      if (mType == TYPE_AXIS)
        mMode = MODE_STABILISED_SPEED;
      else
        mMode = MODE_STABILISED_HEIGHT;
  }

  float currentAltitude = tm.GetTrans().z;

  float desiredRate = 0.0f;
  switch (mMode)
  {
  case MODE_PASS_THROUGH:
    {
      mTargetAltitude = currentAltitude;
      float control = 0.0f;
      for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
        control += mPassThroughPerChannel[i] * controller.GetControl((Controller::Channel) i);
      mOutput = (control + 1.0f) * 0.5f;
      return;
    }
    break;
  case MODE_ORIENTATION:
    {
      mTargetAltitude = currentAltitude;

      float desiredTilt = 0.0f;
      for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
        desiredTilt += mAnglePerChannel[i] * controller.GetControl((Controller::Channel) i);

      Vector3 fwd = axis.Cross(up);
      if (fwd.GetLengthSquared() < 0.001f)
        break;
      fwd.Normalise();
      float x = tm.RowZ().Dot(fwd);
      float y = tm.RowZ().Dot(up);

      float currentTilt = atan2f(x, y);
      float desiredTiltChange = desiredTilt - currentTilt;
      desiredRate = -desiredTiltChange * 5.0f; // +ve goes backwards
    }
    break;
  case MODE_ROTATION_RATE:
    {
      mTargetAltitude = currentAltitude;
      desiredRate = 0.0f;
      for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
        desiredRate += mRotationRatePerChannel[i] * controller.GetControl((Controller::Channel) i);
    }
    break;
  case MODE_STABILISED_HEIGHT:
    {
      float desiredSpeed = 0.0f;
      for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
        desiredSpeed += mHeightRatePerChannel[i] * controller.GetControl((Controller::Channel) i);
      mTargetAltitude += desiredSpeed * deltaTime;
      mTargetAltitude = ClampToRange(mTargetAltitude, currentAltitude - mMaxDeltaAltitude, currentAltitude + mMaxDeltaAltitude);
      mOutput = (mTargetAltitude - currentAltitude) * mHeightStrength - vel.z * mHeightDamping;
      mOutput = (1.0f + mOutput) * 0.5f;
      return;
    }
  case MODE_STABILISED_SPEED:
    {
      mTargetAltitude = currentAltitude;
      float desiredSpeed = 0.0f;
      for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
        desiredSpeed += mSpeedPerChannel[i] * controller.GetControl((Controller::Channel) i);
      Vector3 fwd = axis.Cross(up);
      if (fwd.GetLengthSquared() < 0.001f)
        break;
      fwd.Normalise();
      float currentSpeed = vel.Dot(fwd);
      float desiredSpeedChange = desiredSpeed - currentSpeed;
      float desiredTilt = desiredSpeedChange * mTiltPerSpeed;
      if (desiredTilt > mMaxTilt)
        desiredTilt = mMaxTilt;
      else if (desiredTilt < -mMaxTilt)
        desiredTilt = -mMaxTilt;

      float x = tm.RowZ().Dot(fwd);
      float y = tm.RowZ().Dot(up);

      float currentTilt = atan2f(x, y);
      float desiredTiltChange = desiredTilt - currentTilt;
      desiredRate = -desiredTiltChange * 1.0f; // +ve goes backwards
    }
    break;
  }

  Vector3 angVel = mAeroplane->GetAngularVelocity();
  angVel += axis * desiredRate;
  mOutput = axis.Dot(angVel) * mGain;
}

