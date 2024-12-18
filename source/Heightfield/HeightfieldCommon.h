#ifndef HEIGHTFIELD_COMMON_H
#define HEIGHTFIELD_COMMON_H

namespace Heightfield
{
  // helper fn for user to calculate indices in the input array
  inline int calcIndex(int i, int j, int nx) { return (i + j * nx); }

  //! An element of the HeightfieldBuilder structure
  struct Vertex
  {
    Vertex(float x, float y, float z, float z_m = -100) 
      : x(x), y(y), z(z), z_m(z_m), d(0), r(-1), index(-1) {};

    Vertex() : x(0), y(0), z(0), z_m(-100), d(0), r(-1), index(-1) {};

    //! z_m is the morphed z value
    float x, y, z, z_m;
    // the following are populated by HeightfieldBuilder
    float d, r;
    // index contains the 1D index that you're looking for if you look
    // in calcIndex(i, j, nx)
    int index; 
  };
}

#endif