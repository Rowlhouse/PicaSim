#ifndef AEROFOIL_PARAMETERS_H
#define AEROFOIL_PARAMETERS_H

#include "Framework.h"
/// This is the constant configuration of an aerofoil - secific to that aerofoil
struct AerofoilConfiguration
{
  /// Offset of the aerofoil from the aeroplane's TM
  Transform mOffset;

  /// Extents in local space
  Vector3 mExtents;

  /// Direction and magnitude of the shadowing. This points in the direction of the shadow - i.e. up for a conventional fin being shadowed by the tail.
  Vector3 mShadow;

  /// mExtents.x * mExtents.y
  float mArea;

  /// Whole wing aspect ratio
  float mWingAspectRatio;

  // Efficiency of the wing compared to an elliptical distribution. Typically 0.85 to 0.95.
  float mWingSpanEfficiency;

  /// The speed at which the control effectiveness halves
  float mControlHalfSpeed;

  /// The name of an engine to get the wash from
  std::string mWashFromEngineName;
  float mWashFromEngineFraction;

  /// The name of an wing to get the wash from
  static const size_t sMaxWashFromWings = 4;
  std::string mWashFromWingName[sMaxWashFromWings];
  float mWashFromWingFraction[sMaxWashFromWings];

  bool mGroundEffect;
};

/// These parameters will be updated each tick and are specific to that aerofoil
struct AerofoilParameters
{
  Transform mTM;
  Vector3 mVel;
};

/// Modifications to the aerofoil properties due to control
struct AerofoilControl
{
  float mExtraCL; // Extra CL from control
  float mExtraCM;
  float mExtraCD; // Extra CD from control
  float mExtraCamber;
  float mExtraAngle;
  float mCDScale;
};

#endif