#include "HeightfieldBuilder.h"

#include <cstdio>
#include <cmath>
//#include <cstdlib>
#include <cassert>

namespace Heightfield
{

template<class T>
inline T fmax(T a, T b) {return (a > b ? a : b);}
template<class T>
inline T fmax(T a, T b, T c) {return fmax(fmax(a,b),c);}
template<class T>
inline T fmax(T a, T b, T c, T d) {return fmax(fmax(a,b,c),d);}
template<class T>
inline T fmax(T a, T b, T c, T d, T e) {return fmax(fmax(a,b,c,d),e);}
template<class T>
inline T fmax(T a, T b, T c, T d, T e, T f) {return fmax(fmax(a,b,c,d,e),f);}

#define LOD_M (-7)

inline bool HeightfieldBuilder::onSeaShore(int i, int j) const
{
  bool orig = mVertexIn[calcIndex(i,j)].z > mPlainAltitude;
  
  static const int i_offsets[] = {0, 1, 1, 1, 0, -1, -1, -1};
  static const int j_offsets[] = {1, 1, 0, -1, -1, -1, 0, 1};
  
  for (unsigned int c = 0 ; c < sizeof(i_offsets) / sizeof(i_offsets[0]) ; ++c)
  {
    if (isValid(i+i_offsets[c],j+j_offsets[c])) 
    {
      bool this_one = (mVertexIn[calcIndex(i+i_offsets[c],
                                            j+j_offsets[c])].z > mPlainAltitude);
      if (this_one != orig)
      {
        return true;
      }
    }
  }
  
  return false;
}

HeightfieldBuilder::HeightfieldBuilder(
  Vertex * vertexIn, 
  const int level, 
  const int nx, 
  float coastEnhancement, 
  float plainAltitude, 
  bool simplifyUnderPlain,
  LoadingScreenHelper* loadingScreen)
  :
  mVertexIn(vertexIn),
  mVertexOut(0),
  level(level), nx(nx),
  mVertexOutSize(2*nx*nx), // 2 for some leeway
  mMaxVertexOutIndex(0),
  mCoastEnhancement(coastEnhancement),
  mSimplifyUnderPlain(simplifyUnderPlain),
  mPlainAltitude(plainAltitude)
{
  assert(nx == (int) pow(2.0, level/2.0)+1);
  
  mVertexOut = new Vertex[mVertexOutSize]; // ctor zeros
  
  printf("level = %d, nx = %d\n", level, nx);
  
  // start things off with some "well-known" points
  int i_c, i_sw, i_s, i_se, i_e, i_ne, i_n, i_nw, i_w;
  
  i_sw = 0;
  i_se = 1;
  i_ne = 2;
  i_nw = 3;
  
  i_c = 4;
  
  i_w = 5;
  i_s = 6;
  i_e = 7;
  i_n = 8;
  
  // start things off
  
  int i_mid = (nx-1)/2;
  
  int index_sw = calcIndex( 0,    0    );
  int index_se = calcIndex( nx-1, 0    );
  int index_ne = calcIndex( nx-1, nx-1 );
  int index_nw = calcIndex( 0,    nx-1 );
  
  int index_c  = calcIndex( i_mid,i_mid);
  
  int index_w = calcIndex(0,     i_mid);
  int index_s = calcIndex(i_mid, 0    );
  int index_e = calcIndex(nx-1,  i_mid);
  int index_n = calcIndex(i_mid, nx-1 );
  
  // Calculate delta and r

  if (loadingScreen) loadingScreen->Update("Processing terrain");
  initialiseDR(loadingScreen);
  if (loadingScreen) loadingScreen->Update();
  initialiseDR(loadingScreen);
  if (loadingScreen) loadingScreen->Update();
  initialiseDR(loadingScreen);
  if (loadingScreen) loadingScreen->Update();
  initialiseDR(loadingScreen);
  
  // Re-order things, writing the result in mVertexOut.
  // descend the white tree, starting from the center
  int dist = i_mid/2;

  if (loadingScreen) loadingScreen->Update();

  descendQuadTree(i_mid, i_mid, i_c, dist, LOD_WHITE);
  addToOutput(i_c, mVertexIn[index_c]);

  if (loadingScreen) loadingScreen->Update();

  // descend the black trees, starting from west
  descendQuadTree(0,    i_mid,i_w, dist, LOD_BLACK);
  addToOutput(i_w, mVertexIn[index_w]);
  
  if (loadingScreen) loadingScreen->Update();

  descendQuadTree(i_mid,0,    i_s, dist, LOD_BLACK);
  addToOutput(i_s, mVertexIn[index_s]);
  
  if (loadingScreen) loadingScreen->Update();

  descendQuadTree(nx-1, i_mid,i_e, dist, LOD_BLACK);
  addToOutput(i_e, mVertexIn[index_e]);
  
  if (loadingScreen) loadingScreen->Update();

  descendQuadTree(i_mid,nx-1, i_n, dist, LOD_BLACK);
  addToOutput(i_n, mVertexIn[index_n]);

// final bits  
  mVertexIn[index_sw].r = 0;
  mVertexIn[index_sw].d = 0;
  mVertexIn[index_se].r = 0;
  mVertexIn[index_se].d = 0;
  mVertexIn[index_ne].r = 0;
  mVertexIn[index_ne].d = 0;
  mVertexIn[index_nw].r = 0;
  mVertexIn[index_nw].d = 0;
  
  addToOutput(i_sw, mVertexIn[index_sw]);
  addToOutput(i_se, mVertexIn[index_se]);
  addToOutput(i_ne, mVertexIn[index_ne]);
  addToOutput(i_nw, mVertexIn[index_nw]);

  assert(mMaxVertexOutIndex < mVertexOutSize);
  
  // now zap the r = -1 (improve compression)
  for (int i = 0 ; i <= mMaxVertexOutIndex; ++i)
  {
    if (mVertexOut[i].r < 0)
      mVertexOut[i].r = 0;
  }
  
}

void HeightfieldBuilder::initialiseDR(LoadingScreenHelper* loadingScreen)
{
  int i_mid = (nx-1)/2;
  
//    int index_sw = calcIndex( 0,    0    );
//    int index_se = calcIndex( nx-1, 0    );
//    int index_ne = calcIndex( nx-1, nx-1 );
//    int index_nw = calcIndex( 0,    nx-1 );
  
//    int index_c  = calcIndex( i_mid,i_mid);

  if (loadingScreen) loadingScreen->Update();
  doTriangle(i_mid, i_mid, 
              0, 0,
              nx-1, 0, level-1);
  if (loadingScreen) loadingScreen->Update();
  doTriangle(i_mid, i_mid, 
              nx-1, 0,
              nx-1, nx-1, level-1);
  if (loadingScreen) loadingScreen->Update();
  doTriangle(i_mid, i_mid, 
              nx-1, nx-1,
              0, nx-1, level-1);
  if (loadingScreen) loadingScreen->Update();
  doTriangle(i_mid, i_mid, 
              0, nx-1,
              0, 0, level-1);
}

void HeightfieldBuilder::doTriangle(int i1, int j1,
                      int i2, int j2,
                      int i3, int j3,
                      int lev)
{
// we are interested in calculating r and delta at v1. 
  
  Vertex & v1 = mVertexIn[calcIndex(i1, j1)];
  Vertex & v2 = mVertexIn[calcIndex(i2, j2)];
  Vertex & v3 = mVertexIn[calcIndex(i3, j3)];
  
  if (lev > 0)
  {
    // go down one lev..., then calculate the value at this point
    int i4 = (i2 + i3) / 2;
    int j4 = (j2 + j3) / 2;
    Vertex & v4 = mVertexIn[calcIndex(i4, j4)];
    
    // call this fn recursively
    doTriangle(i4, j4, i1, j1, i2, j2, lev-1);
    doTriangle(i4, j4, i3, j3, i1, j1, lev-1);

    // calculate the morphed z value of v4
    v4.z_m = (v2.z + v3.z)*0.5F;

    // having done that, as we are not a leaf node, calculate r and
    // delta based on the current location, and (for delta) compare
    // with the values from the children. Accept the highest. 
    
    // calculate delta
    float d;
    
    // i1, j1 are in the middle
    // construct i2', j2', which are the opposites of i2, j2 etc
    
    int i2p = i1 - (i2-i1);
    int j2p = j1 - (j2-j1);
    float z2p;
    if ( (i2p >= 0) && (i2p <= nx-1) && (j2p >= 0) && (j2p <= nx-1) )
      z2p = mVertexIn[calcIndex(i2p, j2p)].z;
    else
      z2p = v1.z - (v2.z - v1.z);
    
    int i3p = i1 - (i3-i1);
    int j3p = j1 - (j3-j1);
    float z3p;
    if ( (i3p >= 0) && (i3p <= nx-1) && (j3p >= 0) && (j3p <= nx-1) )
      z3p = mVertexIn[calcIndex(i3p, j3p)].z;
    else
      z3p = v1.z - (v3.z - v1.z);
    
    d = v1.z - (v2.z + v3.z + z2p + z3p) * 0.25f;


    if (mSimplifyUnderPlain && v1.z < mPlainAltitude)
      d = 0.0f; // pretend under plain is flat

    v1.d = fmax(v1.d, fabsf(d), v4.d); // current, new, child
    
    // and set r
    v1.r = fmax(v1.r, sqrtf((v4.x-v1.x) * (v4.x-v1.x) +
                            (v4.y-v1.y) * (v4.y-v1.y) +
                            (v4.z-v1.z) * (v4.z-v1.z)) + v4.r);
  }
  else
  {
    // got here, then we must be a leaf node. Calculate z from
    // adjacent values
    float d;
    if ( (i1 == 0) || (i1 == nx-1) )
      d = v1.z - (mVertexIn[calcIndex(i1, j1+1)].z + 
                  mVertexIn[calcIndex(i1, j1-1)].z) * 0.5f;
    else if ( (j1 == 0) || (j1 == nx-1) )
      d = v1.z - (mVertexIn[calcIndex(i1+1, j1)].z + 
                  mVertexIn[calcIndex(i1-1, j1)].z) * 0.5f;
    else 
    {
      d = v1.z - (mVertexIn[calcIndex(i1, j1+1)].z + 
                  mVertexIn[calcIndex(i1, j1-1)].z +
                  mVertexIn[calcIndex(i1+1, j1)].z + 
                  mVertexIn[calcIndex(i1-1, j1)].z   ) * 0.25f;
    }

    float d_offset = 0.0f;
    if (mCoastEnhancement > 0.0f && onSeaShore(i1, j1))
      d_offset = mCoastEnhancement;
    if (mSimplifyUnderPlain && v1.z < mPlainAltitude)
      d = 0.0f; // pretend under plain is flat
    // just set delta = d
    v1.d = d_offset + fabsf(d) ;
    // and r = 0;
    v1.r = 0;
  }
}

void HeightfieldBuilder::addToOutput(int i, const Vertex & vertex)
{
  if (i >= mVertexOutSize)
  {
    fprintf( stderr, "Error - index %d > %d\n", i, mVertexOutSize);
    exit(-1);
  }
  else if (i < 0)
  {
    fprintf( stderr, "Error - index %d < 0\n", i);
    exit(-1);
  }
  
  mMaxVertexOutIndex = fmax(mMaxVertexOutIndex, i);
  if (mVertexOut[i].r > -0.5f)
  {
    fprintf( stderr, "Error - entry for index %d already exists (%f, %f)\n", 
          i, 
          mVertexOut[i].x,
          mVertexOut[i].y );
    exit(-1);
  }

  int ind = mVertexOut[i].index;
  mVertexOut[i] = Vertex(vertex);
  mVertexOut[i].index = ind;
  mVertexOut[(int) (&vertex - &mVertexIn[0])].index = i;
}

void HeightfieldBuilder::getOutput(Vertex *& _mVertexOut, int & size) const
{
  assert(mVertexOut);
  _mVertexOut = mVertexOut;
  size = mMaxVertexOutIndex+1;
}


void HeightfieldBuilder::mDisplayOutput() const
{
  int i;
  int count = 0;
  printf("######## max index = %d #########\n", mMaxVertexOutIndex);
  for (i = 0 ; i <= mMaxVertexOutIndex; ++i)
  {
//    if (mVertexOut[i].r >= 0)
    {
      ++count;
      printf("%5d, %5.1f, %5.1f, %5.1f, (delta = %5.2f, r = %5.2f), index = %d\n", 
            i, 
            mVertexOut[i].x, mVertexOut[i].y, mVertexOut[i].z,
            mVertexOut[i].d, mVertexOut[i].r,
            mVertexOut[i].index);
    }
  }
  printf("#### storage efficiency = %d%% #####\n", 100*count/mMaxVertexOutIndex);
}

void HeightfieldBuilder::descendQuadTree(int i, int j, int pq, int dist, Colour colour)
{
  const int i_offset_white[] = {-1, 1, 1, -1};
  const int j_offset_white[] = {-1, -1, 1, 1};
  const int i_offset_black[] = {0, 1, 0, -1};
  const int j_offset_black[] = {1, 0, -1, 0};
  
  const int * i_offset;
  const int * j_offset;
  
  if (colour == LOD_WHITE)
  {
    i_offset = i_offset_white;
    j_offset = j_offset_white;
  }
  else
  {
    i_offset = i_offset_black;
    j_offset = j_offset_black;
  }
  
  for (int index = 0 ; index != 4 ; ++index)
  {
    int c0=4*pq + index + LOD_M;
    int i0 = i + i_offset[index] * dist;
    int j0 = j + j_offset[index] * dist;
    if (isValid(i0, j0))
    {
      if (dist/2 > 0)
      {
        descendQuadTree(i0, j0, c0, dist/2, colour);
      }
      addToOutput(c0, mVertexIn[calcIndex(i0, j0)]);
    }
  }
}

}


