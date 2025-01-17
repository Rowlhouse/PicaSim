#ifndef PARTICLE_SOURCE_H
#define PARTICLE_SOURCE_H

/// Particle and ParticleEmitter should not be used directly - access via the ParticleEngine instead. 
/// This is because particles can continue to live after the owner of the emitter has died.

//#include "ParticleEngine.h"
#include "ClassesMarmelade.h"
#include "RenderObject.h"
#include "Helpers.h"
//#include "Graphics.h"

#include "Vecteurs.h"
#include <list>
class Object;

/// A particle. Note that the texture is going to be controlled by the emitter, as it will be shared amongst all the particles.
class Particle
{
public:
  Particle(
    const Vector3& initialPos, 
    const Vector3& initialVel,
    const Vector3& targetVel,
    const Vector3& colour,
    float initialSize,
    float finalSize,
    float initialAlpha,
    float lifetime,
    float rotation,      ///< in degrees
    float rotationRate); ///< in deg/sec
  
  void Update(float dt, float dampingFrac, float dampingTime);

  void Draw(const float* rotationMatrix);

  float GetTimeLeft() const {return mTimeLeft;}
//private:
  Vector3       mPos;
  Vector3       mVel;
  Vector3       mTargetVel;
  Vector3       mColour;
  const float   mInitialSize;
  const float   mFinalSize;
  const float   mInitialAlpha;
  const float   mRotationRate;
  float         mTimeLeft;
  float         mLifetime;
  float         mRotation;
};

/// Source of many particles
class ParticleEmitter : public RenderObject
{
public:
  ParticleEmitter(
    int            maxNumParticles,
    const Vector3& emitterPos,
    const Vector3& particleVel,
    const Vector3& targetVel,
    const Vector3& colour,
    const Texture* texture,
    float          initialAlpha,
    float          initialSize,
    float          finalSize,
    float          lifetime,
    float          dampingTime,
    float          rate);
  ~ParticleEmitter();

  void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

  void SetRate(float rate) {mRate = rate;}
  void SetInitialAlpha(float initialAlpha) {mInitialAlpha = initialAlpha;}
  void SetLifeTime(float lifeTime) {mLifetime = lifeTime;}
  void SetFinalSize(float finalSize) {mFinalSize = finalSize;}
  void SetColour(const Vector3& colour) {mColour = colour;}

  void Update(
    float          dt,
    const Vector3& emitterPos,
    const Vector3& particleVel,
    const Vector3& targetVel,
    float          jitterVelMagnitude);

  void MoveParticles(float dt);

  void CommitSuicide() {mSuicidal = true;}

  bool IsDead() const {return mDead;}

  void SetViewportToUse(const Viewport* viewport) {mViewportToUse = viewport;}

private:

  Vector3 mEmitterPos;
  Vector3 mParticleVel;
  Vector3 mTargetVel;
  Vector3 mColour;
  const Texture* mTexture;

  const int   mMaxNum; // max num particles

  float mRate;
  float mLifetime;
  float mInitialAlpha;
  float mInitialSize;
  float mFinalSize;
  float mDampingTime;

  int mNum;     // num in use - list::size() may be slow
  // we can only add an integer number each frame... keep track of how
  // many we should have added.
  float mNumberToAdd;

  bool mSuicidal, mDead;
  
  // If set, only render in this viewport
  const Viewport* mViewportToUse;

  typedef std::list<Particle> ParticleList;
  ParticleList mParticles;

};

#endif
