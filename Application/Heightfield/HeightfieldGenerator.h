#ifndef HEIGHTFIELD_GENERATOR_H
#define HEIGHTFIELD_GENERATOR_H

#include "HeightfieldCommon.h"
#include "LoadingScreenHelper.h"

namespace Heightfield
{

// Load a heightfield from file, returning the sizes etc
bool LoadHeightfield(
  Heightfield::Vertex * vertices,
  int nx,
  float dx,
  float xMin, float yMin,
  float zMin, float zMax,
  const char* heightmapFile,
  LoadingScreenHelper* loadingScreen);

/// Generate a square heightfield with a Gaussian ridge across the middle
void GenerateRidgeHeightfield(
  Heightfield::Vertex* vertices,
  int nx, float dx,
  float xMin, float yMin,
  float height, float heightClamp, float width, float edgeHeight,
  float horVariation, float horVariationWavelength, float verticalVariation,
  LoadingScreenHelper* loadingScreen);

//! Creates a terrain based on mid-point displacement. On entry,
//! vertices should be allocated.
void GenerateMidpointDisplacementHeightfield(
  Heightfield::Vertex * vertices, 
  int nx, float dx,
  float xmin, float ymin,
  float d_height,
  float rough,
  float edgeHeight,
  float upwardsBias,
  int filterIterations,
  int seed,
  LoadingScreenHelper* loadingScreen);

/// Helper function to smooth the heightfield. amount should be between 0 and 1
void SmoothHeightfield(
  Heightfield::Vertex * vertices, 
  int nx,
  float filter);

/// Helper function to blend the plain so that it slopes more slowly below height
void FlattenHeightfieldBelowHeight(
  Heightfield::Vertex * vertices, 
  int nx,
  float height,
  float slopeReduction);


/// Smoothly blends the edges towards zero over n_offset samples
void ZeroHeightfieldEdges(
  Heightfield::Vertex * vertices, 
  int nx,
  int n_offset,
  float edgeHeight);

// Adds a bulge
void AddGaussianHill(Heightfield::Vertex* vertices, int nx, float xMid, float yMid, float radius, float height);

}
#endif