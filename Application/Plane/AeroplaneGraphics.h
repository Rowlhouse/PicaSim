#ifndef AEROPLANE_GRAPHICS_H
#define AEROPLANE_GRAPHICS_H

// #include "RenderModel.h"

// #include <vector>

#include "Entity.h"
#include "GameSettings.h"
#include "RenderModel.h"
#include "RenderObject.h"
#include "Helpers.h"

class AeroplaneGraphics : public RenderObject, Entity
{
public:
  void Init(class TiXmlDocument& aeroplaneDoc, class Aeroplane* aeroplane);
  void Terminate();

  void RenderUpdate(Viewport* viewport, int renderLevel) OVERRIDE;
  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  const Transform& GetTM() const OVERRIDE;

  float GetRenderBoundingRadius() const OVERRIDE;

private:
  struct Box
  {
    void SetWingColours();
    void SetGrey();
    void SetBlack();
    void SetRed();
    Transform mTM;
    Vector3 mExtents;
    Vector3 mColourTop;
    Vector3 mColourSides;
    Vector3 mColourFront;
    Vector3 mColourBottom;
    std::string mName;
  };

  struct PropDisk
  {
    PropDisk() : mTM(Transform::g_Identity), mRadius(0.0f), mColour(0,0,0,0) {}
    Transform mTM;
    float mRadius;
    Vector4 mColour;
    std::string mName;
  };

  struct ControlSurface
  {
    ControlSurface() : mHingePoint1(0,1,2), mHingePoint2(2,1,0), mAngleMultiplier(1.0f) {}
    std::string mName;
    std::string mSource;
    Vector3 mHingePoint1;
    Vector3 mHingePoint2;
    float mAngleMultiplier;
  };

  void RenderUpdateComponents(Viewport* viewport, int renderLevel);
  void RenderUpdatePropDisks(Viewport* viewport, int renderLevel);
  void RenderUpdate3DS(Viewport* viewport, int renderLevel);
  void RenderDebug(Viewport* viewport, int renderLevel);
  Aeroplane* mAeroplane;

  /// Used to set the graphical control surfaces
  typedef std::vector<ControlSurface> ControlSurfaces;
  ControlSurfaces mControlSurfaces;

  /// Use to render the components
  typedef std::vector<Box> Boxes;
  Boxes mBoxes;

  typedef std::vector<PropDisk> PropDisks;
  PropDisks mPropDisks;

  RenderModel mRenderModel;

  static const int mNumPropDiskPoints = 16;
  static GLfloat mPropDiskPoints[mNumPropDiskPoints * 2];

  float mLastGraphPointTime;

  int mSmokeEmitterIDs[AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE];
  float mSmokeHueOffset[AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE];
};

#endif