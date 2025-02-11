#include "Aerofoil.h"
#include "AerofoilDefinition.h"
#include "AeroplanePhysics.h"
#include "Aeroplane.h"
#include "Environment.h"
#include "GameSettings.h"
#include "Trace.h"

//----------------------------------------------------------------------------------------------------------------------
void Aerofoil::Init(
  const char* name, 
  const AerofoilConfiguration& configuration, 
  Aeroplane* aeroplane,
  uint32_t& checksum)
{
  TRACE_METHOD_ONLY(1);
  mDefinition = AerofoilDefinition::Create(name);
  mConfiguration = configuration;
  mAeroplane = aeroplane;
  mPrevFlying = 1.0f;
  mLastAoA = 0.0f;

  char filename[256];
  sprintf(filename, "%s.xml", name);
  GetFileChecksum(checksum, filename);
}

//----------------------------------------------------------------------------------------------------------------------
void Aerofoil::Terminate()
{
  TRACE_METHOD_ONLY(1);
  AerofoilDefinition::Release(mDefinition);
  mDefinition = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void Aerofoil::UpdatePrePhysics(
  float deltaTime, 
  const AerofoilControl& aerofoilControl, 
  const TurbulenceData& turbulenceData, 
  Vector3& wash, Vector3& force)
{
  mPrevFlying = mDefinition->ApplyForces(
    mAeroplane, mConfiguration, mParameters, aerofoilControl, 
    Environment::GetInstance(), turbulenceData,
    wash, force, deltaTime, mLastAoA);
}

//---------------------------------------------------------------------------------------------------------------------
void Aerofoil::UpdatePostPhysics(float deltaTime)
{
  mParameters.mTM = mConfiguration.mOffset * mAeroplane->GetPhysics()->GetTransform();
  // Evaluate velocity at the leading edge
  mParameters.mVel = mAeroplane->GetPhysics()->GetVelAtPoint(mParameters.mTM.GetTrans() + 
    mParameters.mTM.RowX() * (mConfiguration.mExtents.x * 0.5f * mPrevFlying));

  //RenderManager::GetInstance().GetDebugRenderer().DrawPoint(mParameters.mTM.GetTrans(), 0.1f, Vector3(1,1,1));
  //RenderManager::GetInstance().GetDebugRenderer().DrawVector(mParameters.mTM.GetTrans(), mParameters.mVel * 1.0f, Vector3(1,1,1));
}

