#ifndef HELPERS_H
#define HELPERS_H

//#include "btBulletDynamicsCommon.h"

//#include <IwGeom.h>
//#include <IwGeomFQuat.h>
//#include "Vecteurs.h"
//#include <IwColour.h>
//#include <SDL2/SDL.h>
//#include <MarmaladeVersion.h>

#include "Entete.h"
//#include "FonctionsMarmelade.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "Vecteurs.h"
#ifdef _MSC_VER
#define OVERRIDE override
#else
#define OVERRIDE 
#endif

/*inline bool IsMarmaladeVersionGreaterEqualsThan(int major, int minor, int revision)
{
  int r = MARMALADE_VERSION_FROM_MAJOR_MINOR_REVISION(major, minor, revision);
  int m = MARMALADE_VERSION;
  return m >= r;
}

inline bool IsMarmaladeVersionLessThan(int major, int minor, int revision)
{
  int r = MARMALADE_VERSION_FROM_MAJOR_MINOR_REVISION(major, minor, revision);
  int m = MARMALADE_VERSION;
  return m < r;
}*/

//typedef CIwFVec2  Vector2;
//typedef CIwFVec3  Vector3;
//typedef CIwFVec4  Vector4;
//typedef CIwFMat   Transform;
//typedef CIwFQuat  Quat;




inline void SetRowX(Transform& tm, const Vector3& v) {
  tm.m[0][0] = v.x; tm.m[0][1] = v.y; tm.m[0][2] = v.z;
}
inline void SetRowY(Transform& tm, const Vector3& v) {
  tm.m[1][0] = v.x; tm.m[1][1] = v.y; tm.m[1][2] = v.z;
}
inline void SetRowZ(Transform& tm, const Vector3& v) {
  tm.m[2][0] = v.x; tm.m[2][1] = v.y; tm.m[2][2] = v.z;
}

inline void SafeNormalise(Vector3& v, const Vector3& fallback = Vector3(1,0,0))
{
  if (v.GetLengthSquared() > 0.0f)
    v.Normalise();
  else
    v = fallback;
}

inline Vector3 GetSafeNormalised(const Vector3& v, const Vector3& fallback = Vector3(1,0,0))
{
  if (v.GetLengthSquared() > 0.0f)
    return v.GetNormalised();
  else 
    return fallback;
}

template<typename T>
inline T DegreesToRadians(T deg) {return deg * PI / 180.0f;}
template<typename T>
inline T RadiansToDegrees(T radians) {return radians * 180.0f / PI;}

inline Vector3 ComponentMultiply(const Vector3&a, const Vector3&b)
{
  return Vector3(a.x*b.x, a.y*b.y, a.z*b.z);
}

inline Vector3 Abs(const Vector3&a)
{
  return Vector3(fabsf(a.x), fabsf(a.y), fabsf(a.z));
}

inline Vector3 ComponentSquareSigned(const Vector3&a)
{
  return Vector3(a.x*fabsf(a.x), a.y*fabsf(a.y), a.z*fabsf(a.z));
}

inline float Hypot(float a, float b) {return sqrtf(a*a + b*b);}

inline float SquareSigned(float a) {return a * fabsf(a);}

template<typename T>
inline T Square(T a) {return a * a;}

template<typename T>
inline T Cube(T a) {return a * a * a;}

template<typename T>
inline T Hypercube(T a) {return a * a * a * a;}

template<typename T>
inline T Maximum(T a, T b) {return a > b ? a : b;}

template<typename T>
inline T Minimum(T a, T b) {return a < b ? a : b;}

inline Vector3 Maximum(const Vector3& a, const Vector3& b) 
{
  Vector3 result(Maximum(a.x, b.x), Maximum(a.y, b.y), Maximum(a.z, b.z));
  return result;
}

