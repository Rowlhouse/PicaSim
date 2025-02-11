#include "Environment.h"
#include "HelpersXML.h"
#include "LoadingScreenHelper.h"
#include "ObjectEditingOverlay.h"
#include "ProfilageMarmelade.h"
#include "Runway.h"
#include "SimpleObject.h"
#include "Trace.h"
#include "PicaSim.h"
#include "Aeroplane.h"
// #include "LoadingScreen.h"
#include "PicaDialog.h"
// #include "AeroplanePhysics.h"
#include "EntityManager.h"

// #include <IwUtil.h>

Environment* Environment::mInstance = 0;

//---------------------------------------------------------------------------------------------------------------------
Environment& Environment::GetInstance()
{
  IwAssert(ROWLHOUSE, mInstance != 0);
  return *mInstance;
}

//----------------------------------------------------------------------------------------------------------------------
bool Environment::Init(LoadingScreenHelper* loadingScreen)
{
  TRACE_FUNCTION_ONLY(1);
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();

  if (loadingScreen) loadingScreen->Update("Loading environment");

  IwAssert(ROWLHOUSE, !mInstance);
  mInstance = new Environment;

  mInstance->mChecksum = 0;

  // Set up the lighting first
  float lightBearing = 0.0f;
  float lightElevation = 90.0f;
  Vector3 ambientLight(0.3f, 0.3f, 0.3f);
  Vector3 diffuseLight(1.0f, 1.0f, 1.0f);
  float shadowStrength = 0.2f;
  float shadowDecayHeight = 40.0f;
  float shadowSizeScale = 1.3f;
  std::string skyboxName;

  if (gs.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
    skyboxName = gs.mEnvironmentSettings.mTerrainSettings.mPanoramaName;
  else
    skyboxName = gs.mLightingSettings.mSkyboxName;

  std::string lightingFile = skyboxName + "/Lighting.xml";
  TiXmlDocument doc(lightingFile);
  bool success = doc.LoadFile();
  IwAssert(ROWLHOUSE, success);

  if (!success)
  {
    // tidy up and return failure
    const Language language = PicaSim::GetInstance().GetSettings().mOptions.mLanguage;
    ShowDialog("PicaSim", "Failed to load environment - please load one of the scenery presets and try again", TXT(PS_OK));
    return false;
  }

  TiXmlHandle docHandle( &doc );
  TiXmlElement* element = docHandle.FirstChild( "Lighting" ).ToElement();
  if (element)
  {
    readFromXML(element, "lightBearing", lightBearing);
    readFromXML(element, "lightElevation", lightElevation);
    readFromXML(element, "ambientLightR", ambientLight.x);
    readFromXML(element, "ambientLightG", ambientLight.y);
    readFromXML(element, "ambientLightB", ambientLight.z);
    readFromXML(element, "diffuseLightR", diffuseLight.x);
    readFromXML(element, "diffuseLightG", diffuseLight.y);
    readFromXML(element, "diffuseLightB", diffuseLight.z);
    readFromXML(element, "shadowStrength", shadowStrength);
    readFromXML(element, "shadowDecayHeight", shadowDecayHeight);
    readFromXML(element, "shadowSizeScale", shadowSizeScale);
  }

  RenderManager::GetInstance().SetLightingAmbientColour(ambientLight * gs.mOptions.mAmbientLightingScale);
  RenderManager::GetInstance().SetLightingDiffuseColour(diffuseLight * gs.mOptions.mDiffuseLightingScale);
  RenderManager::GetInstance().SetShadowStrength(shadowStrength);
  RenderManager::GetInstance().SetShadowDecayHeight(shadowDecayHeight);
  RenderManager::GetInstance().SetShadowSizeScale(shadowSizeScale);

  mInstance->mSkybox.Init(skyboxName.c_str(), gs.mOptions.m16BitTextures, gs.mOptions.mMaxSkyboxDetail, loadingScreen);

  if (gs.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
  {
    mInstance->mSkybox.SetOffset(-90.0f);
    RenderManager::GetInstance().SetLightingDirection(lightBearing - 90.0f, lightElevation);
  }
  else
  {
    mInstance->mSkybox.SetOffset((float) gs.mLightingSettings.mSunBearingOffset);
    RenderManager::GetInstance().SetLightingDirection(lightBearing + gs.mLightingSettings.mSunBearingOffset, lightElevation);
  }

  mInstance->mTerrain.Init(EntityManager::GetInstance().GetDynamicsWorld(), loadingScreen, mInstance->mChecksum);

  // Set the wind
  mInstance->UpdateWind(0.0f);

  // Initialise wind flow
  mInstance->InitZeroFlowHeightfield(loadingScreen);

  EntityManager::GetInstance().RegisterEntity(mInstance, ENTITY_LEVEL_PRE_PHYSICS);

  mInstance->mThermalManager.Init(loadingScreen);

  if (gs.mEnvironmentSettings.mRunwayType != EnvironmentSettings::RUNWAY_NONE)
  {
    Transform runwayTM;
    runwayTM.SetRotZ(DegreesToRadians(gs.mEnvironmentSettings.mRunwayAngle));
    runwayTM.SetTrans(gs.mEnvironmentSettings.mRunwayPosition);
    if (gs.mEnvironmentSettings.mRunwayType == EnvironmentSettings::RUNWAY_RUNWAY)
    {
      mInstance->mRunway = new Runway(
        runwayTM, gs.mEnvironmentSettings.mRunwayLength, gs.mEnvironmentSettings.mRunwayWidth, "SystemData/Textures/Runway.png", Runway::RUNWAY);
    }
    else
    {
      mInstance->mRunway = new Runway(
        runwayTM, gs.mEnvironmentSettings.mRunwayLength, gs.mEnvironmentSettings.mRunwayWidth, "SystemData/Textures/Runway.png", Runway::CIRCLE);
    }
  }
  else
  {
    mInstance->mRunway = 0;
  }

  // Objects
  mInstance->mObjectEditingTexture = new Texture;
  mInstance->mObjectEditingTexture->LoadFromFile("SystemData/Menu/Button.png");
  mInstance->mObjectEditingTexture->SetFormatHW(CIwImage::RGBA_4444);
  mInstance->mObjectEditingTexture->Upload();

  ObjectsSettings& os = PicaSim::GetInstance().GetSettings().mObjectsSettings;
  for (size_t i = 0 ; i != os.mBoxes.size() ; ++i)
  {
    ObjectsSettings::Box& box = os.mBoxes[i];
    box.Create();
    mInstance->mBoxes.insert(box.mObject);
  }

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void Environment::Terminate()
{
  TRACE_FUNCTION_ONLY(1);
  IwAssert(ROWLHOUSE, mInstance);

  mInstance->mThermalManager.Terminate();
  mInstance->mSkybox.Terminate();
  mInstance->mTerrain.Terminate();

  delete mInstance->mRunway;
  mInstance->mRunway = 0;

  delete mInstance->mObjectEditingOverlay;
  mInstance->mObjectEditingOverlay = 0;

  delete mInstance->mObjectEditingTexture;
  mInstance->mObjectEditingTexture = 0;

  // Clear any runtime record of boxes
  ObjectsSettings& os = PicaSim::GetInstance().GetSettings().mObjectsSettings;
  for (ObjectsSettings::Boxes::iterator it = os.mBoxes.begin() ; it != os.mBoxes.end() ; ++it)
  {
    it->mObject = 0;
  }
  // Do the actual deletion
  while (!mInstance->mBoxes.empty())
  {
    BoxObject* box = *mInstance->mBoxes.begin();
    delete box;
    mInstance->mBoxes.erase(mInstance->mBoxes.begin());
  }

  EntityManager::GetInstance().UnregisterEntity(mInstance, ENTITY_LEVEL_PRE_PHYSICS);

  delete mInstance;
  mInstance = 0;

}

//---------------------------------------------------------------------------------------------------------------------
Environment::Environment() : mTime(0.0f), mObjectEditingOverlay(0), mRunway(0)
{
}

//---------------------------------------------------------------------------------------------------------------------
Environment::~Environment()
{
}

//---------------------------------------------------------------------------------------------------------------------
void Environment::UpdateWind(float deltaTime)
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings& es = gs.mEnvironmentSettings;

  float windBearing = es.mWindBearing;
  mWindDirection.x = -FastCos(DegreesToRadians(windBearing));
  mWindDirection.y = -FastSin(DegreesToRadians(windBearing));
  mWindDirection.z = 0.0f;

  // Update gusts
  {
    float a = 1.0f;
    float b = 2.345f;
    float c = 6.7f;
    float scale = 1.0f / (1/a + 1/b + 1/c);
    float gustBearingOffset = es.mWindGustBearingAmplitude * scale * (
      FastSin(a * 2.0f * PI * mTime / es.mWindGustTime) / a + 
      FastSin(b * 2.0f * PI * mTime / es.mWindGustTime) / b + 
      FastSin(c * 2.0f * PI * mTime / es.mWindGustTime) / c); 
    windBearing = es.mWindBearing + gustBearingOffset;
    mGustyWindDirection.x = -FastCos(DegreesToRadians(windBearing));
    mGustyWindDirection.y = -FastSin(DegreesToRadians(windBearing));
    mGustyWindDirection.z = 0.0f;
  }

  {
    float a = 1.0f;
    float b = 2.89f;
    float c = 6.1f;
    float scale = 1.0f / (1/a + 1/b + 1/c);
    float gust = es.mWindSpeed * es.mWindGustAmplitudeFraction * scale * (
      FastSin(a * 2.0f * PI * mTime / es.mWindGustTime) / a + 
      FastSin(b * 2.0f * PI * mTime / es.mWindGustTime) / b + 
      FastSin(c * 2.0f * PI * mTime / es.mWindGustTime) / c); 
    mGustyWindSpeed = es.mWindSpeed + gust;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void Environment::EntityUpdate(float deltaTime, int entityLevel)
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  ObjectsSettings& os = gs.mObjectsSettings;

  mTime += deltaTime;

  UpdateWind(deltaTime);

  if (PicaSim::GetInstance().GetPlayerAeroplane())
    mThermalManager.Update(deltaTime, PicaSim::GetInstance().GetPlayerAeroplane()->GetTransform().GetTrans());

  if (gs.mOptions.mEnableObjectEditing && !mObjectEditingOverlay)
  {
    mObjectEditingOverlay = new ObjectEditingOverlay(gs, mInstance->mObjectEditingTexture, *this);
  }
  else if (!gs.mOptions.mEnableObjectEditing && mObjectEditingOverlay)
  {
    delete mObjectEditingOverlay;
    mObjectEditingOverlay = 0;
  }

  if (!mObjectEditingOverlay)
  {
    for (size_t iBox = 0 ; iBox != os.mBoxes.size() ; ++iBox)
    {
      ObjectsSettings::Box& box = os.mBoxes[iBox];
      box.mObject->SetWireframe(false);
      box.mObject->SetVisible(box.mVisible);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
float Environment::GetZeroWindHeightAt(int i, int j) const
{
  const int n = mTerrain.GetHeightfield().getSize();
  const int index = Heightfield::calcIndex(i, j, n);
  return mZeroFlowHeightfield[index];
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 Environment::GetZeroWindPosAt(int i, int j) const
{
  const int n = mTerrain.GetHeightfield().getSize();
  const int index = Heightfield::calcIndex(i, j, n);

  Vector3 pos = mTerrain.GetHeightfield().getPos(i, j);
  pos.z = mZeroFlowHeightfield[index];
  return pos;
}

//----------------------------------------------------------------------------------------------------------------------
float Environment::GetZeroWindHeightFast(float x0, float y0) const
{
  unsigned i11, j11, i22, j22;
  unsigned i12, j12, i21, j21;
  
  const float plainHeight = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  mTerrain.GetIndex(x0, y0, i11, j11);
  
  i12 = i11;
  i21 = i11 + 1;
  i22 = i11 + 1;
  
  j21 = j11;
  j12 = j11 + 1;
  j22 = j11 + 1;
  
  IwAssert(ROWLHOUSE,  
    ((int) i11 < mTerrain.GetHeightfield().getSize()) &&
    ((int) i21 < mTerrain.GetHeightfield().getSize()) &&
    ((int) i12 < mTerrain.GetHeightfield().getSize()) &&
    ((int) i22 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j11 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j21 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j12 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j22 < mTerrain.GetHeightfield().getSize()) );
  
  Vector3 pos11 = GetZeroWindPosAt(i11, j11);
  float z11 = pos11.z;
  float z12 = GetZeroWindHeightAt(i12, j12);
  float z21 = GetZeroWindHeightAt(i21, j21);
  float z22 = GetZeroWindHeightAt(i22, j22);
  
  if (z11 < plainHeight)
    z11 = plainHeight;
  if (z12 < plainHeight)
    z12 = plainHeight;
  if (z21 < plainHeight)
    z21 = plainHeight;
  if (z22 < plainHeight)
    z22 = plainHeight;

  float i = (x0 - pos11.x) / mTerrain.GetHeightfield().getCellSize();
  float j = (y0 - pos11.y) / mTerrain.GetHeightfield().getCellSize();

  float z1 = z11 + i * (z21 - z11);
  float z2 = z12 + i * (z22 - z12);

  float z = z1 + j * (z2 - z1);
  IwAssert(ROWLHOUSE, z >= plainHeight);
  return z;
}


//----------------------------------------------------------------------------------------------------------------------
float Environment::GetZeroWindHeight(float x0, float y0) const
{
  IW_PROFILE("GetZeroWindHeight");
  unsigned i11, j11, i22, j22;
  unsigned i12, j12, i21, j21;
  
  const float plainHeight = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight;
  mTerrain.GetIndex(x0, y0, i11, j11);
  
  i12 = i11;
  i21 = i11 + 1;
  i22 = i11 + 1;
  
  j21 = j11;
  j12 = j11 + 1;
  j22 = j11 + 1;
  
  IwAssert(ROWLHOUSE,  
    ((int) i11 < mTerrain.GetHeightfield().getSize()) &&
    ((int) i21 < mTerrain.GetHeightfield().getSize()) &&
    ((int) i12 < mTerrain.GetHeightfield().getSize()) &&
    ((int) i22 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j11 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j21 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j12 < mTerrain.GetHeightfield().getSize()) &&
    ((int) j22 < mTerrain.GetHeightfield().getSize()) );
  
  Vector3 pos11 = GetZeroWindPosAt(i11, j11);
  Vector3 pos12 = GetZeroWindPosAt(i12, j12);
  Vector3 pos21 = GetZeroWindPosAt(i21, j21);
  Vector3 pos22 = GetZeroWindPosAt(i22, j22);
  
  if (pos11.z < plainHeight)
    pos11.z = plainHeight;
  if (pos12.z < plainHeight)
    pos12.z = plainHeight;
  if (pos21.z < plainHeight)
    pos21.z = plainHeight;
  if (pos22.z < plainHeight)
    pos22.z = plainHeight;

  int downwardSlope = (i11 + j11) % 2;
  return interpolateForZ(
    Vector3(x0, y0, 0.0f), pos11, pos12, pos21, pos22, downwardSlope);
}

// Wind height profile
float windPower = 0.16f;
float minAltitude = 0.1f;

//---------------------------------------------------------------------------------------------------------------------
static float GetWindSpeedAtHeight(float freeWindSpeed, float height, float boundaryLayerDepth)
{
  float altitude = ClampToRange(height, minAltitude, boundaryLayerDepth);
  float windSpeed = freeWindSpeed * powf(altitude / 5.0f, windPower);
  return windSpeed;
}

//---------------------------------------------------------------------------------------------------------------------
TurbulenceData Environment::PrepareTurbulenceData(const Vector3& pos)
{
  IW_PROFILE("PrepareTurbulenceData");
  const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;

  TurbulenceData turbulenceData;
  turbulenceData.mPosition = pos;
  turbulenceData.mTurbulence = Vector3(0,0,0);
  turbulenceData.mTurbulencePerX = Vector3(0,0,0);
  turbulenceData.mTurbulencePerY = Vector3(0,0,0);
  turbulenceData.mTurbulencePerZ = Vector3(0,0,0);

  if (es.mTurbulenceAmount < 0.0001f)
    return turbulenceData;

  Vector3 windUpDir(0,0,1);
  Vector3 windSideDir = windUpDir.Cross(mWindDirection);
  const float refDist = 2.0f;

  float x = pos.Dot(mWindDirection);
  float y = pos.Dot(windSideDir);
  float z = pos.Dot(windUpDir);

  float zeroWindHeight = GetZeroWindHeight(pos.x, pos.y);
  float terrainHeight = mTerrain.GetTerrainHeightFast(pos.x, pos.y, true);

  float terrainAltitude = ClampToRange(pos.z - terrainHeight, minAltitude, es.mBoundaryLayerDepth);

  float u0 = GetWindSpeedAtHeight(es.mWindSpeed, terrainAltitude, es.mBoundaryLayerDepth);
  float u1 = GetWindSpeedAtHeight(es.mWindSpeed, terrainAltitude + refDist, es.mBoundaryLayerDepth);

  // This represents the way that objects on the ground will mix in stationary air with air that is moving 
  // (i.e. relative difference is u). It will decrease with height.
  float groundAmount = es.mWindSpeed * es.mSurfaceTurbulence * 1.0f * (ClampToRange(1.0f - terrainAltitude/es.mBoundaryLayerDepth, 0.0f, 1.0f));
  Vector3 groundAndThermalTurbulence(groundAmount, groundAmount, 0.2f * groundAmount);

  Vector3 thermalWind = mThermalManager.GetThermalWindAtPosition(pos, terrainHeight);
  if (thermalWind.z > 0.0f)
  {
    float extraTurbulenceFromThermal = 1.0f * thermalWind.z;
    groundAndThermalTurbulence.z += extraTurbulenceFromThermal;
  }

  float overallScale = es.mTurbulenceAmount;
  if (pos.z < zeroWindHeight)
  {
    float deadAmount = es.mDeadAirTurbulence * es.mWindSpeed;
    groundAndThermalTurbulence += Vector3(1,1,1) * deadAmount;
  }
  // reduce with increasing wind to stop it getting out of control
  // scale = (1 - a * x) ^ n
  // Want approx
  // scale = 1 for speed = 5
  // scale = 0.25 for speed = 10
  // scale = 0.15 for speed = 20
  float scaleForMax = 0.15f;
  float windFrac = (es.mWindSpeed - 5.0f) / (20.0f - 5.0f);
  if (windFrac > 0.0f)
  {
    float n = 4.0f;
    float windScale = scaleForMax + (1.0f - scaleForMax) * powf(1.0f - windFrac, n);;
    overallScale *= windScale;
  }

#if 1
  float distScales[] = {8, 16, 32, 64, 128, 256};
  float amounts[]    = {1, 2, 3, 5, 7, 10};
#else
  //float distScales[] = {200.0f};
  //float amounts[]    = {2.0f};
#endif
  float angularAmount = 1.2f;
  float linearAmount = 0.02f;
  size_t numScales = sizeof(distScales)/sizeof(distScales[0]);
  for (size_t iScale = 0 ; iScale != numScales ; ++iScale)
  {
    float distScale = distScales[iScale];

    // Mix in velocity from a height distance that depends on the wavelength
    float aspectRatio = 0.1f;
    float shearVel = es.mShearTurbulence * (u1 - u0) * aspectRatio * distScale / refDist;
    const Vector3 shearTurbulence(shearVel, shearVel * 0.5f, shearVel * 0.2f);

    const Vector3 turbulence = groundAndThermalTurbulence + shearTurbulence;

    float amount = amounts[iScale] * overallScale;

    float k = 2.0f * PI / (distScale * refDist);
    float w = u0 * k;

    float angle = distScale; // arbitrary really
    float kx = k * FastSin(angle);
    float ky = k * FastCos(angle);

    // T = sin(kx*x-w*t)*sin(ky*y)
    // so
    // dTdx = kx*cos(kx*x-t*w)*sin(ky*y)
    // dTdy = ky*sin(kx*x-t*w)*cos(ky*y)
    // dTdz = 0

    float sinky = FastSin(ky*y);
    float sinkxtw = FastSin(kx*x - mTime*w);
    float cosky = FastCos(ky*y);
    float coskxtw = FastCos(kx*x - mTime*w);

    float T = sinkxtw * sinky;
    float dTdx = kx * coskxtw * sinky;
    float dTdy = ky * sinkxtw * cosky;
    float dTdz = k * sinkxtw * sinky;

    turbulenceData.mTurbulence     += (T    * amount * linearAmount)  * turbulence;
    turbulenceData.mTurbulencePerX += (dTdx * amount * angularAmount) * turbulence;
    turbulenceData.mTurbulencePerY += (dTdy * amount * angularAmount) * turbulence;
    //turbulenceData.mTurbulencePerZ += (dTdz * amount * angularAmount) * turbulence;
  }

  IwAssert(ROWLHOUSE, turbulenceData.mTurbulence == turbulenceData.mTurbulence);
  IwAssert(ROWLHOUSE, turbulenceData.mTurbulencePerX == turbulenceData.mTurbulencePerX);
  IwAssert(ROWLHOUSE, turbulenceData.mTurbulencePerY == turbulenceData.mTurbulencePerY);
  IwAssert(ROWLHOUSE, turbulenceData.mTurbulencePerZ == turbulenceData.mTurbulencePerZ);

  return turbulenceData;
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 Environment::GetWindDirection(uint32_t windType) const
{
  if (windType & WIND_TYPE_GUSTY)
  {
    return mGustyWindDirection;
  }
  else
  {
    return mWindDirection;
  }
}

//---------------------------------------------------------------------------------------------------------------------
float Environment::GetSurfaceRoughness(const Vector3& pos) const
{
  return PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mSurfaceRoughness;
}

//---------------------------------------------------------------------------------------------------------------------
float GetAdjustedWindSpeedBelowZeroWindHeight(float windSpeed, float z, float zeroWindHeight, float terrainHeight, float rotorTendency)
{
  float deadLayerDepth = zeroWindHeight - terrainHeight;
  float fracUp = (z - terrainHeight) / deadLayerDepth;
  fracUp = Square(fracUp);
  float multiplier = -FastCos(fracUp * PI);

  // If reversalAmount is 1, then a full rotor forms. If 0 then the normal wind profile is returned. 
  // If 0.5 then the wind almost reverses.
  float reversalAmount = rotorTendency * deadLayerDepth / 50.0f;
  reversalAmount = ClampToRange(reversalAmount, 0.0f, 0.6f);
  multiplier = 1.0f + (multiplier - 1.0f) * reversalAmount;

  return windSpeed * multiplier;
}


//---------------------------------------------------------------------------------------------------------------------
Vector3 Environment::GetWindAtPosition(const Vector3& pos, uint32_t windType, const TurbulenceData* turbulenceData) const
{
  IW_PROFILE("GetWindAtPosition");
  const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;

  const float minWindHeight = 0.5f;

  const float minWindLiftDistance = 1.0f;
  const float maxWindLiftDistance = 100.0f;
  float verticalWindDecayDistance = es.mVerticalWindDecayDistance;

  float freeWindSpeed;
  Vector3 windDirection;
  if (windType & WIND_TYPE_GUSTY)
  {
    freeWindSpeed = mGustyWindSpeed;
    windDirection = mGustyWindDirection;
  }
  else
  {
    freeWindSpeed = es.mWindSpeed;
    windDirection = mWindDirection;
  }

  float referenceDecayWindSpeed = 7.0f;
  if (freeWindSpeed > referenceDecayWindSpeed)
  {
    float decayScale = referenceDecayWindSpeed / freeWindSpeed;
    verticalWindDecayDistance *= decayScale;
  }

  Vector3 windLeftDirection(-windDirection.y, windDirection.x, 0.0f); 

  if (pos.z < es.mTerrainSettings.mPlainHeight)
    return mWindDirection * freeWindSpeed * 0.000001f;

  Vector3 wind = windDirection;

  float zeroWindHeight = GetZeroWindHeight(pos.x, pos.y);
  float terrainHeight = mTerrain.GetTerrainHeightFast(pos.x, pos.y, true);

  float altitudeAboveZeroWindHeight = Maximum(pos.z - zeroWindHeight, minWindHeight);

  float windLiftDist = altitudeAboveZeroWindHeight * es.mWindLiftSmoothing;

  windLiftDist = ClampToRange(windLiftDist, minWindLiftDistance, maxWindLiftDistance);

  // 0 is left, 1 is right
  Vector3 pos0 = pos + windLeftDirection * windLiftDist;
  Vector3 pos1 = pos - windLeftDirection * windLiftDist;

  Vector3 posUpwind0 = pos0 - windDirection * windLiftDist;
  Vector3 posDownwind0 = pos0 + windDirection * windLiftDist;
  Vector3 posUpwind1 = pos1 - windDirection * windLiftDist;
  Vector3 posDownwind1 = pos1 + windDirection * windLiftDist;

  float heightUpwind0 = GetZeroWindHeight(posUpwind0.x, posUpwind0.y);
  float heightDownwind0 = GetZeroWindHeight(posDownwind0.x, posDownwind0.y);

  float heightUpwind1 = GetZeroWindHeight(posUpwind1.x, posUpwind1.y);
  float heightDownwind1 = GetZeroWindHeight(posDownwind1.x, posDownwind1.y);

  float verticalComponent0 = (heightDownwind0 - heightUpwind0) / (windLiftDist * 2.0f);
  float verticalComponent1 = (heightDownwind1 - heightUpwind1) / (windLiftDist * 2.0f);

  // Blend to horizontal at the ground if below the zero wind height
  if (zeroWindHeight - terrainHeight > 0.01f)
  {
    float frac = (pos.z - terrainHeight) / (zeroWindHeight - terrainHeight);
    frac = ClampToRange(frac, 0.0f, 1.0f);
    verticalComponent0 *= frac;
    verticalComponent1 *= frac;
  }

  float verticalComponent = 0.5f * (verticalComponent0 + verticalComponent1);

  // Decay with height
  if (altitudeAboveZeroWindHeight > 0.0f)
    verticalComponent *= expf(-altitudeAboveZeroWindHeight / verticalWindDecayDistance);

  wind.z += verticalComponent;

  float sideFrac = 1.0f;
  wind += (sideFrac * (verticalComponent1 - verticalComponent0)) * windLeftDirection;

  float f = wind.z;
  wind.Normalise();
  wind.z = f;

  float slope = wind.z / hypotf(wind.x, wind.y);
  float windSpeed = GetWindSpeedAtHeight(freeWindSpeed, pos.z - terrainHeight, es.mBoundaryLayerDepth);

  if (pos.z < zeroWindHeight && (zeroWindHeight - terrainHeight) > 0.01f)
  {
    windSpeed = GetAdjustedWindSpeedBelowZeroWindHeight(windSpeed, pos.z, zeroWindHeight, terrainHeight, es.mRotorTendency);
  }
  else if (slope > 0.0f)
  {
    // reduce the horizontal wind (but not the vertical - compensate for the subsequent scale) in the region in front of a slope
    float scale = 1.0f / (1.0f + slope); 
    windSpeed *= scale;
    wind.z /= scale;
  }
  wind *= windSpeed;

  if (windType & WIND_TYPE_TURBULENT && es.mTurbulenceAmount > 0.0f)
  {
    const Vector3 windUpDir(0,0,1);
    const Vector3 windSideDir = windUpDir.Cross(mWindDirection);
    const Vector3 delta = pos - turbulenceData->mPosition;

    const float x = delta.Dot(mWindDirection);
    const float y = delta.Dot(windSideDir);
    const float z = delta.Dot(windUpDir);

    Vector3 turbulentWind = 
      turbulenceData->mTurbulencePerX * x + 
      turbulenceData->mTurbulencePerY * y + 
      turbulenceData->mTurbulencePerZ * z;
    
    Vector3 total = turbulentWind + turbulenceData->mTurbulence;
    const float maxTurbulenceWind = freeWindSpeed * 0.3f;
    total.x = ClampToRange(total.x, -maxTurbulenceWind, maxTurbulenceWind);
    total.y = ClampToRange(total.y, -maxTurbulenceWind, maxTurbulenceWind);
    total.z = ClampToRange(total.z, -maxTurbulenceWind, maxTurbulenceWind);
    wind += total;
  }

  if (windType & WIND_TYPE_THERMALS)
  {
    const Vector3 thermalWind = mThermalManager.GetThermalWindAtPosition(pos, terrainHeight);
    wind += thermalWind;
  }

  return wind;
}

//----------------------------------------------------------------------------------------------------------------------
void Environment::InitZeroFlowHeightfield(LoadingScreenHelper* loadingScreen)
{
  if (loadingScreen) loadingScreen->Update("Initialise wind");

  const EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;

  const float plainHeight = es.mTerrainSettings.mPlainHeight;
  const Heightfield::HeightfieldRuntime& heightfield = mTerrain.GetHeightfield();

  int n = heightfield.getSize();
  mZeroFlowHeightfield.resize(n*n);

  for (int i = 0 ; i < n ; ++i)
  {
    for (int j = 0 ; j < n ; ++j)
    {
      int index = Heightfield::calcIndex(i, j, n);
      float z = heightfield.getPos(i, j).z;
      if (z < plainHeight)
        z = plainHeight;
      mZeroFlowHeightfield[index] = z;
    }
  }

  if (loadingScreen) loadingScreen->Update();
  Vector3 windDir = Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH);
  int iStart = 0;
  int iEnd = n;
  int iIncr = 1;
  int jStart = 0;
  int jEnd = n;
  int jIncr = 1;
  if (windDir.x < 0.0f)
  {
    iStart = n-1; iEnd = -1; iIncr = -1;
  }
  if (windDir.y < 0.0f)
  {
    jStart = n-1; jEnd = -1; jIncr = -1;
  }

  const float delta = heightfield.getCellSize() / Maximum(fabsf(windDir.x), fabsf(windDir.y));

  // We look two points upwind. The zero wind height is allowed to have a maximum 2nd deriv - 
  // we adjust the point here so that is satisfied.

  // TODO make this depend on wind speed?
  float separationTendency = es.mSeparationTendency * ClampToRange(es.mWindSpeed / 20.0f, 0.00001f, 1.0f);
  
  // zero results in no rejoining. infinity is completely unseparated
  float maxD2ZDR2 = 0.004f / Maximum(separationTendency, 0.000001f);

  float offset = -maxD2ZDR2 * Square(delta);

  const int iterations = 2;
  for (int iteration = 0 ; iteration != iterations ; ++iteration)
  {
    for (int i = iStart ; i != iEnd ; i += iIncr)
    {
      if (loadingScreen && i % 16 == 0) loadingScreen->Update();
      for (int j = jStart ; j != jEnd ; j += jIncr)
      {
        int index = Heightfield::calcIndex(i, j, n);
        float& z = mZeroFlowHeightfield[index];

        Vector3 pos0 = GetZeroWindPosAt(i, j);

        Vector3 pos1 = pos0 - windDir * delta;
        Vector3 pos2 = pos0 - windDir * (2.0f * delta);

        float z1 = GetZeroWindHeight(pos1.x, pos1.y);
        float z2 = GetZeroWindHeight(pos2.x, pos2.y);

        IwAssert(ROWLHOUSE, z1 > -1000.0f && z1 < 1000.0f);
        IwAssert(ROWLHOUSE, z2 > -1000.0f && z2 < 1000.0f);

        // 2nd order FD has f''(x) = (f(x+2h) - 2 f(x+h) + f(x) ) / h^2
        float minAllowedZ = offset + 2.0f * z1 - z2;
        if (z < minAllowedZ)
          z = minAllowedZ;
      }
    }
  }

  int filterIterations = 4;
  for (int iteration = 0 ; iteration != filterIterations ; ++iteration)
  {
    iStart += iIncr;
    iEnd -= iIncr;
    jStart += jIncr;
    jEnd -= jIncr;
    for (int i = iStart ; i != iEnd ; i += iIncr)
    {
      if (loadingScreen && i % 10 == 0) loadingScreen->Update();
      for (int j = jStart ; j != jEnd ; j += jIncr)
      {
        int i0 = Heightfield::calcIndex(i, j, n);
        int i1 = Heightfield::calcIndex(i+1, j, n);
        int i2 = Heightfield::calcIndex(i-1, j, n);
        int i3 = Heightfield::calcIndex(i, j+1, n);
        int i4 = Heightfield::calcIndex(i, j-1, n);
        float& z = mZeroFlowHeightfield[i0];
        float z1 = mZeroFlowHeightfield[i1];
        float z2 = mZeroFlowHeightfield[i2];
        float z3 = mZeroFlowHeightfield[i3];
        float z4 = mZeroFlowHeightfield[i4];

        z = (z + z1 + z2 + z3 + z4) / 5.0f;
        if (z < plainHeight)
          z = plainHeight;
        float z0 = heightfield.getPos(i, j).z;
        if (z < z0)
          z = z0;
        IwAssert(ROWLHOUSE, z == z);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
float Environment::GetAirDensity(const Vector3& pos)
{
  const float h = pos.z;
  const float p0 = 101.325e3f;
  const float density0 = 1.225f;
  const float L = 0.0065f; // lapse rate
  const float T0 = 288.15f;
  const float g = 9.81f;
  const float M = 0.0289644f;
  const float R = 8.31447f;
  const float T = T0 - L * h;
  const float p = p0 * (powf(1.0f - L * h / T0, g * M / (R * L)));
  const float density = p * M / (R * T);
  return density;
}

