#include "Terrain.h"
// #include "PicaSim.h"
// #include "ShaderManager.h"
// #include "Shaders.h"

// #include "HeightfieldRuntime.h"
// #include "HeightfieldBuilder.h"
// #include "HeightfieldGenerator.h"

// #include <IwUtil.h>
// #include "Menus/PicaDialog.h"


//----------------------------------------------------------------------------------------------------------------------
Terrain::Terrain()
{
  mHeightfield = 0;
  mVertexOut = 0;
  mTerrainVertexBuffer = 0;
  mPlainRigidBody = 0;
  mPlainCollisionShape = 0;
  mHeightfieldTerrainShape = 0;
  mHeightfieldRigidBody = 0;
  mDynamicsWorld = 0;
}

//----------------------------------------------------------------------------------------------------------------------
Terrain::~Terrain()
{
  if (mTerrainVertexBuffer)
    glDeleteBuffers(1, &mTerrainVertexBuffer);
  mTerrainVertexBuffer = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::GetIndex(
  float x0, float y0,                       
  unsigned & i11, unsigned & j11) const
{  
  const float cellSize = mHeightfield->getCellSize();
  // i11 is just less than x0
  i11 = (int) ( (x0 - mHeightfield->sw().x) / cellSize );
  j11 = (int) ( (y0 - mHeightfield->sw().y) / cellSize );
  
  if ((int) i11 > (mHeightfield->getSize()-2))
  {
    i11 = mHeightfield->getSize()-2;
  }
  else if ((int) i11 < 0)
  {
    i11 = 0;
  }
  
  if ((int) j11 > (mHeightfield->getSize()-2))
  {
    j11 = mHeightfield->getSize()-2;
  }
  else if ((int) j11 < 0)
  {
    j11 = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// Interpolates within a quad to return the height, dividing the quad
/// into two triangles. Returns the interpolated height.
/// This function comes high on the list with gprof
float interpolateForZ(
  Vector3 pos,
  const Vector3& pos11,
  const Vector3& pos12,
  const Vector3& pos21,
  const Vector3& pos22,
  int downwardSlope) // direction of the diagonal
{
  // P = c0*P0 + c1*P1 + c2*P2
  // arrange it so that at the end we interpolate over a triangle:
  //    /| P2
  //   / |   
  //  /__|   
  // P1   P0 
  
  // make sure pos is within triangle
  if (pos[0] < pos11[0])
    pos[0] = pos11[0];
  else if (pos[0] > pos22[0])
    pos[0] = pos22[0];
  if (pos[1] < pos11[1])
    pos[1] = pos11[1];
  else if (pos[1] > pos22[1])
    pos[1] = pos22[1];
  
  const Vector3 * P0;
  const Vector3 * P1;
  const Vector3 * P2;
  
  if (downwardSlope)
  {
    if ( (pos[0] - pos11[0]) > (pos12[1] - pos[1]) )
    {
      // right hand side \|
      P0 = &pos22;
      P1 = &pos21;
      P2 = &pos12;
    }
    else
    {
      // left hand side
      P0 = &pos11;
      P1 = &pos12;
      P2 = &pos21;
    }
  }
  else
  {
    if ( (pos[0] - pos11[0]) > (pos[1] - pos11[1]) )
    {
      // right hand side /|
      P0 = &pos21;
      P1 = &pos11;
      P2 = &pos22;
    }
    else
    {
      // left hand side
      P0 = &pos12;
      P1 = &pos22;
      P2 = &pos11;
    }
  }
    
  const float E1x = (*P1)[0] - (*P0)[0];
  const float E1y = (*P1)[1] - (*P0)[1];
  
  const float E2x = (*P2)[0] - (*P0)[0];
  const float E2y = (*P2)[1] - (*P0)[1];

  const float Ex = pos[0] - (*P0)[0];
  const float Ey = pos[1] - (*P0)[1];
  
  const float e11 = E1x * E1x + E1y * E1y; //dot(E1, E1);
  const float e12 = E1x * E2x + E1y * E2y; //dot(E1, E2);
  const float e22 = E2x * E2x + E2y * E2y; //dot(E2, E2);
  const float inv_delta = 1.0f/(e11*e22 - e12*e12);
  
  const float p1 = Ex * E1x + Ey * E1y; //dot(E, E1);
  const float p2 = Ex * E2x + Ey * E2y; //dot(E, E2);
  
  const float c1 = (e22*p1 - e12*p2) * inv_delta;
  const float c2 = (e11*p2 - e12*p1) * inv_delta;
  const float c0 = 1.0f - c1 - c2;
  
  return c0 * (*P0)[2] + c1 * (*P1)[2] + c2 * (*P2)[2];
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::GetLocalTerrain(const Vector3 & pos,
                                    Vector3 & terrain_pos,
                                    Vector3 & normal, 
                                    bool clampToPlain) const
{
  unsigned i11, j11, i22, j22;
  unsigned i12, j12, i21, j21;
  
  float x0 = pos.x;
  float y0 = pos.y;
  
  GetIndex(x0, y0, i11, j11);
  
  i12 = i11;
  i21 = i11 + 1;
  i22 = i11 + 1;
  
  j21 = j11;
  j12 = j11 + 1;
  j22 = j11 + 1;
  
  //IwAssert(ROWLHOUSE,  ((int) i11 < mHeightfield->getSize()) &&
  //         ((int) i21 < mHeightfield->getSize()) &&
  //         ((int) i12 < mHeightfield->getSize()) &&
  //         ((int) i22 < mHeightfield->getSize()) &&
  //         ((int) j11 < mHeightfield->getSize()) &&
  //         ((int) j21 < mHeightfield->getSize()) &&
  //         ((int) j12 < mHeightfield->getSize()) &&
  //         ((int) j22 < mHeightfield->getSize()) );
  
  Vector3 pos11 = mHeightfield->getPos(i11, j11);
  Vector3 pos12 = mHeightfield->getPos(i12, j12);
  Vector3 pos21 = mHeightfield->getPos(i21, j21);
  Vector3 pos22 = mHeightfield->getPos(i22, j22);
  
  const float plainHeight = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  if (clampToPlain)
  {
    if (pos11.z < plainHeight)
      pos11.z = plainHeight;
    if (pos12.z < plainHeight)
      pos12.z = plainHeight;
    if (pos21.z < plainHeight)
      pos21.z = plainHeight;
    if (pos22.z < plainHeight)
      pos22.z = plainHeight;
  }

  int downwardSlope = (i11 + j11) % 2;  
  float interpolatedHeight = interpolateForZ(pos, 
                                     pos11,
                                     pos12, 
                                     pos21,
                                     pos22,
                                     downwardSlope);
  
  // We've calculated the terrain point, at least for now. We could do
  // better by recalculating this using the surface normal.
  terrain_pos = pos;
  terrain_pos[2] = interpolatedHeight;
  
  normal = ( (pos22-pos11).Cross(pos12 - pos21) ).GetNormalised();
}

//----------------------------------------------------------------------------------------------------------------------
float Terrain::GetTerrainHeightFast(float x0, float y0, bool clampToPlain) const
{
  unsigned i11, j11, i22, j22;
  unsigned i12, j12, i21, j21;
  
  GetIndex(x0, y0, i11, j11);
  
  i12 = i11;
  i21 = i11 + 1;
  i22 = i11 + 1;
  
  j21 = j11;
  j12 = j11 + 1;
  j22 = j11 + 1;
  
  //IwAssert(ROWLHOUSE,  
  //  ((int) i11 < mHeightfield->getSize()) &&
  //  ((int) i21 < mHeightfield->getSize()) &&
  //  ((int) i12 < mHeightfield->getSize()) &&
  //  ((int) i22 < mHeightfield->getSize()) &&
  //  ((int) j11 < mHeightfield->getSize()) &&
  //  ((int) j21 < mHeightfield->getSize()) &&
  //  ((int) j12 < mHeightfield->getSize()) &&
  //  ((int) j22 < mHeightfield->getSize()) );
  
  Vector3 pos11 = mHeightfield->getPos(i11, j11);
  float z11 = pos11.z;
  float z12 = mHeightfield->getHeight(i12, j12);
  float z21 = mHeightfield->getHeight(i21, j21);
  float z22 = mHeightfield->getHeight(i22, j22);
  
  const float plainHeight = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  if (clampToPlain)
  {
    if (z11 < plainHeight)
      z11 = plainHeight;
    if (z12 < plainHeight)
      z12 = plainHeight;
    if (z21 < plainHeight)
      z21 = plainHeight;
    if (z22 < plainHeight)
      z22 = plainHeight;
  }

  float i = ClampToRange((x0 - pos11.x) / mHeightfield->getCellSize(), 0.0f, 1.0f);
  float j = ClampToRange((y0 - pos11.y) / mHeightfield->getCellSize(), 0.0f, 1.0f);

  float z1 = z11 + i * (z21 - z11);
  float z2 = z12 + i * (z22 - z12);

  float z = z1 + j * (z2 - z1);
  return z;
}


//----------------------------------------------------------------------------------------------------------------------
float Terrain::GetTerrainHeight(float x0, float y0, bool clampToPlain) const
{
  unsigned i11, j11, i22, j22;
  unsigned i12, j12, i21, j21;
  
  GetIndex(x0, y0, i11, j11);
  
  i12 = i11;
  i21 = i11 + 1;
  i22 = i11 + 1;
  
  j21 = j11;
  j12 = j11 + 1;
  j22 = j11 + 1;
  
  //IwAssert(ROWLHOUSE,  
  //  ((int) i11 < mHeightfield->getSize()) &&
  //  ((int) i21 < mHeightfield->getSize()) &&
  //  ((int) i12 < mHeightfield->getSize()) &&
  //  ((int) i22 < mHeightfield->getSize()) &&
  //  ((int) j11 < mHeightfield->getSize()) &&
  //  ((int) j21 < mHeightfield->getSize()) &&
  //  ((int) j12 < mHeightfield->getSize()) &&
  //  ((int) j22 < mHeightfield->getSize()) );
  
  Vector3 pos11 = mHeightfield->getPos(i11, j11);
  Vector3 pos12 = mHeightfield->getPos(i12, j12);
  Vector3 pos21 = mHeightfield->getPos(i21, j21);
  Vector3 pos22 = mHeightfield->getPos(i22, j22);
  
  const float plainHeight = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  if (clampToPlain)
  {
    if (pos11.z < plainHeight)
      pos11.z = plainHeight;
    if (pos12.z < plainHeight)
      pos12.z = plainHeight;
    if (pos21.z < plainHeight)
      pos21.z = plainHeight;
    if (pos22.z < plainHeight)
      pos22.z = plainHeight;
  }

  int downwardSlope = (i11 + j11) % 2;
  return interpolateForZ(Vector3(x0, y0, 0), 
                           pos11,
                           pos12, 
                           pos21,
                           pos22,
                           downwardSlope);
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::GetTerrainHeight(Vector3 & pos, bool clampToPlain) const
{
  pos[2] = GetTerrainHeight(pos[0], pos[1], clampToPlain);
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::GetTerrainHeightFast(Vector3 & pos, bool clampToPlain) const
{
  pos[2] = GetTerrainHeightFast(pos[0], pos[1], clampToPlain);
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::GetTerrainRange(float& xMin, float& xMax, float& yMin, float& yMax) const
{
  xMin = mHeightfield->sw().x;
  xMax = mHeightfield->se().x;
  yMin = mHeightfield->sw().y;
  yMax = mHeightfield->nw().y;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 Terrain::GetTerrainMidpoint(bool clampToPlain) const
{
  Vector3 pos(mHeightfield->c().x, mHeightfield->c().y, mHeightfield->c().z);
  GetTerrainHeight(pos, clampToPlain);
  return pos;
}


//----------------------------------------------------------------------------------------------------------------------
static unsigned calcRGBTextureIndex(int i, int j, int w, int /*h*/) {return (i + w * j)*3;}

//----------------------------------------------------------------------------------------------------------------------
static void GetRGB(Vector3& rgb, uint i, uint j, uint size, const CIwImage& img)
{
  rgb.x = img.GetTexels()[(j*size + i)*3 + 2] / 255.0f; 
  rgb.y = img.GetTexels()[(j*size + i)*3 + 1] / 255.0f; 
  rgb.z = img.GetTexels()[(j*size + i)*3 + 0] / 255.0f; 
}

//----------------------------------------------------------------------------------------------------------------------
static void GetInterpolatedRGB(
  Vector3& rgb,
  uint i, uint j, uint size,
  float textureScaleX, float textureScaleY,
  const CIwImage& img)
{
  float fi = (i * textureScaleX) - (int) (i * textureScaleX);
  float fj = (j * textureScaleY) - (int) (j * textureScaleY);
  int i00 = ((int) (i * textureScaleX)) % size;
  int j00 = ((int) (j * textureScaleY)) % size;
  int i11 = (i00+1) % size;
  int j11 = (j00+1) % size;
  int i01 = i00;
  int j10 = j00;
  int i10 = i11;
  int j01 = j11;

  Vector3 rgb00, rgb10, rgb01, rgb11;
  GetRGB(rgb00, i00, j00, size, img);
  GetRGB(rgb01, i01, j01, size, img);
  GetRGB(rgb10, i10, j10, size, img);
  GetRGB(rgb11, i11, j11, size, img);

  Vector3 rgbx0 = rgb00 * (1.0f-fi) + rgb10 * fi;
  Vector3 rgbx1 = rgb01 * (1.0f-fi) + rgb11 * fi;

  rgb = rgbx0 * (1.0f-fj) + rgbx1 * fj;
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::CalculateLightmapTexture(unsigned int size, float gamma, const char* basicTexture, Vector3& plainColour, LoadingScreenHelper* loadingScreen)
{
  TRACE_FILE_IF(1) TRACE("Calculating lightmap...");
  CIwImage origImg;
  origImg.LoadFromFile(basicTexture);
  if (origImg.GetWidth() == 0)
  {
    TRACE_FILE_IF(1) TRACE("Failed to load %s", basicTexture);
    return;
  }

  if (loadingScreen) loadingScreen->Update("Processing terrain lightmap");

  GLint maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

  if (size > (unsigned int) maxTextureSize)
    size = maxTextureSize;

  CIwImage img;
  img.SetFormat(CIwImage::RGB_888);
  img.SetHeight(size);
  img.SetWidth(size);
  origImg.ConvertToImage(&img);

  GLubyte* image = new GLubyte[size*size*3]; // RGB
  float ambientMat = 1.0f;
  float diffuseMat = 1.0f;

  Vector3 dirToLight = -RenderManager::GetInstance().GetLightingDirection().GetNormalised();
  Vector3 ambientColour = RenderManager::GetInstance().GetLightingAmbientColour() * ambientMat;
  Vector3 diffuseColour = RenderManager::GetInstance().GetLightingDiffuseColour() * diffuseMat;

  const TerrainSettings& ts = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings;

  const float plainHeight = ts.mPlainHeight;
  int n = mHeightfield->getSize();
  Vector3 pos00 = mHeightfield->getPos(0, 0);
  Vector3 posnn = mHeightfield->getPos(n-1, n-1);
  Vector3 pos11 = mHeightfield->getPos(1, 1);
  
  float xmin = pos00[0];
  float ymin = pos00[1];
  float xmax = posnn[0];
  float ymax = posnn[1];
  float dx = pos11[0] - pos00[0];
  float dy = pos11[1] - pos00[1];
  
  dx *= 2.0f; // Smooth out the artefacts due to tesselation
  dy *= 2.0f;
  
  const float beachDeltaZ = 3.0f;
  const Vector3 beachRGB(ts.mBeachColourR, ts.mBeachColourG, ts.mBeachColourB);

#ifdef DO_SNOW
  const float snowDeltaZ = 30.0f;
  const float snowHeight = 100.0f;
  const float snowR = 1.0f;
  const float snowG = 1.0f;
  const float snowB = 1.0f;
#endif

  const float waterDeltaZ = 1.0f;
  const Vector3 waterRGB(ts.mPlainColourR, ts.mPlainColourG, ts.mPlainColourB);

  const float plainOffset = 3.0f;

  const float basicTextureScaleX = ts.mTerrainTextureScaleX;
  const float basicTextureScaleY = ts.mTerrainTextureScaleY;

  float waterLightDot = Maximum(dirToLight.Dot(Vector3(0,0,1)), 0.0f);

  unsigned int i, j;
  for (i = 0 ; i < size ; ++i)
  {
    if (i % (size/32) == 0)
      if (loadingScreen) loadingScreen->Update();
    float x = xmin + i * (xmax-xmin) / (size-1);
    for (j = 0 ; j < size ; ++j)
    {
      Vector3 normal;
      float y = ymin + j * (ymax-ymin) / (size-1);
      Vector3 pos_w(x - dx, y, 0);
      Vector3 pos_e(x + dx, y, 0);
      Vector3 pos_s(x, y - dy, 0);
      Vector3 pos_n(x, y + dy, 0);
      Vector3 pos(x, y, 0);
      GetTerrainHeightFast(pos_w, false);
      GetTerrainHeightFast(pos_e, false);
      GetTerrainHeightFast(pos_s, false);
      GetTerrainHeightFast(pos_n, false);
      GetTerrainHeightFast(pos, false);
      normal = ((pos_e - pos_w).Cross(pos_n - pos_s)).GetNormalised();
      // lightmap is the dot product of the normal and the sun dir
      float lightDot = Maximum(dirToLight.Dot(normal), 0.0f);

      lightDot *= (normal.z);

      float averageZ = 0.25f * (pos_w.z + pos_e.z + pos_s.z + pos_n.z);
      float curvature = (averageZ - pos.z) / dx;
      if (curvature > 0.0f)
      {
        curvature *= 0.0f;
        lightDot *= 1.0f - curvature;
      }

      Vector3 rgb;
      GetInterpolatedRGB(rgb, i, j, size, basicTextureScaleX, basicTextureScaleY, img);

      // Blend to a beach colour
      float beachColourFrac = 1.0f - (pos.z - (plainHeight+plainOffset)) / beachDeltaZ;
      beachColourFrac = ClampToRange(beachColourFrac, 0.0f, 1.0f);
      rgb = rgb * (1 - beachColourFrac) + beachRGB * beachColourFrac;

      // blend to a water colour
      float waterColourFrac = ((plainHeight+plainOffset) - pos.z) / waterDeltaZ;
      waterColourFrac = ClampToRange(waterColourFrac, 0.0f, 1.0f);
      rgb = rgb * (1 - waterColourFrac) + waterRGB * waterColourFrac;
      lightDot = lightDot * (1 - waterColourFrac) + waterLightDot * waterColourFrac;

#if 0
      image[calcRGBTextureIndex(i, j, size, size) + 0] = (GLuint) (tb * 255);
      image[calcRGBTextureIndex(i, j, size, size) + 1] = (GLuint) (tg * 255);
      image[calcRGBTextureIndex(i, j, size, size) + 2] = (GLuint) (tr * 255);
      continue;
#endif

#ifdef DO_SNOW
      // Blend to a snow colour
      float snowColourFrac = (pos.z - snowHeight) / snowDeltaZ;
      snowColourFrac = ClampToRange(snowColourFrac, 0.0f, 1.0f);
      float threshold = 0.8f;
      snowColourFrac *= lightDot < threshold ? 1.0f : Square(1.0f - (lightDot-threshold) / (1.0f - threshold));
      snowColourFrac *= normal.z * normal.z * normal.z;
      tr = tr * (1 - snowColourFrac) + snowR * snowColourFrac;
      tg = tg * (1 - snowColourFrac) + snowG * snowColourFrac;
      tb = tb * (1 - snowColourFrac) + snowB * snowColourFrac;
#endif

      float r = Minimum(lightDot * diffuseColour.x + ambientColour.x, 0.9999f);
      float g = Minimum(lightDot * diffuseColour.y + ambientColour.y, 0.9999f);
      float b = Minimum(lightDot * diffuseColour.z + ambientColour.z, 0.9999f);

      r *= rgb.x;
      g *= rgb.y;
      b *= rgb.z;

      r = powf(r, gamma);
      g = powf(g, gamma);
      b = powf(b, gamma);

      image[calcRGBTextureIndex(i, j, size, size) + 0] = (GLuint) (b * 255);
      image[calcRGBTextureIndex(i, j, size, size) + 1] = (GLuint) (g * 255);
      image[calcRGBTextureIndex(i, j, size, size) + 2] = (GLuint) (r * 255);
    }
  }
  
  mHeightfieldTexture.CopyFromBuffer(size, size, CIwImage::RGB_888, size * 3, image, 0);
  mHeightfieldTexture.Upload();
  TRACE_FILE_IF(1) TRACE("Uploaded heightfield texture id %d", mHeightfieldTexture.m_HWID);
  delete [] image;
  TRACE_FILE_IF(1) TRACE("done\n");

  // Also calculate the plain colour
  float r = Minimum(waterLightDot * diffuseColour.x + ambientColour.x, 0.9999f);
  float g = Minimum(waterLightDot * diffuseColour.y + ambientColour.y, 0.9999f);
  float b = Minimum(waterLightDot * diffuseColour.z + ambientColour.z, 0.9999f);

  plainColour.x = r * ts.mPlainColourR;
  plainColour.y = g * ts.mPlainColourG;
  plainColour.z = b * ts.mPlainColourB;

  plainColour.x = powf(plainColour.x, gamma);
  plainColour.y = powf(plainColour.y, gamma);
  plainColour.z = powf(plainColour.z, gamma);

}


//----------------------------------------------------------------------------------------------------------------------
void Terrain::Init(btDynamicsWorld& dynamicsWorld, LoadingScreenHelper* loadingScreen, uint32& checksum)
{
  TRACE_METHOD_ONLY(1);
  mLastLOD = 0;
  mLastCameraPosition = Vector3(0,0,0);

  mDynamicsWorld = &dynamicsWorld;
  const TerrainSettings& ts = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings;
  const LightingSettings& ls = PicaSim::GetInstance().GetSettings().mLightingSettings;
  const Options& options = PicaSim::GetInstance().GetSettings().mOptions;

  int n = 0;
  int nx = 0;
  Heightfield::Vertex * vertices = 0;
  float xMin = 0.0f;

  // Use terrain building heap
  s3eMemorySetInt(S3E_MEMORY_HEAP, 1);

  if (ts.mType == TerrainSettings::TYPE_PANORAMA || ts.mType == TerrainSettings::TYPE_PANORAMA_3D)
  {
    std::string panoramaFile = ts.mPanoramaName + "/Panorama.xml";
    GetFileChecksum(checksum, panoramaFile.c_str());
    TiXmlDocument doc(panoramaFile);
    bool success = doc.LoadFile();
    IwAssert(ROWLHOUSE, success);

    TiXmlHandle docHandle(&doc);
    TiXmlElement* element = docHandle.FirstChild("Panorama").ToElement();
    IwAssert(ROWLHOUSE, element);

    readFromXML(element, "nx", nx);
    readFromXML(element, "dx", mHeightfieldBoxSize);
    float minZ = readFloatFromXML(element, "minZ");
    float maxZ = readFloatFromXML(element, "maxZ");
    std::string heightmapFile = readStringFromXML(element, "heightmapFile");
    GetFileChecksum(checksum, heightmapFile.c_str());

    vertices = new Heightfield::Vertex[nx*nx];

    float terrainSize = (nx - 1) * mHeightfieldBoxSize;
    xMin = -0.5f * terrainSize;

    // Override certain things only if it's a true panorama
    if (ts.mType == TerrainSettings::TYPE_PANORAMA)
    {
      float windBearing = readFloatFromXML(element, "windBearing");
      Vector3 cameraPos = readVector3FromXML(element, "cameraPos");
      float plainHeight = readFloatFromXML(element, "plainHeight");
      PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mWindBearing = windBearing;
      PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight = plainHeight;
      PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mObserverPosition = cameraPos + Vector3(xMin, xMin, 0.0f);
    }

    n = (int)floor(2 * (log(nx - 1.0) / log(2.0)) + 0.5);
    IwAssert(ROWLHOUSE, nx == (int)floor(pow(2.0, (n / 2.0)) + 1.5));

    Heightfield::LoadHeightfield(
      vertices,
      nx,
      mHeightfieldBoxSize,
      xMin, xMin,
      minZ, maxZ,
      heightmapFile.c_str(),
      loadingScreen);
  }
  else
  {
    int terrainDetail = ts.mHeightmapDetail;
    nx = 1;
    for (int i = 0; i < terrainDetail; ++i)
      nx *= 2;
    nx += 1;

    // terrain
    n = (int)floor(2 * (log(nx - 1.0) / log(2.0)) + 0.5);
    IwAssert(ROWLHOUSE, nx == (int)floor(pow(2.0, (n / 2.0)) + 1.5));

    mHeightfieldBoxSize = ts.mTerrainSize / (nx - 1);

    // Make the heightfield origin by (0,0) so that it's simple to use the vertex position as a texture coordinate with simple scaling
    xMin = -0.5f * ts.mTerrainSize;

    // create the terrain on a regular grid

    vertices = new Heightfield::Vertex[nx*nx];

    TRACE_FILE_IF(1) TRACE("Calculating terrain...");
    if (ts.mType == TerrainSettings::TYPE_MIDPOINT_DISPLACEMENT)
    {
      float midpointDisplacementHeight = ts.mMidpointDisplacementHeight;
      float midpointDisplacementRoughness = ts.mMidpointDisplacementRoughness;
      float midpointDisplacementEdgeHeight = ts.mMidpointDisplacementEdgeHeight;
      float midpointDisplacementUpwardsBias = ts.mMidpointDisplacementUpwardsBias;
      int midpointDisplacementFilterIterations = ts.mMidpointDisplacementFilterIterations;
      int midpointDisplacementSeed = ts.mMidpointDisplacementSeed;

      Heightfield::GenerateMidpointDisplacementHeightfield(
        vertices,
        nx,
        mHeightfieldBoxSize,
        xMin, xMin,
        midpointDisplacementHeight * (nx / 65.0f) * (mHeightfieldBoxSize / 10),
        midpointDisplacementRoughness,
        midpointDisplacementEdgeHeight,
        midpointDisplacementUpwardsBias,
        midpointDisplacementFilterIterations,
        midpointDisplacementSeed,
        loadingScreen);

      float beachHeight = ts.mPlainHeight + 5.0f;
      float slopeReduction = 0.15f;
      Heightfield::FlattenHeightfieldBelowHeight(
        vertices, nx, beachHeight, slopeReduction);
    }
    else if (ts.mType == TerrainSettings::TYPE_RIDGE)
    {
      Heightfield::GenerateRidgeHeightfield(
        vertices,
        nx,
        mHeightfieldBoxSize,
        xMin, xMin,
        ts.mRidgeHeight, ts.mRidgeHeight * ts.mRidgeMaxHeightFraction,
        ts.mRidgeWidth, ts.mRidgeEdgeHeight,
        ts.mRidgeHorizontalVariation, ts.mRidgeHorizontalVariationWavelength, ts.mRidgeVerticalVariationFraction,
        loadingScreen);
    }
    else if (ts.mType == TerrainSettings::TYPE_FILE_TERRAIN)
    {
      CIwImage image;

      std::string filename = ts.mFileTerrainName + "/image.jpg";
      GetFileChecksum(checksum, filename.c_str());

      image.LoadFromFile(filename.c_str());

      int iW = (int)image.GetWidth();
      int iH = (int)image.GetHeight();

      if (iW != 0 && iH != 0)
      {
        for (int i = 0; i != nx; ++i)
        {
          float x = xMin + i * mHeightfieldBoxSize;
          for (int j = 0; j != nx; ++j)
          {
            float y = xMin + j * mHeightfieldBoxSize;
            int ii = nx - (1 + i);

            int imageI = (int)((float(ii) / nx) * iW);
            int imageJ = (int)((float(j) / nx) * iH);

            float tr = image.GetTexels()[(imageJ*iW + imageI) * 3 + 2] / 255.0f;
            float tg = image.GetTexels()[(imageJ*iW + imageI) * 3 + 1] / 255.0f;
            float tb = image.GetTexels()[(imageJ*iW + imageI) * 3 + 0] / 255.0f;

            float f = (tr + tg + tb) / 3.0f;

            float z = ts.mFileTerrainMinZ + (ts.mFileTerrainMaxZ - ts.mFileTerrainMinZ) * f;
            vertices[Heightfield::calcIndex(i, j, nx)] = Heightfield::Vertex(x, y, z);;
          }
        }
      }
      else
      {
        const Language language = PicaSim::GetInstance().GetSettings().mOptions.mLanguage;
        ShowDialog("PicaSim", "Failed to load terrain file", TXT(PS_OK));
      }

    }
    else
    {
      IwAssertMsg(ROWLHOUSE, false, ("Failed to find a valid terrain type"));
    }
    TRACE_FILE_IF(1) TRACE("done\n");
  }

  TRACE_FILE_IF(1) TRACE("Processing terrain...");
  loadingScreen->Update("Processing terrain");
  Heightfield::HeightfieldBuilder lod(vertices, n, nx,
    ts.mCoastEnhancement,
    ts.mPlainHeight,
    ts.mSimplifyUnderPlain, loadingScreen);
  TRACE_FILE_IF(1) TRACE("done\n");

  // now have the output available
  int heightfieldOutputSize;
  lod.getOutput(mVertexOut, heightfieldOutputSize);
  TRACE_FILE_IF(1) TRACE("heightfieldOutputSize = %d, memory = %d = %5.3f MB\n",
    heightfieldOutputSize,
    heightfieldOutputSize * sizeof(mVertexOut[0]),
    heightfieldOutputSize * sizeof(mVertexOut[0]) / (1024.0f * 1024));

  // delete the vertices temporary
  delete[] vertices;

  // Go back to the normal game heap
  s3eMemorySetInt(S3E_MEMORY_HEAP, 0);

  mHeightfield = new Heightfield::HeightfieldRuntime(nx, n, &mVertexOut[0]);
  mHeightfield->ReservePoints(32000); // seen with lod = 500, 512x512

  // Register for the plain, after the skybox
  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_PLAIN);
  // Register for the terrain
  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_TERRAIN);
  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_TERRAIN_SHADOW);

  Vector3 plainColour(ts.mPlainColourR, ts.mPlainColourG, ts.mPlainColourB);

  if (ts.mType != TerrainSettings::TYPE_PANORAMA)
  {
    int textureSize = 1 << options.mBasicTextureDetail;
    CalculateLightmapTexture(textureSize, ls.mGamma, ts.mBasicTexture.c_str(), plainColour, loadingScreen);

    LoadTextureFromFile(mDetailTexture, ts.mDetailTexture.c_str());
    mDetailTexture.SetClamping(false);
    mDetailTexture.SetFiltering(true);
    mDetailTexture.SetMipMapping(true);
    mDetailTexture.Upload();
    TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", ts.mBasicTexture.c_str(), mDetailTexture.m_HWID);

    // nice mipmapping
    if (mDetailTexture.GetFlags() & Texture::UPLOADED_F)
    {
      glBindTexture(GL_TEXTURE_2D, mDetailTexture.m_HWID);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      mDetailTexture.Upload();
      TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", ts.mBasicTexture.c_str(), mDetailTexture.m_HWID);
    }
  }

  // Set up the plain
  float xMid = mHeightfield->c().x;
  float yMid = mHeightfield->c().y;
  mPlainPts[0] = xMid;
  mPlainPts[1] = yMid;
  mPlainPts[2] = ts.mPlainHeight;
  mPlainCols[0] = plainColour.x;
  mPlainCols[1] = plainColour.y;
  mPlainCols[2] = plainColour.z;
  mPlainCols[3] = 1.0f;

  for (int i = 1; i != mNumPlainPts; ++i)
  {
    float angle = (2.0f * PI * i) / (mNumPlainPts - 2);
    mPlainPts[i * 3 + 0] = xMid + ts.mPlainInnerRadius * FastCos(angle);
    mPlainPts[i * 3 + 1] = xMid + ts.mPlainInnerRadius * FastSin(angle);
    mPlainPts[i * 3 + 2] = ts.mPlainHeight;

    mPlainCols[i * 4 + 0] = plainColour.x;
    mPlainCols[i * 4 + 1] = plainColour.y;
    mPlainCols[i * 4 + 2] = plainColour.z;
    mPlainCols[i * 4 + 3] = 1.0f;
  }

  for (int i = 0; i != mNumOuterPlainPts; ++i)
  {
    float angle = (2.0f * PI * i) / (mNumOuterPlainPts - 1);

    int j = i * 2;
    mOuterPlainPts[j * 3 + 0] = xMid + ts.mPlainInnerRadius * FastCos(angle);
    mOuterPlainPts[j * 3 + 1] = xMid + ts.mPlainInnerRadius * FastSin(angle);
    mOuterPlainPts[j * 3 + 2] = ts.mPlainHeight;

    mOuterPlainCols[j * 4 + 0] = plainColour.x;
    mOuterPlainCols[j * 4 + 1] = plainColour.y;
    mOuterPlainCols[j * 4 + 2] = plainColour.z;
    mOuterPlainCols[j * 4 + 3] = 1.0f;

    ++j;
    mOuterPlainPts[j * 3 + 0] = xMid + (ts.mPlainInnerRadius + ts.mPlainFogDistance) * FastCos(angle);
    mOuterPlainPts[j * 3 + 1] = xMid + (ts.mPlainInnerRadius + ts.mPlainFogDistance)  * FastSin(angle);
    mOuterPlainPts[j * 3 + 2] = ts.mPlainHeight;

    mOuterPlainCols[j * 4 + 0] = plainColour.x;
    mOuterPlainCols[j * 4 + 1] = plainColour.y;
    mOuterPlainCols[j * 4 + 2] = plainColour.z;
    mOuterPlainCols[j * 4 + 3] = 0.0f;
  }

  // Prepare shadow textures
  {
    mGenericShadowTexture = new Texture;
    // TODO need to create this texture by rendering the object into it...
    LoadTextureFromFile(*mGenericShadowTexture, "SystemData/Textures/ShadowTexture.png");
    mGenericShadowTexture->SetClamping(false);
    mGenericShadowTexture->SetFiltering(false);
    mGenericShadowTexture->SetFormatHW(CIwImage::RGBA_4444);
    mGenericShadowTexture->Upload();
    TRACE_FILE_IF(1) TRACE("Uploaded shadow texture id %d", mGenericShadowTexture->m_HWID);
    glBindTexture(GL_TEXTURE_2D, mGenericShadowTexture->m_HWID);
    if (mGenericShadowTexture->GetFlags() & Texture::UPLOADED_F)
    {
      // mipmapping
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      mGenericShadowTexture->Upload();
      TRACE_FILE_IF(1) TRACE("Uploaded shadow texture id %d", mGenericShadowTexture->m_HWID);
    }

    if (gGLVersion == 2 || IwGLExtAvailable(IW_GL_OES_framebuffer_object))
    {
      int size = 1 << options.mProjectedShadowDetail;
      mShadowFrameBufferObject = new FrameBufferObject(size, size, GL_RGB, GL_UNSIGNED_BYTE);
    }
    else
    {
      mShadowFrameBufferObject = 0;
    }
  }



  InitPhysics(loadingScreen);

}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::InitPhysics(LoadingScreenHelper* loadingScreen)
{
  loadingScreen->Update("Terrain collision");
  const TerrainSettings& terrainSettings = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings;
  if (terrainSettings.mCollideWithPlain)
  {
    // The Plain
    mPlainCollisionShape = new btStaticPlaneShape(btVector3(0,0,1), 0);

    btTransform tm;
    tm.setIdentity();
    tm.setOrigin(btVector3(0,0,terrainSettings.mPlainHeight));

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, 0, mPlainCollisionShape);
    rbInfo.m_startWorldTransform = tm;
    rbInfo.m_friction = terrainSettings.mFriction;
    rbInfo.m_restitution = 0.0f;
    mPlainRigidBody = new btRigidBody(rbInfo);
    mDynamicsWorld->addRigidBody(mPlainRigidBody);
  }
  else
  {
    mPlainCollisionShape = 0;
    mPlainRigidBody = 0;
  }

  {
    // The heightfield

    int n = mHeightfield->getSize();
    mPhysicsHeightfield.resize(n*n);
    float zMin = 999999.0f;
    float zMax = -zMin;
    for (int i = 0 ; i < n ; ++i)
    {
      for (int j = 0 ; j < n ; ++j)
      {
        int index = i + j * n;
        float z = mHeightfield->getPos(i, j).z;
        mPhysicsHeightfield[index] = z;
        if (z > zMax)
          zMax = z;
        if (z < zMin)
          zMin = z;
      }
    }

    float xMin = mHeightfield->sw().x;
    float xMax = mHeightfield->se().x;
    float yMin = mHeightfield->sw().y;
    float yMax = mHeightfield->nw().y;

    // The local origin of the heightfield is assumed to be the exact
    // center (as determined by width and length and height, with each
    // axis multiplied by the localScaling).
    mHeightfieldTerrainShape = new btHeightfieldTerrainShape(
      n, n, 
      &mPhysicsHeightfield[0], 1.0f,
      zMin, zMax, 
      2, PHY_FLOAT, false);

    btVector3 scaling(mHeightfieldBoxSize, mHeightfieldBoxSize, 1.0f);
    mHeightfieldTerrainShape->setLocalScaling(scaling);
    mHeightfieldTerrainShape->setUseDiamondSubdivision(true);

    btTransform tm;
    tm.setIdentity();
    tm.setOrigin(btVector3((xMin+xMax)*0.5f, (yMin+yMax)*0.5f, (zMax+zMin)*0.5f));

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, 0, mHeightfieldTerrainShape);
    rbInfo.m_startWorldTransform = tm;
    rbInfo.m_friction = terrainSettings.mFriction;
    rbInfo.m_restitution = 0.0f;

    mHeightfieldRigidBody = new btRigidBody(rbInfo);
    mDynamicsWorld->addRigidBody(mHeightfieldRigidBody);

    mHeightfieldRigidBody->setCollisionFlags(mHeightfieldRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

  }
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::Terminate()
{
  TRACE_METHOD_ONLY(1);
  {
    // plain physics
    if (mPlainRigidBody)
    {
      mDynamicsWorld->removeCollisionObject(mPlainRigidBody);
      delete mPlainRigidBody;
      mPlainRigidBody = 0;
    }
    if (mPlainCollisionShape)
    {
      delete mPlainCollisionShape;
      mPlainCollisionShape = 0;
    }
  }

  {
    // heightfield physics
    mPhysicsHeightfield.clear();
    if (mHeightfieldRigidBody)
    {
      mDynamicsWorld->removeCollisionObject(mHeightfieldRigidBody);
      delete mHeightfieldRigidBody;
      mHeightfieldRigidBody = 0;
    }
    delete mHeightfieldTerrainShape;
    mHeightfieldTerrainShape = 0;
  }

  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_PLAIN);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_TERRAIN);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_TERRAIN_SHADOW);

  delete mHeightfield;
  mHeightfield = 0;

  delete [] mVertexOut;
  mVertexOut = 0;

  delete mGenericShadowTexture;
  mGenericShadowTexture = 0;

  delete mShadowFrameBufferObject;
  mShadowFrameBufferObject = 0;

  if (mTerrainVertexBuffer)
    glDeleteBuffers(1, &mTerrainVertexBuffer);
  mTerrainVertexBuffer = 0;
}


//----------------------------------------------------------------------------------------------------------------------
void Terrain::RenderUpdate(Viewport* viewport, int renderLevel)
{
  TRACE_METHOD_ONLY(2);
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings&es = gs.mEnvironmentSettings;

  DisableFog disableFog;

  switch (renderLevel)
  {
  case RENDER_LEVEL_PLAIN:
    if (es.mTerrainSettings.mRenderPlain)
    {
      RenderPlain(viewport);
    }
    break;
  case RENDER_LEVEL_TERRAIN:
    {
      RenderHeightfield(viewport);
    }
    break;
  case RENDER_LEVEL_TERRAIN_SHADOW:
    {
      if (gs.mOptions.mControlledPlaneShadows || gs.mOptions.mOtherShadows)
      {
        size_t numShadowCasters = RenderManager::GetInstance().GetNumShadowCasterObjects();
        for (size_t iShadowCaster = 0 ; iShadowCaster != numShadowCasters ; ++iShadowCaster)
        {
          RenderObject& shadowCaster = RenderManager::GetInstance().GetShadowCasterObject(iShadowCaster);
          if (shadowCaster.GetShadowVisible())
            RenderShadow(shadowCaster, viewport);
        }
      }
    }
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::RenderPlain(Viewport* viewport)
{
  TRACE_METHOD_ONLY(2);
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings& es = gs.mEnvironmentSettings;

  const TerrainPanoramaShader* terrainPanoramaShader = (TerrainPanoramaShader*) ShaderManager::GetInstance().GetShader(SHADER_TERRAIN_PANORAMA);
  const PlainShader* plainShader = (PlainShader*) ShaderManager::GetInstance().GetShader(SHADER_PLAIN);

  if (es.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
  {
    // Set up the vertex buffer and shader program
    if (gGLVersion == 1)
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      // Inner plain
      glVertexPointer(3, GL_FLOAT, 0, &mPlainPts[0]);
    }
    else
    {
      terrainPanoramaShader->Use();
      glVertexAttribPointer(terrainPanoramaShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &mPlainPts[0]);
      glEnableVertexAttribArray(terrainPanoramaShader->a_position);
      esSetModelViewProjectionMatrix(terrainPanoramaShader->u_mvpMatrix);
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, mNumPlainPts);
    // Don't bother with the outer plain
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if (gGLVersion == 1)
      glDisableClientState(GL_VERTEX_ARRAY);
  }
  else
  {
    // Set up the vertex buffer and shader program
    GLuint shaderProgram = -1;

    if (gGLVersion == 1)
    {
      // Inner plain
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      glVertexPointer(3, GL_FLOAT, 0, &mPlainPts[0]);
      glColorPointer(4, GL_FLOAT, 0, &mPlainCols[0]);

    }
    else
    {
      plainShader->Use();

      glVertexAttribPointer(plainShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &mPlainPts[0]);
      glEnableVertexAttribArray(plainShader->a_position);

      glVertexAttribPointer(plainShader->a_colour, 4, GL_FLOAT, GL_FALSE, 0, &mPlainCols[0]);
      glEnableVertexAttribArray(plainShader->a_colour);

      esSetModelViewProjectionMatrix(plainShader->u_mvpMatrix);
    }

    const float scaleX = es.mTerrainSettings.mPlainDetailTextureScaleX;
    const float scaleY = es.mTerrainSettings.mPlainDetailTextureScaleY;


    if (mDetailTexture.GetFlags() & Texture::UPLOADED_F)
    {
      float heightfieldRange = mHeightfield->getRange();

      glActiveTexture(GL_TEXTURE0);
      int textureMatrixLoc = -1;
      if (gGLVersion == 1)
      {
        glClientActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexCoordPointer(3, GL_FLOAT, 0, &mPlainPts[0]);
      }
      else
      {
        textureMatrixLoc = plainShader->u_textureMatrix;
        glUniform1i(plainShader->u_texture, 0);
      }

      glBindTexture(GL_TEXTURE_2D, mDetailTexture.m_HWID);

      // Use the x/y positions as texture coordinates, after scaling
      esMatrixMode( GL_TEXTURE );
      esPushMatrix();
      esScalef(scaleX/heightfieldRange, scaleY/heightfieldRange, 1.0f);
      esSetTextureMatrix(textureMatrixLoc);
      esMatrixMode( GL_MODELVIEW );
    }

    glDrawArrays(GL_TRIANGLE_FAN, 0, mNumPlainPts);

    if (mDetailTexture.GetFlags() & Texture::UPLOADED_F)
    {
      esMatrixMode( GL_TEXTURE );
      esPopMatrix();
      esMatrixMode( GL_MODELVIEW );
    }

#if 1
    // Outer plain
    EnableBlend enableBlend;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (gGLVersion == 1)
    {
      glVertexPointer(3, GL_FLOAT, 0, &mOuterPlainPts[0]);
      glColorPointer(4, GL_FLOAT, 0, &mOuterPlainCols[0]);
    }
    else
    {
      glVertexAttribPointer(plainShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &mOuterPlainPts[0]);
      glEnableVertexAttribArray(plainShader->a_position);

      glVertexAttribPointer(plainShader->a_colour, 4, GL_FLOAT, GL_FALSE, 0, &mOuterPlainCols[0]);
      glEnableVertexAttribArray(plainShader->a_colour);
    }

    if (mDetailTexture.GetFlags() & Texture::UPLOADED_F)
    {
      float heightfieldRange = mHeightfield->getRange();

      glActiveTexture(GL_TEXTURE0);
      int textureMatrix = -1;
      if (gGLVersion == 1)
      {
        glClientActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexCoordPointer(3, GL_FLOAT, 0, &mOuterPlainPts[0]);
      }
      else
      {
        glUniform1i(plainShader->u_texture, 0);
      }
      glBindTexture(GL_TEXTURE_2D, mDetailTexture.m_HWID);

      // Use the x/y positions as texture coordinates, after scaling
      esMatrixMode( GL_TEXTURE );
      esPushMatrix();
      esScalef(scaleX/heightfieldRange, scaleY/heightfieldRange, 1.0f);
      esSetTextureMatrix(textureMatrix);
      glMatrixMode( GL_MODELVIEW );
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, mNumOuterPlainPts*2);

    if (mDetailTexture.GetFlags() & Texture::UPLOADED_F)
    {
      if (gGLVersion == 1)
      {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
      }

      esMatrixMode( GL_TEXTURE );
      esPopMatrix();
      esMatrixMode( GL_MODELVIEW );
    }
#endif
    // Tidy up
    if (gGLVersion == 1)
    {
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::RenderHeightfield(Viewport* viewport)
{
  TRACE_METHOD_ONLY(2);
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings&es = gs.mEnvironmentSettings;

  if (!mHeightfield)
    return;

  Vector3 cameraPosition = viewport->GetCamera()->GetPosition();

  if (gs.mOptions.mDrawSunPosition)
  {
    const Vector3 dirToLight = -RenderManager::GetInstance().GetLightingDirection().GetNormalised();
    RenderManager::GetInstance().GetDebugRenderer().DrawPoint(cameraPosition + dirToLight * 10.0f, 1.0f, Vector3(0,0,0));
  }

  float lod;
  bool clip;
  if (PicaSim::GetInstance().GetMode() == PicaSim::MODE_GROUND)
  {
    lod = gs.mOptions.mGroundViewTerrainLOD;
    clip = gs.mOptions.mGroundViewUpdateTerrainLOD;
    if (clip)
      lod *= DegreesToRadians(60.0f) / viewport->GetCamera()->GetVerticalFOV();
  }
  else
  {
    lod = gs.mOptions.mAeroplaneViewTerrainLOD;
    lod *= DegreesToRadians(60.0f) / viewport->GetCamera()->GetVerticalFOV();
    clip = true;
  }

  if (clip)
  {
    mHeightfield->setClipping(
      viewport->GetCamera()->GetTransform(),
      viewport->GetCamera()->GetVerticalFOV(),
      viewport->GetAspectRatio(),
      viewport->GetCamera()->GetNearClipPlaneDistance(),
      viewport->GetCamera()->GetFarClipPlaneDistance());
  }
  else
  {
    mHeightfield->setClipping(false);
  }

  bool needToRegen = mHeightfield->getSavedPoints().empty();
  if (!needToRegen)
    needToRegen = clip;
  if (!needToRegen)
    needToRegen = lod != mLastLOD;
  if (!needToRegen)
    needToRegen = cameraPosition != mLastCameraPosition;

  if (needToRegen)
  {
    TRACE_FILE_IF(3) TRACE("Regenerating Heightfield");
    mHeightfield->clearSaved();
    mHeightfield->meshRefine(cameraPosition, lod);
    mLastLOD = lod;
    mLastCameraPosition = cameraPosition;

    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    if (mTerrainVertexBuffer)
      glDeleteBuffers(1, &mTerrainVertexBuffer);
    mTerrainVertexBuffer = 0;
    if (!clip)
    {
      glGenBuffers(1, &mTerrainVertexBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, mTerrainVertexBuffer);

      // allocate enough space for the VBO
      const Heightfield::HeightfieldRuntime::SavedPoints& savedPoints = mHeightfield->getSavedPoints();
      glBufferData(GL_ARRAY_BUFFER, 
        sizeof(Heightfield::HeightfieldRuntime::Pos) * savedPoints.size(), &savedPoints[0], GL_STATIC_DRAW);
    }
  }

  TRACE_FILE_IF(3) TRACE("Rendering Heightfield");
  const Heightfield::HeightfieldRuntime::SavedPoints& savedPoints = mHeightfield->getSavedPoints();

  // Now actually draw
  unsigned num = savedPoints.size();
  TRACE_FILE_IF(3) TRACE("Num points = %d", num);
  if (num == 0)
    return;

  // Switch to CW 
  FrontFaceCW frontFaceCW;
  EnableCullFace enableCullFace(GL_BACK);

  size_t start = (size_t) (&savedPoints[0]);
  if (mTerrainVertexBuffer)
  {
    glBindBuffer(GL_ARRAY_BUFFER, mTerrainVertexBuffer);
    start = 0;
  }

  // Set up the vertex buffer and shader program
  const TerrainPanoramaShader* terrainPanoramaShader = (TerrainPanoramaShader*) ShaderManager::GetInstance().GetShader(SHADER_TERRAIN_PANORAMA);
  const TerrainShader* terrainShader = (TerrainShader*) ShaderManager::GetInstance().GetShader(SHADER_TERRAIN);
  int textureMatrix0Loc = -1;
  int textureMatrix1Loc = -1;
  int texture0Loc = -1;
  int texture1Loc = -1;

  if (gGLVersion == 1)
  {
    TRACE_FILE_IF(3) TRACE("Setting up vertex buffer", num);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*) start);
  }
  else
  {
    TRACE_FILE_IF(3) TRACE("Setting up shaders", num);
    if (es.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
    {
      terrainPanoramaShader->Use();
      glVertexAttribPointer(terrainPanoramaShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) start);
      glEnableVertexAttribArray(terrainPanoramaShader->a_position);
      esSetModelViewProjectionMatrix(terrainPanoramaShader->u_mvpMatrix);
    }
    else
    {
      textureMatrix0Loc = terrainShader->u_textureMatrix0;
      textureMatrix1Loc = terrainShader->u_textureMatrix1;
      texture0Loc = terrainShader->u_texture0;
      texture1Loc = terrainShader->u_texture1;
      terrainShader->Use();
      glVertexAttribPointer(terrainShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) start);
      glEnableVertexAttribArray(terrainShader->a_position);
      esSetModelViewProjectionMatrix(terrainShader->u_mvpMatrix);
    }
  }

  if (es.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
  {
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    TRACE_FILE_IF(4) TRACE("Terrain::RenderHeightfield: num verts = %d", num);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, num);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
  else
  {
    GLint textureUnit = GL_TEXTURE0;
    GLint maxTextureUnits = 8;
    if (gGLVersion == 1)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
    //maxTextureUnits = 0;
    GLint maxTextureUnit = GL_TEXTURE0 + maxTextureUnits;

    // Basic texture
    if (
      textureUnit < maxTextureUnit && 
      mHeightfieldTexture.GetFlags() & Texture::UPLOADED_F
      )
    {
      TRACE_FILE_IF(3) TRACE("Setting up basic texture");
      glActiveTexture(textureUnit);
      if (gGLVersion == 1)
      {
        glClientActiveTexture(textureUnit);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*) start);
      }
      else
      {
        glUniform1i(texture0Loc, textureUnit - GL_TEXTURE0);
      }

      glBindTexture(GL_TEXTURE_2D, mHeightfieldTexture.m_HWID);

      // Use the x/y positions as texture coordinates, after scaling
      esMatrixMode( GL_TEXTURE );
      esPushMatrix();
      float xMin = mHeightfield->sw().x;
      float xMax = mHeightfield->se().x;
      float yMin = mHeightfield->sw().y;
      float yMax = mHeightfield->nw().y;
      float scale = 1.0f;
      esScalef(scale/(xMax - xMin), scale/(yMax - yMin), 1.0f);
      esTranslatef(-xMin, -yMin, 0.0f);
      esSetTextureMatrix(textureMatrix0Loc);
      esMatrixMode( GL_MODELVIEW );

      ++textureUnit;
    } 
    else
    {
      glActiveTexture(textureUnit);
      glDisable(GL_TEXTURE_2D);
    }

    // Detail texture
    if (
      textureUnit < maxTextureUnit && 
      mDetailTexture.GetFlags() & Texture::UPLOADED_F
      )
    {
      TRACE_FILE_IF(3) TRACE("Setting up detail texture");
      glActiveTexture(textureUnit);
      int textureMatrix = -1;
      if (gGLVersion == 1)
      {
        glClientActiveTexture(textureUnit);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*) start);
      }
      else
      {
        glUniform1i(texture1Loc, textureUnit - GL_TEXTURE0);
      }

      glBindTexture(GL_TEXTURE_2D, mDetailTexture.m_HWID);
      // Use the x/y positions as texture coordinates, after scaling
      float xMin = mHeightfield->sw().x;
      float xMax = mHeightfield->se().x;
      float yMin = mHeightfield->sw().y;
      float yMax = mHeightfield->nw().y;

      esMatrixMode( GL_TEXTURE );
      esPushMatrix();
      esLoadIdentity();
      esScalef(
        es.mTerrainSettings.mTerrainDetailTextureScaleX/(xMax - xMin), 
        es.mTerrainSettings.mTerrainDetailTextureScaleX/(yMax - yMin), 1.0f);
      esTranslatef(-xMin, -yMin, 0.0f);
      esSetTextureMatrix(textureMatrix1Loc);
      esMatrixMode( GL_MODELVIEW );

      ++textureUnit;
    }
    else
    {
      glActiveTexture(textureUnit);
      glDisable(GL_TEXTURE_2D);
    }

    TRACE_FILE_IF(3) TRACE("Terrain::RenderHeightfield: num verts = %d", num);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, num);

    TRACE_FILE_IF(3) TRACE("Clearing up", num);
    textureUnit = GL_TEXTURE0;
    if (
      textureUnit < maxTextureUnit && 
      mHeightfieldTexture.GetFlags() & Texture::UPLOADED_F
      )
    {
      if (gGLVersion == 1)
      {
        glActiveTexture(textureUnit);
        glClientActiveTexture(textureUnit);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
      }

      esMatrixMode( GL_TEXTURE );
      esPopMatrix();
      esMatrixMode( GL_MODELVIEW );

      ++textureUnit;
    }

    if (
      textureUnit < maxTextureUnit && 
      mDetailTexture.GetFlags() & Texture::UPLOADED_F
      )
    {
      if (gGLVersion == 1)
      {
        glActiveTexture(textureUnit);
        glClientActiveTexture(textureUnit);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
        glClientActiveTexture(GL_TEXTURE0);
      }
      esMatrixMode( GL_TEXTURE );
      esPopMatrix();
      esMatrixMode( GL_MODELVIEW );

      glActiveTexture(GL_TEXTURE0);

      ++textureUnit;
    }
  }

  if (mTerrainVertexBuffer)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (gGLVersion == 1)
  {
    if (gs.mOptions.mTerrainWireframe)
    {
      glColor4f(1, 1, 1, 1);

      glDrawArrays(GL_LINE_STRIP, 0, num);

      glVertexPointer(3, GL_FLOAT, 6 * 4, &savedPoints[0]);
      glDrawArrays(GL_LINE_STRIP, 0, num / 2);

      glVertexPointer(3, GL_FLOAT, 6 * 4, &savedPoints[1]);
      glDrawArrays(GL_LINE_STRIP, 0, num / 2);
    }
  }

  if (gGLVersion == 1)
    glDisableClientState(GL_VERTEX_ARRAY);
  TRACE_FILE_IF(2) TRACE("Finished RenderHeightfield");
}

//----------------------------------------------------------------------------------------------------------------------
void RenderTerrainQuad(Heightfield::HeightfieldRuntime& heightfield, int i, int j, int positionLoc, int texCoordLoc)
{
  Vector3 pts[4];
  if ( (i + j) % 2)
  {
    // Quad is divided by a downward slope
    pts[0] = heightfield.getPos(i+1, j);
    pts[1] = heightfield.getPos(i+1, j+1);
    pts[2] = heightfield.getPos(i,   j+1);
    pts[3] = heightfield.getPos(i,   j);
  }
  else
  {
    // Quad is divided by an upward slope
    pts[0] = heightfield.getPos(i,   j);
    pts[1] = heightfield.getPos(i+1, j);
    pts[2] = heightfield.getPos(i+1, j+1);
    pts[3] = heightfield.getPos(i,   j+1);
  }

  float plainHeight = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  if (pts[0].z < plainHeight) pts[0].z = plainHeight;
  if (pts[1].z < plainHeight) pts[1].z = plainHeight;
  if (pts[2].z < plainHeight) pts[2].z = plainHeight;
  if (pts[3].z < plainHeight) pts[3].z = plainHeight;

  if (gGLVersion == 1)
  {
    glVertexPointer(3, GL_FLOAT, 0, &pts[0]);
    glTexCoordPointer(3, GL_FLOAT, 0, &pts[0]);
  }
  else
  {
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glVertexAttribPointer(texCoordLoc, 3, GL_FLOAT, GL_FALSE, 0, pts);

  }
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// This is mad - I seem to have to use this (or something like it) to do a single raycast against a known shape
struct btSingleRayCallback : public btBroadphaseRayCallback
{
  btVector3  m_rayFromWorld;
  btVector3  m_rayToWorld;
  btTransform  m_rayFromTrans;
  btTransform  m_rayToTrans;
  btVector3  m_hitNormal;

  const btCollisionWorld*  m_world;
  btCollisionWorld::RayResultCallback&  m_resultCallback;

  btSingleRayCallback(const btVector3& rayFromWorld,const btVector3& rayToWorld,const btCollisionWorld* world,btCollisionWorld::RayResultCallback& resultCallback)
    :m_rayFromWorld(rayFromWorld),
    m_rayToWorld(rayToWorld),
    m_world(world),
    m_resultCallback(resultCallback)
  {
    m_rayFromTrans.setIdentity();
    m_rayFromTrans.setOrigin(m_rayFromWorld);
    m_rayToTrans.setIdentity();
    m_rayToTrans.setOrigin(m_rayToWorld);

    btVector3 rayDir = (rayToWorld-rayFromWorld);

    rayDir.normalize ();
    ///what about division by zero? --> just set rayDirection[i] to INF/BT_LARGE_FLOAT
    m_rayDirectionInverse[0] = rayDir[0] == btScalar(0.0) ? btScalar(BT_LARGE_FLOAT) : btScalar(1.0) / rayDir[0];
    m_rayDirectionInverse[1] = rayDir[1] == btScalar(0.0) ? btScalar(BT_LARGE_FLOAT) : btScalar(1.0) / rayDir[1];
    m_rayDirectionInverse[2] = rayDir[2] == btScalar(0.0) ? btScalar(BT_LARGE_FLOAT) : btScalar(1.0) / rayDir[2];
    m_signs[0] = m_rayDirectionInverse[0] < 0.0;
    m_signs[1] = m_rayDirectionInverse[1] < 0.0;
    m_signs[2] = m_rayDirectionInverse[2] < 0.0;

    m_lambda_max = rayDir.dot(m_rayToWorld-m_rayFromWorld);
  }

  virtual bool  process(const btBroadphaseProxy* proxy)
  {
    ///terminate further ray tests, once the closestHitFraction reached zero
    if (m_resultCallback.m_closestHitFraction == btScalar(0.f))
      return false;

    btCollisionObject*  collisionObject = (btCollisionObject*)proxy->m_clientObject;

    //only perform raycast if filterMask matches
    if(m_resultCallback.needsCollision(collisionObject->getBroadphaseHandle())) 
    {
      {
        m_world->rayTestSingle(m_rayFromTrans,m_rayToTrans,
          collisionObject,
          collisionObject->getCollisionShape(),
          collisionObject->getWorldTransform(),
          m_resultCallback);
      }
    }
    return true;
  }
};

//----------------------------------------------------------------------------------------------------------------------
void Terrain::RenderShadow(RenderObject& shadowCaster, Viewport* viewport)
{
  TRACE_METHOD_ONLY(2);
  Vector3 pos = shadowCaster.GetTM().GetTrans();
  float radius = shadowCaster.GetRenderBoundingRadius();
  RenderShadow(shadowCaster, pos, radius, viewport);
}

//----------------------------------------------------------------------------------------------------------------------
void Terrain::RenderShadow(RenderObject& shadowCaster, const Vector3& shadowCasterPos, const float shadowCasterRadius, Viewport* viewport)
{
  TRACE_METHOD_ONLY(2);

  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  Vector3 dirToLight = -RenderManager::GetInstance().GetLightingDirection().GetNormalised();

  // Avoid problems with light directly overhead
  const float eps = 0.001f;
  if (hypotf(dirToLight.x, dirToLight.y) < eps)
  {
    dirToLight = Vector3(eps, 0.0f, 1.0f).GetNormalised();
  }

  bool useBlob = true;
  if (&shadowCaster == (void*) PicaSim::GetInstance().GetPlayerAeroplane()->GetGraphics())
  {
    if (gs.mOptions.mControlledPlaneShadows == Options::NONE)
      return;
    useBlob = gs.mOptions.mControlledPlaneShadows == Options::BLOB;
  }
  else
  {
    if (gs.mOptions.mOtherShadows == Options::NONE)
      return;
    useBlob = gs.mOptions.mOtherShadows == Options::BLOB;
  }
  if (!mShadowFrameBufferObject)
    useBlob = true;

  Vector3 shadowPos = shadowCasterPos;
  float maxDistToShadow = RenderManager::GetInstance().GetShadowDecayHeight() * shadowCasterRadius / 0.7f; // scale according to a reference distance (here 1)
  Vector3 rayDelta = -maxDistToShadow * dirToLight;
  Vector3 rayEnd = shadowCasterPos + rayDelta;

  btCollisionWorld::ClosestRayResultCallback result(Vector3ToBulletVector3(shadowCasterPos), Vector3ToBulletVector3(rayEnd));
  btSingleRayCallback rayCB(
    Vector3ToBulletVector3(shadowCasterPos),
    Vector3ToBulletVector3(rayEnd),
    &EntityManager::GetInstance().GetDynamicsWorld(),
    result);
  rayCB.process(mHeightfieldRigidBody->getBroadphaseHandle());

  // Also collide against the terrain plain - and choose the closes hit
  float plainHeight = gs.mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  float heightAbovePlain = shadowCasterPos.z - plainHeight;
  if (heightAbovePlain < 0.0f)
    return;
  float frac = heightAbovePlain / -rayDelta.z;

  if (rayCB.m_resultCallback.m_closestHitFraction < frac)
    frac = rayCB.m_resultCallback.m_closestHitFraction;

  shadowPos = shadowCasterPos + frac * rayDelta;

  // Check if the shadow would be in the view frustum
  const Camera& camera = *viewport->GetCamera();
  if (!camera.isSpherePartlyInFrustum(shadowPos, shadowCasterRadius))
    return;

  // This offsets in the horizontal direction towards the light
  if (useBlob)
  {
    float shadowOffsetFrac = 0.6f;
    shadowPos += Vector3(dirToLight.x * shadowCasterRadius * shadowOffsetFrac, dirToLight.y * shadowCasterRadius * shadowOffsetFrac, 0.0f);
  }
  float distToShadow = (shadowPos - shadowCasterPos).GetLength();

  float shadowAmount = 1.0f - distToShadow / maxDistToShadow;
  if (shadowAmount < 0.0f)
    return;
  shadowAmount *= shadowAmount;
  shadowAmount *= RenderManager::GetInstance().GetShadowStrength();

  if (!useBlob)
    shadowAmount = sqrtf(shadowAmount);
#if 0
  RenderManager::GetInstance().GetDebugRenderer().DrawLine(shadowCasterPos, shadowCasterPos + dirToLight*2, Vector3(1,1,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawPoint(shadowPos, 1.0f, Vector3(1,0,0));
#endif

#define SHOW_FBOx

  if (!useBlob)
  {
    // Render the caster into the FBO
#ifndef SHOW_FBO
    mShadowFrameBufferObject->Bind();
#endif

    // Clear to white
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // set up projection
    esMatrixMode(GL_PROJECTION);
    esPushMatrix();
      
    esLoadIdentity();
    esOrthof(
      -shadowCasterRadius, shadowCasterRadius, 
      -shadowCasterRadius, shadowCasterRadius, 
      1-shadowCasterRadius, 1+shadowCasterRadius);
      
    esMatrixMode(GL_MODELVIEW);
    esPushMatrix();
      
    //glPushAttrib(GL_ALL_ATTRIB_BITS);
      
    // set up view 
    const Vector3& to = shadowCasterPos;
    const Vector3 from = to + dirToLight;
    Vector3 up(0,0,1);
    // hard-wire "up" = hope the sun isn't overhead!
    esLoadIdentity();

    GLMat44 viewMatrix;
    esMatrixLoadIdentity(viewMatrix);
    LookAt(
      viewMatrix,
      from[0], from[1], from[2],
      to[0],   to[1],   to[2],
      up[0],   up[1],   up[2]);

    shadowCaster.SetShadowAmount(shadowAmount);      
    glDisable(GL_DEPTH_TEST);
    shadowCaster.RenderUpdate(viewport, RENDER_LEVEL_TERRAIN_SHADOW);
    glEnable(GL_DEPTH_TEST);

    esMatrixMode(GL_PROJECTION);
    esPopMatrix();
    esMatrixMode(GL_MODELVIEW);
    esPopMatrix();

#ifdef SHOW_FBO
    return;
#else
    mShadowFrameBufferObject->Release();
#endif
  }

  // Start rendering
  esPushMatrix();

  DisableDepthMask disableDepthMask;
  EnableCullFace cullFace(GL_BACK);
  EnableBlend enableBlend;

  if (useBlob)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);

  // Offset shadow towards the camera, depending how far away it is
  Vector3 dirToCamera = viewport->GetCamera()->GetTransform().GetTrans() - shadowCasterPos;
  float distance = dirToCamera.GetLength();
  if (distance > 0.0f)
    dirToCamera *= 1.0f / distance;
  if (dirToCamera.z < 0.0f)
    dirToCamera.z = 0.0f;
  Vector3 offset = ClampToRange(distance * 0.01f, 0.01f, 5.0f) * dirToCamera;
  esTranslatef(offset.x, offset.y, offset.z + 0.02f); // small upwards to stop shimmering when camera is at the object's location

  const ShadowShader* shadowShader = (ShadowShader*) ShaderManager::GetInstance().GetShader(SHADER_SHADOW);
  if (gGLVersion == 1)
  {
    glEnable(GL_TEXTURE_2D);
    if (useBlob)
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    else
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(1,1,1,shadowAmount);
    glClientActiveTexture(GL_TEXTURE0);
  }
  else
  {
    shadowShader->Use();
    glUniform1i(shadowShader->u_texture, 0);
    glEnableVertexAttribArray(shadowShader->a_position);
    glEnableVertexAttribArray(shadowShader->a_texCoord);
    glUniform4f(shadowShader->u_colour, 1.0f, 1.0f, 1.0f, shadowAmount);
  }

  glActiveTexture(GL_TEXTURE0);
  if (useBlob)
    glBindTexture(GL_TEXTURE_2D, mGenericShadowTexture->m_HWID);
  else
    glBindTexture(GL_TEXTURE_2D, mShadowFrameBufferObject->GetTextureHandle());

  // Use the x/y positions as texture coordinates, after scaling
  esMatrixMode(GL_TEXTURE);
  esPushMatrix();
  esLoadIdentity();

  if (useBlob)
  {
    // smaller results in a bigger shadow
    float textureScale = 1.0f / (shadowCasterRadius * RenderManager::GetInstance().GetShadowSizeScale()); 
    esTranslatef(0.5f, 0.5f, 0.0f);
    float eps = 1.0f; // Larger circularises the shadow when the sun is low
    float textureScaleX = textureScale * eps / (eps + fabsf(dirToLight.x));
    float textureScaleY = textureScale * eps/ (eps + fabsf(dirToLight.y));
    esScalef(textureScaleX, textureScaleY, 1.0f);
    esTranslatef(-shadowPos.x, -shadowPos.y, 0.0f);
  }
  else
  {
    // Work out stuff for automatic texture coords
    // Assume that "up" is not parallel to dirToLight
    // Normal for texture "x" coord
    Vector3 Nx = -dirToLight.Cross(Vector3(0,0,1)).GetNormalised();
    // Normal for texture "y" coord
    Vector3 Ny = -Nx.Cross(dirToLight).GetNormalised();

    float invRadius = 0.5f/shadowCasterRadius;
    float Dx = -Nx.Dot(shadowPos);
    float Dy = -Ny.Dot(shadowPos);

    GLMat44 mat;

    mat[0][0] = Nx[0]*invRadius;
    mat[1][0] = Nx[1]*invRadius;
    mat[2][0] = Nx[2]*invRadius;
    mat[3][0] = 0.5f+Dx*invRadius; 

    mat[0][1] = Ny[0]*invRadius;
    mat[1][1] = Ny[1]*invRadius;
    mat[2][1] = Ny[2]*invRadius;
    mat[3][1] = 0.5f+Dy*invRadius; 
  
    mat[0][2] = 0.0f;
    mat[1][2] = 0.0f;
    mat[2][2] = 0.0f;
    mat[3][2] = 0.0f;

    mat[0][3] = 0.0f;
    mat[1][3] = 0.0f;
    mat[2][3] = 0.0f;
    mat[3][3] = 1.0f;

    esMultMatrixf(&mat[0][0]);
  }

  esSetTextureMatrix(shadowShader->u_textureMatrix);
  esMatrixMode( GL_MODELVIEW );
  esSetModelViewProjectionMatrix(shadowShader->u_mvpMatrix);

  float cellSize = mHeightfield->getCellSize();
  int heightfieldSize = mHeightfield->getSize();

  int i0, j0;
  {
    unsigned i, j;
    GetIndex(shadowPos.x, shadowPos.y, i, j);
    i0 = (int) i;
    j0 = (int) j;
  }

  int di = 1 + (int) (shadowCasterRadius / (1.0f + fabsf(dirToLight.x) * cellSize));
  int dj = 1 + (int) (shadowCasterRadius / (1.0f + fabsf(dirToLight.y) * cellSize));

  for (int i = i0 - di ; i < i0 + di + 1; ++i)
  {
    if (i < 0 || i > heightfieldSize-2)
      continue;
    for (int j = j0 - dj ; j < j0 + dj + 1; ++j)
    {
      if (j < 0 || j > heightfieldSize-2)
        continue;
      
      RenderTerrainQuad(*mHeightfield, i, j, shadowShader->a_position, shadowShader->a_texCoord);
    }
  }

  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  esMatrixMode( GL_TEXTURE );
  esPopMatrix();
  esMatrixMode( GL_MODELVIEW );
  esPopMatrix();
}

