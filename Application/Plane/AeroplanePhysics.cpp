#include "AeroplanePhysics.h"
#include "Aeroplane.h"
#include "Aerofoil.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "Controller.h"
#include "DebugRenderer.h"
#include "HelpersXML.h"
#include "ProfilageMarmelade.h"
#include "Runway.h"
#include "Trace.h"
#include "Wing.h"
#include "Fuselage.h"
#include "JetEngine.h"
#include "PropellerEngine.h"
#include "PicaSim.h"
#include "DimensionalScaling.h"
#include "Environment.h"
#include "EntityManager.h"

// #include "tinyxml.h"
// #include <vector>
// #include <IwUtil.h>
// #include <s3e.h>

typedef btAlignedObjectArray<btVector3> Pts;

//---------------------------------------------------------------------------------------------------------------------
static void AddPointToAABB(Vector3& aabbMin, Vector3& aabbMax, Vector3 pt)
{
  aabbMax.x = Maximum(aabbMax.x, pt.x);
  aabbMax.y = Maximum(aabbMax.y, pt.y);
  aabbMax.z = Maximum(aabbMax.z, pt.z);

  aabbMin.x = Minimum(aabbMin.x, pt.x);
  aabbMin.y = Minimum(aabbMin.y, pt.y);
  aabbMin.z = Minimum(aabbMin.z, pt.z);
}

