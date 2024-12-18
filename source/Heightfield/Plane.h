/*
  Sss - a slope soaring simulater.
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#ifndef ClipPlane_H
#define ClipPlane_H

/*!
  \file
  This defines the stuff needed for mClipClipPlanes processing. This
  needs 4 vectors and 4x4 matrices.
  Really something should be done about the matrix stuff being 
  in here too
*/
#include "Helpers.h"

#include <cmath>

class Matrix44;

class ClipPlane
{
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

class Matrix44
{
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

inline float ClipPlane::distanceToPoint(const Vector3& pos) const
{
  Vector3 normal(data[0], data[1], data[2]);
  // normal is already normalised.. I hope...!
  float dist =  normal.Dot(pos);
  return dist + data[3];
}

inline float ClipPlane::distanceToPoint(float x, float y, float z) const
{
  return data[0]*x + data[1]*y + data[2]*z + data[3];
}

inline void ClipPlane::show() const
{
  printf("ClipPlane %p: %6.2f %6.2f %6.2f %6.2f\n",
         this,
         data[0], data[1], data[2], data[3]);
}

inline ClipPlane operator*(const Matrix44 & lhs, const ClipPlane & rhs)
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

#endif
