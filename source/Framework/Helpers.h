#ifndef HELPERS_H
#define HELPERS_H

#include "btBulletDynamicsCommon.h"
#include "Platform.h"

// GLM math library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <cmath>
#include <cstdlib>
#include <cstdint>

// Type aliases for compatibility
typedef unsigned int uint;

// Override macro for older compilers (C++11 adds this)
#ifndef OVERRIDE
#define OVERRIDE override
#endif

// Mathematical constants
#ifndef PI
#define PI 3.14159265358979323846f
#endif

//==============================================================================
// Vector2 - Wrapper around glm::vec2 with Marmalade-compatible API
//==============================================================================
class Vector2
{
public:
    float x, y;

    Vector2() : x(0), y(0) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    Vector2(const glm::vec2& v) : x(v.x), y(v.y) {}

    operator glm::vec2() const { return glm::vec2(x, y); }

    float GetLength() const { return std::sqrt(x*x + y*y); }
    float GetLengthSquared() const { return x*x + y*y; }

    void Normalise() {
        float len = GetLength();
        if (len > 0.0f) {
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
        }
    }

    Vector2 GetNormalised() const {
        Vector2 result = *this;
        result.Normalise();
        return result;
    }

    Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
    Vector2 operator*(float s) const { return Vector2(x * s, y * s); }
    Vector2 operator/(float s) const { float inv = 1.0f / s; return Vector2(x * inv, y * inv); }
    Vector2 operator-() const { return Vector2(-x, -y); }

    Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
    Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    Vector2& operator*=(float s) { x *= s; y *= s; return *this; }
    Vector2& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; return *this; }

    bool operator==(const Vector2& v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vector2& v) const { return !(*this == v); }

    // Dot product
    float operator*(const Vector2& v) const { return x * v.x + y * v.y; }

    // Conversion to Vec2f for Marmalade compatibility
    operator Vec2f() const { return Vec2f(x, y); }
};

inline Vector2 operator*(float s, const Vector2& v) { return v * s; }

//==============================================================================
// Vector3 - Wrapper around glm::vec3 with Marmalade-compatible API
//==============================================================================
class Vector3
{
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vector3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}

    operator glm::vec3() const { return glm::vec3(x, y, z); }

    float GetLength() const { return std::sqrt(x*x + y*y + z*z); }
    float GetLengthSquared() const { return x*x + y*y + z*z; }

    void Normalise() {
        float len = GetLength();
        if (len > 0.0f) {
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
    }

    Vector3 GetNormalised() const {
        Vector3 result = *this;
        result.Normalise();
        return result;
    }

    // IsNormalised check
    bool IsNormalised() const {
        float lenSq = GetLengthSquared();
        return std::abs(lenSq - 1.0f) < 0.001f;
    }

    // IsZero check
    bool IsZero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }

    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
    Vector3 operator/(float s) const { float inv = 1.0f / s; return Vector3(x * inv, y * inv, z * inv); }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vector3& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; z *= inv; return *this; }

    bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vector3& v) const { return !(*this == v); }

    // Array-style access
    float& operator[](int i) { return (&x)[i]; }
    const float& operator[](int i) const { return (&x)[i]; }

    // Dot product (operator form)
    float operator*(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }

    // Dot product (named method form)
    float Dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }

    // Cross product
    Vector3 Cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    // Alias for cross product (Marmalade style)
    Vector3 operator^(const Vector3& v) const { return Cross(v); }

    // Static constants
    static const Vector3 g_Zero;
};

inline Vector3 operator*(float s, const Vector3& v) { return v * s; }

// Static constant definitions (need to be defined in a cpp file or inline)
// For header-only use, we use inline definition
inline const Vector3 Vector3::g_Zero = Vector3(0, 0, 0);

