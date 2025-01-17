#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// #include "Runway.h"

#include "Entity.h"
#include "ShaderManager.h"
#include "Skybox.h"
#include "Terrain.h"
#include "ThermalManager.h"
#include "Vecteurs.h"
#include "Helpers.h"
#include <set>
class ObjectEditingOverlay;
class Runway;

struct TurbulenceData
{
  Vector3 mPosition;
  /// Linear Turbulence - m/s at mPosition
  Vector3 mTurbulence;
  // Rate of change of turbulence in each axis direction
  Vector3 mTurbulencePerX;
  Vector3 mTurbulencePerY;
  Vector3 mTurbulencePerZ;
};


class Environment : public Entity
{
public:
  /// Gets the singleton
  static Environment& GetInstance(); 

  static bool Init(LoadingScreenHelper* loadingScreen);

  static void Terminate();

  static Terrain& GetTerrain() {return mInstance->mTerrain;}

  static Skybox& getSkybox() {return mInstance->mSkybox;}

  void InitZeroFlowHeightfield(class LoadingScreenHelper* loadingScreen = 0);

  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  ThermalManager& GetThermalManager() {return mThermalManager;}
  const ThermalManager& GetThermalManager() const {return mThermalManager;}

  /// When requesting the turbulent wind you need to pass in a TurbulenceData structure which has various things
  /// precalculated. This may be a little slow, but then each call to GetWindAtPosition should be quick
  TurbulenceData PrepareTurbulenceData(const Vector3& pos);

  enum WindType {
    WIND_TYPE_SMOOTH = 1 << 0, 
    WIND_TYPE_GUSTY  = 1 << 1, 
    WIND_TYPE_TURBULENT = 1 << 2,
    WIND_TYPE_THERMALS = 1 << 3
  };
  static const uint32_t WIND_TYPE_ALL = 0xffffffff;
  Vector3 GetWindAtPosition(const Vector3& pos, uint32_t windType, const TurbulenceData* turbulenceData = 0) const;

  Vector3 GetWindDirection(uint32_t windType) const;

  float GetZeroWindHeightFast(float x, float y) const;

  float GetZeroWindHeight(float x, float y) const;

  uint32_t GetChecksum() const {return mChecksum;}

  float GetSurfaceRoughness(const Vector3& pos) const;

  const Runway* GetRunway() const {return mRunway;}

  void ResetTime() {mTime = 0.0f;}

  float GetTime() const { return mTime; }

  static float GetAirDensity(const Vector3& pos);

private:
  friend class ObjectEditingOverlay;

  Environment();
  ~Environment();

  float GetZeroWindHeightAt(int i, int j) const;
  Vector3 GetZeroWindPosAt(int i, int j) const;

  void UpdateWind(float deltaTime);
  static Environment* mInstance;

  const char* GetEditModeString() const;

  Terrain mTerrain;
  Runway* mRunway;

  ObjectEditingOverlay* mObjectEditingOverlay;

  typedef std::set<class BoxObject*> Boxes;
  Boxes mBoxes;

  Texture* mObjectEditingTexture;

  ThermalManager mThermalManager;

  /// The absolute height of the zero flow transition. This will be different to the terrain height when the flow separates. 
  std::vector<float> mZeroFlowHeightfield;

  // Note that the skybox can be a panorama or a simple skybox
  Skybox mSkybox;

  Vector3 mWindDirection;

  // Gusts are assumed to take place on time scales longer than the update itself, and on a global scale, so calculated once per update
  float mTime;
  Vector3 mGustyWindDirection;
  float   mGustyWindSpeed;

  uint32_t mChecksum;
};

#endif