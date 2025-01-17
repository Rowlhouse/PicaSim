#ifndef VECTEURS_H
#define VECTEURS_H



// #include "Entete.h"
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <stdexcept>

class Quat;
class Transform;
class Vector2;
class Vector3;
class Vector4;
class Matrix44;

class Vector2 {
public:
    glm::vec2 vec; // Contient un vecteur GLM
    float& x; // Référence à vec.x
    float& y; // Référence à vec.y

    // Constructeurs
    Vector2() : vec(0.0f, 0.0f), x(vec.x), y(vec.y) {} // Vecteur nul
    Vector2(float x, float y) : vec(x, y), x(vec.x), y(vec.y) {}
    Vector2(const glm::vec2& v) : vec(v), x(vec.x), y(vec.y) {}

    // Accès aux composantes
    //float x() const { return vec.x; }
    //float y() const { return vec.y; }

    // Opérateurs pour imiter Marmalade
    Vector2 operator+(const Vector2& other) const { return Vector2(vec + other.vec); }
    Vector2 operator-(const Vector2& other) const { return Vector2(vec - other.vec); }
    Vector2 operator*(float scalar) const { return Vector2(vec * scalar); }
    void operator=(const Vector2& other) { vec = other.vec; }

    // Fonction utilitaire
    float GetLengthSquared() const { return glm::length2(vec); }
    float GetLength() const { return glm::length(vec); }
    Vector2 GetNormalised() const { return Vector2(glm::normalize(vec)); }

    void Normalise() { vec = glm::normalize(vec); }

    void Print() const {
        std::cout << "Position: (" << x << ", " << y << ")\n";
    }
};


class Vector3 {
public:
    glm::vec3 vec; // Contient un vecteur GLM
    float& x; // Référence à vec.x
    float& y; // Référence à vec.y
    float& z; // Référence à vec.z

    // Constructeurs
    Vector3() : vec(0.0f, 0.0f, 0.0f), x(vec.x), y(vec.y), z(vec.z) {} // Vecteur nul
    Vector3(float x, float y, float z) : vec(x       , y       , z       ), x(vec.x), y(vec.y), z(vec.z) {}
    Vector3(int x  , int y  , int z  ) : vec(float(x), float(y), float(z)), x(vec.x), y(vec.y), z(vec.z) {}
    //Vector3(int x  , int y  , float z  ) : vec(float(x), float(y), z), x(vec.x), y(vec.y), z(vec.z) {}
    Vector3(const glm::vec3& v) : vec(v), x(vec.x), y(vec.y), z(vec.z) {}
    Vector3(const glm::vec4& v) : vec(glm::vec3(v)), x(vec.x), y(vec.y), z(vec.z) {}

    /*// Accès aux composantes
    float x() const { return vec.x; }
    float y() const { return vec.y; }
    float z() const { return vec.z; }*/

    // Opérateurs pour imiter Marmalade
    void operator=(const Vector3& other) { vec = other.vec; }
    Vector3 operator+(const Vector3& other) const { return Vector3(vec + other.vec); }
    Vector3 operator-() const { return Vector3(- vec); }
    Vector3 operator-(const Vector3& other) const { return Vector3(vec - other.vec); }
    float operator*(const Vector3& other) const { return glm::dot(vec, other.vec); }
    Vector3 operator*(float scalar) const { return Vector3(vec * scalar); }
    Vector3 operator/(float scalar) const { return Vector3(vec / scalar); }
    float& operator[](size_t index) { if (index > 2) { throw std::out_of_range("Index out of range for Vector3");} return vec[index];}
    const float& operator[](size_t index) const { if (index > 2) { throw std::out_of_range("Index out of range for Vector3");} return vec[index];}

    bool operator==(const Vector3& other) const {return vec == other.vec;}
    bool operator!=(const Vector3& other) const {return vec != other.vec;}


