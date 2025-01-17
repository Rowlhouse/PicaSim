#ifndef AEROPLANE_PHYSICS_H
#define AEROPLANE_PHYSICS_H

//#include "GameSettings.h"
//#include "btDistanceConstraint.h"

#include "Accelerometer.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "BulletSoftBody/btSoftBody.h"
#include "Entity.h"
#include "Gyro.h"
#include "Rope.h"
#include "Viewport.h"
#include "Wheel.h"
#include "Helpers.h"
#include <cstdint>
#include <string>
#include <vector>

class Wing;
class Fuselage;
class Engine;

struct CrashObject
{
  std::string        mName;
  std::string        mEngineName;
  float              mCrashPropSpeed;
  btCollisionObject* mCollisionObject;
  btCollisionShape*  mCollisionShape;
  btTransform        mLocalTransform;
};

class PicaSimVehicleRaycaster : public btDefaultVehicleRaycaster
{
public:
  PicaSimVehicleRaycaster(btDynamicsWorld* world, const btCollisionShape* vehicleCollision) : btDefaultVehicleRaycaster(world, vehicleCollision) {}

  void* castRay(const btVector3& from,const btVector3& to, btVehicleRaycasterResult& result) OVERRIDE;
};

class AeroplanePhysics : public Entity
{
public:
  typedef std::vector<CrashObject> CrashObjects;
  typedef std::vector<Gyro> Gyros;
  typedef std::vector<Accelerometer> Accelerometers;
  typedef std::vector<Wheel> Wheels;
  typedef std::vector<Wing*> Wings;
  typedef std::vector<Fuselage*> Fuselages;
  typedef std::vector<Engine*> Engines;

  void Init(class TiXmlDocument& aeroplaneDoc, class Aeroplane* aeroplane, uint32_t& checksum);
  void Terminate();

  void Launched();

  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  /// Sets the object transform (offset from CoM) and CoM velocities
  void SetTransform(const Transform& tm, const Vector3& vel, const Vector3& angVel);

  /// Sets the CoM velocities
  void SetVelocity(const Vector3& vel, const Vector3& angVel);

  /// Gets the CoM velocity in world space
  Vector3 GetVelocity() const {return mCoMVel;}

  /// Gets the angular velocity in world space
  Vector3 GetAngularVelocity() const {return mCoMAngVel;}

  /// The object transform (offset from CoM)
  const Transform& GetTransform() const {return mTM;}

  /// The object CoM transform
  const Transform& GetCoMTransform() const {return mCoMTM;}

  /// Gets the moments of inertia in the CoM transform frame
  Vector3 GetInertiaLocal() const;

  /// Returns the velocity at the world space point
  Vector3 GetVelAtPoint(const Vector3 point) const;

  /// Returns the total area from marked up wings
  float GetTotalWingArea() const;

  /// Applies the world force at the centre of mass
  void ApplyWorldForceAtCoM(const Vector3& force);

  /// Applies the world force at the world position
  void ApplyWorldForceAtWorldPosition(const Vector3& force, const Vector3& worldPos);

  /// Applies the world torque
  void ApplyWorldTorque(const Vector3& torque);

  float GetMass() const {return mMass;}

  /// Gets the named engine, if it exists
  const Engine* GetEngine(const std::string& name) const;

  const Engines& GetEngines() const {return mEngines;}

  /// Gets the named wing, if it exists
  const Wing* GetWing(const std::string& name) const;

  /// Gets all the wings with the name
  size_t GetWings(const std::string& name, Wing** wings, size_t maxWings) const;

  /// Gets the named wheel, if it exists
  const Wheel* GetWheel(const std::string& name) const;

  /// Gets the named gyro, if it exists
  const Gyro* GetGyro(const std::string& name) const;

  /// Gets the named accelerometer, if it exists
  const Accelerometer* GetAccelerometer(const std::string& name) const;

  /// Adds an angular momentum (e.g. for an engine) in world space.
  void AddExtraAngularMomentum(const Vector3 extra);

  /// Returns the AABB min/max in local space
  void getLocalAABB(Vector3& AABBMin, Vector3& AABBMax) const;
  float getAABBRadius() const;

  /// Returns if the plane is controllable - controlled will be false for tethered planes that are 
  /// at their tether range. responsiveness will be 0 if tether lines are wound.
  void GetIsControllable(bool& controlled, float& responsiveness) const;

  float GetContactTime() const {return mContactTime;}

  /// Returns true if tethering is active, in which case the handle position is set
  bool GetTetherHandlePos(Vector3& pos) const;

  void AttachToTug(Aeroplane& tugAeroplane);

  // This is in completely the wrong place
  void AddRopesToViewport(Viewport* viewport);

  /// Returns the force associated with the tow rope, or zero if it doesn't exist
  float GetTowForce() const;

private:
  /// This gets called to update the positions etc of the wings
  void UpdateComponentsPostPhysics(float deltaTime, bool checkForCrash);

  /// This gets called to apply forces
  void UpdateComponentsPrePhysics(float deltaTime, const struct TurbulenceData& turbulenceData);

  /// For non-component based forces
  void UpdateAeroplanePrePhysics(float deltaTime);

  /// For tethering
  void UpdateTetheringPrePhysics(float deltaTime);

  bool CalculateIsInContact() const;

  Aeroplane* mAeroplane;

  btCompoundShape*  mCollisionShape;
  btRigidBody*      mRigidBody;

  btRaycastVehicle* mRaycastVehicle;
  btVehicleRaycaster* mVehicleRaycaster;

  btSoftBody*       mRopeSoftBody;
  Vector3           mHandlePos;
  float             mTetherDistance;

  // Tracks the twist etc when tethering
  float             mTwist;
  float             mLastTwist;

  Rope              mRopeUpper;
  Rope              mRopeLower;

  Aeroplane*        mTugAeroplane;
  btSoftBody*       mTowRopeSoftBody;
  Rope              mTowRope;
  Vector3           mTwoRopeAttachmentOffset;

  float             mMass;

  /// Offset of the CoM position/principle axes of inertia from the object transform
  Transform         mCoMOffset;
  /// Inverse CoM offset
  Transform         mInvCoMOffset;

  /// The physics transform. May differ from the Aeroplane TM as the latter may interpolate between physics updates.
  Transform         mTM;
  Transform         mCoMTM;

  /// Linear velocity of the centre of mass
  Vector3           mCoMVel;
  /// Angular velocity
  Vector3           mCoMAngVel;

  Vector3           mAABBMax;
  Vector3           mAABBMin;

  /// > 0 if we're touching something
  float mContactTime;

  Wings mWings;
  Fuselages mFuselages;
  Engines mEngines;
  Wheels mWheels;
  Gyros mGyros;
  Accelerometers mAccelerometers;
  CrashObjects mCrashObjects;

  float mWingSpan;
  float mWingChord;
  float mCMRollFromY;

  float mFlutterDragSpeed;
  float mFlutterDragScale;
};

#endif