//==============================================================================
// Vector4 - Wrapper around glm::vec4 with Marmalade-compatible API
//==============================================================================
class Vector4
{
public:
    float x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
    Vector4(const Vector3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
    Vector4(const glm::vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    operator glm::vec4() const { return glm::vec4(x, y, z, w); }

    float GetLength() const { return std::sqrt(x*x + y*y + z*z + w*w); }
    float GetLengthSquared() const { return x*x + y*y + z*z + w*w; }

    void Normalise() {
        float len = GetLength();
        if (len > 0.0f) {
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
            w *= invLen;
        }
    }

    Vector4 GetNormalised() const {
        Vector4 result = *this;
        result.Normalise();
        return result;
    }

    Vector4 operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vector4 operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vector4 operator*(float s) const { return Vector4(x * s, y * s, z * s, w * s); }
    Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

    Vector4& operator+=(const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    Vector4& operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    Vector4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
};

inline Vector4 operator*(float s, const Vector4& v) { return v * s; }

//==============================================================================
// Forward declaration for Transform
//==============================================================================
class Quat;
class Transform;

//==============================================================================
// Quat - Wrapper around glm::quat with Marmalade-compatible API
// Note: Marmalade uses (s, x, y, z) ordering where s is the scalar/w component
//==============================================================================
class Quat
{
public:
    float s;  // Scalar component (w in GLM)
    float x, y, z;  // Vector components

    Quat() : s(1), x(0), y(0), z(0) {}
    Quat(float s_, float x_, float y_, float z_) : s(s_), x(x_), y(y_), z(z_) {}

    // Construct from GLM quaternion (GLM uses w,x,y,z order)
    Quat(const glm::quat& q) : s(q.w), x(q.x), y(q.y), z(q.z) {}

    // Convert to GLM quaternion
    operator glm::quat() const { return glm::quat(s, x, y, z); }

    // Construct from rotation matrix (Transform)
    Quat(const Transform& tm);

    // Construct from axis and angle (angle in radians)
    Quat(const Vector3& axis, float angle) {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        s = std::cos(halfAngle);
        x = axis.x * sinHalf;
        y = axis.y * sinHalf;
        z = axis.z * sinHalf;
    }

    float GetLength() const { return std::sqrt(s*s + x*x + y*y + z*z); }
    float GetLengthSquared() const { return s*s + x*x + y*y + z*z; }

    void Normalise() {
        float len = GetLength();
        if (len > 0.0f) {
            float invLen = 1.0f / len;
            s *= invLen;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
    }

    Quat GetNormalised() const {
        Quat result = *this;
        result.Normalise();
        return result;
    }

    // Quaternion multiplication
    Quat operator*(const Quat& q) const {
        return Quat(
            s * q.s - x * q.x - y * q.y - z * q.z,
            s * q.x + x * q.s + y * q.z - z * q.y,
            s * q.y - x * q.z + y * q.s + z * q.x,
            s * q.z + x * q.y - y * q.x + z * q.s
        );
    }

    // Conjugate
    Quat GetConjugate() const { return Quat(s, -x, -y, -z); }

    // Rotate a vector
    Vector3 RotateVec(const Vector3& v) const {
        glm::quat q(s, x, y, z);
        glm::vec3 result = q * glm::vec3(v.x, v.y, v.z);
        return Vector3(result.x, result.y, result.z);
    }

    // Alias for RotateVec (Marmalade compatibility)
    Vector3 RotateVector(const Vector3& v) const { return RotateVec(v); }

    // Slerp interpolation
    static Quat Slerp(const Quat& q1, const Quat& q2, float t) {
        glm::quat result = glm::slerp(glm::quat(q1), glm::quat(q2), t);
        return Quat(result);
    }
};

//==============================================================================
// Transform - 3x3 rotation matrix with translation (matches CIwFMat interface)
// Storage: m[row][col] for rotation, t for translation
//==============================================================================
class Transform
{
public:
    float m[3][3];  // Rotation matrix (row-major: m[row][col])
    Vector3 t;      // Translation

    Transform() {
        SetIdentity();
    }

    // Construct from quaternion
    Transform(const Quat& q) {
        SetFromQuat(q);
        t = Vector3(0, 0, 0);
    }

    // Construct from GLM mat4
    Transform(const glm::mat4& mat) {
        m[0][0] = mat[0][0]; m[0][1] = mat[1][0]; m[0][2] = mat[2][0];
        m[1][0] = mat[0][1]; m[1][1] = mat[1][1]; m[1][2] = mat[2][1];
        m[2][0] = mat[0][2]; m[2][1] = mat[1][2]; m[2][2] = mat[2][2];
        t = Vector3(mat[3][0], mat[3][1], mat[3][2]);
    }

    void SetIdentity() {
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        t = Vector3(0, 0, 0);
    }

    // Convert quaternion to rotation matrix
    // Uses Marmalade's transposed convention to work with row-vector TransformVec
    void SetFromQuat(const Quat& q) {
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float yz = q.y * q.z;
        float sx = q.s * q.x;
        float sy = q.s * q.y;
        float sz = q.s * q.z;

        // Marmalade stores the transposed rotation matrix
        m[0][0] = 1 - 2 * (yy + zz);
        m[0][1] = 2 * (xy + sz);   // Note: signs swapped from standard formula
        m[0][2] = 2 * (xz - sy);
        m[1][0] = 2 * (xy - sz);
        m[1][1] = 1 - 2 * (xx + zz);
        m[1][2] = 2 * (yz + sx);
        m[2][0] = 2 * (xz + sy);
        m[2][1] = 2 * (yz - sx);
        m[2][2] = 1 - 2 * (xx + yy);
    }

    // Set rotation from axis and angle (radians)
    // Uses Marmalade's transposed convention to work with row-vector TransformVec
    void SetAxisAngle(const Vector3& axis, float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        float omc = 1.0f - c;
        float x = axis.x, y = axis.y, z = axis.z;

        // Transposed from standard Rodrigues formula
        m[0][0] = c + x*x*omc;        m[0][1] = x*y*omc + z*s;     m[0][2] = x*z*omc - y*s;
        m[1][0] = y*x*omc - z*s;      m[1][1] = c + y*y*omc;       m[1][2] = y*z*omc + x*s;
        m[2][0] = z*x*omc + y*s;      m[2][1] = z*y*omc - x*s;     m[2][2] = c + z*z*omc;
    }

    // Set rotation around X axis
    // Uses Marmalade's transposed convention
    void SetRotX(float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;
        m[1][0] = 0; m[1][1] = c;  m[1][2] = s;   // Transposed: was -s
        m[2][0] = 0; m[2][1] = -s; m[2][2] = c;   // Transposed: was s
    }

    // Set rotation around Y axis
    // Uses Marmalade's transposed convention
    void SetRotY(float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        m[0][0] = c;  m[0][1] = 0; m[0][2] = -s;  // Transposed: was s
        m[1][0] = 0;  m[1][1] = 1; m[1][2] = 0;
        m[2][0] = s;  m[2][1] = 0; m[2][2] = c;   // Transposed: was -s
    }

    // Set rotation around Z axis
    // Uses Marmalade's transposed convention
    void SetRotZ(float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        m[0][0] = c;  m[0][1] = s;  m[0][2] = 0;  // Transposed: was -s
        m[1][0] = -s; m[1][1] = c;  m[1][2] = 0;  // Transposed: was s
        m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 1;
    }

    // Set rotation around Z axis with options to preserve trans and scale
    // Uses Marmalade's transposed convention
    void SetRotZ(float angle, bool preserveTrans, bool preserveScale) {
        Vector3 savedTrans = t;
        float c = std::cos(angle);
        float s = std::sin(angle);
        m[0][0] = c;  m[0][1] = s;  m[0][2] = 0;  // Transposed: was -s
        m[1][0] = -s; m[1][1] = c;  m[1][2] = 0;  // Transposed: was s
        m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 1;
        if (preserveTrans) {
            t = savedTrans;
        }
    }

    // Normalise the rotation part (orthonormalize the matrix)
    void Normalise() {
        // Gram-Schmidt orthonormalization
        Vector3 rx = RowX();
        Vector3 ry = RowY();
        Vector3 rz;

        rx.Normalise();
        rz = rx.Cross(ry);
        rz.Normalise();
        ry = rz.Cross(rx);
        ry.Normalise();

        m[0][0] = rx.x; m[0][1] = rx.y; m[0][2] = rx.z;
        m[1][0] = ry.x; m[1][1] = ry.y; m[1][2] = ry.z;
        m[2][0] = rz.x; m[2][1] = rz.y; m[2][2] = rz.z;
    }

    // Row accessors (returns row as Vector3)
    Vector3 RowX() const { return Vector3(m[0][0], m[0][1], m[0][2]); }
    Vector3 RowY() const { return Vector3(m[1][0], m[1][1], m[1][2]); }
    Vector3 RowZ() const { return Vector3(m[2][0], m[2][1], m[2][2]); }

    // Column accessors
    Vector3 ColumnX() const { return Vector3(m[0][0], m[1][0], m[2][0]); }
    Vector3 ColumnY() const { return Vector3(m[0][1], m[1][1], m[2][1]); }
    Vector3 ColumnZ() const { return Vector3(m[0][2], m[1][2], m[2][2]); }

    // Translation accessor
    Vector3 GetTrans() const { return t; }
    void SetTrans(const Vector3& trans) { t = trans; }

    // Transform a point (rotation + translation)
    // Uses row-vector convention (v * M) matching Marmalade's CIwFMat
    Vector3 TransformVec(const Vector3& v) const {
        return Vector3(
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + t.x,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + t.y,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + t.z
        );
    }

    // Rotate a vector (no translation)
    // Uses row-vector convention (v * M) matching Marmalade's CIwFMat
    Vector3 RotateVec(const Vector3& v) const {
        return Vector3(
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z
        );
    }

    // Rotate a vector by the transpose (inverse for orthonormal matrix)
    // Uses column-vector convention (M^T * v) matching Marmalade's CIwFMat
    Vector3 TransposeRotateVec(const Vector3& v) const {
        return Vector3(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }

    // Transpose the rotation part (returns new transform)
    Transform GetTranspose() const {
        Transform result;
        result.m[0][0] = m[0][0]; result.m[0][1] = m[1][0]; result.m[0][2] = m[2][0];
        result.m[1][0] = m[0][1]; result.m[1][1] = m[1][1]; result.m[1][2] = m[2][1];
        result.m[2][0] = m[0][2]; result.m[2][1] = m[1][2]; result.m[2][2] = m[2][2];
        result.t = t;
        return result;
    }

    // Transpose the rotation part in-place
    void Transpose() {
        float tmp;
        tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
        tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
        tmp = m[1][2]; m[1][2] = m[2][1]; m[2][1] = tmp;
    }

    // Multiply this transform by another
    // Matches Marmalade's CIwFMat operator*: N.t = M.TransformVec(t)
    Transform operator*(const Transform& other) const {
        Transform result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.m[i][j] = m[i][0] * other.m[0][j] + m[i][1] * other.m[1][j] + m[i][2] * other.m[2][j];
            }
        }
        result.t = other.TransformVec(t);
        return result;
    }

    // Pre-multiply: combine rotation and then translate
    void PreMult(const Transform& other) {
        *this = other * (*this);
    }

    // Post-multiply
    Transform& PostMult(const Transform& other) {
        *this = (*this) * other;
        return *this;
    }

    // Post-rotate around X axis
    void PostRotateX(float angle) {
        Transform rot;
        rot.SetRotX(angle);
        PostMult(rot);
    }

    // Post-rotate around Y axis
    void PostRotateY(float angle) {
        Transform rot;
        rot.SetRotY(angle);
        PostMult(rot);
    }

    // Post-rotate around Z axis
    void PostRotateZ(float angle) {
        Transform rot;
        rot.SetRotZ(angle);
        PostMult(rot);
    }

    // Post-rotate by a quaternion
    void PostRotate(const Quat& q) {
        Transform rot(q);
        PostMult(rot);
    }

    // Convert to GLM mat4
    glm::mat4 ToGLM() const {
        glm::mat4 result(1.0f);
        result[0][0] = m[0][0]; result[1][0] = m[0][1]; result[2][0] = m[0][2]; result[3][0] = t.x;
        result[0][1] = m[1][0]; result[1][1] = m[1][1]; result[2][1] = m[1][2]; result[3][1] = t.y;
        result[0][2] = m[2][0]; result[1][2] = m[2][1]; result[2][2] = m[2][2]; result[3][2] = t.z;
        return result;
    }

    // Create rotation transform from axis and angle
    static Transform FromAxisAngle(const Vector3& axis, float angle) {
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(axis.x, axis.y, axis.z));
        return Transform(mat);
    }

    // Look at (creates transform looking from eye to target)
    static Transform LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
        glm::mat4 mat = glm::lookAt(glm::vec3(eye), glm::vec3(target), glm::vec3(up));
        return Transform(mat);
    }

    // Static identity transform (for compatibility with CIwFMat::g_Identity)
    static const Transform g_Identity;

    // Comparison operators
    bool operator==(const Transform& other) const {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (m[i][j] != other.m[i][j]) return false;
        return t == other.t;
    }
    bool operator!=(const Transform& other) const { return !(*this == other); }
};

// Implement Quat constructor from Transform
// Extracts quaternion from Marmalade's transposed matrix convention
inline Quat::Quat(const Transform& tm) {
    // Convert rotation matrix to quaternion using Shepperd's method
    // Modified for transposed matrix: swap indices [i][j] <-> [j][i] for off-diagonals
    float trace = tm.m[0][0] + tm.m[1][1] + tm.m[2][2];

    if (trace > 0) {
        float s = 0.5f / std::sqrt(trace + 1.0f);
        this->s = 0.25f / s;
        x = (tm.m[1][2] - tm.m[2][1]) * s;  // Swapped for transposed
        y = (tm.m[2][0] - tm.m[0][2]) * s;  // Swapped for transposed
        z = (tm.m[0][1] - tm.m[1][0]) * s;  // Swapped for transposed
    } else if (tm.m[0][0] > tm.m[1][1] && tm.m[0][0] > tm.m[2][2]) {
        float s = 2.0f * std::sqrt(1.0f + tm.m[0][0] - tm.m[1][1] - tm.m[2][2]);
        this->s = (tm.m[1][2] - tm.m[2][1]) / s;  // Swapped for transposed
        x = 0.25f * s;
        y = (tm.m[0][1] + tm.m[1][0]) / s;  // Symmetric - no change needed
        z = (tm.m[0][2] + tm.m[2][0]) / s;  // Symmetric - no change needed
    } else if (tm.m[1][1] > tm.m[2][2]) {
        float s = 2.0f * std::sqrt(1.0f + tm.m[1][1] - tm.m[0][0] - tm.m[2][2]);
        this->s = (tm.m[2][0] - tm.m[0][2]) / s;  // Swapped for transposed
        x = (tm.m[0][1] + tm.m[1][0]) / s;  // Symmetric - no change needed
        y = 0.25f * s;
        z = (tm.m[1][2] + tm.m[2][1]) / s;  // Symmetric - no change needed
    } else {
        float s = 2.0f * std::sqrt(1.0f + tm.m[2][2] - tm.m[0][0] - tm.m[1][1]);
        this->s = (tm.m[0][1] - tm.m[1][0]) / s;  // Swapped for transposed
        x = (tm.m[0][2] + tm.m[2][0]) / s;  // Symmetric - no change needed
        y = (tm.m[1][2] + tm.m[2][1]) / s;
        z = 0.25f * s;
    }

    Normalise();
}

//==============================================================================
// Plane - Geometric plane (normal vector + distance from origin)
//==============================================================================
class Plane
{
public:
    Vector3 v;  // Normal vector
    float k;    // Distance from origin (plane equation: v.x*x + v.y*y + v.z*z + k = 0)

