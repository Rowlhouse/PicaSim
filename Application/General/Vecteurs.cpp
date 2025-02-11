#include "Vecteurs.h"

void Transform::operator=(const Quat& other)
{ 
    m = glm::mat3_cast(other.quat);
}

void Transform::PostRotate(const Quat& Q) {
  glm::mat3 rotationMatrix = glm::mat3_cast(Q.quat);  // Convertir le quaternion en matrice de rotation (3x3)
  m = rotationMatrix * m;  // Appliquer la rotation à la matrice existante
}


float ClipPlane::distanceToPoint(const Vector3& pos) const
{
  Vector3 normal(data[0], data[1], data[2]);
  // normal is already normalised.. I hope...!
  float dist =  normal.Dot(pos);
  return dist + data[3];
}

float ClipPlane::distanceToPoint(float x, float y, float z) const
{
  return data[0]*x + data[1]*y + data[2]*z + data[3];
}

void ClipPlane::show() const
{
  printf("ClipPlane %p: %6.2f %6.2f %6.2f %6.2f\n",
         this,
         data[0], data[1], data[2], data[3]);
}

ClipPlane operator*(const Matrix44 & lhs, const ClipPlane & rhs)
{
  ClipPlane result;
  
  for (uint i = 0 ; i != 4 ; i++)
  {
    result(i) = 
      lhs(i,0) * rhs(0) +
      lhs(i,1) * rhs(1) +
      lhs(i,2) * rhs(2) +
      lhs(i,3) * rhs(3);
  }
  return result;
}


Vector3 Vector3::g_Zero = Vector3();
// Définition de l'instance statique g_Identity
Transform Transform::g_Identity = Transform();