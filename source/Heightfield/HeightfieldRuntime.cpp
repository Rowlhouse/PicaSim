/*!
  Sss - a slope soaring simulater.
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
  
  \file vertex.cpp
*/
#include "HeightfieldRuntime.h"
#include "Plane.h"

#include <cstdio>
#include <cmath>

namespace Heightfield
{

template<class T>
inline T max2(T t1, T t2) { return t1 > t2 ? t1: t2; }

template<class T>
inline T max3(T t1, T t2, T t3) { return max2(t1, max2(t2, t3)); }

//----------------------------------------------------------------------------------------------------------------------
HeightfieldRuntime::HeightfieldRuntime(int size, int degree, const Vertex* vertices)
  :
  mDegree(degree), mSize(size), mVertices(vertices),
  mClipToPlanes(false),
  mNumVertices(0),
  xMid(c().x), yMid(c().x),
  xMin(sw().x), xMax(se().x),
  yMin(sw().y), yMax(nw().y),
  cellSize(getPos(1, 1).x - getPos(0, 0).x)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! 
  
plotZ is set to the z-value that needs to be plotted (i.e. for
morphing) - only relevant if the return value is true.  clipMask is
0 if the point is (potentially) in the region of that mClipPlanes, or 1 if
it is definitely not (i.e. the point is definitely in the view
frustum according to that mClipPlanes).

*/
inline bool HeightfieldRuntime::active(
  int v, 
  float & plotZ, 
  unsigned & clipMask) const
{
  const Vertex & curVertex = mVertices[v];
  
  if (mClipToPlanes)
  {
    // effectively move the clipping planes out by dx, otherwise we
    // get popping at the screen edge.
    static const float dx = sqrtf(3.0f) * (getPos(1,0)[0] - getPos(0,0)[0]);
    
    // pre-calculate some stuff
    // the following is very "conservative"
    const float r0 = -(dx + fabsf(curVertex.z_m - curVertex.z) + 
                       curVertex.r);
    
    // if any of the planes indicate that the point is well outside,
    // we can return.  Note that we can make a small optimisation by
    // _not_ clipping against the near mClipPlanes. This is because the
    // top/bottom/side planes meet anyway, so they're sufficient to
    // exclude regions behind the viewpoint. Similarly, in practice
    // none of the terrains I've generated are so big that they go
    // beyond the far clipping mClipPlanes, so that can be excluded (should
    // make this configurable). Full clipping would use 6 in the loop
    // below.
    for (unsigned int i = 4 ; i-- != 0 ; )
    {
      if ( !(clipMask & (1u<<i)) )
      {
        float dist = mClipPlanes[i].distanceToPoint(
          curVertex.x, curVertex.y, curVertex.z);
        if (dist < r0)
        {
          // point is on outside of mClipPlanes
          return false;
        }
        else if (dist > curVertex.r)
        {
          // point is on inside of mClipPlanes - don't need to check it in
          // future (i.e. lower in the tree)
          clipMask |= (1u<<i);
        }
        // otherwise point "straddles the mClipPlanes" - need to check it in
        // future, so don't modify clipMask
      } // clipMask
    }
  }   
  
  // now check for activity according to the HeightfieldBuilder algorithm
  const float a = (mLOD * curVertex.d + curVertex.r);
  const float b = (mEyeX - curVertex.x);
  const float c = (mEyeY - curVertex.y);
  const float d = (mEyeZ - curVertex.z);
  float metric = a*a / ( b*b + c*c + d*d);
  const float metricMin = 1.0f; // always 1
  
  if (metric < metricMin)
  {
//    plotZ = -100; // always ignored
    return false;
  }

  const float metrixMax = 1.5f;
  if (metric > metrixMax)
  {
    plotZ = curVertex.z;
    return true;
  }
  
//   static const float metricScale = 1.0f/(metrixMax-metricMin);
  // define it by hand in case the compiler can't work it out from
  // above...
  const float metricScale = 2.0f;
  // morph
  metric = (metric - metricMin) * metricScale;
  plotZ = metric * curVertex.z + (1.0f-metric) * curVertex.z_m;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline void HeightfieldRuntime::appendVertex(int v, float plotZ)
{
  vn1 = vn0; // move stuff along
  vn0 = v;
  mPlotZ1 = mPlotZ0;
  mPlotZ0 = plotZ;
  mSavedPoints.push_back(Pos(mVertices[v].x, mVertices[v].y, plotZ));  
  ++mNumVertices;
}

//----------------------------------------------------------------------------------------------------------------------
inline void HeightfieldRuntime::tStripAppend(int v, int p, float plotZ)
{
  if ( (v != vn1) && (v != vn0) )
  {
    if (p != mParity)
      mParity = p;
    else
      appendVertex(vn1, mPlotZ1);
    
    // append new mVertices
    appendVertex(v, plotZ);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// note - we move the test up one level, as suggested in the original paper
void HeightfieldRuntime::submeshRefine(int i, int j, int level, 
                                  float plot_z_i, 
                                  unsigned clipMask)
{
  float plot_z_j;
  bool doit = ( (level-1 > 0) && active(j, plot_z_j, clipMask) );
  
  if (doit)
  {
    int t0 = 4*i + M;
    int tl = (2*i + j + M + 1) % 4;
    int tr = (tl+1) % 4;
    submeshRefine(j, t0+tl, level-1, plot_z_j, clipMask);
    tStripAppend(i, level % 2, plot_z_i);
    submeshRefine(j, t0+tr, level-1, plot_z_j, clipMask);
  }
  else
  {
    tStripAppend(i, level % 2, plot_z_i);
  }  
}

//----------------------------------------------------------------------------------------------------------------------
void HeightfieldRuntime::meshRefine(const Vector3& eyePos, float lod)
{
  mEyeX = eyePos[0];
  mEyeY = eyePos[1];
  mEyeZ = eyePos[2];
  mLOD = lod;

  mNumVertices = 0;  
  mParity = 0;
  
  appendVertex(0, mVertices[0].z); // sw
  appendVertex(0, mVertices[0].z); // sw
  unsigned clipMask = 0u;
  submeshRefine(4, 6, mDegree, mVertices[4].z, clipMask);
  tStripAppend(1, 1, mVertices[1].z);
  clipMask = 0u;
  submeshRefine(4, 7, mDegree, mVertices[4].z, clipMask);
  tStripAppend(2, 1, mVertices[2].z);
  clipMask = 0u;
  submeshRefine(4, 8, mDegree, mVertices[4].z, clipMask);
  tStripAppend(3, 1, mVertices[3].z);
  clipMask = 0u;
  submeshRefine(4, 5, mDegree, mVertices[4].z, clipMask);
  appendVertex(0, mVertices[0].z);
}

//----------------------------------------------------------------------------------------------------------------------
void HeightfieldRuntime::setClipping(
  const Transform& cameraTM, 
  float verticalFOV,
  float aspect,
  float Near,
  float Far)
{
  mClipToPlanes = true;
  
  // calculate the clipping planes.
  // first convert the fovy and aspect into the "focal length"
  
  float horizontalFOV = verticalFOV * aspect;
  float focalx = 1.0f/tanf(horizontalFOV*0.5f);
  float focaly = 1.0f/tanf(verticalFOV*0.5f);
  
  // clipping planes in eye coords
  // Note that the Planes have their normals pointing into the view frustrum.
  // Taken (almost) from Lengyel p. 93
  float e1x = sqrtf(focalx*focalx + 1.0f);
  float e1y = sqrtf(focaly*focaly + 1.0f);
  ClipPlane near0  ( 1,         0,           0,         -Near);
  ClipPlane far0   (-1,         0,           0,          Far);
  ClipPlane left0  ( 1/e1x,    -focalx/e1x,  0,          0);
  ClipPlane right0 ( 1/e1x,     focalx/e1x,  0,          0);
  ClipPlane bottom0( 1/e1y,     0,           focaly/e1y, 0);
  ClipPlane top0   ( 1/e1y,     0,          -focaly/e1y, 0);
  
  // p88 of Lengyel describes how to transform these planes using a 3x3 rotation matrix M
  // and translation vector T, so plane L_new = (F.inverse()).transpose() * L
  Transform MTrans = cameraTM;
  MTrans.Transpose();

  Vector3 minus_M_inv_T = -MTrans.RotateVec(cameraTM.GetTrans());
  
  Matrix44 F_inv_trans(
    MTrans.m[0][0], MTrans.m[0][1], MTrans.m[0][2], 0,
    MTrans.m[1][0], MTrans.m[1][1], MTrans.m[1][2], 0,
    MTrans.m[2][0], MTrans.m[2][1], MTrans.m[2][2], 0,
    minus_M_inv_T.x, minus_M_inv_T.y, minus_M_inv_T.z, 1 );

  mClipPlanes[0] = F_inv_trans * bottom0;
  mClipPlanes[1] = F_inv_trans * left0;
  mClipPlanes[2] = F_inv_trans * right0;
  mClipPlanes[3] = F_inv_trans * top0;
  mClipPlanes[4] = F_inv_trans * far0;
  mClipPlanes[5] = F_inv_trans * near0;
}


//----------------------------------------------------------------------------------------------------------------------
void HeightfieldRuntime::clearSaved()
{
  mSavedPoints.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void HeightfieldRuntime::ReservePoints(size_t num)
{
  mSavedPoints.reserve(num);
}

}

