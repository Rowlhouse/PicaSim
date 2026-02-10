#include "Accelerometer.h"

#include "Framework.h"
#include "Aerofoil.h"
#include "AerofoilParameters.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "Controller.h"
#include "Environment.h"
#include "PicaSim.h"

//======================================================================================================================
Accelerometer::Accelerometer()
{
}

//======================================================================================================================
void Accelerometer::Init(class TiXmlElement* accelerometerElement, class Aeroplane* aeroplane)
{
    TRACE_METHOD_ONLY(ONCE_2);
    mAeroplane = aeroplane;
    const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();

    mAxis = Vector3(0,0,0);
    mOffset = Vector3(0,0,0);
    mMinAccel = 0.0f;
    mMaxAccel = 1.0f;
    mMinOutput = 0.0f;
    mMaxOutput = 0.0f;
    mSmoothTime = 0.0f;

    // Read data for this engine
    mName = readStringFromXML(accelerometerElement, "name");

    mAxis = readVector3FromXML(accelerometerElement, "axis");
    mOffset = readVector3FromXML(accelerometerElement, "offset");
    mMinAccel = readFloatFromXML(accelerometerElement, "minAccel");
    mMaxAccel = readFloatFromXML(accelerometerElement, "maxAccel");
    mMinOutput = readFloatFromXML(accelerometerElement, "minOutput");
    mMaxOutput = readFloatFromXML(accelerometerElement, "maxOutput");
    mSmoothTime = readFloatFromXML(accelerometerElement, "smoothTime");

    mAxis.Normalise();

    // Initial values
    Launched();
}

//======================================================================================================================
void Accelerometer::Terminate()
{
    TRACE_METHOD_ONLY(ONCE_2);
}

//======================================================================================================================
void Accelerometer::Launched()
{
    Vector3 pos = mAeroplane->GetTransform().TransformVec(mOffset);
    mPrevDeltaTime = 1.0f;
    mPrevVel = Vector3(0,0,0);
    mSmoothedAccel = 0.0f;
    mOutput = 0.0f;
}

//======================================================================================================================
void Accelerometer::UpdatePrePhysics(float deltaTime)
{
    float dt = deltaTime > 0.0f ? deltaTime : mPrevDeltaTime;

    const Controller& controller = mAeroplane->GetController();

    const Transform& tm = mAeroplane->GetPhysics()->GetCoMTransform();
    const Vector3 axis = tm.RotateVec(mAxis);
    const Vector3 pos = tm.TransformVec(mOffset);

#if 0
    Vector3 vel = (pos - mPrevPos) / deltaTime;
    Vector3 accel = (vel - mPrevVel) / deltaTime;
#else
    Vector3 vel = mAeroplane->GetPhysics()->GetVelAtPoint(pos);
    Vector3 accel = (vel - mPrevVel) / dt;
#endif
    accel -= BulletVector3ToVector3(EntityManager::GetInstance().GetDynamicsWorld().getGravity());

    //RenderManager::GetInstance().GetDebugRenderer().DrawPoint(pos, 0.5f, Vector3(1,0,0));
    //RenderManager::GetInstance().GetDebugRenderer().DrawArrow(pos, pos + axis * 2.0f, Vector3(1,0,1));
    //RenderManager::GetInstance().GetDebugRenderer().DrawArrow(pos, pos + vel * 0.1f, Vector3(0,1,0));
    //RenderManager::GetInstance().GetDebugRenderer().DrawArrow(pos, pos + accel * 0.01f, Vector3(0,0,1));

    float a = accel.Dot(axis);
    SmoothExponential(mSmoothedAccel, deltaTime, a, mSmoothTime);

    float t = (mSmoothedAccel - mMinAccel) / (mMaxAccel - mMinAccel);
    t = ClampToRange(t, 0.0f, 1.0f);
    mOutput = mMinOutput + (mMaxOutput - mMinOutput) * t;

    //TRACE("accel = (%5.2f %5.2f %5.2f) smoothedAccel = %5.2f output = %5.2f\n", accel.x, accel.y, accel.z, mSmoothedAccel, mOutput);

    if (deltaTime > 0.0f)
    {
        mPrevVel = vel;
        mPrevDeltaTime = deltaTime;
    }
}

