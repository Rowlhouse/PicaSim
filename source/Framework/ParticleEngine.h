#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include "Helpers.h"
#include "Entity.h"
#include "Graphics.h"

#include <map>
#include <vector>
#include <string>

class Object;
class ParticleEmitter;

class ParticleEngine : public Entity
{
public:
  enum ParticleType {TYPE_SMOKE, NUM_PARTICLE_TYPES};

  ParticleEngine();
  ~ParticleEngine();

  void Init();
  void Terminate();

  /// @returns ID for the new emitter.
  /// @param num: The max number of particles in use by this source
  /// @param type: Type of particle emitted by this source
  /// @param particle_size: Size of the billboard used for this particle
  int RegisterEmitter(
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
    float          rate);

  /// deregisters a source. If wait is true, then the particles will
  /// be maintained until they expire. The source will not be referenced
  /// during this time.
  void DeregisterEmitter(int id);

  void SetRate(int id, float rate);
  void SetInitialAlpha(int id, float initialAlpha);
  void SetLifeTime(int id, float lifeTime);

  ParticleEmitter* GetEmitter(int id);
  const ParticleEmitter* GetEmitter(int id) const;
                
  /// Updates the position of the source and emits particles between 
  /// the new and old position at the already-specified rate, with
  /// the pos/vel parameters
  void UpdateEmitter(
    int            id,
    float          dt,
    const Vector3& emitterPos,
    const Vector3& particleVel,
    const Vector3& targetVel,
    float velocityJitterMagnitude);

  /// Does 2 things:
  /// 1. Moves the particles by dt
  /// 2. Removes any expired particles
  /// 3. Removes any dead sources
  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

private:

  typedef std::map<int, ParticleEmitter *> ParticleEmitterMap;
  ParticleEmitterMap mParticleEmitters;
  Texture mTextures[NUM_PARTICLE_TYPES];

  std::vector<int> mFreeIDs;
};

#endif
