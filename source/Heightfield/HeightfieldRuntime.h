/*
  Sss - a slope soaring simulater.
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#ifndef VERTEX_H
#define VERTEX_H

#include "HeightfieldCommon.h"

#include "Plane.h"

// Changed from CIwArray to std::vector during SDL2 migration
#include <vector>
#define ARRAY_TYPE std::vector

namespace Heightfield
{

/// Deals with the terrain mesh. Support automatic per-vertex colouring
/// and saving the calculated vertices (for multi-pass rundering)
class HeightfieldRuntime
{
public:
  struct Pos
  {
    Pos() {}
    Pos(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;
  };

  typedef ARRAY_TYPE<Pos> SavedPoints;

  // ctor sets up x, y, z, delta, r
  HeightfieldRuntime(int size, int degree, const Vertex* vertices);
  ~HeightfieldRuntime() {}

  void ReservePoints(size_t num);
  
  Vector3 getPos(int i, int j) const; // i and j are "x" and "y"
  
  float getHeight(int i, int j) const; // i and j are "x" and "y"
  
  int getSize() const {return mSize;}

  float getRange() const {return xMax - xMin;}

  float getCellSize() const {return cellSize;}
  
  // mesh refinement fns
  //! Top-level refinement function - generates the array of saved points that can be rendered as a triangle strip
  void meshRefine(const Vector3& eye_pos, float lod);
  
  //! Sets clipping parameters (and enables)
  void setClipping(
    const Transform& cameraTM, 
    float verticalFOV,
    float aspect,
    float Near, 
    float Far);

  //! Just enables/disables clipping
  void setClipping(bool clip) {mClipToPlanes = clip;}
  
  /// plots the saved points
  void clearSaved();

  const SavedPoints& getSavedPoints() const {return mSavedPoints;}

  /// returns the number of triangles in the last terrain Render
  unsigned getNumTriangles() const {return mNumVertices - 2;}

  /// Returns the points marking the heightfield extents
  const Vertex & sw() const {return mVertices[0];}
  const Vertex & se() const {return mVertices[1];}
  const Vertex & ne() const {return mVertices[2];}
  const Vertex & nw() const {return mVertices[3];}

  /// Returns the point in the centre
  const Vertex & c() const {return mVertices[4];}

private:
  HeightfieldRuntime();
  
  const Vertex & w() const {return mVertices[5];}
  const Vertex & s() const {return mVertices[6];}
  const Vertex & e() const {return mVertices[7];}
  const Vertex & n() const {return mVertices[8];}
  
  void submeshRefine(int i, int j, int l, float plot_z_i, unsigned clip_mask);
  
  inline void tStripAppend(int v, int p, float plot_z);
  inline void appendVertex(int v, float plot_z);
  
  enum {M=-7};

  inline bool active(int v, float & plot_z, unsigned & clip_mask) const;
  
  const int mDegree; // the degree
  const int mSize;
  const Vertex* mVertices;
  // internal variables for the mesh refinement
  int vn0; // ptr to V(n-0)
  int vn1; // ptr to V(n-1)
  
  float mPlotZ0;
  float mPlotZ1;
  
  int mParity;
  
  float mEyeX, mEyeY, mEyeZ;
  float mLOD;
  
  // clipping planes
  ClipPlane mClipPlanes[6];
  bool mClipToPlanes;

  /// Save the points as we go into an array
  SavedPoints mSavedPoints;

  unsigned mNumVertices;
  
  const float xMid, yMid;
  const float xMin, xMax, yMin, yMax;
  const float cellSize;
};

//------------- Inline functions ------------------

inline Vector3 HeightfieldRuntime::getPos(int i, int j) const
{
  int index = mVertices[Heightfield::calcIndex(i, j, mSize)].index;  
  return Vector3(mVertices[index].x, mVertices[index].y, mVertices[index].z);
}

inline float HeightfieldRuntime::getHeight(int i, int j) const
{
  int index = mVertices[Heightfield::calcIndex(i, j, mSize)].index;  
  return mVertices[index].z;
}
}
#undef ARRAY_TYPE

#endif
