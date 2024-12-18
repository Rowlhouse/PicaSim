#include "HeightfieldGenerator.h"

#include <cmath>
#include <cstdio>
#include <cassert>
#include <ctime>

//----------------------------------------------------------------------------------------------------------------------
#define Z(i, j) vertices[Heightfield::calcIndex(i, j, nx)].z

namespace Heightfield
{

inline float sqr(float f) {return f*f;}

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
      long k;
      float ans;
      
      m_seed ^= MASK;
      k = m_seed/IQ;
      m_seed = IA * (m_seed - k * IQ) - IR * k;
      if (m_seed < 0) m_seed += IM;
      ans = AM * (m_seed);
      m_seed ^= MASK;
      return ans;
    }

  float GetValue(float v1, float v2) 
    {
      return v1 + (v2-v1) * ( (float)GetValue() );
    }
  
private:
  long m_seed;
  const long IA, IM, IQ, IR, MASK;
  const float AM;
};

static RandomGenerator terrain_random(10);

//----------------------------------------------------------------------------------------------------------------------
void ZeroHeightfieldEdges(
  Heightfield::Vertex * vertices, 
  int nx,
  int n_offset,
  float edgeHeight)
{
  int i, j;
  for (int offset = 0 ; offset < n_offset ; ++offset)
  {
    float scale = (float) offset/n_offset;

    for (i = offset ; i < (nx-offset)-1 ; ++i)
    {
      {
        float& z = vertices[Heightfield::calcIndex(i, offset, nx)].z;
        z = edgeHeight + (z - edgeHeight) * scale;
      }
      {
        float& z = vertices[Heightfield::calcIndex(i+1, (nx-1)-offset, nx)].z;
        z = edgeHeight + (z - edgeHeight) * scale;
      }
    }

    for (j = offset ; j < (nx-offset)-1 ; ++j)
    {
      {
        float& z = vertices[Heightfield::calcIndex(offset, j+1, nx)].z *= scale;
        z = edgeHeight + (z - edgeHeight) * scale;
      }
      {
        float& z = vertices[Heightfield::calcIndex((nx-1)-offset, j, nx)].z *= scale;
        z = edgeHeight + (z - edgeHeight) * scale;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AddGaussianHill(Heightfield::Vertex* vertices, int nx, float xMid, float yMid, float radius, float height)
{
  float radius2 = radius * radius;
  for (int i = 0 ; i < nx ; ++i)
  {
    for (int j = 0 ; j < nx ; ++j)
    {
      int index = Heightfield::calcIndex(i, j, nx);
      float r2 = sqr(vertices[index].x - xMid) + sqr(vertices[index].y - yMid);
      float dz = height * expf(-r2 / radius2);
      vertices[index].z += dz;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GenerateRidgeHeightfield(Heightfield::Vertex * vertices, //!< modified on output
  int nx,
  float dx,
  float xMin, float yMin,
  float height, float heightClamp, float width, float edgeHeight,
  float horizontalVariation, float horizontalVariationWavelength, float verticalVariationFraction,
  LoadingScreenHelper* loadingScreen)
{  
  float xMax = xMin + nx * dx;
  float xMid = 0.5f * (xMin + xMax);

  float horWavelength1 = horizontalVariationWavelength;
  float horAmplitude1 = 2.0f;

  float horWavelength2 = horizontalVariationWavelength * 0.43f;
  float horAmplitude2 = 1.0f;

  float horWavelength3 = horizontalVariationWavelength * 0.17f;
  float horAmplitude3 = 0.25f;

  float totalAmplitude = horAmplitude1 + horAmplitude2 + horAmplitude3;

  int i, j;
  // Set the x and y values (not the z yet)
  for (i = 0 ; i < nx ; ++i)
  {
    if (loadingScreen && ((i % 10) == 0)) loadingScreen->Update("Generating terrain");
    for (j = 0 ; j < nx ; ++j)
    {
      float x = xMin + dx*i;
      float y = yMin + dx*j;

      float offset = 
        horAmplitude1 * sinf(2.0f * 3.142f * y/horWavelength1) + 
        horAmplitude2 * sinf(2.0f * 3.142f * y/horWavelength2) + 
        horAmplitude3 * sinf(2.0f * 3.142f * y/horWavelength3);
      float x0 = xMid + offset * horizontalVariation;

      float d = x - x0;

      float heightScale = (1.0f - verticalVariationFraction * offset / totalAmplitude);
      float z = heightScale * height * expf( -d*d/(width*width) );
      if (z > heightClamp * heightScale)
        z = heightClamp * heightScale;
      z += edgeHeight;
      assert(Heightfield::calcIndex(i, j, nx) < (nx*nx));
      vertices[Heightfield::calcIndex(i, j, nx)] = Heightfield::Vertex(x, y, z);;
    }
  }
  ZeroHeightfieldEdges(vertices, nx, 4, edgeHeight);
}

//----------------------------------------------------------------------------------------------------------------------
void SmoothHeightfield(
  Heightfield::Vertex * vertices, 
  int nx,
  float filter)
{
  int i, j;
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < nx ; ++j)
    {
      int i1 = (i + 1) % nx;
      int j1 = (j + 1) % nx;
      int i0 = (i + nx - 1) % nx;
      int j0 = (j + nx - 1) % nx;

      Z(i, j) = (1-filter) * Z(i, j) + 
        (Z(i0, j) + Z(i1, j) + Z(i, j0) + Z(i, j1))*0.25f * filter;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FlattenHeightfieldBelowHeight(
  Heightfield::Vertex * vertices, 
  int nx,
  float height,
  float slopeReduction)
{
  int i, j;
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < nx ; ++j)
    {
      float& z = Z(i, j);
      if (z > height)
        continue;
      float below = height - z;
      below *= slopeReduction;
      z = height - below;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void seedRand(int seed)
{
  if (seed == 0)
  {
    seed = (int) time( NULL );
    printf("Add builtin_terrain_seed = %d to sss.cfg to reproduce\n",
      seed);
  }
  else
  {
    printf("Using builtin_terrain_seed = %d (from sss.cfg)\n",seed);
  }

  terrain_random.SetSeed((long) seed);
}

//----------------------------------------------------------------------------------------------------------------------
void GenerateMidpointDisplacementHeightfield(
  Heightfield::Vertex* vertices, 
  int nx,
  float dx,
  float xmin, float ymin,
  float d_height,
  float rough,
  float edgeHeight,
  float upwardsBias,
  int filterIterations,
  int seed,
  LoadingScreenHelper* loadingScreen)
{
  printf("Calculating midpoint displacement terrain\n");
  seedRand(seed);

  float r = powf(2.0f,-1.0f*rough);

  if (loadingScreen) loadingScreen->Update("Generating terrain");
  int i, j;
  // Set the x and y values (not the z yet)
  for (i = 0 ; i < nx ; ++i)
  {
    for (j = 0 ; j < nx ; ++j)
    {
      float x = xmin + dx*i;
      float y = ymin + dx*j;
      assert(Heightfield::calcIndex(i, j, nx) < (nx*nx));
      vertices[Heightfield::calcIndex(i, j, nx)] = Heightfield::Vertex(x, y, edgeHeight);
    }
  }

  int size = nx-1;
  int rect_size = size;

  float fracLeft = 1.0f;

  int loadingScreenCounter = 0;

  while (rect_size > 0)
  {
    // diamond step
    for (i = 0 ; i < size ; i += rect_size)
    {
      if (loadingScreen && (loadingScreenCounter++ % 16) == 0) loadingScreen->Update();
      for (j = 0 ; j < size ; j += rect_size)
      {
        int ni = (i + rect_size) % nx;
        int nj = (j + rect_size) % nx;

        int mi = (i + rect_size/2);
        int mj = (j + rect_size/2);        

        // ensure that we go up in the middle
        float offset;
        offset = d_height * upwardsBias + terrain_random.GetValue(-d_height, d_height);

        Z(mi, mj) = ( Z(i, j) + Z(ni, j) + Z(i, nj) + Z(ni, nj) ) * 0.25f + 
          offset;
      }
    }

    if (loadingScreen) loadingScreen->Update();
    // square step. Note that we have to be more careful about the
    // boundary conditions.

    for (i = 0 ; i < size ; i += rect_size)
    {
      if (loadingScreen && (loadingScreenCounter++ % 16) == 0) loadingScreen->Update();
      for (j = 0 ; j < size ; j += rect_size)
      {
        int ni = (i + rect_size) % nx;
        int nj = (j + rect_size) % nx;

        int mi = (i + rect_size/2);
        int mj = (j + rect_size/2);

        int pmi = (i-rect_size/2 + nx) % nx;
        int pmj = (j-rect_size/2 + nx) % nx;

        /*
        Calculate the square value for the top side of the rectangle
        */
        Z(mi, j) = ( Z(i, j) + Z(ni, j) + Z(mi, pmj) + Z(mi, mj) ) * 0.25f +
          terrain_random.GetValue(-d_height, d_height);

        /*
        Calculate the square value for the left side of the rectangle
        */
        Z(i, mj) = ( Z(i, j) + Z(i, nj) + Z(pmi, mj) + Z(mi, mj) ) * 0.25f +
          terrain_random.GetValue(-d_height, d_height);
      }
    }

    rect_size /= 2;
    d_height *= r;
  }

  if (loadingScreen) loadingScreen->Update();

  ZeroHeightfieldEdges(vertices, nx, nx / 16, edgeHeight);

  for (i = 0 ; i < filterIterations ; ++i)
  {
    if (loadingScreen) loadingScreen->Update();
    SmoothHeightfield(vertices, nx, 0.5f);
  }

  if (loadingScreen) loadingScreen->Update();

  ZeroHeightfieldEdges(vertices, nx, 4, edgeHeight);
  printf("Done calculating terrain\n");
}

//----------------------------------------------------------------------------------------------------------------------
bool LoadHeightfield(
  Heightfield::Vertex* vertices,
  int nx,
  float dx,
  float xMin, float yMin,
  float zMin, float zMax,
  const char* heightmapFile,
  LoadingScreenHelper* loadingScreen)
{
  printf("Reading raw data file: %s\n", heightmapFile);

  FILE * dataFile = fopen(heightmapFile, "rb");
  if (dataFile == 0)
  {
    printf("Unable to open raw data file: %s\n", heightmapFile);
    return false;
  }

  for (int j = 0 ; j < nx ; ++j)
  {
    for (int i = 0 ; i < nx ; ++i)
    {
      unsigned short val;
      size_t num = fread(&val, sizeof(val), 1, dataFile);
      if (num == 0)
      {
        printf("Error reading raw file: i = %d, j = %d\n", i, j);
        fclose(dataFile);
        return false;
      }
      float z = zMin + (zMax - zMin) * val / (256*256 - 1);
      float x = xMin + i * dx;
      float y = yMin + j * dx;
      vertices[Heightfield::calcIndex(i, j, nx)] = Heightfield::Vertex(x, y, z);
    }
  }
  fclose(dataFile);
  return true;
}


}