    Plane() : v(0, 1, 0), k(0) {}
    Plane(const Vector3& normal, float distance) : v(normal), k(distance) {}

    void Normalise() {
        float invM = 1.0f / v.GetLength();
        v *= invM;
        k *= invM;
    }

    float GetDistanceToPoint(const Vector3& pt) const {
        return v * pt + k;
    }
};

//==============================================================================
// Colour - RGBA color (8 bits per channel)
//==============================================================================
class Colour
{
public:
    uint8_t r, g, b, a;

    Colour(uint8_t r_ = 255, uint8_t g_ = 255, uint8_t b_ = 255, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    // Construct from ARGB uint32 (for compatibility with Marmalade font functions)
    explicit Colour(uint32_t argb) {
        a = (argb >> 24) & 0xFF;
        r = (argb >> 16) & 0xFF;
        g = (argb >> 8) & 0xFF;
        b = argb & 0xFF;
    }

    void Set(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255) {
        r = r_; g = g_; b = b_; a = a_;
    }

    void Set(uint32_t argb) {
        a = (argb >> 24) & 0xFF;
        r = (argb >> 16) & 0xFF;
        g = (argb >> 8) & 0xFF;
        b = argb & 0xFF;
    }

    // Convert to float components (0-1 range)
    float GetR() const { return r / 255.0f; }
    float GetG() const { return g / 255.0f; }
    float GetB() const { return b / 255.0f; }
    float GetA() const { return a / 255.0f; }

    // Set from float components (0-1 range)
    void SetFloat(float r_, float g_, float b_, float a_ = 1.0f) {
        r = static_cast<uint8_t>(r_ * 255);
        g = static_cast<uint8_t>(g_ * 255);
        b = static_cast<uint8_t>(b_ * 255);
        a = static_cast<uint8_t>(a_ * 255);
    }

    // Convert to 32-bit RGBA value
    uint32_t GetRGBA() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    // Convert to 32-bit ABGR value (common OpenGL format)
    uint32_t GetABGR() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    // Convert to glm::vec4
    glm::vec4 ToVec4() const {
        return glm::vec4(GetR(), GetG(), GetB(), GetA());
    }

    // Implicit conversion to uint32 for compatibility with Marmalade color functions
    operator uint32_t() const {
        return GetABGR();
    }

};

//==============================================================================
// Transform helper functions (matching original API)
//==============================================================================

inline void SetRowX(Transform& tm, const Vector3& v) {
    tm.m[0][0] = v.x; tm.m[0][1] = v.y; tm.m[0][2] = v.z;
}
inline void SetRowY(Transform& tm, const Vector3& v) {
    tm.m[1][0] = v.x; tm.m[1][1] = v.y; tm.m[1][2] = v.z;
}
inline void SetRowZ(Transform& tm, const Vector3& v) {
    tm.m[2][0] = v.x; tm.m[2][1] = v.y; tm.m[2][2] = v.z;
}

//==============================================================================
// Vector utility functions
//==============================================================================

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
    PS_ASSERT(v.x == v.x);  // NaN check
    PS_ASSERT(v.y == v.y);
    PS_ASSERT(v.z == v.z);
}

inline void CheckSanity(const Transform& tm)
{
    CheckSanity(tm.RowX());
    CheckSanity(tm.RowY());
    CheckSanity(tm.RowZ());
    CheckSanity(tm.GetTrans());
}

//======================================================================================================================
template<typename T>
inline T Interpolate(const T& v1, const T& v2, float t)
{
    return v1 + (v2 - v1) * t;
}

//======================================================================================================================
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

//======================================================================================================================
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
    PS_ASSERT(minVal < maxVal);
    const float delta = maxVal - minVal;
    float u = (val - minVal) / delta;
    u -= floorf(u);
    val = delta * u + minVal;
    return val;
}

