#ifndef OBSERVER_H
#define OBSERVER_H

#include "RenderModel.h"
#include "SkyGrid.h"
#include "Framework.h"

/// Represents a person/entity that watches other objects - e.g. the pilot watching/controlling their aeroplane
class Observer : public CameraTransform, public Entity, public RenderObject
{
public:
  /// This projects down onto the local terrain, which should have been initialised. The camera offset is point.z
  void Init(const Vector3& point, CameraTarget* target);
  void Terminate();

  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  /// This projects down onto the local terrain
  void SetTransform(const Transform& tm);

  /// Returns the tm that has been projected onto the terrain
  const Transform& GetTransform() const {return mTM;}

  Transform GetCameraTransform(void* cameraUserData) const OVERRIDE;

  void RenderUpdate(Viewport* viewport, int renderLevel) OVERRIDE;
  const Transform& GetTM() const OVERRIDE {return mTM;}
  float GetRenderBoundingRadius() const OVERRIDE;

private:
  CameraTarget* mTarget;
  Transform mTM;
  Vector3 mCameraOffset;
  RenderModel mRenderModel;
  SkyGrid mSkyGrid;

  // For lookaround
  float mLookYaw;
  float mLookPitch;
  float mLookYawRate;
  float mLookPitchRate;
};

#endif