inline void CheckSanity(const Vector3& v)
{
  IwAssert(ROWLHOUSE, v.x == v.x);
  IwAssert(ROWLHOUSE, v.y == v.y);
  IwAssert(ROWLHOUSE, v.z == v.z);
}

inline void CheckSanity(const Transform& tm)
{
  CheckSanity(tm.RowX());
  CheckSanity(tm.RowY());
  CheckSanity(tm.RowZ());
  CheckSanity(tm.GetTrans());
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T Interpolate(const T& v1, const T& v2, float t)
{
  return v1 + (v2 - v1) * t;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T ClampToRange(T val, T minVal, T maxVal) 
{
  if (val < minVal)
    return minVal;
  else if (val > maxVal)
    return maxVal;
  else
    return val;
}

inline Vector3 ClampToRange(Vector3 val, float minVal, float maxVal) 
{
  val.x = ClampToRange(val.x, minVal, maxVal);
  val.y = ClampToRange(val.y, minVal, maxVal);
  val.z = ClampToRange(val.z, minVal, maxVal);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T WrapToRange(T val, T minVal, T maxVal)
{
  float range = maxVal - minVal;
  while (val < minVal)
    val += range;
  while (val > maxVal)
    val -= range;
  return val;
}

template <>
inline float WrapToRange<float>(float val, float minVal, float maxVal)
{
  IwAssert(ROWLHOUSE, minVal < maxVal);
  const float delta = maxVal - minVal;  
  float u = (val - minVal) / delta;
  u -= floorf(u);
  val = delta * u + minVal;
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T ExponentialApproach(T val, T target, float dt, float lifetime)
{
  T error = val - target;
  if (fabsf(error) > 0.0001f)
    error *= expf(-dt / lifetime);
  else
    error = 0.0f; // avoid underflow? Seems to be causing glitches on Galaxy Y
  return target + error;
}

//----------------------------------------------------------------------------------------------------------------------
// cubic easing in/out - acceleration until halfway, then deceleration
// http://www.dzone.com/snippets/robert-penner-easing-equations
// Doesn't work?
inline float cubicEaseInOut(float t, float b, float c, float d)
{
  // http://blog.greweb.fr/2012/02/bezier-curve-based-easing-functions-from-concept-to-implementation/
  if (t < 0.5f)
    return 4.0f * Cube(t);
  else
    return (t-1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;

  //if ((t /= d/2.0f) < 1.0f)
  //  return c * 0.5f * t * t * t + b;
  //else
  //  return c * 0.5f * ((t -= 2.0f) * t * t + 2.0f) + b;

//easeInOutCubic: function (t) { return t<.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1 }

//if ((t/=d/2) < 1) return c/2*t*t*t + b;
//return c/2*((t-=2)*t*t + 2) + b;
};

//----------------------------------------------------------------------------------------------------------------------
// steps from 0 to 1 as t goes from 0 to 1
inline float SmoothStep(float t)
{
  if (t > 1.0f)
    return 1.0f;
  if (t < 0.0f)
    return 0.0f;
  return 3.0f * Square(t) - 2.0f * Cube(t);
}

 
inline float Hill(float x)
{
  const float a0 = 1.0f;
  const float a2 = 2.0f / PI - 12.0f / (PI * PI);
  const float a3 = 16.0f / (PI * PI * PI) - 4.0f / (PI * PI);
  const float xx = x * x;
  const float xxx = xx * x;
  return a0 + a2 * xx + a3 * xxx;
}
 
inline float FastSin(float x)
{
  // wrap x within [0, TWO_PI)
  x = WrapToRange(x, 0.0f, TWO_PI);
  // 4 pieces of hills
  if (x < HALF_PI)
    return Hill(HALF_PI - x);
  else if (x < PI)
    return Hill(x - HALF_PI);
  else if (x < 3.0f * HALF_PI)
    return -Hill(3.0f * HALF_PI - x);
  else
    return -Hill(x - 3.0f * HALF_PI);
}

inline float FastCos(float x)
{
  return FastSin(x + HALF_PI);
}

class Parabola
{
  // Calculate parabola coefficients given two points (x0,y0) and (x1,y1) and slope0 at (x0,y0)
  Parabola(float x0, float y0, float slope0, float x1, float y1)
  {
    // A = a*B + b* C
    float A = y0-y1;
    float B = Square(x0) - Square(x1);
    float C = x0-x1;
    // Also 2*a*x0 + b = slope
    // so A = a*B + C * (slope - 2*a*x0)
    // so a = (A - C*slope) / (B - 2 * x0)
    a = (A - C * slope0) / (B - 2 * x0 * C);
    b = (A - a * B) / C;
    c = y0 - (a * Square(x0) + b * x0);
  }

  float GetY(float x)
  {
    return a * x * x + b * x + c;
  }

private:
  float a, b, c;
};

inline float RangedRandom(float min, float max)
{
  float f = (float) (rand()) / RAND_MAX;
  float res = min + f * (max - min);
  return res;
}

class RandomGenerator
{
public:
  RandomGenerator(long seed) : 
    m_seed(seed), 
    IA(16807),
    IM(2147483647),
    IQ(127773),
    IR(2836),
    MASK(123459876),
    AM(1.0f/IM)
 {}

  void SetSeed(long seed) {m_seed = seed;}

  /// Returns from 0 to 1
  float GetValue() 
  {
    m_seed ^= MASK;
    long k = m_seed/IQ;
    m_seed = IA * (m_seed - k * IQ) - IR * k;
    if (m_seed < 0) m_seed += IM;
    float ans = AM * (m_seed);
    m_seed ^= MASK;
    return ans;
  }

  long GetLongValue()
  {
    m_seed ^= MASK;
    long k = m_seed/IQ;
    m_seed = IA * (m_seed - k * IQ) - IR * k;
    if (m_seed < 0) m_seed += IM;
    m_seed ^= MASK;
    return m_seed;
  }

  /// Returns between v1 and v2
  float GetValue(float v1, float v2) 
  {
    return v1 + (v2-v1) * ( GetValue() );
  }

  float GetValuePow(float v1, float v2, float pow) 
  {
    return v1 + (v2-v1) * ( powf(GetValue(), pow) );
  }
private:
  long m_seed;
  const long IA, IM, IQ, IR, MASK;
  const float AM;
};

inline btVector3 Vector3ToBulletVector3(const Vector3& v) {return btVector3(v.x, v.y, v.z);}
inline Vector3   BulletVector3ToVector3(const btVector3& v) {return Vector3(v.x(), v.y(), v.z());}

inline btQuaternion QuatToBulletQuaternion(const Quat& q) {return btQuaternion(q.x, q.y, q.z, q.s);}
inline Quat BulletQuaternionToQuat(const btQuaternion& q) {return Quat(q.w(), q.x(), q.y(), q.z());}

inline btTransform TransformToBulletTransform(const Transform& tm) 
{
  return btTransform(QuatToBulletQuaternion(Quat(tm)), Vector3ToBulletVector3(tm.t));
}

inline Transform BulletTransformToTransform(const btTransform& tm) 
{
  Quat q = BulletQuaternionToQuat(tm.getRotation());
  Transform r;
  r = q;
  r.t = BulletVector3ToVector3(tm.getOrigin());
  return r;
}

void ApplyRollPitchYawToRotationDegrees(float roll, float pitch, float yaw, Vector3& rotation);

//----------------------------------------------------------------------------------------------------------------------
/// Critically damped method of making a variable smoothly approach a value. Requires
/// timeDelta < 0.5 * smoothTime for good accuracy.
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
inline void SmoothCD(
  T &val,          // in/out: value to be smoothed
  T &valRate,      // in/out: rate of change of the value
  const float timeDelta, // in: time interval
  const T &to,     // in: the target value
  const float smoothTime)// in: timescale for smoothing
{
  if (smoothTime > 0.0f)
  {
    float omega = 2.0f / smoothTime;
    float x = omega * timeDelta;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    T change = val - to;
    T temp = (valRate + omega * change) * timeDelta;
    valRate = (valRate - omega * temp) * exp;
    val = to + (change + temp) * exp;
  }
  else if (timeDelta > 0.0f)
  {
    valRate = (to - val) / timeDelta;
    val = to;
  }
  else
  {
    val = to;
    valRate -= valRate; // zero it...
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Exponential damping of a variable towards a target
/// Smooths val towards the target to with a timestep of timeDelta, smoothing over smoothTime. This
/// avoids the cost of evaluating the exponential function by a second order expansion, and is
/// stable and accurate (for all practical purposes) for all timeDelta values.
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothExponential(
  T&          val,       ///< in/out: value to be smoothed.
  const float timeDelta, ///< in: time interval.
  const T&    to,        ///< in: the target value.
  const float smoothTime ///< in: timescale for smoothing.
  )
{
  if (smoothTime > 0.0f)
  {
    // A quantity is subject to exponential decay if it decreases at a rate proportional
    // to its current value. dN/dt = -k * N, where N is the value and k is the rate of decay.
    // Rearranging and integrating gives you N(t) = N0 * exp(-k * t), where N0 is
    // the initial value. Over the interval k * t = [0:1], the value N will have
    // decreased by about 65% of its original value.  
    float lambda = timeDelta / smoothTime;

    // The exponential function exp(lambda) is approximated by a second order Maclaurin's series
    val = to + (val - to) / (1.0f + lambda + 0.5f * lambda * lambda);
  }
  else
  {
    val = to;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Spring/damped method of making a variable smoothly approach a value. Very fast but not
/// very accurate version, especially for larger timesteps, where it can be unstable (if
/// timeDelta approaches smoothTime, or dampingRatio becomes large).
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothSpringDamperApprox(
  T&          val,         ///< in/out: value to be smoothed.
  T&          valRate,     ///< in/out: rate of change of the value.
  const float timeDelta,   ///< in: time interval.
  const T&    to,          ///< in: the target value.
  const float smoothTime,  ///< in: timescale for smoothing (see smoothCD). Determines the rate at which the target is approached - i.e. the strength of the spring.
                           ///<  The undamped angular frequency is 2 / smoothTime. The period of the undamped oscillation is therefore Pi * smoothTime. 
                           ///<  Can be explained it as the time by which the spring lags behind the target when travelling at its maximum velocity...
  const float dampingRatio ///< in: damping ratio - i.e. amount of damping relative to critical damping.
  )
{
  if (smoothTime > 0.0f)
  {
    float omega = 2.0f / smoothTime;
    float stiffness = Square(omega);
    float damping = 2.0f * dampingRatio * omega;
    T accel = (to - val) * stiffness - valRate * damping;
    valRate += accel * timeDelta;
    val += valRate * timeDelta;
  }
  else if (timeDelta > 0.0f)
  {
    valRate = (to - val) / timeDelta;
    val = to;
  }
  else
  {
    val = to;
    valRate -= valRate;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Spring/damped method of making a variable smoothly approach a value. Very fast but not
/// very accurate version, especially for larger timesteps, where it can be unstable (if
/// timeDelta approaches smoothTime, or dampingRatio becomes large).
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothStiffnessDampingApprox(
  T&          val,         ///< in/out: value to be smoothed.
  T&          valRate,     ///< in/out: rate of change of the value.
  const float timeDelta,   ///< in: time interval.
  const T&    to,          ///< in: the target value.
  const float stiffness,   ///< in: spring stiffness
  const float damping      ///< in: damping 
  )
{
  T accel = (to - val) * stiffness - valRate * damping;
  valRate += accel * timeDelta;
  val += valRate * timeDelta;
}

//----------------------------------------------------------------------------------------------------------------------
/// Spring/damped method of making a variable smoothly approach a value. Exact solution for any input.
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothSpringDamper(
  T&          val,         ///< in/out: value to be smoothed.
  T&          valRate,     ///< in/out: rate of change of the value.
  const float timeDelta,   ///< in: time interval.
  const T&    to,          ///< in: the target value.
  const float smoothTime,  ///< in: timescale for smoothing (see smoothCD). Determines the rate at which the target is approached - i.e. the strength of the spring.
                           ///<  The undamped angular frequency is 2 / smoothTime. The period of the undamped oscillation is therefore Pi * smoothTime. 
                           ///<  Can be explained it as the time by which the spring lags behind the target when travelling at its maximum velocity...
                           ///<  The larger the value the longer it takes to reach the target.
  const float dampingRatio ///< in: damping ratio - i.e. amount of damping relative to critical damping.
  )
{
  if (smoothTime > 0.0f)
  {
    float w = 2.0f / smoothTime;

    const T a = val - to;
    const T& b = valRate;

    // Handle the three cases separately
    if (dampingRatio > 1.0f)
    {
      float DR = sqrtf(Square(dampingRatio) - 1.0f);
      const T C2 = -(b + a * (w * (dampingRatio - DR))) / (2.0f * w * DR);
      const T C1 = a - C2;

      float A1 = (DR - dampingRatio) * w;
      float A2 = -(DR + dampingRatio) * w;

      float E1 = expf(A1 * timeDelta);
      float E2 = expf(A2 * timeDelta);

      val     = C1 * E1 + C2 * E2;
      valRate = C1 * A1 * E1 + C2 * A2 * E2;
      val += to;
    }
    else if (dampingRatio < 1.0f)
    {
      float DR = sqrtf(1.0f - Square(dampingRatio));
      const T& A = a;
      const T B = (b + a * (dampingRatio * w)) / (w * DR);

      float C = FastCos(w * DR * timeDelta);
      float S = FastSin(w * DR * timeDelta);
      float E = expf(-dampingRatio * w * timeDelta);

      val = (A * C + B * S) * E;
      valRate = val * (-dampingRatio * w);
      valRate += (B * (w * DR * C) - A * (w * DR * S)) * E;
      val += to;
    }
    else
    {
      const T& C1 = a;
      const T C2 = b + a * w;
      float E = expf(-w * timeDelta);
      val = (C1 + C2 * timeDelta) * E;
      valRate = (C2 - C1 * w - C2 * (w * timeDelta)) * E;
      val += to;
    }
  }
  else if (timeDelta > 0.0f)
  {
    valRate = (to - val) / timeDelta;
    val = to;
  }
  else
  {
    val = to;
    valRate -= valRate;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Spring/damped method of making a variable smoothly approach a value. Exact solution for any input.
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothStiffnessDamping(
  T&          val,         ///< in/out: value to be smoothed.
  T&          valRate,     ///< in/out: rate of change of the value.
  const float timeDelta,   ///< in: time interval.
  const T&    to,          ///< in: the target value.
  const float stiffness,   ///< in: stiffness
  const float damping      ///< in: damping 
  )
{
  if (stiffness > 0.0f)
  {
    float strength = sqrtf(stiffness);
    float smoothTime = 2.0f / strength;
    float dampingRatio = damping / (2.0f * strength);
    smoothSpringDamper(val, valRate, timeDelta, to, smoothTime, dampingRatio);
  }
  else
  {
    if (damping > 0.0f)
    {
      float E2 = expf(-damping * timeDelta);
      val     += valRate * ((1.0f - E2) / damping);
      valRate *= E2;
    }
    else
    {
      val += valRate * timeDelta;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Spring/damped method of making a variable smoothly approach a value. Uses approximations
/// to remove some of the exp etc functions, but is otherwise more accurate than the
/// smoothSpringDamperApprox version. Requires timeDelta < 0.5 * smoothTime
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothSpringDamperFast(
  T&          val,         ///< in/out: value to be smoothed.
  T&          valRate,     ///< in/out: rate of change of the value.
  const float timeDelta,   ///< in: time interval.
  const T&    to,          ///< in: the target value.
  const float smoothTime,  ///< in: timescale for smoothing (see smoothCD). Determines the rate at which the target is approached - i.e. the strength of the spring.
                           ///<  The undamped angular frequency is 2 / smoothTime. The period of the undamped oscillation is therefore Pi * smoothTime. 
                           ///<  Can be explained it as the time by which the spring lags behind the target when travelling at its maximum velocity...
                           ///<  The larger the value the longer it takes to reach the target.
  const float dampingRatio ///< in: damping ratio - i.e. amount of damping relative to critical damping.
  )
{
  if (smoothTime > 0.0f)
  {
    float w = 2.0f / smoothTime;

    const T a = val - to;
    const T& b = valRate;

    // Handle the three cases separately
    if (dampingRatio > 1.0f)
    {
      float DR = sqrtf(Square(dampingRatio) - 1.0f);
      const T C2 = -(b + a * w * (dampingRatio - DR)) / (2.0f * w * DR);
      const T C1 = a - C2;

      float A1 = (DR - dampingRatio) * w;
      float A2 = -(DR + dampingRatio) * w;

      float E1 = 1.0f / expf(-A1 * timeDelta);
      float E2 = 1.0f / expf(-A2 * timeDelta);

      val     = C1 * E1 + C2 * E2;
      valRate = C1 * A1 * E1 + C2 * A2 * E2;
      val += to;
    }
    else if (dampingRatio < 1.0f)
    {
      float DR = sqrtf(1.0f - Square(dampingRatio));
      const T& A = a;
      const T B = (b + a * (dampingRatio * w)) / (w * DR);

      float S = FastSin(w * DR * timeDelta);
      float C = FastCos(w * DR * timeDelta);
      float E = 1.0f / expf(dampingRatio * w * timeDelta);

      val = (A * C + B * S) * E;
      valRate = val * (-dampingRatio * w);
      valRate += (B * (w * DR * C) - A * (w * DR * S)) * E;
      val += to;
    }
    else
    {
      const T& C1 = a;
      const T C2 = b + a * w;
      float E = 1.0f / expf(w * timeDelta);
      val = (C1 + C2 * timeDelta) * E;
      valRate = (C2 - C1 * w - C2 * (w * timeDelta)) * E;
      val += to;
    }
  }
  else if (timeDelta > 0.0f)
  {
    valRate = (to - val) / timeDelta;
    val = to;
  }
  else
  {
    val = to;
    valRate -= valRate;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Spring/damped method of making a variable smoothly approach a value. Uses approximations
/// to remove some of the exp etc functions, but is otherwise more accurate than the
/// smoothSpringDamperApprox version. Requires timeDelta < 1 / sqrtf(stiffness)
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void SmoothStiffnessDampingFast(
  T&          val,         ///< in/out: value to be smoothed.
  T&          valRate,     ///< in/out: rate of change of the value.
  const float timeDelta,   ///< in: time interval.
  const T&    to,          ///< in: the target value.
  const float stiffness,   ///< in: stiffness
  const float damping      ///< in: damping 
  )
{
  if (stiffness > 0.0f)
  {
    float strength = sqrtf(stiffness);
    float smoothTime = 2.0f / strength;
    float dampingRatio = damping / (2.0f * strength);
    smoothSpringDamperFast(val, valRate, timeDelta, to, smoothTime, dampingRatio);
  }
  else
  {
    if (damping > 0.0f)
    {
      float E2 = expf(-damping * timeDelta);
      val     += valRate * ((1.0f - E2) / damping);
      valRate *= E2;
    }
    else
    {
      val += valRate * timeDelta;
    }
  }
}


#endif