    // Nouvel opérateur : multiplication d'un scalaire par un Vector3
    friend Vector3 operator*(float scalar, const Vector3& v) {return Vector3(v.vec * scalar); }

    void operator+=(const Vector3& v) { vec = vec + v.vec; }
    void operator-=(const Vector3& other) { vec -= other.vec; }
    void operator*=(float scalar) { vec = vec * scalar; }

    float Dot (const Vector3& other) const {return glm::dot(vec,other.vec);}
    Vector3 Cross (const Vector3& other) const {return Vector3(glm::cross(vec, other.vec));}

    // Fonction utilitaire
    float GetLengthSquared() const { return glm::length2(vec); }
    float GetLength() const { return glm::length(vec); }
    Vector3 GetNormalised() const { return Vector3(glm::normalize(vec)); }

    void Normalise() { vec = glm::normalize(vec); }

    bool IsZero() {return vec == glm::vec3(0.0f);}

    static Vector3 g_Zero;

    void Print() const {
        std::cout << "Position: (" << x << ", " << y << ", " << z << ")\n";
    }
};

Vector3 Vector3::g_Zero = Vector3();

class Vector4 {
public:
    glm::vec4 vec; // Contient un vecteur GLM
    float& x; // Référence à vec.x
    float& y; // Référence à vec.y
    float& z; // Référence à vec.z
    float& w; // Référence à vec.w


    // Constructeurs
    Vector4() : vec(0.0f, 0.0f, 0.0f, 0.0f), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {} // Vecteur nul
    Vector4(float x, float y, float z, float w) : vec(x, y, z, w), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
    Vector4(const glm::vec4& v) : vec(v), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

    // Accès aux composantes
    // float x() const { return vec.x; }
    // float y() const { return vec.y; }
    // float z() const { return vec.z; }
    // float w() const { return vec.w; }

    // Opérateurs pour imiter Marmalade
    Vector4 operator+(const Vector4& other) const { return Vector4(vec + other.vec); }
    Vector4 operator-(const Vector4& other) const { return Vector4(vec - other.vec); }
    Vector4 operator*(float scalar) const { return Vector4(vec * scalar); }
    void operator=(const Vector4& other) { vec = other.vec; }

    // Fonctions utilitaires
    float GetLengthSquared() const { return glm::length2(vec); }
    float GetLength() const { return glm::length(vec); }
    Vector4 GetNormalized() const { return Vector4(glm::normalize(vec)); }

    void Normalise() { vec = glm::normalize(vec); }

    void Print() const {
        std::cout << "Position: (" << x << ", " << y << ", " << z << ", " << w << ")\n";
    }
};


class Transform {
public : 
    glm::mat3 m;
    Vector3 t;

    //Contructeurs
    Transform () : m(glm::mat3(1.0f)), t(Vector3())                                                         {}
    Transform (glm::mat3 M, Vector3 T) : m(M), t(T)                                                         {}
    Transform (const glm::mat4& mat) : m(glm::mat3(mat)), t(Vector3(mat[3][0],mat[3][1],mat[3][2]))         {}

    //Opérations
    void operator=(const Quat& other);
    Transform operator-() const { return Transform (-m, -t);}
    Transform operator-(const Transform& other) const { return Transform(m - other.m, t - other.t); }
    Transform operator*(const Transform& other) const { return Transform(glm::translate(glm::mat4(other.m), other.t.vec) * glm::translate(glm::mat4(m), t.vec)); }

    bool operator!=(const Transform& other) const {return m != other.m;}

    // Fonctions utilitaires
    Vector3 RowX () const {return Vector3(m[0]);}
    Vector3 RowY () const {return Vector3(m[1]);}
    Vector3 RowZ () const {return Vector3(m[2]);}
    Vector3 GetTrans() const {return t;}
    //Vector3 t() const {return Vector3(m[3]);}
    Vector3 RotateVec (const Vector3& v)    const {return Vector3( m * v.vec );}
    Vector3 TransformVec (const Vector3& v) const {return Vector3((m * v.vec) + t.vec);}

