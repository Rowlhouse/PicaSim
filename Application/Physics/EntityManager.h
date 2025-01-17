#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "Helpers.h"

//#include "btBulletDynamicsCommon.h"
//#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"

#include "BulletSoftBody/btSoftBody.h"
#include "Entity.h"
#include <map>
enum EntityLevels
{
  ENTITY_LEVEL_ANY = -9999,  // Only used for queries
  ENTITY_LEVEL_CONTROL           =   0,
  ENTITY_LEVEL_PRE_PHYSICS       = 200,
  // The next block get called multiple times to sub-step the physics
  ENTITY_LEVEL_LOOP_PRE_PHYSICS  = 300,
  ENTITY_LEVEL_LOOP_PHYSICS      = 400, // Used internally to trigger the physics update - don't use
  ENTITY_LEVEL_LOOP_POST_PHYSICS = 500,
  // Single post-physics update
  ENTITY_LEVEL_POST_PHYSICS      = 600
};

/// Manages non-singleton objects, though the app is responsible for creating/destroying them. 
/// Also lets them register for updates, and provides access to physics.
class EntityManager : public Entity
{
public:
  /// Key into Entities is the "level", so smaller numbers get called first.
  typedef std::multimap<int, Entity*> Entities;

  /// Gets the singleton
  static EntityManager& GetInstance(); 

  /// Creates the singleton
  static void Init(const struct FrameworkSettings& frameworkSettings);

  /// Deletes the singleton
  static void Terminate();

  void UpdateEntities(float deltaTime);

  /// Register for a callback. The ordering is determined by the level, smaller 
  /// numbers happen earlier, with ranges suggested by EntityLevels
  void RegisterEntity(Entity* entity, int entityLevel);

  void UnregisterEntity(Entity* entity, int entityLevel);

  bool IsEntityRegistered(const Entity* entity, int entityLevel) const;

  Entities& GetEntities() {return mEntities;}
  const Entities& GetEntities() const {return mEntities;}

  /// When there are multiple iterations, this is only true for the first
  bool IsFinalIteration() {return mIsFinalIteration;}

  class btSoftRigidDynamicsWorld& GetDynamicsWorld() {return *mDynamicsWorld;}
  const class btSoftRigidDynamicsWorld& GetDynamicsWorld() const {return *mDynamicsWorld;}

  btSoftBodyWorldInfo& GetSoftBodyWorldInfo() {return mSoftBodyWorldInfo;}
  const btSoftBodyWorldInfo& GetSoftBodyWorldInfo() const {return mSoftBodyWorldInfo;}

  const struct FrameworkSettings& GetFrameworkSettings() const {return mFrameworkSettings;}

private:
  EntityManager(const struct FrameworkSettings& frameworkSettings);

  /// We register with ourselves in order to update physics
  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  static EntityManager* mInstance;

  /// All the Entities in the game. The first of these will always be the one under user control.
  Entities mEntities; 

  const struct FrameworkSettings& mFrameworkSettings;

  // Physics
  class btCollisionConfiguration*            mCollisionConfiguration;
  class btCollisionDispatcher*               mDispatcher;
  class btBroadphaseInterface*               mBroadPhase;
  class btSequentialImpulseConstraintSolver* mSolver;
  class btSoftRigidDynamicsWorld*            mDynamicsWorld;
  btSoftBodyWorldInfo                        mSoftBodyWorldInfo;

  bool mIsFinalIteration;
  float mLeftOverTime;
};

#endif