//---------------------------------------------------------------------------------------------------------------------
static void AddPoints(Pts& pts, Vector3& aabbMin, Vector3& aabbMax, const Vector3& extents, const Transform& tm, float margin)
{
  Vector3 h = extents * 0.5f;

  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3( h.x,  h.y,  h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3( h.x,  h.y, -h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3( h.x, -h.y,  h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3( h.x, -h.y, -h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3(-h.x,  h.y,  h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3(-h.x,  h.y, -h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3(-h.x, -h.y,  h.z)));
  AddPointToAABB(aabbMin, aabbMax, tm.TransformVec(Vector3(-h.x, -h.y, -h.z)));

  h.x = Maximum(0.001f, h.x - margin);
  h.y = Maximum(0.001f, h.y - margin);
  h.z = Maximum(0.001f, h.z - margin);

  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3( h.x,  h.y,  h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3( h.x,  h.y, -h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3( h.x, -h.y,  h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3( h.x, -h.y, -h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3(-h.x,  h.y,  h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3(-h.x,  h.y, -h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3(-h.x, -h.y,  h.z))));
  pts.push_back(Vector3ToBulletVector3(tm.TransformVec(Vector3(-h.x, -h.y, -h.z))));

}

//---------------------------------------------------------------------------------------------------------------------
static void ReduceCollidingPoints(Pts& pts, float eps)
{
  size_t numPts = pts.size();
  for (size_t i = 0 ; i < numPts ; ++i)
  {
    btVector3& p = pts[i];
    btVector3 avPos = p;
    int num = 1;
    for (size_t j = i + 1 ; j < numPts ; )
    {
      float d = (avPos - pts[j]).length();
      if (d < eps)
      {
        avPos += pts[j];
        pts[j] = pts[numPts-1];
        pts.pop_back();
        --numPts;
        ++num;
      }
      else
      {
        ++j;
      }
    }
    p = avPos * (1.0f/num);
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool IsOnRunway(const Vector3& pos)
{
  const Runway* runway = Environment::GetInstance().GetRunway();
  if (!runway)
    return false;
  return runway->ContainsPoint(pos);
}

//---------------------------------------------------------------------------------------------------------------------
void* PicaSimVehicleRaycaster::castRay(const btVector3& from,const btVector3& to, btVehicleRaycasterResult& result)
{
  void* retVal = btDefaultVehicleRaycaster::castRay(from, to, result);
  if (retVal)
  {
    if (!IsOnRunway(BulletVector3ToVector3(from)))
    {
      float roughness = Environment::GetInstance().GetSurfaceRoughness(BulletVector3ToVector3(from));
      float x = from.x();
      float y = from.y();
      float z = FastSin(x * 10.0f + y * 7.3f) + FastSin(x * 5.0f + y * 9.7f);
      z *= roughness;
      float raylen = to.distance(from);
      result.m_distFraction += z/ raylen;
    }
  }
  return retVal;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::Init(TiXmlDocument& aeroplaneDoc, Aeroplane* aeroplane, uint32_t& checksum)
{
  TRACE_METHOD_ONLY(1);
  mAeroplane = aeroplane;
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();

  DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);

  // Distance constraint can only be created once everything is set up
  mRopeSoftBody = 0;
  mTwist = mLastTwist = 0.0f;
  mContactTime = 0.0f;
  mTowRopeSoftBody = 0;
  mTugAeroplane = 0;

  mAABBMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  mAABBMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

  EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_LOOP_PRE_PHYSICS);
  EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_LOOP_POST_PHYSICS);
  EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);

  TiXmlHandle docHandle( &aeroplaneDoc );

  // Read some whole-aeroplane stuff


  // Create the gyros
  TiXmlHandle gyroHandle = docHandle.FirstChild("Physics").FirstChild("Gyros");
  TiXmlElement* gyroElement = gyroHandle.ToElement();
  for (int iGyro = 0 ; ; ++iGyro)
  {
    TiXmlElement* gyroElement = gyroHandle.Child("Gyro", iGyro).ToElement();
    if (!gyroElement)
      break;
    mGyros.push_back(Gyro());
    mGyros.back().Init(gyroElement, mAeroplane);
  }

  // Create the accelerometers
  TiXmlHandle accelerometerHandle = docHandle.FirstChild("Physics").FirstChild("Accelerometers");
  TiXmlElement* accelerometerElement = accelerometerHandle.ToElement();
  for (int iAccelerometer = 0 ; ; ++iAccelerometer)
  {
    TiXmlElement* accelerometerElement = accelerometerHandle.Child("Accelerometer", iAccelerometer).ToElement();
    if (!accelerometerElement)
      break;
    mAccelerometers.push_back(Accelerometer());
    mAccelerometers.back().Init(accelerometerElement, mAeroplane);
  }

  // Create the aerodynamics
  TiXmlHandle aerodynamicsHandle = docHandle.FirstChild("Physics").FirstChild("Aerodynamics");
  TiXmlElement* aerodynamicsElement = aerodynamicsHandle.ToElement();

  for (int iWing = 0 ; ; ++iWing)
  {
    TiXmlElement* wingElement = aerodynamicsHandle.Child("Wing", iWing).ToElement();
    if (!wingElement)
      break;
    Wing* wing= new Wing;
    wing->Init(wingElement, aerodynamicsHandle, mAeroplane, checksum);
    mWings.push_back(wing);
  }

  for (int iFuselage = 0 ; ; ++iFuselage)
  {
    TiXmlElement* fuselageElement = aerodynamicsHandle.Child("Fuselage", iFuselage).ToElement();
    if (!fuselageElement)
      break;
    Fuselage* fuselage= new Fuselage;
    fuselage->Init(fuselageElement, mAeroplane);
    mFuselages.push_back(fuselage);
  }

  for (int iJetEngine = 0 ; ; ++iJetEngine)
  {
    TiXmlElement* engineElement = aerodynamicsHandle.Child("JetEngine", iJetEngine).ToElement();
    if (!engineElement)
      break;
    Engine* engine = new JetEngine;
    engine->Init(engineElement, mAeroplane);
    mEngines.push_back(engine);
  }

  for (int iPropellerEngine = 0 ; ; ++iPropellerEngine)
  {
    TiXmlElement* engineElement = aerodynamicsHandle.Child("PropellerEngine", iPropellerEngine).ToElement();
    if (!engineElement)
      break;
    Engine* engine = new PropellerEngine;
    engine->Init(engineElement, mAeroplane);
    mEngines.push_back(engine);
  }

  // Create the geometry
  mCollisionShape = new btCompoundShape();
  std::vector<float> masses;
  mMass = 0.0f;
  float margin = ds.GetScaledLength(0.01f);;

  Pts collidingPoints;

  // Loop over the wings to create geometry
  for (Wings::iterator itWing = mWings.begin() ; itWing != mWings.end() ; ++itWing)
  {
    Wing* wing = *itWing;

    float mass = wing->GetMass();
    masses.push_back(mass);
    mMass += mass;
    Vector3 extents = wing->GetExtents();
    const Transform& wingTMLocal = wing->GetTMLocal();
    btCollisionShape* collisionShape = new btBoxShape(Vector3ToBulletVector3(extents * 0.5f));
    mCollisionShape->addChildShape(TransformToBulletTransform(wingTMLocal), collisionShape);

    bool collide = wing->GetCollide();
    if (collide)
      AddPoints(collidingPoints, mAABBMin, mAABBMax, extents, wingTMLocal, margin);
  }

  // Loop over the fuselages to create geometry
  for (Fuselages::iterator itFuselage = mFuselages.begin() ; itFuselage != mFuselages.end() ; ++itFuselage)
  {
    Fuselage* fuselage = *itFuselage;

    float mass = fuselage->GetMass();
    masses.push_back(mass);
    mMass += mass;
    Vector3 extents = fuselage->GetExtents();
    const Transform& fuselageTMLocal = fuselage->GetTMLocal();
    btCollisionShape* collisionShape = new btBoxShape(Vector3ToBulletVector3(extents * 0.5f));
    mCollisionShape->addChildShape(TransformToBulletTransform(fuselageTMLocal), collisionShape);
    bool collide = fuselage->GetCollide();
    if (collide)
      AddPoints(collidingPoints, mAABBMin, mAABBMax, extents, fuselageTMLocal, margin);
  }

  // Loop over the additional geometry
  TiXmlHandle geometryHandle = docHandle.FirstChild("Physics").FirstChild("Geometry");
  TiXmlElement* geometryElement = geometryHandle.ToElement();
  std::string shapeType;
  for (int iShape = 0 ; ; ++iShape)
  {
    TiXmlElement* shapeElement = geometryHandle.Child("Shape", iShape).ToElement();
    if (!shapeElement)
      break;
    readFromXML(shapeElement, "type", shapeType);
    float mass = Maximum(readFloatFromXML(shapeElement, "mass"), 1e-8f);
    ds.ScaleMass(mass);
    masses.push_back(mass);
    mMass += mass;
    if (shapeType == "box")
    {
      Vector3 extents = ds.GetScaledLength(readVector3FromXML(shapeElement, "extents"));
      Vector3 position = ds.GetScaledLength(readVector3FromXML(shapeElement, "position"));
      Vector3 rotation = readVector3FromXML(shapeElement, "rotation");

      float angle = DegreesToRadians(rotation.GetLength());
      if (angle > 0.0f)
        rotation.Normalise();
      else
        rotation = Vector3(1,0,0);

      Quat quat(rotation, angle);
      btTransform localTransform;
      localTransform.setRotation(QuatToBulletQuaternion(quat));
      localTransform.setOrigin(Vector3ToBulletVector3(position));

      btCollisionShape* collisionShape = new btBoxShape(Vector3ToBulletVector3(extents * 0.5f));
      mCollisionShape->addChildShape(localTransform, collisionShape);

      bool collide = true;
      readFromXML(shapeElement, "collide", collide);
      if (collide)
        AddPoints(collidingPoints, mAABBMin, mAABBMax, extents, BulletTransformToTransform(localTransform), margin);
    }
    else
    {
      IwAssert(ROWLHOUSE, false);
    }
  } // Loop over shapes

  // Loop over the crash shapes
  TiXmlHandle crashShapesHandle = docHandle.FirstChild("Physics").FirstChild("CrashShapes");
  TiXmlElement* crashShapesElement = crashShapesHandle.ToElement();
  for (int iShape = 0 ; ; ++iShape)
  {
    TiXmlElement* shapeElement = crashShapesHandle.Child("CrashShape", iShape).ToElement();
    if (!shapeElement)
      break;
    readFromXML(shapeElement, "type", shapeType);
    CrashObject crashObject;
    if (shapeType == "box")
    {
      Vector3 extents = ds.GetScaledLength(readVector3FromXML(shapeElement, "extents"));
      Vector3 position = ds.GetScaledLength(readVector3FromXML(shapeElement, "position"));
      Vector3 rotation = readVector3FromXML(shapeElement, "rotation");
      crashObject.mName = readStringFromXML(shapeElement, "name");
      crashObject.mEngineName = readStringFromXML(shapeElement, "engineName");
      crashObject.mCrashPropSpeed = readFloatFromXML(shapeElement, "crashPropSpeed") / sqrtf(as.mSizeScale);

      float angle = DegreesToRadians(rotation.GetLength());
      if (angle > 0.0f)
        rotation.Normalise();
      else
        rotation = Vector3(1,0,0);

      Quat quat(rotation, angle);
      crashObject.mLocalTransform.setRotation(QuatToBulletQuaternion(quat));
      crashObject.mLocalTransform.setOrigin(Vector3ToBulletVector3(position));
      crashObject.mCollisionObject = new btCollisionObject;
      crashObject.mCollisionShape = new btBoxShape(Vector3ToBulletVector3(extents * 0.5f));
      crashObject.mCollisionShape->setMargin(0.001f);
      crashObject.mCollisionObject->setCollisionShape(crashObject.mCollisionShape);
      crashObject.mCollisionObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);



      mCrashObjects.push_back(crashObject);
    }
    else
    {
      IwAssert(ROWLHOUSE, false);
    }
  }

  // Create the dynamics representation
  TiXmlElement* dynamicsElement = docHandle.FirstChild("Physics").FirstChild("Dynamics").ToElement();
  IwAssert(ROWLHOUSE, dynamicsElement);

  mWingSpan    = ds.GetScaledLength(readFloatFromXML(dynamicsElement, "wingSpan"));
  mWingChord   = ds.GetScaledLength(readFloatFromXML(dynamicsElement, "wingChord"));
  mCMRollFromY = ds.GetScaledLength(readFloatFromXML(dynamicsElement, "CMRollFromY"));

  float friction = 0.5f;
  Vector3 frictionScale(1.0f, 1.0f, 1.0f);
  readFromXML(dynamicsElement, "friction", friction);
  readFromXML(dynamicsElement, "frictionScale", frictionScale);

  mFlutterDragScale = readFloatFromXML(dynamicsElement, "flutterDragScale");
  mFlutterDragSpeed = ds.GetScaledLength(readFloatFromXML(dynamicsElement, "flutterDragSpeed"));

  btVector3 localInertia(0,0,0);

  // Calculate inertia
  {
    btTransform principal;
    mCollisionShape->calculatePrincipalAxisTransform(&masses[0], principal, localInertia);

    // Add on the user-specified offsets
    float origMass = mMass;
    float extraMass = 0.01f * as.mExtraMassPerCent * mMass;
    principal.getOrigin() += Vector3ToBulletVector3(ds.GetScaledLength(as.mExtraMassOffset)) * 
      extraMass / (origMass + extraMass);
    mMass += extraMass;

    TRACE("CG x position is %5.3f %5.3f %5.3f\n", principal.getOrigin().getX(), principal.getOrigin().getY(), principal.getOrigin().getZ());

#if 1
    btTransform invPrincipal = principal.inverse();

    mCoMOffset = BulletTransformToTransform(principal);
    mInvCoMOffset = BulletTransformToTransform(invPrincipal);

    // Now we have to shift all the shapes so the rigid body is aligned with the CoM frame
    int numCollisionShapes = mCollisionShape->getNumChildShapes();
    for (int iShape = 0 ; iShape != numCollisionShapes ; ++iShape)
      mCollisionShape->getChildTransform(iShape) =  invPrincipal * mCollisionShape->getChildTransform(iShape);

    for (CrashObjects::iterator it = mCrashObjects.begin() ; it != mCrashObjects.end() ; ++it)
    {
      CrashObject& object = *it;
      object.mLocalTransform = invPrincipal * object.mLocalTransform;
    }
#else
    mCoMOffset.SetIdentity();
    mInvCoMOffset.SetIdentity();
#endif
  }

  // Now replace all the collision with a convex hull
  {
    while (mCollisionShape->getNumChildShapes())
    {
      btCollisionShape* shape = mCollisionShape->getChildShape(0);
      mCollisionShape->removeChildShapeByIndex(0);
      delete shape;
    }

    TRACE("Original convex shape has %d points\n", collidingPoints.size());
    btConvexHullShape originalShape(&collidingPoints[0].x(), collidingPoints.size());
    btShapeHull hull(&originalShape);
    hull.buildHull(2.2f * margin);

    TRACE("Making convex shape with %d points\n", hull.numVertices());
    btConvexHullShape* convexShape = new btConvexHullShape(&hull.getVertexPointer()->x(), hull.numVertices());
    convexShape->setMargin(margin * 1.0f); // Margin goes _out_
    btTransform tm = TransformToBulletTransform(mInvCoMOffset);

    mCollisionShape->addChildShape(tm, convexShape);
  }

  mTM = mAeroplane->GetTransform();
  mCoMTM = mCoMOffset * mTM;

  mCoMVel = Vector3(0,0,0);
  mCoMAngVel = Vector3(0,0,0);

  btRigidBody::btRigidBodyConstructionInfo rbInfo(mMass, 0, mCollisionShape, localInertia);
  rbInfo.m_startWorldTransform = TransformToBulletTransform(mCoMTM);
  rbInfo.m_friction = friction;
  rbInfo.m_restitution = 0.0f;
  // disable sleeping
  rbInfo.m_linearSleepingThreshold = 0.0f;
  rbInfo.m_angularSleepingThreshold = 0.0f;
  mRigidBody = new btRigidBody(rbInfo);
  mRigidBody->setFlags(BT_ENABLE_GYROSCOPIC_FORCE_IMPLICIT_CATTO); // Same as BODY

  mRigidBody->setLinearVelocity(Vector3ToBulletVector3(mCoMVel));
  mRigidBody->setAngularVelocity(Vector3ToBulletVector3(mCoMAngVel));

  mRigidBody->setAnisotropicFriction(Vector3ToBulletVector3(frictionScale));

  EntityManager::GetInstance().GetDynamicsWorld().addRigidBody(mRigidBody);

  // Create the vehicle
  TRACE_FILE_IF(1) TRACE("AeroplanePhysics::Init Vehicle");
  btRaycastVehicle::btVehicleTuning vehicleTuning;

  const btCollisionShape* vehicleCollision = mRigidBody->getCollisionShape();

  mVehicleRaycaster = new PicaSimVehicleRaycaster(&EntityManager::GetInstance().GetDynamicsWorld(), vehicleCollision);
  mRaycastVehicle = new btRaycastVehicle(vehicleTuning, mRigidBody, mVehicleRaycaster);
  EntityManager::GetInstance().GetDynamicsWorld().addVehicle(mRaycastVehicle);
  mRaycastVehicle->setCoordinateSystem(0, 1, 2);

  // Loop over the wheels
  TiXmlHandle wheelsHandle = docHandle.FirstChild("Physics").FirstChild("Wheels");
  TiXmlElement* wheelsElement = wheelsHandle.ToElement();
  for (int iWheel = 0 ; ; ++iWheel)
  {
    TiXmlElement* wheelElement = wheelsHandle.Child("Wheel", iWheel).ToElement();
    if (!wheelElement)
      break;

    Wheel wheel;
    wheel.Init(mAeroplane, wheelElement, mRaycastVehicle, vehicleTuning, iWheel, mInvCoMOffset, as);
    mWheels.push_back(wheel);
  }

  // Update our wings etc
  UpdateComponentsPostPhysics(0.0f, false);
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::Terminate()
{
  TRACE_METHOD_ONLY(1);

  if (mRopeSoftBody)
  {
    EntityManager::GetInstance().GetDynamicsWorld().removeSoftBody(mRopeSoftBody);
    delete mRopeSoftBody;
    mRopeSoftBody = 0;
  }

  if (mTowRopeSoftBody)
  {
    EntityManager::GetInstance().GetDynamicsWorld().removeSoftBody(mTowRopeSoftBody);
    delete mTowRopeSoftBody;
    mTowRopeSoftBody = 0;
  }

  EntityManager::GetInstance().GetDynamicsWorld().removeCollisionObject(mRigidBody);
  delete mRigidBody;
  mRigidBody = 0;

  int numCollisionShapes = mCollisionShape->getNumChildShapes();
  for (int iShape = 0 ; iShape != numCollisionShapes ; ++iShape)
    delete (mCollisionShape->getChildShape(iShape));
  delete mCollisionShape;
  mCollisionShape = 0;

  while (!mWings.empty())
  {
    Wing* wing = mWings.back();
    wing->Terminate();
    delete wing;
    mWings.pop_back();
  }

  while (!mGyros.empty())
  {
    Gyro& gyro = mGyros.back();
    gyro.Terminate();
    mGyros.pop_back();
  }

  while (!mAccelerometers.empty())
  {
    Accelerometer& accelerometer = mAccelerometers.back();
    accelerometer.Terminate();
    mAccelerometers.pop_back();
  }

  while (!mFuselages.empty())
  {
    Fuselage* fuselage = mFuselages.back();
    fuselage->Terminate();
    delete fuselage;
    mFuselages.pop_back();
  }

  while (!mEngines.empty())
  {
    Engine* engine = mEngines.back();
    engine->Terminate();
    delete engine;
    mEngines.pop_back();
  }

  while (!mCrashObjects.empty())
  {
    CrashObject& crashObject = mCrashObjects.back();
    delete crashObject.mCollisionShape;
    delete crashObject.mCollisionObject;
    mCrashObjects.pop_back();
  }

  mWheels.clear();

  EntityManager::GetInstance().GetDynamicsWorld().removeVehicle(mRaycastVehicle);
  delete mVehicleRaycaster;
  mVehicleRaycaster = 0;
  delete mRaycastVehicle;
  mRaycastVehicle = 0;

  EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_LOOP_PRE_PHYSICS);
  EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_LOOP_POST_PHYSICS);
  EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::Launched()
{
  for (Gyros::iterator it = mGyros.begin() ; it != mGyros.end() ; ++it)
  {
    it->Launched();
  }
  for (Accelerometers::iterator it = mAccelerometers.begin() ; it != mAccelerometers.end() ; ++it)
  {
    it->Launched();
  }
  for (Engines::iterator it = mEngines.begin() ; it != mEngines.end() ; ++it)
  {
    (*it)->Launched();
  }
  for (Wheels::iterator it = mWheels.begin() ; it != mWheels.end() ; ++it)
  {
    it->Launched();
  }
  if (mRopeSoftBody)
  {
    EntityManager::GetInstance().GetDynamicsWorld().removeSoftBody(mRopeSoftBody);
    delete mRopeSoftBody;
    mRopeSoftBody = 0;
  }
  mTwist = mLastTwist = 0.0f;
  mContactTime = 0.0f;
}

//---------------------------------------------------------------------------------------------------------------------
static btVector3 GetNodePos(const btSoftBody::tNodeArray& nodes, float f)
{
  const int numNodes = nodes.size();
  if (f < 0.0f)
  {
    f = -f;
    const int i0 = Minimum((int) (f * (numNodes-1)), numNodes-2);
    const float frac = f * (numNodes - 1) - i0;
    btVector3 result = nodes[i0+1].m_x * frac + nodes[i0].m_x * (1.0f - frac);
    result = 2.0f * nodes[0].m_x - result;
    return result;
  }
  else if (f > 1.0f)
  {
    f = 2.0f - f;
    const int i0 = Minimum((int) (f * (numNodes-1)), numNodes-2);
    const float frac = f * (numNodes - 1) - i0;
    btVector3 result = nodes[i0+1].m_x * frac + nodes[i0].m_x * (1.0f - frac);
    result = 2.0f * nodes[numNodes-1].m_x - result;
    return result;
  }
  else
  {
    const int i0 = Minimum((int) (f * (numNodes-1)), numNodes-2);
    const float frac = f * (numNodes - 1) - i0;
    btVector3 result = nodes[i0+1].m_x * frac + nodes[i0].m_x * (1.0f - frac);
    return result;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DrawTether(
  Rope& tether, 
  const btSoftBody::tNodeArray& nodes, 
  const Vector3& startPos, 
  const Vector3& endPos, 
  int numSegments, 
  float smoothing)
{
  const int numNodes = nodes.size();
  const Vector3 startOffset = startPos - BulletVector3ToVector3(nodes[0].m_x);
  const Vector3 endOffset = endPos - BulletVector3ToVector3(nodes[numNodes-1].m_x);

  Rope::Points& points = tether.GetPoints();
  points.resize(numSegments + 1, Vector3(0,0,0));
  points[0] = startPos;

  for (int iPoint = 0 ; iPoint != numSegments ; ++iPoint)
  {
    float fraction = iPoint / (numSegments - 1.0f);
    Vector3 offset = endOffset * fraction + startOffset * (1.0f - fraction);

    const btVector3 pos0 = GetNodePos(nodes, fraction);
    const btVector3 pos1p = GetNodePos(nodes, fraction - smoothing);
    const btVector3 pos1m = GetNodePos(nodes, fraction + smoothing);

    float w0 = 1.0f;
    float w1 = 1.0f;

    Vector3 pos = BulletVector3ToVector3((pos0 * w0 + (pos1p + pos1m) * w1) / (w0 + 2.0f * w1));
    pos += offset;

    points[iPoint+1] = pos;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void CompressTethers(Rope* ropes[], size_t numRopes, float compression)
{
  size_t numPts = ropes[0]->GetPoints().size();
  for (size_t i = 0 ; i != numPts ; ++i)
  {
    float fraction = i / (numPts - 1.0f);
    float c = fabsf(2.0f * (fraction - 0.5f));
    c = (1.0f - compression) * 1.0f + compression * c;
    c = ClampToRange(1.0f - c, 0.0f, 1.0f);

    Vector3 avPos(0,0,0);
    for (size_t j = 0 ; j != numRopes ; ++j)
      avPos += ropes[j]->GetPoints()[i];
    avPos *= 1.0f/numRopes;
    for (size_t j = 0 ; j != numRopes ; ++j)
      ropes[j]->GetPoints()[i] += c * (avPos - ropes[j]->GetPoints()[i]);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::EntityUpdate(float deltaTime, int entityLevel)
{
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
  const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;
  if (entityLevel == ENTITY_LEVEL_LOOP_PRE_PHYSICS)
  {
    IW_PROFILE("AeroplanePhysics::EntityUpdate Pre Physics");
    UpdateTetheringPrePhysics(deltaTime);

    // Update for non-component based things;
    UpdateAeroplanePrePhysics(deltaTime);
  }
  else if (entityLevel == ENTITY_LEVEL_LOOP_POST_PHYSICS)
  {
    IW_PROFILE("AeroplanePhysics::EntityUpdate Post Physics");
    // Position
    mCoMTM = BulletTransformToTransform(mRigidBody->getWorldTransform());

    // Prevent terrain tunnelling.
    float terrainHeight = Environment::GetInstance().GetTerrain().GetTerrainHeightFast(
      mCoMTM.GetTrans().x, mCoMTM.GetTrans().y, es.mTerrainSettings.mCollideWithPlain);
    Vector3 pos = mCoMTM.GetTrans();
    float altitude = pos.z - terrainHeight;
    if (altitude < 0.0f)
    {
      pos.z = terrainHeight;
      mCoMTM.SetTrans(pos);
      mRigidBody->setWorldTransform(TransformToBulletTransform(mCoMTM));
    }
    mTM = mInvCoMOffset * mCoMTM;

    // velocity
    const Vector3 newCoMVel = BulletVector3ToVector3(mRigidBody->getVelocityInLocalPoint(btVector3(0,0,0)));
    const Vector3 newCoMAngVel = BulletVector3ToVector3(mRigidBody->getAngularVelocity());
    const Vector3 deltaVel = newCoMVel - mCoMVel;
    const Vector3 deltaAngVel = newCoMAngVel - mCoMAngVel;
    mCoMVel = newCoMVel;
    mCoMAngVel = newCoMAngVel;

    // Check for crash
    if (deltaTime > 0.0f)
    {
      if (!mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_AIRFRAME))
      {
        float deltaVelX = fabsf(deltaVel.Dot(mTM.RowX()));
        float deltaVelY = fabsf(deltaVel.Dot(mTM.RowY()));
        float deltaVelZ = fabsf(deltaVel.Dot(mTM.RowZ()));

        float deltaAngVelX = RadiansToDegrees(fabsf(deltaAngVel.Dot(mTM.RowX())));
        float deltaAngVelY = RadiansToDegrees(fabsf(deltaAngVel.Dot(mTM.RowY())));
        float deltaAngVelZ = RadiansToDegrees(fabsf(deltaAngVel.Dot(mTM.RowZ())));

        //TRACE("dv = %5.2f %5.2f %5.2f dav = %5.2f %5.2f %5.2f", deltaVelX, deltaVelY, deltaVelZ, deltaAngVelX, deltaAngVelY, deltaAngVelZ);

        float distScale = as.mSizeScale;
        float timeScale = sqrtf(distScale);
        float velScale = distScale / timeScale;
        float angVelScale = 1.0f / timeScale;

        if (deltaVelX > as.mCrashDeltaVel.x * velScale)
        {
          TRACE("Crash from velocity change in x %5.2f > %5.2f\n", deltaVelX, as.mCrashDeltaVel.x * velScale);
          mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_AIRFRAME);
        }
        if (deltaVelY > as.mCrashDeltaVel.y * velScale)
        {
          TRACE("Crash from velocity change in y %5.2f > %5.2f\n", deltaVelY, as.mCrashDeltaVel.y * velScale);
          mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_AIRFRAME);
        }
        if (deltaVelZ > as.mCrashDeltaVel.z * velScale)
        {
          TRACE("Crash from velocity change in y %5.2f > %5.2f\n", deltaVelZ, as.mCrashDeltaVel.z * velScale);
          mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_AIRFRAME);
        }
        if (deltaAngVelX > as.mCrashDeltaAngVel.x * angVelScale)
        {
          TRACE("Crash from ang velocity change in x %5.2f > %5.2f\n", deltaAngVelX, as.mCrashDeltaAngVel.x * angVelScale);
          mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_AIRFRAME);
        }
        if (deltaAngVelY > as.mCrashDeltaAngVel.y * angVelScale)
        {
          TRACE("Crash from ang velocity change in y %5.2f > %5.2f\n", deltaAngVelY, as.mCrashDeltaAngVel.y * angVelScale);
          mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_AIRFRAME);
        }
        if (deltaAngVelZ > as.mCrashDeltaAngVel.z * angVelScale)
        {
          TRACE("Crash from ang velocity change in z %5.2f > %5.2f\n", deltaAngVelZ, as.mCrashDeltaAngVel.z * angVelScale);
          mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_AIRFRAME);
        }
      }

      if (!mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_UNDERCARRIAGE))
      {
        for (size_t iWheel = 0 ; iWheel != mWheels.size() ; ++iWheel)
        {
          Wheel& wheel = mWheels[iWheel];
          if (wheel.GetCollapsed(as.mCrashSuspensionForceScale))
          {
            float force = wheel.GetSuspensionForce();
            TRACE("Crash from wheel suspension force\n", force);
            mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_UNDERCARRIAGE);
          }
        }
      }
    }

    // Update our wings etc
    UpdateComponentsPostPhysics(deltaTime, true);
  }
  else if (entityLevel == ENTITY_LEVEL_POST_PHYSICS)
  {
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    if (options.mAerofoilInfo >= 0)
    {
      for (int i=0;i<mRaycastVehicle->getNumWheels();i++)
      {
        btWheelInfo& wheel = mRaycastVehicle->getWheelInfo(i);

        RenderManager::GetInstance().GetDebugRenderer().DrawVector(
          BulletVector3ToVector3(wheel.m_raycastInfo.m_hardPointWS),
          BulletVector3ToVector3(wheel.m_raycastInfo.m_wheelDirectionWS * wheel.m_raycastInfo.m_suspensionLength),
          Vector3(1,0,0));
      }
    }
    // Update the twist. 
    if (as.mTetherLines > 1)
    {
      Vector3 dirToPlane = (mTM.GetTrans() - mHandlePos).GetNormalised();
      Vector3 handleLeft = (Vector3(0,0,1).Cross(dirToPlane)).GetNormalised();
      Vector3 handleUp = dirToPlane.Cross(handleLeft);
      float up = mTM.RowZ().Dot(handleUp);
      float left = mTM.RowZ().Dot(handleLeft);
      float angle = -atan2f(left, up); // +ve is pitching up
      mTwist = angle / (2.0f * PI);
      while (mTwist > mLastTwist + 0.5f)
        mTwist -= 1.0f;
      while (mTwist < mLastTwist - 0.5f)
        mTwist += 1.0f;
      mLastTwist = mTwist;
    }

    if (mRopeSoftBody)
    {
      int numNodes = mRopeSoftBody->m_nodes.size();
      for (int iNode = 0 ; iNode < numNodes ; ++iNode)
      {
        btSoftBody::Node& node = mRopeSoftBody->m_nodes[iNode];
        float terrainZ = 0.04f * as.mSizeScale + Environment::GetInstance().GetTerrain().GetTerrainHeightFast(node.m_x.x(), node.m_x.y(), false);
        if (node.m_x.z() < terrainZ)
        {
          node.m_x.setZ(terrainZ);
          if (node.m_v.getZ() < 0.0f)
            node.m_v.setZ(0.0f);
        }
      }
      Vector3 startRopePos = mHandlePos;
      Vector3 dirToPlane = mTM.GetTrans() - startRopePos;
      Vector3 rightDir = dirToPlane.Cross(Vector3(0,0,1)).GetNormalised();
      startRopePos += rightDir * 0.1f;
      Vector3 endRopePos = mCoMTM.TransformVec(mInvCoMOffset.TransformVec(as.mTetherVisualOffset * as.mSizeScale));
      int numSegments = 24;
      if (as.mTetherLines == 1)
      {
        DrawTether(mRopeUpper, mRopeSoftBody->m_nodes, startRopePos, endRopePos, numSegments, 0.1f);
      }
      else if (as.mTetherLines == 2)
      {
        float handleD = 0.16f * as.mSizeScale; // Only go up, to avoid going down through the ground
        float planeD = 0.02f * as.mSizeScale;
        float compression = Maximum(2.0f * fabsf(mTwist + 0.25f) - 0.5f, 0.0f);
        DrawTether(mRopeUpper, mRopeSoftBody->m_nodes, startRopePos + Vector3(0.0f,0.0f,handleD), endRopePos - mTM.RowX() * planeD, numSegments, 0.1f);
        DrawTether(mRopeLower, mRopeSoftBody->m_nodes, startRopePos                       , endRopePos + mTM.RowX() * planeD, numSegments, 0.1f);
        Rope* ropes[] = {&mRopeUpper, &mRopeLower};
        CompressTethers(ropes, 2, compression);
      }

      mRopeUpper.SetColour(mAeroplane->GetAeroplaneSettings().mTetherColour);
      mRopeLower.SetColour(mAeroplane->GetAeroplaneSettings().mTetherColour);

      const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
      if (options.mDrawAeroplaneCoM != Options::COM_NONE)
      {
        Vector3 point = mCoMTM.TransformVec(mInvCoMOffset.TransformVec(as.mTetherPhysicsOffset * as.mSizeScale));
        RenderManager::GetInstance().GetDebugRenderer().DrawPoint(point, 1.0f, Vector3(1,1,0));
      }
    }
    else
    {
      mRopeUpper.GetPoints().clear();
      mRopeLower.GetPoints().clear();
    }

    if (mTowRopeSoftBody && mTugAeroplane)
    {
      int numNodes = mTowRopeSoftBody->m_nodes.size();
      for (int iNode = 0 ; iNode < numNodes ; ++iNode)
      {
        btSoftBody::Node& node = mTowRopeSoftBody->m_nodes[iNode];
        float terrainZ = 0.04f + Environment::GetInstance().GetTerrain().GetTerrainHeightFast(node.m_x.x(), node.m_x.y(), false);
        if (node.m_x.z() < terrainZ)
        {
          node.m_x.setZ(terrainZ);
          node.m_v.setZ(0.0f);
        }
        //RenderManager::GetInstance().GetDebugRenderer().DrawPoint(BulletVector3ToVector3(node.m_x), 1.0f, Vector3(1,1,0));
      }

      if (mTowRopeSoftBody->m_anchors[0].m_influence)
      {
        DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);
        mTwoRopeAttachmentOffset = 
          mAeroplane->GetTransform().TransformVec(ds.GetScaledLength(as.mNoseHookOffset)) -
          BulletVector3ToVector3(mTowRopeSoftBody->m_nodes[0].m_x);
      }
      else
      {
        SmoothExponential(mTwoRopeAttachmentOffset, deltaTime, Vector3(0,0,0), 1.0f);
      }
      Vector3 startRopePos = BulletVector3ToVector3(mTowRopeSoftBody->m_nodes[0].m_x) + mTwoRopeAttachmentOffset;
      Vector3 endRopePos = mTowRopeSoftBody->m_anchors[1].m_influence ?         
        mTugAeroplane->GetTransform().TransformVec(mTugAeroplane->GetAeroplaneSettings().mTailHookOffset * mTugAeroplane->GetAeroplaneSettings().mSizeScale) :
        BulletVector3ToVector3(mTowRopeSoftBody->m_nodes[mTowRopeSoftBody->m_nodes.size()-1].m_x);
      int numSegments = 24;
      DrawTether(mTowRope, mTowRopeSoftBody->m_nodes, startRopePos, endRopePos, numSegments, 0.1f);
      mTowRope.SetColour(mAeroplane->GetAeroplaneSettings().mTetherColour);
      mTowRope.SetColour(mAeroplane->GetAeroplaneSettings().mTetherColour);
    }
    else
    {
      mTowRope.GetPoints().clear();
    }
  }

  if (mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_NONE && mTowRopeSoftBody)
  {
    mTowRopeSoftBody->m_anchors[0].m_influence = 0.0f;
  }
}

struct CrashCallback : public btCollisionWorld::ContactResultCallback
{
  CrashCallback(btCollisionObject* aeroplaneCollision) : mAeroplaneCollision(aeroplaneCollision), mGotCollision(false) {}

  virtual bool needsCollision(btBroadphaseProxy* proxy) const OVERRIDE
  {
    IwAssert(ROWLHOUSE, proxy);
    if (mAeroplaneCollision == static_cast<btCollisionObject*>(proxy->m_clientObject))
      return false;
    else
      return true;
  }

  //! Called with each contact for your own processing (e.g. test if contacts fall in within sensor parameters)
#if BT_BULLET_VERSION < 280
  virtual  btScalar  addSingleResult(
    btManifoldPoint& cp,  
    const btCollisionObject* colObj0Wrap,
    int partId0,
    int index0,
    const btCollisionObject* colObj1Wrap,
    int partId1,
    int index1)  OVERRIDE
#else
  virtual  btScalar  addSingleResult(
    btManifoldPoint& cp,  
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0,
    int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1,
    int index1)  OVERRIDE
#endif
  {
    if (cp.getDistance() <= 0.0f)
      mGotCollision = true;
    return 0; // not actually sure if return value is used for anything...?
  }

  bool mGotCollision;
  btCollisionObject* mAeroplaneCollision;
};

//---------------------------------------------------------------------------------------------------------------------
bool AeroplanePhysics::CalculateIsInContact() const
{
  btDynamicsWorld& dynamicsWorld = EntityManager::GetInstance().GetDynamicsWorld();
  int numManifolds = dynamicsWorld.getDispatcher()->getNumManifolds();
  for (int i=0; i < numManifolds ; i++)
  {
    const btPersistentManifold* contactManifold = dynamicsWorld.getDispatcher()->getManifoldByIndexInternal(i);
    const btCollisionObject* obA = contactManifold->getBody0();
    const btCollisionObject* obB = contactManifold->getBody1();

    if (obA == mRigidBody || obB == mRigidBody)
    {
      if (contactManifold->getNumContacts() > 0)
      {
        return true;
      }
    }
  }

  for (Wheels::const_iterator it = mWheels.begin() ; it != mWheels.end() ; ++it)
  {
    const Wheel& wheel = *it;
    if (wheel.GetSuspensionForce() > 0.0f)
      return true;
  }

  return false;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::UpdateComponentsPostPhysics(float deltaTime, bool checkForCrash)
{
  for (Wings::iterator it = mWings.begin() ; it != mWings.end() ; ++it)
  {
    Wing* wing = *it;
    wing->UpdatePostPhysics(deltaTime);
  }
  for (Fuselages::iterator it = mFuselages.begin() ; it != mFuselages.end() ; ++it)
  {
    Fuselage* fuselage = *it;
    fuselage->UpdatePostPhysics(deltaTime);
  }
  for (Engines::iterator it = mEngines.begin() ; it != mEngines.end() ; ++it)
  {
    Engine* engine = *it;
    engine->UpdatePostPhysics(deltaTime);
  }

  if (CalculateIsInContact())
  {
    mContactTime += deltaTime;
  }
  else
  {
    mContactTime = 0.0f;
  }

  // Check for crash
  if (checkForCrash && !mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_PROPELLER))
  {
    for (CrashObjects::iterator it = mCrashObjects.begin() ; it != mCrashObjects.end() ; ++it)
    {
      CrashObject& crashObject = *it;

      const std::string& engineName = crashObject.mEngineName;
      if (!engineName.empty())
      {
        const Engine* engine = GetEngine(engineName);
        if (engine)
        {
          float propSpeed = engine->GetPropSpeed();
          if (propSpeed < crashObject.mCrashPropSpeed)
            continue;
        }
      }

      crashObject.mCollisionObject->setWorldTransform(mRigidBody->getWorldTransform() * crashObject.mLocalTransform);

      btCollisionWorld* collisionWorld = &EntityManager::GetInstance().GetDynamicsWorld();
      CrashCallback crashCallback(mRigidBody);
      collisionWorld->contactTest(crashObject.mCollisionObject, crashCallback);
      if (crashCallback.mGotCollision)
      {
        TRACE("Crash from crash object\n");
        mAeroplane->SetCrashFlag(Aeroplane::CRASHFLAG_PROPELLER);
        break;
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::UpdateAeroplanePrePhysics(float deltaTime)
{
  btVector3 sphereCenter;
  TurbulenceData turbulenceData = Environment::GetInstance().PrepareTurbulenceData(mTM.GetTrans());

  // Update our wings etc
  UpdateComponentsPrePhysics(deltaTime, turbulenceData);

  // And update non-components
  Vector3 globalWind = Environment::GetInstance().GetWindAtPosition(
    mCoMTM.GetTrans(), Environment::WIND_TYPE_ALL, &turbulenceData);
  IwAssert(ROWLHOUSE, globalWind == globalWind);

  // Airflow is relative to the plane so +ve X is forward, +ve Y is left, +ve Z is up
  Vector3 airflow = mCoMTM.GetTranspose().RotateVec(globalWind - mCoMVel);

  // Cm = M / (q * S * c)
  // where M is the pitching moment, q is the dynamic pressure, S is the planform area and 
  // c is the length of the chord.

  float CMRoll = mCMRollFromY;
  const float airDensity = mAeroplane->GetAirDensity();
  Vector3 dynamicPressure = 0.5f * airDensity * ComponentMultiply(airflow, Abs(airflow));

  float rollingMoment = -CMRoll * dynamicPressure.y * mWingSpan * mWingChord * mWingSpan;
  Vector3 moment = rollingMoment * mTM.RowX();
  ApplyWorldTorque(moment);

  // Apply additional "flutter" drag
  if (mFlutterDragScale*mFlutterDragSpeed != 0.0f)
  {
    float CDFlutter = airflow.GetLengthSquared() / Square(mFlutterDragSpeed);

    float flutterDrag = mFlutterDragScale * dynamicPressure.x * Square(mWingSpan) * mAeroplane->GetAeroplaneSettings().mDragScale;
    Vector3 flutterForce = flutterDrag * mTM.RowX();
    ApplyWorldForceAtCoM(flutterForce);
  }

  // Compensate for rotation - otherwise we tend to accelerate tangentially when it should just be radial acceleration.
  btVector3 force = mRigidBody->getTotalForce();
  const btTransform& TM = mRigidBody->getWorldTransform();
  btVector3 localForce = quatRotate(TM.getRotation().inverse(), force);

  btTransform predictedTM;
  mRigidBody->predictIntegratedTransform(deltaTime * 0.5f, predictedTM);

  force = quatRotate(predictedTM.getRotation(), localForce);
  mRigidBody->setTotalForce(force);
}

//---------------------------------------------------------------------------------------------------------------------
bool AeroplanePhysics::GetTetherHandlePos(Vector3& pos) const
{
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
  if (as.mTetherLines == 0)
    return false;
  pos = mHandlePos;
  return true;
}


//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::UpdateTetheringPrePhysics(float deltaTime)
{
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
  DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);

  // Update/enable/disable tethering
  if (as.mTetherLines > 0)
  {
    if (!mRopeSoftBody)
    {
      int numMidPoints = 8;
      Vector3 to = mCoMTM.TransformVec(mInvCoMOffset.TransformVec(ds.GetScaledLength(as.mTetherPhysicsOffset)));
      mHandlePos = mTM.TransformVec(Vector3(0.0f, ds.GetScaledLength(as.mTetherDistanceLeft), 0.0f));
      mHandlePos.z = Environment::GetTerrain().GetTerrainHeight(mHandlePos.x, mHandlePos.y, true) + 1.5f;

      mRopeSoftBody = btSoftBodyHelpers::CreateRope(
        EntityManager::GetInstance().GetSoftBodyWorldInfo(), 
        Vector3ToBulletVector3(mHandlePos), Vector3ToBulletVector3(to),
        numMidPoints, 1);

      mTetherDistance = (to - mHandlePos).GetLength();
      float radius = ds.GetScaledLength(0.00015f);
      float density = 3000 * as.mMassScale; // kg/m^3

      float volume = PI * Square(radius) * mTetherDistance;
      float mass = volume * density;
      float sideArea = 2.0f * radius * mTetherDistance;

      int maxIters = 16;
      float subSteps = ClampToRange<float>((float) PicaSim::GetInstance().GetSettings().mOptions.mFrameworkSettings.mPhysicsSubsteps, 4.0f, 12.0f);
      float p = (subSteps - 4.0f) / 4.0f;
      float fiters = float(maxIters) / powf(2, p);
      int iters = ClampToRange((int) (fiters + 0.5f), 4, maxIters);
      iters = 2 * (iters / 2);

      mRopeSoftBody->setTotalMass(mass);
      mRopeSoftBody->m_cfg.aeromodel = btSoftBody::eAeroModel::V_Drag;
      mRopeSoftBody->m_cfg.piterations = iters; // Needs to be even to avoid an offset?
      mRopeSoftBody->m_cfg.viterations = 0;
      mRopeSoftBody->m_cfg.diterations = 0;
      mRopeSoftBody->m_cfg.citerations = 0; // cluster iterations - related to joints?
      mRopeSoftBody->m_cfg.kVCF = 1.0f; // Velocities correction factor (Baumgarte) - used for drift iters
      mRopeSoftBody->m_cfg.kDP = 0.0f; // Damping coefficient [0,1]
      mRopeSoftBody->m_cfg.kDG = 0.5f; // Drag coefficient
      mRopeSoftBody->m_cfg.kAHR = 1.0f; // Anchor hardness
      mRopeSoftBody->m_cfg.kDF = 0.0f; // Dynamic friction
      mRopeSoftBody->m_cfg.collisions = 0;
      mRopeSoftBody->m_cfg.ropeNodeArea = sideArea / (numMidPoints+1);

      mRopeSoftBody->m_cfg.m_vsequence.clear();
      mRopeSoftBody->m_cfg.m_psequence.clear();
      mRopeSoftBody->m_cfg.m_dsequence.clear();

      mRopeSoftBody->m_cfg.m_psequence.push_back(btSoftBody::ePSolver::Anchors);
      mRopeSoftBody->m_cfg.m_psequence.push_back(btSoftBody::ePSolver::Linear);  
      mRopeSoftBody->m_cfg.m_vsequence.push_back(btSoftBody::eVSolver::Linear);
      mRopeSoftBody->m_cfg.m_dsequence.push_back(btSoftBody::ePSolver::Linear);

      EntityManager::GetInstance().GetDynamicsWorld().addSoftBody(mRopeSoftBody, 2, 0);
      mRopeSoftBody->appendAnchor(mRopeSoftBody->m_nodes.size()-1, mRigidBody, true);
    }
    mRopeSoftBody->m_windVelocity = Vector3ToBulletVector3(Environment::GetInstance().GetWindAtPosition(mTM.GetTrans(), Environment::WIND_TYPE_SMOOTH));
  }
  else
  {
    if (mRopeSoftBody)
    {
      EntityManager::GetInstance().GetDynamicsWorld().removeSoftBody(mRopeSoftBody);
      delete mRopeSoftBody;
      mRopeSoftBody = 0;
    }
  }

  if (mTowRopeSoftBody && mTugAeroplane)
  {
    Transform tugTM = mTugAeroplane->GetTransform();
    mTowRopeSoftBody->m_windVelocity = Vector3ToBulletVector3(
      Environment::GetInstance().GetWindAtPosition(tugTM.GetTrans(), Environment::WIND_TYPE_SMOOTH));

    if (mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_AEROTOW)
    {
      float force = mTowRopeSoftBody->m_anchors[0].m_appliedForce.length();
      float aeroplaneWeight = mMass * EntityManager::GetInstance().GetDynamicsWorld().getGravity().length();

      float ratio = force / aeroplaneWeight;
      if (ratio > as.mAeroTowRopeStrength)
      {
        TRACE("Tow release with force ratio = %5.2f\n", ratio);
        mAeroplane->SetLaunchMode(Aeroplane::LAUNCHMODE_NONE);
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::UpdateComponentsPrePhysics(float deltaTime, const TurbulenceData& turbulenceData)
{
  mRigidBody->setExtraLocalAngularMomentum(btVector3(0,0,0));

  const Controller& controller = mAeroplane->GetController();
  for (size_t iWheel = 0 ; iWheel != mWheels.size() ; ++iWheel)
  {
    Wheel& wheel = mWheels[iWheel];
    wheel.UpdatePrePhysics(mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_UNDERCARRIAGE) ? 0 : &controller, deltaTime);
    if (mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_UNDERCARRIAGE))
    {
      wheel.SetBrake(5.0f);
    }
  }

  // Update gyros first as they provide controls
  for (Gyros::iterator it = mGyros.begin() ; it != mGyros.end() ; ++it)
  {
    Gyro& gyro = *it;
    gyro.UpdatePrePhysics(deltaTime);
  }
  for (Accelerometers::iterator it = mAccelerometers.begin() ; it != mAccelerometers.end() ; ++it)
  {
    Accelerometer& accelerometer = *it;
    accelerometer.UpdatePrePhysics(deltaTime);
  }
  // Update engines next as they generate wash
  for (Engines::iterator it = mEngines.begin() ; it != mEngines.end() ; ++it)
  {
    Engine* engine = *it;
    engine->UpdatePrePhysics(deltaTime, turbulenceData);
  }
  for (Wings::iterator it = mWings.begin() ; it != mWings.end() ; ++it)
  {
    Wing* wing = *it;
    wing->UpdatePrePhysics(deltaTime, turbulenceData);
  }
  for (Fuselages::iterator it = mFuselages.begin() ; it != mFuselages.end() ; ++it)
  {
    Fuselage* fuselage = *it;
    fuselage->UpdatePrePhysics(deltaTime, turbulenceData);
  }
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 AeroplanePhysics::GetVelAtPoint(const Vector3 point) const
{
  Vector3 offset = point - mCoMTM.GetTrans();
  Vector3 vel = BulletVector3ToVector3(mRigidBody->getVelocityInLocalPoint(Vector3ToBulletVector3(offset)));
  return vel;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::ApplyWorldForceAtCoM(const Vector3& force)
{
  mRigidBody->applyCentralForce(Vector3ToBulletVector3(force));
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::ApplyWorldForceAtWorldPosition(const Vector3& force, const Vector3& worldPos)
{
  CheckSanity(force);
  mRigidBody->applyForce(Vector3ToBulletVector3(force), Vector3ToBulletVector3(worldPos - mCoMTM.GetTrans()));
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::ApplyWorldTorque(const Vector3& torque)
{
  CheckSanity(torque);
  mRigidBody->applyTorque(Vector3ToBulletVector3(torque));
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::AddExtraAngularMomentum(const Vector3 extra)
{
  CheckSanity(extra);
  btTransform tm = mRigidBody->getWorldTransform();
  btVector3 localExtra = tm.inverse().getBasis() * Vector3ToBulletVector3(extra);
  btVector3 current = mRigidBody->getExtraLocalAngularMomentum();
  mRigidBody->setExtraLocalAngularMomentum(current + localExtra);
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::getLocalAABB(Vector3& AABBMin, Vector3& AABBMax) const
{
  AABBMax = mAABBMax;
  AABBMin = mAABBMin;
}

//---------------------------------------------------------------------------------------------------------------------
float AeroplanePhysics::getAABBRadius() const
{
  float d = (mAABBMax - mAABBMin).GetLength();
  return d * 0.5f;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::SetVelocity(const Vector3& vel, const Vector3& angVel)
{
  CheckSanity(vel);
  CheckSanity(angVel);
  mCoMVel = vel;
  mCoMAngVel = angVel;
  mRigidBody->setLinearVelocity(Vector3ToBulletVector3(vel));
  mRigidBody->setAngularVelocity(Vector3ToBulletVector3(angVel));
  mRigidBody->activate(true);
}


//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::SetTransform(const Transform& tm, const Vector3& vel, const Vector3& angVel)
{
  CheckSanity(tm);
  CheckSanity(vel);
  CheckSanity(angVel);
  mTM = tm;
  mCoMTM = mCoMOffset * tm;

  mRigidBody->setWorldTransform(TransformToBulletTransform(mCoMTM));
  SetVelocity(vel, angVel);

  // Update our wings etc
  UpdateComponentsPostPhysics(0.0f, false);
}

//---------------------------------------------------------------------------------------------------------------------
const Engine* AeroplanePhysics::GetEngine(const std::string& engineName) const
{
  for (size_t i = 0 ; i != mEngines.size() ; ++i)
  {
    if (mEngines[i]->GetName() == engineName)
      return mEngines[i];
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
const Wheel* AeroplanePhysics::GetWheel(const std::string& name) const
{
  for (size_t i = 0 ; i != mWheels.size() ; ++i)
  {
    if (mWheels[i].GetName() == name)
      return &mWheels[i];
  }
  return 0;
}


//---------------------------------------------------------------------------------------------------------------------
const Wing* AeroplanePhysics::GetWing(const std::string& name) const
{
  for (size_t i = 0 ; i != mWings.size() ; ++i)
  {
    if (mWings[i]->GetName() == name)
      return mWings[i];
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
size_t AeroplanePhysics::GetWings(const std::string& name, Wing** wings, size_t maxWings) const
{
  size_t nWings = 0;
  for (size_t i = 0 ; i != mWings.size() && nWings < maxWings; ++i)
  {
    if (mWings[i]->GetName() == name)
    {
      wings[nWings] = mWings[i];
      ++nWings;
    }
  }
  return nWings;
}

//---------------------------------------------------------------------------------------------------------------------
const Gyro* AeroplanePhysics::GetGyro(const std::string& name) const
{
  for (size_t i = 0 ; i != mGyros.size() ; ++i)
  {
    if (mGyros[i].GetName() == name)
      return &mGyros[i];
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
const Accelerometer* AeroplanePhysics::GetAccelerometer(const std::string& name) const
{
  for (size_t i = 0 ; i != mAccelerometers.size() ; ++i)
  {
    if (mAccelerometers[i].GetName() == name)
      return &mAccelerometers[i];
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::GetIsControllable(bool& controlled, float& responsiveness) const
{
  controlled = true;
  responsiveness = 1.0f;

  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
  if (as.mTetherRequiresTension && mRopeSoftBody && mRopeSoftBody->m_anchors.size() > 0)
  {
    float force = mRopeSoftBody->m_anchors[0].m_appliedForce.length();
    float aeroplaneWeight = mMass * EntityManager::GetInstance().GetDynamicsWorld().getGravity().length();
    controlled = force > aeroplaneWeight * 0.1f;
  }

  float freeTwists = 3.0f;
  float responsivenessPerTwist = 0.1f;

  float twist = Maximum(fabsf(mTwist) - freeTwists, 0.0f);
  responsiveness = powf(2.0f, -twist * responsivenessPerTwist);
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 AeroplanePhysics::GetInertiaLocal() const
{
  return BulletVector3ToVector3(mRigidBody->getLocalInertia());
}

//---------------------------------------------------------------------------------------------------------------------
float AeroplanePhysics::GetTowForce() const
{
  if (!mTowRopeSoftBody)
    return -1.0f;
  float force = mTowRopeSoftBody->m_anchors[0].m_appliedForce.length();
  return force;
}


//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::AttachToTug(Aeroplane& tugAeroplane)
{
  if (mTowRopeSoftBody)
  {
    EntityManager::GetInstance().GetDynamicsWorld().removeSoftBody(mTowRopeSoftBody);
    delete mTowRopeSoftBody;
    mTowRopeSoftBody = 0;
  }

  mTugAeroplane = &tugAeroplane;

  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
  const AeroplaneSettings& tas = tugAeroplane.GetAeroplaneSettings();
  DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);

  Vector3 noseHookPos = mAeroplane->GetTransform().TransformVec(ds.GetScaledLength(as.mNoseHookOffset));
  Vector3 tailHookPos = tugAeroplane.GetTransform().TransformVec(ds.GetScaledLength(tas.mTailHookOffset));

  int numMidPoints = 4;

  mTowRopeSoftBody = btSoftBodyHelpers::CreateRope(
    EntityManager::GetInstance().GetSoftBodyWorldInfo(), 
    Vector3ToBulletVector3(noseHookPos), Vector3ToBulletVector3(tailHookPos),
    numMidPoints, 0);

  mTetherDistance = (noseHookPos - tailHookPos).GetLength();
  float radius = getAABBRadius() / 1000.0f;
  float density = 500; // kg/m^3

  float volume = PI * Square(radius) * mTetherDistance;
  float mass = volume * density;
  float sideArea = 2.0f * radius * mTetherDistance;

  float drag = 0.8f;
  mass *= as.mAeroTowRopeMassScale;
  drag *= as.mAeroTowRopeDragScale;

  int maxIters = 16;
  float subSteps = ClampToRange<float>((float) PicaSim::GetInstance().GetSettings().mOptions.mFrameworkSettings.mPhysicsSubsteps, 4.0f, 12.0f);
  float p = (subSteps - 4.0f) / 4.0f;
  float fiters = float(maxIters) / powf(2, p);
  int iters = ClampToRange((int) (fiters + 0.5f), 4, maxIters);
  iters = 2 * (iters / 2); // make it even

  mTowRopeSoftBody->setTotalMass(mass);
  mTowRopeSoftBody->m_cfg.aeromodel = btSoftBody::eAeroModel::V_Drag;
  mTowRopeSoftBody->m_cfg.piterations = iters; // Needs to be even to avoid an offset?
  mTowRopeSoftBody->m_cfg.viterations = 0;
  mTowRopeSoftBody->m_cfg.diterations = 0;
  mTowRopeSoftBody->m_cfg.citerations = 0; // cluster iterations - related to joints?
  mTowRopeSoftBody->m_cfg.kVCF = 1.0f; // Velocities correction factor (Baumgarte) - used for drift iters
  mTowRopeSoftBody->m_cfg.kDP = 0.0f; // Damping coefficient [0,1]
  mTowRopeSoftBody->m_cfg.kDG = 0.8f; // Drag coefficient
  mTowRopeSoftBody->m_cfg.kLF = 0.0f; // Lift coefficient
  mTowRopeSoftBody->m_cfg.kAHR = 1.0f; // Anchor hardness
  mTowRopeSoftBody->m_cfg.kDF = 0.0f; // Dynamic friction
  mTowRopeSoftBody->m_cfg.collisions = 0;
  mTowRopeSoftBody->m_cfg.ropeNodeArea = sideArea / (numMidPoints+1);

  mTowRopeSoftBody->m_cfg.m_vsequence.clear();
  mTowRopeSoftBody->m_cfg.m_psequence.clear();
  mTowRopeSoftBody->m_cfg.m_dsequence.clear();

  mTowRopeSoftBody->m_cfg.m_psequence.push_back(btSoftBody::ePSolver::Anchors);
  mTowRopeSoftBody->m_cfg.m_psequence.push_back(btSoftBody::ePSolver::Linear);  
  mTowRopeSoftBody->m_cfg.m_vsequence.push_back(btSoftBody::eVSolver::Linear);
  mTowRopeSoftBody->m_cfg.m_dsequence.push_back(btSoftBody::ePSolver::Linear);

  EntityManager::GetInstance().GetDynamicsWorld().addSoftBody(mTowRopeSoftBody, 4, 0);
  mTowRopeSoftBody->appendAnchor(0, mRigidBody, true);
  mTowRopeSoftBody->appendAnchor(mTowRopeSoftBody->m_nodes.size()-1, tugAeroplane.GetPhysics()->mRigidBody, true);

  mTwoRopeAttachmentOffset = Vector3(0,0,0);
}

//---------------------------------------------------------------------------------------------------------------------
float AeroplanePhysics::GetTotalWingArea() const
{
  float totalArea = 0.0f;
  for (size_t iWing = 0 ; iWing != mWings.size() ; ++iWing)
  {
    const Wing* wing = mWings[iWing];
    float area = wing->GetExtents().x * wing->GetExtents().y;
    totalArea += area;
  }
  return totalArea;
}

//---------------------------------------------------------------------------------------------------------------------
void AeroplanePhysics::AddRopesToViewport(Viewport* viewport)
{
  viewport->AddRenderObject(&mRopeUpper);
  viewport->AddRenderObject(&mRopeLower);
  viewport->AddRenderObject(&mTowRope);
}

