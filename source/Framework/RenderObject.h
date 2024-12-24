#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "Helpers.h"

/// Base class from which all objects that expect to be rendered should derive
class RenderObject
{
public:
  RenderObject() : mShadowAmount(1.0f) {}

  /// Unregister from RenderManager before deleting
  virtual ~RenderObject() {}

  virtual void RenderUpdate(class Viewport* viewport, int renderLevel) = 0;

  virtual const glm::mat4& GetTM() const {return glm::mat4(1.0f);}

  virtual float GetRenderBoundingRadius() const {return FLT_MAX;}

  virtual bool GetShadowVisible() const {return true;}

  void SetShadowAmount(float amount) {mShadowAmount = amount;}

protected:
  float mShadowAmount;
};

#endif
