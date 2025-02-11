/*

  Interface for the facililty to preprocess a height field, the result
  being a 1D array of interleaved quad-trees (as "described" in
  Lindstrom + Pascucci).

  This header also defines the type of the output - so the final
  application will include this, so that it knows how to cast the data
  in the terrain file.

  Sss - a slope soaring simulater.
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
 */

#ifndef HEIGHTFIELDBUILDER_H
#define HEIGHTFIELDBUILDER_H

#include "LoadingScreenHelper.h"

//#include <cmath>

#include "HeightfieldCommon.h"
namespace Heightfield
{

//! This is used in the creation of the mesh - note that the
//! implementation is not needed when just reading the file.
class HeightfieldBuilder
{
public:
  // User passes in a pointer to the start of an array of type
  // Vertex. The array is of size (nx*nx), where nx = 2^(level/2) +
  // 1.
  // note that the input array gets modified (d and r get populated)
  HeightfieldBuilder(Vertex * vertexIn, int level, int nx, 
    float coastEnhancement, float plainAltitude, bool simplifyUnderPlain, LoadingScreenHelper* loadingScreen);
  
  //! dumps the output
  void mDisplayOutput() const;

  //! obtains the output (normally call this after creating a HeightfieldBuilder
  //! object).
  void getOutput(Vertex *& mVertexOut, int & size) const;
  
private: 

  enum Colour { LOD_WHITE, LOD_BLACK };
  
  // Structure used by HeightfieldBuilder
  struct DeltaR
  {
    float d, r;
    DeltaR(float d, float r) : d(d), r(r) {};
    DeltaR() : d(0), r(0) {};
  };

  void doTriangle(int i1, int j1,
                   int i2, int j2,
                   int i3, int j3,
                   int lev);
  
  void initialiseDR(LoadingScreenHelper* loadingScreen);
  
  void descendQuadTree(int i, int j, int pq, int dist, Colour colour);

  void addToOutput(int i, 
                     const Vertex & vertex);
  int calcIndex(int i, int j) const {return Heightfield::calcIndex(i, j, nx);}
  bool isValid(int i, int j) const
    { return ( (i >= 0) && (j >= 0) && (i < nx) && (j < nx) ); }

  inline bool onSeaShore(int i, int j) const;

  Vertex * const mVertexIn;
  Vertex * mVertexOut;

  const int level, nx;

  const int mVertexOutSize;
  int mMaxVertexOutIndex;

  // use these to increase the resolution around a particular altitude
  float mCoastEnhancement;
  float mPlainAltitude;
  bool  mSimplifyUnderPlain;
};

}

#endif