//======================================================================================================================
template<typename T>
inline T ExponentialApproach(T val, T target, float dt, float lifetime)
{
    T error = val - target;
    if (fabsf(error) > 0.0001f)
        error *= expf(-dt / lifetime);
    else
        error = 0.0f;
    return target + error;
}

//======================================================================================================================
inline float cubicEaseInOut(float t, float /*b*/, float /*c*/, float /*d*/)
{
    if (t < 0.5f)
        return 4.0f * Cube(t);
    else
        return (t-1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
}

//======================================================================================================================
inline float SmoothStep(float t)
{
    if (t > 1.0f)
        return 1.0f;
    if (t < 0.0f)
        return 0.0f;
    return 3.0f * Square(t) - 2.0f * Cube(t);
}

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
    x = WrapToRange(x, 0.0f, TWO_PI);
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
    Parabola(float x0, float y0, float slope0, float x1, float y1)
    {
        float A = y0-y1;
        float B = Square(x0) - Square(x1);
        float C = x0-x1;
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

//==============================================================================
// Bullet Physics conversion functions
//==============================================================================

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
    Transform r(q);
    r.t = BulletVector3ToVector3(tm.getOrigin());
    return r;
}

void ApplyRollPitchYawToRotationDegrees(float roll, float pitch, float yaw, Vector3& rotation);

//==============================================================================
// Smoothing functions (templates work with float, Vector3, etc.)
//==============================================================================

template <typename T>
inline void SmoothCD(
    T &val,
    T &valRate,
    const float timeDelta,
    const T &to,
    const float smoothTime)
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
        valRate -= valRate;
    }
}