    Transform GetTranspose() const {return Transform(glm::transpose(m));}

    void Transpose() { m = glm::transpose(m); }
    void SetIdentity() { m = glm::mat3(1.0f);}
    void SetTrans(const Vector3& translation) { t.vec = translation.vec;}
    void SetAxisAngle (const Vector3& v, const float angle) {m = glm::mat3(glm::rotate(glm::mat4(1.0f), angle, v.vec));}
    void PostRotate(const Quat& Q);

    //glm::translate(glm::mat4(m), t);

    // Méthode statique pour obtenir la transformation identité
    static Transform g_Identity;
};

// Définition de l'instance statique g_Identity
Transform Transform::g_Identity = Transform();

/*
| `mTM.SetAxisAngle(glm::vec3(axis), angle)`             | `glm::mat4 mTM = glm::rotate(glm::mat4(1.0f), angle, axis)`                           |*/


class Quat {
public :
    glm::quat quat;
    float& x; 
    float& y;
    float& z;
    float& s;


    //Constructeurs
    Quat () : quat(glm::quat()), x(quat.x), y(quat.y), z(quat.z), s(quat.w) {}
    Quat (const Transform mat) : quat(glm::quat(mat.m)), x(quat.x), y(quat.y), z(quat.z), s(quat.w) {}
    Quat (const float s, const float x, const float y, const float z) : quat(glm::quat(x,y,z,s)), x(quat.x), y(quat.y), z(quat.z), s(quat.w) {}
    Quat (const Vector3& v, float angle) : quat(glm::angleAxis(angle, v.vec)), x(quat.x), y(quat.y), z(quat.z), s(quat.w) {}

    void operator=(const Quat& other) { quat = other.quat;}

    Vector3 RotateVector(Vector3& vector)  {return Vector3(glm::rotate(quat, vector.vec));}

};


class ClipPlane{
public:
  ClipPlane(float a, float b, float c, float d) 
  {
    data[0] = a; data[1] = b; data[2] = c; data[3] = d;
  }
  ClipPlane() {}

  float & operator()(int i) {return data[i];}
  const float & operator()(int i) const {return data[i];}

  inline float distanceToPoint(const Vector3& pos) const;
  
  inline float distanceToPoint(float x, float y, float z) const;
  
  inline void show() const;
  
  friend ClipPlane operator*(const Matrix44 & lhs, const ClipPlane & rhs);

  void normalise()
  {
    float length = sqrtf(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    data[0] /= length;
    data[1] /= length;
    data[2] /= length;
    data[3] /= length;
  }

  void negate() 
  {
    data[0] = -data[0];
    data[1] = -data[1];
    data[2] = -data[2];
    data[3] = -data[3];
  }
private:
  float data[4];
};

class Matrix44 {
public:
  Matrix44(
    float d00, float d01, float d02, float d03,
    float d10, float d11, float d12, float d13,
    float d20, float d21, float d22, float d23,
    float d30, float d31, float d32, float d33)
  {
    data[0][0] = d00; data[0][1] = d01; data[0][2] = d02; data[0][3] = d03;
    data[1][0] = d10; data[1][1] = d11; data[1][2] = d12; data[1][3] = d13;
    data[2][0] = d20; data[2][1] = d21; data[2][2] = d22; data[2][3] = d23;
    data[3][0] = d30; data[3][1] = d31; data[3][2] = d32; data[3][3] = d33;
  }
  float & operator()(int i, int j) {return data[i][j];}
  const float & operator()(int i, int j) const {return data[i][j];}
  friend ClipPlane operator*(const Matrix44 & lhs, const ClipPlane & rhs);
private:
  float data[4][4];
};



typedef Vector2 CIwSVec2;
typedef Vector2 CIwFVec2;
typedef Vector2 CIwVec2;


#endif