#ifndef HELPERS_H
#define HELPERS_H

#include "btBulletDynamicsCommon.h"

//#include <IwGeom.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>  // Pour les vecteurs 3D
#include <glm/mat4x4.hpp>  // Pour les matrices 4x4

//#include <IwGeomFQuat.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//#include <IwColour.h>
#include <SDL2/SDL.h>

//#include <MarmaladeVersion.h>

#include <cassert> // Pour l'assertion

#ifdef _MSC_VER
#define OVERRIDE override
#else
#define OVERRIDE 
#endif

/* inline bool IsMarmaladeVersionGreaterEqualsThan(int major, int minor, int revision)
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
} */

class Plane
{
public:
  glm::vec3 v; // Le vecteur normal
  float k;   // Le décalage

  // Constructeurs
  Plane(const glm::vec3& v, float k) : v(v), k(k) {}
  Plane() : v(0.0f, 0.0f, 1.0f), k(0.0f) {} // Valeur par défaut


  void Normalise() {float invM = 1.0f / glm::length(v); v *= invM; k *= invM;}
  float GetDistanceToPoint(const glm::vec3& pt) const { return glm::dot(v, pt) + k;}
};

class Colour //: public CIwColour
{
public:
  SDL_Color col;
  
  Colour(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255)
  {
    col.r = r;
    col.g = g;
    col.b = b;
    col.a = a;
  }
  Colour(const SDL_Color& col) : col(col) {}
};

inline void SetRowX(glm::mat4& tm, const glm::vec3& v) {
    tm[0][0] = v.x; tm[0][1] = v.y; tm[0][2] = v.z; 
}

inline void SetRowY(glm::mat4& tm, const glm::vec3& v) {
    tm[1][0] = v.x; tm[1][1] = v.y; tm[1][2] = v.z;
}

inline void SetRowZ(glm::mat4& tm, const glm::vec3& v) {
    tm[2][0] = v.x; tm[2][1] = v.y; tm[2][2] = v.z;
}

inline void SafeNormalise(glm::vec3& v, const glm::vec3& fallback = glm::vec3(1,0,0))
{
  if (glm::length(v) > 0.0f)
    v = glm::normalize(v);
  else
    v = fallback;
}

inline glm::vec3 GetSafeNormalised(const glm::vec3& v, const glm::vec3& fallback = glm::vec3(1,0,0))
{
  if (glm::length(v) > 0.0f)
    return glm::normalize(v);
  else 
    return fallback;
}

template<typename T>
inline T DegreesToRadians(T deg) {return deg * PI / 180.0f;}
template<typename T>
inline T RadiansToDegrees(T radians) {return radians * 180.0f / PI;}

inline glm::vec3 ComponentMultiply(const glm::vec3& a, const glm::vec3& b)
{
  return glm::vec3(a.x*b.x, a.y*b.y, a.z*b.z);
}

inline glm::vec3 Abs(const glm::vec3&a)
{
  return glm::vec3(fabsf(a.x), fabsf(a.y), fabsf(a.z));
}

inline glm::vec3 ComponentSquareSigned(const glm::vec3& a)
{
  return glm::vec3(a.x*fabsf(a.x), a.y*fabsf(a.y), a.z*fabsf(a.z));
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

inline glm::vec3 Maximum(const glm::vec3& a, const glm::vec3& b) 
{
  glm::vec3 result(Maximum(a.x, b.x), Maximum(a.y, b.y), Maximum(a.z, b.z));
  return result;
}

inline void CheckSanity(const glm::vec3& v)
{
   // Vérifie que les coordonnées du vecteur ne sont pas NaN (Not a Number)
    assert(v.x == v.x); // Vérifie que x n'est pas NaN
    assert(v.y == v.y); // Vérifie que y n'est pas NaN
    assert(v.z == v.z); // Vérifie que z n'est pas NaN
}

inline void CheckSanity(const glm::mat4& tm)
{
    // Vérifie chaque ligne de la matrice de transformation
    CheckSanity(tm[0]); // Ligne X
    CheckSanity(tm[1]); // Ligne Y
    CheckSanity(tm[2]); // Ligne Z
    CheckSanity(tm[3]); // Translation (ligne T)
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

inline glm::vec3 ClampToRange(glm::vec3 val, float minVal, float maxVal) 
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
  assert(minVal < maxVal);
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

constexpr float PI = 3.14159265358979323846f;
//constexpr float HALF_PI = 0.5f * PI;
//constexpr float TWO_PI = 2.0f * PI;
//constexpr float TWO_PI_INV = 1.0f / TWO_PI;
#define HALF_PI    (0.5f * PI)
#define TWO_PI     (2.0f * PI)
#define TWO_PI_INV (1.0f / TWO_PI)
 
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

inline btVector3 Vector3ToBulletVector3(const glm::vec3& v) {return btVector3(v.x, v.y, v.z);}
inline glm::vec3   BulletVector3ToVector3(const btVector3& v) {return glm::vec3(v.x(), v.y(), v.z());}

inline btQuaternion QuatToBulletQuaternion(const glm::quat& q) {return btQuaternion(q.x, q.y, q.z, q.w);}
inline glm::quat BulletQuaternionToQuat(const btQuaternion& q) {return glm::quat(q.w(), q.x(), q.y(), q.z());}

inline btTransform TransformToBulletTransform(const glm::mat4& tm) 
{
  // Extraire la rotation (matrice 3x3) et la translation (vecteur 3D) de la matrice 4x4
  glm::mat3 rotation = glm::mat3(tm);  // Partie rotation (les 3 premières colonnes)
  glm::vec3 translation = glm::vec3(tm[3][0], tm[3][1], tm[3][2]);  // Dernière colonne pour la translation
  
  return btTransform(QuatToBulletQuaternion(glm::quat(tm)), Vector3ToBulletVector3(translation));
}

inline glm::mat4 BulletTransformToTransform(const btTransform& tm) 
{
  // Convertir le quaternion Bullet en quaternion GLM
  glm::quat q = BulletQuaternionToQuat(tm.getRotation());

  // Créer une matrice 4x4 à partir du quaternion pour la rotation
  glm::mat4 r = glm::mat4_cast(q);

  // Extraire la translation Bullet et la convertir en glm::vec3
  glm::vec3 t = BulletVector3ToVector3(tm.getOrigin());

  // Mettre à jour la dernière colonne de la matrice 4x4 pour la translation
  r[3] = glm::vec4(t, 1.0f);

  return r;
}

void ApplyRollPitchYawToRotationDegrees(float roll, float pitch, float yaw, glm::vec3& rotation);

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