template <typename T>
void SmoothExponential(
    T&          val,
    const float timeDelta,
    const T&    to,
    const float smoothTime)
{
    if (smoothTime > 0.0f)
    {
        float lambda = timeDelta / smoothTime;
        val = to + (val - to) / (1.0f + lambda + 0.5f * lambda * lambda);
    }
    else
    {
        val = to;
    }
}

template <typename T>
void SmoothSpringDamperApprox(
    T&          val,
    T&          valRate,
    const float timeDelta,
    const T&    to,
    const float smoothTime,
    const float dampingRatio)
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

template <typename T>
void SmoothStiffnessDampingApprox(
    T&          val,
    T&          valRate,
    const float timeDelta,
    const T&    to,
    const float stiffness,
    const float damping)
{
    T accel = (to - val) * stiffness - valRate * damping;
    valRate += accel * timeDelta;
    val += valRate * timeDelta;
}

template <typename T>
void SmoothSpringDamper(
    T&          val,
    T&          valRate,
    const float timeDelta,
    const T&    to,
    const float smoothTime,
    const float dampingRatio)
{
    if (smoothTime > 0.0f)
    {
        float w = 2.0f / smoothTime;

        const T a = val - to;
        const T& b = valRate;

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

template <typename T>
void SmoothStiffnessDamping(
    T&          val,
    T&          valRate,
    const float timeDelta,
    const T&    to,
    const float stiffness,
    const float damping)
{
    if (stiffness > 0.0f)
    {
        float strength = sqrtf(stiffness);
        float smoothTime = 2.0f / strength;
        float dampingRatio = damping / (2.0f * strength);
        SmoothSpringDamper(val, valRate, timeDelta, to, smoothTime, dampingRatio);
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

template <typename T>
void SmoothSpringDamperFast(
    T&          val,
    T&          valRate,
    const float timeDelta,
    const T&    to,
    const float smoothTime,
    const float dampingRatio)
{
    if (smoothTime > 0.0f)
    {
        float w = 2.0f / smoothTime;

        const T a = val - to;
        const T& b = valRate;

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

template <typename T>
void SmoothStiffnessDampingFast(
    T&          val,
    T&          valRate,
    const float timeDelta,
    const T&    to,
    const float stiffness,
    const float damping)
{
    if (stiffness > 0.0f)
    {
        float strength = sqrtf(stiffness);
        float smoothTime = 2.0f / strength;
        float dampingRatio = damping / (2.0f * strength);
        SmoothSpringDamperFast(val, valRate, timeDelta, to, smoothTime, dampingRatio);
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


#endif // HELPERS_H
