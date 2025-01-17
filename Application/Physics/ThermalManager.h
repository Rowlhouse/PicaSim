#ifndef THERMALMANAGER_H
#define THERMALMANAGER_H

//#include <list>

// Change this to a std::vector like array type. However CIwArray has got a fast 
// resize_quick(0) that doesn't call destructors
//#include <IwUtil.h>
#include "RenderModel.h"
#include "RenderObject.h"
#include "ShaderManager.h"
#include "Helpers.h"

#define ARRAY_TYPE std::vector

class ThermalManager : public RenderObject
{
public:
  ThermalManager();

  void Init(LoadingScreenHelper* loadingScreen);
  void Terminate();

  void Update(float dt, const Vector3& centre);

  void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

  Vector3 GetThermalWindAtPosition(const Vector3& pos, float terrainHeight) const;

  void Repopulate() {mPreviousDensity = -1.0f;}

  void SetSeed(long seed) {mRandom.SetSeed(seed);}

  Vector3 GetNearestThermalPosition(const Vector3& pos, float& bestDist) const;

private:
  struct Thermal
  {
    Vector3 mPos;
    float mAge;
    float mActivity; // Ramps for 0 to 1 then back again depending on the age

    float GetRadius1() const {return mRadius1 * (1.0f + (mAge/mLifespan) * mThermalExpansionOverLifespan);}
    float GetRadius2() const {return mRadius2 * (1.0f + (mAge/mLifespan) * mThermalExpansionOverLifespan);}
    float GetRadius3() const {return mRadius3 * (1.0f + (mAge/mLifespan) * mThermalExpansionOverLifespan);}
    float GetDepth() const {return mDepth * (1.0f + (mAge/mLifespan) * mThermalExpansionOverLifespan);}
    float GetCoreUpdraftSpeed() const {return mUpdraftSpeed * mActivity;}

    // Speed of the downdraft at mRadius2
    float GetDowndraftSpeed() const {return mUpdraftSpeed * Square(GetRadius1()) / (Square(GetRadius3()) - Square(GetRadius2()));}

    // Constants for this thermal
    float mLifespan;
    float mDepth;
    float mThermalExpansionOverLifespan;
    float mRadius1; // Initial radius of the updraft
    float mRadius2; // Initial radius of the transition to downdraft
    float mRadius3; // Initial radius of the downdraft
    float mUpdraftSpeed; // Speed of the main updraft
    float mVerticalSpeed; // Motion of the thermal
    float mTrackerAngleOffset; // Initial angle of the tracker
  };

  typedef std::vector<Thermal> Thermals;

  const Thermal* GetNearestThermal(const Vector3& pos, float& bestDist) const;

  Vector3 GetThermalWindAtPosition(const Thermal& thermal, const Vector3& pos, float terrainHeight) const;

  float GetHorizontalDistanceSquaredToNearestThermal(const Vector3& pos) const;

  Thermals mThermals;

  RandomGenerator mRandom;

  float mPreviousDensity;

  static const int NUM_LODS = 4;
  RenderModel mRenderModel[NUM_LODS];

  struct RenderDetail
  {
    Vector3 pos;
    float radius;
    float angle;
  };
  typedef ARRAY_TYPE<RenderDetail> RenderDetails;
  RenderDetails mRenderDetails[NUM_LODS];
};

#undef ARRAY_TYPE

#endif
