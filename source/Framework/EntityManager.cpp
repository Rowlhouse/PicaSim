#include "EntityManager.h"
#include "FrameworkSettings.h"
#include "Trace.h"
#include "../Platform/S3ECompat.h"

#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "BulletCollision/CollisionShapes/btTriangleShape.h"

std::unique_ptr<EntityManager> EntityManager::mInstance;

//======================================================================================================================
EntityManager& EntityManager::GetInstance()
{
    IwAssert(ROWLHOUSE, mInstance != nullptr);
    return *mInstance;
}

//======================================================================================================================
bool ContactAdded(
    btManifoldPoint& cp, 
    const btCollisionObjectWrapper* colObj0, int partId0, int index0, 
    const btCollisionObjectWrapper* colObj1, int partId1, int index1)
{
    if (colObj0->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
    {
        btVector3 tempNorm;
        ((btTriangleShape*)colObj0->getCollisionShape())->calcNormal(tempNorm);
        float dot = tempNorm.dot(cp.m_normalWorldOnB);
        cp.m_normalWorldOnB = dot > 0.0f ? tempNorm : -tempNorm;
    }
    else if (colObj1->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
    {
        btVector3 tempNorm;
        ((btTriangleShape*)colObj1->getCollisionShape())->calcNormal(tempNorm);
        float dot = tempNorm.dot(cp.m_normalWorldOnB);
        cp.m_normalWorldOnB = dot > 0.0f ? tempNorm : -tempNorm;
    }
    return true;
}

//======================================================================================================================
void EntityManager::Init(const FrameworkSettings& frameworkSettings)
{
    TRACE_FUNCTION_ONLY(ONCE_1);
    IwAssert(ROWLHOUSE, !mInstance);
    mInstance.reset(new EntityManager(frameworkSettings));

    // Initialise physics SDK
    mInstance->mCollisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    mInstance->mDispatcher = new  btCollisionDispatcher(mInstance->mCollisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    mInstance->mBroadPhase = new btDbvtBroadphase();

    //btVector3 worldAabbMin(-1000,-1000,-1000);
    //btVector3 worldAabbMax(1000,1000,1000);
    //const int maxProxies = 32766;
    //mInstance->mBroadPhase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    mInstance->mSolver = new btSequentialImpulseConstraintSolver;

    mInstance->mSoftBodyWorldInfo.m_dispatcher = mInstance->mDispatcher;
    mInstance->mSoftBodyWorldInfo.m_broadphase = mInstance->mBroadPhase;
    mInstance->mSoftBodyWorldInfo.m_gravity = btVector3(0,0,-9.81f);
    mInstance->mSoftBodyWorldInfo.m_sparsesdf.Initialize();

    mInstance->mDynamicsWorld = new btSoftRigidDynamicsWorld(
        mInstance->mDispatcher,
        mInstance->mBroadPhase,
        mInstance->mSolver,
        mInstance->mCollisionConfiguration);

    mInstance->mDynamicsWorld->setGravity(mInstance->mSoftBodyWorldInfo.m_gravity);

    // Register for physics
    mInstance->RegisterEntity(mInstance.get(), ENTITY_LEVEL_LOOP_PHYSICS);

    mInstance->mIsFinalIteration = true;
    mInstance->mLeftOverTime = 0.0f;

    gContactAddedCallback = ContactAdded;
}

//======================================================================================================================
void EntityManager::Terminate()
{
    TRACE_FUNCTION_ONLY(ONCE_1);
    IwAssert(ROWLHOUSE, mInstance);

    mInstance->UnregisterEntity(mInstance.get(), ENTITY_LEVEL_LOOP_PHYSICS);

    IwAssert(ROWLHOUSE, mInstance->mEntities.empty());

    // terminate physics
    delete mInstance->mDynamicsWorld;
    delete mInstance->mSolver;
    delete mInstance->mBroadPhase;
    delete mInstance->mDispatcher;
    delete mInstance->mCollisionConfiguration;

    mInstance.reset();
}

//======================================================================================================================
EntityManager::EntityManager(const FrameworkSettings& frameworkSettings)
    : mFrameworkSettings(frameworkSettings)
{
}

//======================================================================================================================
void EntityManager::UpdateEntities(float deltaTime)
{
    Entities::iterator it, itEnd;

    mIsFinalIteration = true;

    // Do the pre-physics objects
    it = mEntities.begin();
    itEnd = mEntities.lower_bound(ENTITY_LEVEL_LOOP_PRE_PHYSICS);
    for (; it != itEnd ; ++it)
        (it->second)->EntityUpdate(deltaTime, it->first);

    // The physics loop
    itEnd = mEntities.lower_bound(ENTITY_LEVEL_POST_PHYSICS);

#if 0
    const float maxDeltaTime = 1.0f / (30.0f * mFrameworkSettings.mPhysicsSubsteps);
    int numLoops = Maximum(1, (int) (deltaTime / maxDeltaTime));
    float physicsDeltaTime = deltaTime / numLoops;
#endif
#if 0
    int numLoops = mFrameworkSettings.mPhysicsSubsteps;
    float physicsDeltaTime = deltaTime / numLoops;
#endif
#if 1
    mLeftOverTime += deltaTime;
    int numLoops = 1;
    float physicsDeltaTime = 1.0f / (30.0f * mFrameworkSettings.mPhysicsSubsteps);  
    if (mLeftOverTime > 0.0f && mLeftOverTime < physicsDeltaTime)
    {
        physicsDeltaTime = mLeftOverTime;
        mLeftOverTime = 0.0f;
    }
    else
    {
        numLoops = (int) (mLeftOverTime / physicsDeltaTime);
        mLeftOverTime -= numLoops * physicsDeltaTime;
        numLoops = ClampToRange(numLoops, 0, mFrameworkSettings.mPhysicsSubsteps * 4);
    }

    if (deltaTime == 0.0f)
    {
        numLoops = 1;
        mLeftOverTime = 0.0f;
        physicsDeltaTime = 0.0f;
    }
#endif
    for (int i = 0 ; i != numLoops ; ++i)
    {
        it = mEntities.lower_bound(ENTITY_LEVEL_LOOP_PRE_PHYSICS);
        for (; it != itEnd ; ++it)
        {
            Entities::iterator itNext = it;
            ++itNext;
            if (itNext == itEnd)
                mIsFinalIteration = true;
            else
                mIsFinalIteration = false;
            (it->second)->EntityUpdate(physicsDeltaTime, it->first);
        }
    }

    mIsFinalIteration = true;

    // The post-physics objects
    it = mEntities.lower_bound(ENTITY_LEVEL_POST_PHYSICS);
    itEnd = mEntities.end();
    for (; it != itEnd ; ++it)
        (it->second)->EntityUpdate(deltaTime, it->first);
}

//======================================================================================================================
void EntityManager::EntityUpdate(float deltaTime, int entityLevel)
{
    IwAssert(ROWLHOUSE, entityLevel == ENTITY_LEVEL_LOOP_PHYSICS);
    TRACE_FILE_IF(FRAME_1) TRACE("Physics Step start");
    mDynamicsWorld->stepSimulation(deltaTime, 0);
    TRACE_FILE_IF(FRAME_1) TRACE("Physics Step end");
}

//======================================================================================================================
void EntityManager::RegisterEntity(Entity* entity, int entityLevel)
{
    IwAssertMsg(ROWLHOUSE, !IsEntityRegistered(entity, entityLevel), ("Entity should be unregistered before registration")); 
    mEntities.insert(std::make_pair(entityLevel, entity));
}

//======================================================================================================================
void EntityManager::UnregisterEntity(Entity* entity, int entityLevel)
{
    IwAssert(ROWLHOUSE, entityLevel != ENTITY_LEVEL_ANY);
    // TODO make this more efficient
    for (Entities::iterator it = mEntities.begin() ; it != mEntities.end() ; ++it)
    {
        if (it->second == entity && it->first == entityLevel)
        {
            mEntities.erase(it);
            return;
        }
    }
    IwAssertMsg(ROWLHOUSE, false, ("Entity should be registered before unregistration")); 
}

//======================================================================================================================
bool EntityManager::IsEntityRegistered(const Entity* entity, int entityLevel) const
{
    for (Entities::const_iterator it = mEntities.begin() ; it != mEntities.end() ; ++it)
    {
        if (it->second == entity && (entityLevel == ENTITY_LEVEL_ANY || it->first == entityLevel))
            return true;
    }
    return false;
}

