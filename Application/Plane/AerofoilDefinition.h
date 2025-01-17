#ifndef AEROFOIL_DEFINITION_H
#define AEROFOIL_DEFINITION_H

// #include "Helpers.h"

// #include <string>

#include "Vecteurs.h"
#include <map>
#include <vector>
struct AerodynamicData
{
  float mCL;
  float mFlying; ///< State of the next segment. -1 is in reverse, 0 stalled, 1 forward
  float mTurbulentDrag; ///< 0 means use the drag from the CL etc calculations. 1 means calculate it assuming turbulent.
};

class AerofoilDefinition
{
public:
  /// Creates/adds reference to a definition and returns it
  static AerofoilDefinition* Create(const char* name);

  /// Decrements reference count and deletes if it's no longer used
  static void Release(AerofoilDefinition* aerofoilDefinition);

  const char* GetName() {return mName.c_str();}

  /// Returns "flying" = i.e. 1 for forwards, 0 for stalled, -1 for backwards
  float ApplyForces(
    class Aeroplane*                    aeroplane, 
    const struct AerofoilConfiguration& configuration, 
    const struct AerofoilParameters&    parameters,
    const struct AerofoilControl&       control,
    const class Environment&            environment, 
    const struct TurbulenceData&        turbulenceData,
    Vector3&                            wash,
    Vector3&                            force,
    float                               deltaTime,                
    float&                              lastAoA) const;

private:
  AerofoilDefinition(const char* name);
  ~AerofoilDefinition();

  void GetGraphData(float& angleOfAttack, float& CL, float& flying, float& turbulentDrag, float effectiveAR) const;

  void GetFlightData(
    const AerofoilConfiguration& configuration,
    const AerofoilControl& control,
    const float effectiveAR,
    float speedSq,
    float angleOfAttack, 
    float& CL, float& CD, float& CM, float& flying) const;

  void DrawAerofoilPlot(
    const AerofoilConfiguration& configuration, 
    const AerofoilParameters& parameters,
    const AerofoilControl& control,
    float angleOfAttack,
    float speedSq) const;

  void DrawAerofoilPolar(
    const AerofoilConfiguration& configuration, 
    const AerofoilParameters& parameters,
    const AerofoilControl& control,
    float angleOfAttack,
    float speedSq) const;

  typedef std::map<std::string, AerofoilDefinition*> AerofoilDefinitions;
  typedef std::vector<AerodynamicData> GraphData;

  static AerofoilDefinitions mAerofoilDefinitions;

  int mReferenceCount;
  std::string mName;

  /// Offsets in the x direction for flying forwards/backwards as a fraction of the extent
  float mLiftPositionOffsetFractionForwards;
  float mLiftPositionOffsetFractionReverse;
  float mCDFlying;
  float mCDStalled;
  float mCDMax;
  float mCM0;
  float mCMPerDeg;

  float mRefRe;     // Reference Re for the aerofoil data
  float mMinReFrac; // Minimum Re as a fraction of the reference Re to use for scaling
  float mCDPower;   // How CD scales with Re

  GraphData mGraphData;
};

#endif