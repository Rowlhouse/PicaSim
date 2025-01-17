#include "Fuselage.h"

// #include "Aeroplane.h"
// #include "AeroplanePhysics.h"
// #include "Environment.h"
// #include "PicaSim.h"

//----------------------------------------------------------------------------------------------------------------------
void Fuselage::Init(class TiXmlElement* fuselageElement, class Aeroplane* aeroplane)
{
  TRACE_METHOD_ONLY(1);
  mAeroplane = aeroplane;
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();

  // Read data for this fuselage
  mName = readStringFromXML(fuselageElement, "name");
  mMass = Maximum(readFloatFromXML(fuselageElement, "mass"), 1e-8f) * Cube(as.mSizeScale) * as.mMassScale;
  mCollide = true;
  readFromXML(fuselageElement, "collide", mCollide);
  mCD = readVector3FromXML(fuselageElement, "CD");

  Vector3 position = readVector3FromXML(fuselageElement, "position") * as.mSizeScale;
  Vector3 rotation = readVector3FromXML(fuselageElement, "rotation");
  mExtents  = readVector3FromXML(fuselageElement, "extents") * as.mSizeScale;

  float roll = readFloatFromXML(fuselageElement, "roll");
  float pitch = readFloatFromXML(fuselageElement, "pitch");
  float yaw = readFloatFromXML(fuselageElement, "yaw");
  ApplyRollPitchYawToRotationDegrees(roll, pitch, yaw, rotation);

  float angle = DegreesToRadians(rotation.GetLength());
  if (angle > 0.0f)
    rotation.Normalise();
  else
    rotation = Vector3(1,0,0);

  mTMLocal.SetAxisAngle(rotation, angle);
  mTMLocal.SetTrans(position);
}

//----------------------------------------------------------------------------------------------------------------------
void Fuselage::Terminate()
{
  TRACE_METHOD_ONLY(1);
}

//---------------------------------------------------------------------------------------------------------------------
void Fuselage::UpdatePrePhysics(float deltaTime, const TurbulenceData& turbulenceData)
{
  const Transform& TM = mTMLocal * mAeroplane->GetTransform();
  const Vector3& velocity = mAeroplane->GetVelocity();
  Vector3 globalWind = Environment::GetInstance().GetWindAtPosition(TM.GetTrans(), Environment::WIND_TYPE_ALL, &turbulenceData);

  Vector3 airflow = TM.GetTranspose().RotateVec(globalWind - velocity);

  const float airDensity = mAeroplane->GetAirDensity();
  Vector3 dynamicPressure = (0.5f * airDensity) * ComponentSquareSigned(airflow);

  Vector3 areas(
    mExtents.y * mExtents.z,
    mExtents.z * mExtents.x,
    mExtents.x * mExtents.y
    );

  float CDScale = mAeroplane->GetAeroplaneSettings().mDragScale;

  Vector3 dragForce = ComponentMultiply(ComponentMultiply(areas, dynamicPressure), mCD * CDScale);

  Vector3 force = TM.RotateVec(dragForce);
  mAeroplane->GetPhysics()->ApplyWorldForceAtCoM(force);

#if 0
  const float forceRenderScale = 50.3f;
  RenderManager::GetInstance().GetDebugRenderer().DrawVector(
    TM.GetTrans(), force * forceRenderScale, Vector3(1,0,0));
#endif

}

//---------------------------------------------------------------------------------------------------------------------
void Fuselage::UpdatePostPhysics(float deltaTime)
{
}

