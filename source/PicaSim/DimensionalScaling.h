#ifndef DIMENSIONALSCALING_H
#define DIMENSIONALSCALING_H

#include "Helpers.h"

class DimensionalScaling
{
public:
  /// If useAeroScaling is set then the dimensional scaling treats the value as if 
  /// it is controlled by aerodynamic forces. Otherwise it is structural.
  DimensionalScaling(float sizeScale, float massScale, bool useAeroScaling)
    : mLengthScale(sizeScale)
  {
    mMassScale = massScale * Cube(sizeScale);
    mTimeScale = sqrtf(sizeScale);
    if (useAeroScaling)
      mTimeScale /= sqrtf(massScale);
  };

  template<typename T>
  T GetScaledLength(const T& val) {return val * (mLengthScale);}
  template<typename T>
  void ScaleLength(T& val) {val = GetScaledLength(val);}

  template<typename T>
  T GetScaledArea(const T& val) {return val * (Square(mLengthScale));}
  template<typename T>
  void ScaleArea(T& val) {val = GetScaledArea(val);}

  template<typename T>
  T GetScaledVolume(const T& val) {return val * (Cube(mLengthScale));}
  template<typename T>
  void ScaleVolume(T& val) {val = GetScaledVolume(val);}

  template<typename T>
  T GetScaledMass(const T& val) {return val * (mMassScale);}
  template<typename T>
  void ScaleMass(T& val) {val = GetScaledMass(val);}

  template<typename T>
  T GetScaledInertia(const T& val) {return val * (mMassScale * Square(mLengthScale));}
  template<typename T>
  void ScaleInertia(T& val) {val = GetScaledInertia(val);}

  template<typename T>
  T GetScaledTime(const T& val) {return val * (mTimeScale);}
  template<typename T>
  void ScaleTime(T& val) {val = GetScaledTime(val);}

  template<typename T>
  T GetScaledFreq(const T& val) {return val * (1.0f / mTimeScale);}
  template<typename T>
  void ScaleFreq(T& val) {val = GetScaledFreq(val);}

  template<typename T>
  T GetScaledStiffness(const T& val) {return val * (1.0f / Square(mTimeScale));} ///< Acceleration spring
  template<typename T>
  void ScaleStiffness(T& val) {val = GetScaledStiffness(val);}

  template<typename T>
  T GetScaledDamping(const T& val) {return val * (1.0f / mTimeScale);} ///< Acceleration spring
  template<typename T>
  void ScaleDamping(T& val) {val = GetScaledDamping(val);}

  template<typename T>
  T GetScaledVel(const T& val) {return val * (mLengthScale / mTimeScale);}
  template<typename T>
  void ScaleVel(T& val) {val = GetScaledVel(val);}

  template<typename T>
  T GetScaledAngVel(const T& val) {return val * (1.0f / mTimeScale);}
  template<typename T>
  void ScaleAngVel(T& val) {val = GetScaledAngVel(val);}

  template<typename T>
  T GetScaledAccel(const T& val) {return val * (mLengthScale / Square(mTimeScale));}
  template<typename T>
  void ScaleAccel(T& val) {val = GetScaledAccel(val);}

  template<typename T>
  T GetScaledAngAccel(const T& val) {return val * (1.0f / Square(mTimeScale));}
  template<typename T>
  void ScaleAngAccel(T& val) {val = GetScaledAngAccel(val);}

  template<typename T>
  T GetScaledForce(const T& val) {return val * (mMassScale * mLengthScale / Square(mTimeScale));}
  template<typename T>
  void ScaleForce(T& val) {val = GetScaledForce(val);}

  template<typename T>
  T GetScaledTorque(const T& val) {return val * (mMassScale * Square(mLengthScale / mTimeScale));}
  template<typename T>
  void ScaleTorque(T& val) {val = GetScaledTorque(val);}

private:
  float mLengthScale;
  float mMassScale;
  float mTimeScale;
};

#endif
