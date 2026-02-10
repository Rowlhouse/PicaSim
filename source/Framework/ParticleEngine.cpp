/*
    Sss - a slope soaring simulater.
    Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/

#include "ParticleEngine.h"
#include "ParticleEmitter.h"
#include "EntityManager.h"
#include "Trace.h"

using namespace std;

//======================================================================================================================
ParticleEngine::ParticleEngine()
{
    TRACE_METHOD_ONLY(ONCE_2);
}

//======================================================================================================================
ParticleEngine::~ParticleEngine()
{
    TRACE_METHOD_ONLY(ONCE_2);
}

static void LoadTexture(Texture& texture, const char* fileName)
{
    LoadTextureFromFile(texture, fileName);
    texture.SetMipMapping(true);
    texture.SetFiltering(true);
    texture.SetClamping(true);
    texture.SetModifiable(false);
    texture.SetFormatHW(CIwImage::RGBA_4444);
    texture.Upload();
}

//======================================================================================================================
void ParticleEngine::Init()
{
    TRACE_METHOD_ONLY(ONCE_1);

    // initialise the free id list. Note that if we run out of ids we 
    // can always add more.
    for (unsigned i = 0 ; i < 256 ; ++i)
        mFreeIDs.push_back(i);

    LoadTexture(mTextures[TYPE_SMOKE], "SystemData/Textures/Smoke.png");

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_LOOP_PRE_PHYSICS);
}

//======================================================================================================================
void ParticleEngine::Terminate()
{
    TRACE_METHOD_ONLY(ONCE_1);

    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_LOOP_PRE_PHYSICS);

    for (ParticleEmitterMap::iterator it = mParticleEmitters.begin();
              it != mParticleEmitters.end();
              ++it)
    {
        delete (it->second);
    }
}

//======================================================================================================================
int ParticleEngine::RegisterEmitter(
    ParticleType   type,
    const Vector3& emitterPos,
    const Vector3& particleVel,
    const Vector3& targetVel,
    const Vector3& colour,
    int            maxNumParticles,
    float          initialAlpha,
    float          initialSize,
    float          finalSize,
    float          lifetime,
    float          dampingTime,
    float          rate)
{
    TRACE_METHOD_ONLY(ONCE_2);

    if (mFreeIDs.empty())
    {
        /// Rather than expanding the id pool, just don't allocate a source
        /// If there are that many sources, who will notice?!
        //return -1;

        // all the ids must be in the map, so make some more starting
        // at the size of the map. double it.
        unsigned num = mParticleEmitters.size();
        for (unsigned i = 0 ; i < num ; ++i)
        {
            mFreeIDs.push_back(num + i);
        }
    }

    IwAssert(ROWLHOUSE, !mFreeIDs.empty());

    int id = mFreeIDs.back();
    mFreeIDs.pop_back();

    const Texture* texture = &mTextures[type];

    ParticleEmitter * emitter = new ParticleEmitter(
        maxNumParticles,
        emitterPos,
        particleVel,
        targetVel,
        colour,
        texture,
        initialAlpha,
        initialSize,
        finalSize,
        lifetime,
        dampingTime,
        rate);
    mParticleEmitters[id] = emitter;
    return id;
}

//======================================================================================================================
ParticleEmitter* ParticleEngine::GetEmitter(int id)
{
    TRACE_METHOD_ONLY(FRAME_1);
    // make sure it exists
    if (id == -1)
        return 0;
    ParticleEmitterMap::iterator it = mParticleEmitters.find(id);
    IwAssert(ROWLHOUSE, it != mParticleEmitters.end());
    return it->second;
}

//======================================================================================================================
const ParticleEmitter* ParticleEngine::GetEmitter(int id) const
{
    TRACE_METHOD_ONLY(FRAME_1);
    // make sure it exists
    if (id == -1)
        return 0;
    ParticleEmitterMap::const_iterator it = mParticleEmitters.find(id);
    IwAssert(ROWLHOUSE, it != mParticleEmitters.end());
    return it->second;
}

//======================================================================================================================
void ParticleEngine::DeregisterEmitter(int id)
{
    TRACE_METHOD_ONLY(ONCE_2);
    // make sure it exists
    if (id == -1)
        return;
    ParticleEmitterMap::iterator it = mParticleEmitters.find(id);
    IwAssert(ROWLHOUSE, it != mParticleEmitters.end());

    // just disable it - when all the particles have expired it will 
    // get cleaned up.
    it->second->CommitSuicide();
}

//======================================================================================================================
void ParticleEngine::UpdateEmitter(
    int id,
    float dt,
    const Vector3& emitterPos,
    const Vector3& particleVel,
    const Vector3& targetVel,
    float velocityJitterMagnitude)
{
    if (id == -1)
        return;

    // make sure it exists
    ParticleEmitterMap::iterator it = mParticleEmitters.find(id);
    IwAssert(ROWLHOUSE, it != mParticleEmitters.end());

    it->second->Update(dt, emitterPos, particleVel, targetVel, velocityJitterMagnitude);
}

//======================================================================================================================
void ParticleEngine::EntityUpdate(float deltaTime, int entityLevel)
{
    for (ParticleEmitterMap::iterator it = mParticleEmitters.begin(); it != mParticleEmitters.end(); )
    {
        it->second->MoveParticles(deltaTime);
        if (it->second->IsDead())
        {
            TRACE_FILE_IF(FRAME_1)
                TRACE("Removing particle source %p\n", it->second);
            delete it->second;
            mFreeIDs.push_back(it->first);
            mParticleEmitters.erase(it++);
        }
        else
        {
            ++it;
        }
    }

}

