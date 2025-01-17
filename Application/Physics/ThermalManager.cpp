#include "ThermalManager.h"
//#include "PicaSim.h"
//#include "AeroplaneGraphics.h"
//#include "Environment.h"

#define DEBUG_DRAWx

//---------------------------------------------------------------------------------------------------------------------
static void LoadModel(RenderModel& renderModel, const char* file)
{
  ACModel model;
  if (ACLoadModel(model, file))
    renderModel.Init(model, file, Vector3(0,0,0), 0.0f, Vector3(1.0f, 1.0f, 1.0f), true, false);
}

//---------------------------------------------------------------------------------------------------------------------
ThermalManager::ThermalManager() : mRandom(time(0))
{
}

//---------------------------------------------------------------------------------------------------------------------
void ThermalManager::Init(LoadingScreenHelper* loadingScreen)
{
  TRACE_METHOD_ONLY(1);
  mPreviousDensity = -1.0f;

  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);

  loadingScreen->Update("Thermals");
  LoadModel(mRenderModel[0], "SystemData/Objects/Eagle0.ac");
  loadingScreen->Update();
  LoadModel(mRenderModel[1], "SystemData/Objects/Eagle1.ac");
  loadingScreen->Update();
  LoadModel(mRenderModel[2], "SystemData/Objects/Eagle2.ac");
  loadingScreen->Update();
  LoadModel(mRenderModel[3], "SystemData/Objects/Eagle3.ac");
  loadingScreen->Update();
}

//---------------------------------------------------------------------------------------------------------------------
void ThermalManager::Terminate()
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);

  mRenderModel[0].Terminate();
  mRenderModel[1].Terminate();
  mRenderModel[2].Terminate();
  mRenderModel[3].Terminate();
}

//---------------------------------------------------------------------------------------------------------------------
const ThermalManager::Thermal* ThermalManager::GetNearestThermal(const Vector3& pos, float& bestDist) const
{
  float minDistSq = FLT_MAX;
  bestDist = FLT_MAX;
  const Thermal* bestThermal = 0;
  for (Thermals::const_iterator it = mThermals.begin() ; it != mThermals.end() ; ++it)
  {
    const Thermal& thermal = *it;
    float d2 = Square(thermal.mPos.x - pos.x) + Square(thermal.mPos.y - pos.y);
    if (d2 < minDistSq)
    {
      minDistSq = d2;
      bestThermal = &thermal;
      bestDist = sqrtf(minDistSq);
    }
  }
  return bestThermal;
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 ThermalManager::GetNearestThermalPosition(const Vector3& pos, float& bestDist) const
{
  bestDist = FLT_MAX;
  const Thermal* bestThermal = GetNearestThermal(pos, bestDist);
  if (bestThermal)
    return bestThermal->mPos;
  else
    return pos;
}


//---------------------------------------------------------------------------------------------------------------------
float ThermalManager::GetHorizontalDistanceSquaredToNearestThermal(const Vector3& pos) const
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings& es = gs.mEnvironmentSettings;

  float minDistSq = Square(es.mThermalRange);
  for (Thermals::const_iterator it = mThermals.begin() ; it != mThermals.end() ; ++it)
  {
    const Thermal& thermal = *it;
    float d2 = Square(thermal.mPos.x - pos.x) + Square(thermal.mPos.y - pos.y);
    if (d2 < minDistSq)
      minDistSq = d2;
  }
  return minDistSq;
}


//---------------------------------------------------------------------------------------------------------------------
void ThermalManager::Update(float dt, const Vector3& centre)
{
  IW_PROFILE("ThermalManager::Update");
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings& es = gs.mEnvironmentSettings;
  const LightingSettings& ls = gs.mLightingSettings;

  // See if we need to add another thermal
  const float thermalActivity = 
    gs.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA ? 1.0f : ls.mThermalActivity;
  const float thermalDensity = thermalActivity * es.mThermalDensity / 1e6f;

  float currentDensity = mThermals.size() / Square(es.mThermalRange);

  bool initialise = false;
  if (thermalDensity != mPreviousDensity)
  {
    mPreviousDensity = thermalDensity;
    currentDensity = 0.0f;
    mThermals.clear();
    initialise = true;
  }

  IW_PROFILE_START("Add thermals");
  const float hr2 = es.mThermalRange * 0.5f;
  float minDistSquared = Square(es.mThermalAverageDowndraftRadius);
  while (currentDensity < thermalDensity)
  {
    // Note - order of arg evaluation is not well defined
    float r1 = mRandom.GetValue(-hr2, hr2);
    float r2 = mRandom.GetValue(-hr2, hr2);
    Vector3 offset(r1, r2, 0.0f);

    Thermal thermal;
    thermal.mPos = centre + offset;

    float d2 = GetHorizontalDistanceSquaredToNearestThermal(thermal.mPos);
    float ran = mRandom.GetValue();
    if (ran > d2/minDistSquared)
      continue;

    Environment::GetInstance().GetTerrain().GetTerrainHeight(thermal.mPos, true);

    float size = mRandom.GetValue(0.5f, 1.5f);

    thermal.mLifespan = es.mThermalAverageLifeSpan * size;
    thermal.mDepth = es.mThermalAverageDepth * size;
    thermal.mRadius1 = es.mThermalAverageCoreRadius * size;
    thermal.mRadius2 = thermal.mRadius1 * 1.2f;
    thermal.mRadius3 = thermal.mRadius2 + es.mThermalAverageDowndraftRadius * size;
    thermal.mUpdraftSpeed = thermalActivity * es.mThermalAverageUpdraftSpeed * size;
    thermal.mVerticalSpeed = thermalActivity * es.mThermalAverageAscentRate * size;
    thermal.mThermalExpansionOverLifespan = es.mThermalExpansionOverLifespan;
    thermal.mTrackerAngleOffset = size * PI * 2.0f;

    if (initialise)
    {
      thermal.mAge = mRandom.GetValue(0.0f, thermal.mLifespan);
      thermal.mPos.z += thermal.mVerticalSpeed * thermal.mAge;
    }
    else
    {
      thermal.mAge = 0.0f;
    }

    mThermals.push_back(thermal);
    currentDensity = mThermals.size() / Square(es.mThermalRange);
  }
  IW_PROFILE_STOP();

  // Use this to profile with a fixed set of thermals
  //dt *= 0.0f;

  // Update all thermals
  IW_PROFILE_START("Update thermals");
  const Vector3 dirToLight = -RenderManager::GetInstance().GetLightingDirection().GetNormalised();
  for (Thermals::iterator it = mThermals.begin() ; it != mThermals.end() ;)
  {
    Thermal& thermal = *it;

    thermal.mAge += dt;
    if (thermal.mAge > thermal.mLifespan)
    {
      it = mThermals.erase(it);
      continue;
    }

    // Make the thermal distribution wrap around in space
    Vector3 origPos = thermal.mPos;
    bool recalcZ = false;
    if (thermal.mPos.x > centre.x + hr2)
    {
      thermal.mPos.x -= es.mThermalRange;
      recalcZ = true;
    }
    else if (thermal.mPos.x < centre.x - hr2)
    {
      thermal.mPos.x += es.mThermalRange;
      recalcZ = true;
    }
    if (thermal.mPos.y > centre.y + hr2)
    {
      thermal.mPos.y -= es.mThermalRange;
      recalcZ = true;
    }
    else if (thermal.mPos.y < centre.y - hr2)
    {
      thermal.mPos.y += es.mThermalRange;
      recalcZ = true;
    }
    if (recalcZ)
    {
      float origAlt = Environment::GetInstance().GetTerrain().GetTerrainHeight(origPos.x, origPos.y, true);
      thermal.mPos.z = Environment::GetInstance().GetTerrain().GetTerrainHeight(thermal.mPos.x, thermal.mPos.y, true) + origAlt;
    }

    // Cycle the activity over time
    {
      float ageFrac = thermal.mAge / thermal.mLifespan;
      thermal.mActivity = FastSin(ageFrac * PI);

      Vector3 wind = Environment::GetInstance().GetWindAtPosition(thermal.mPos, Environment::WIND_TYPE_SMOOTH);
      thermal.mPos += wind * dt;

      thermal.mPos.z += thermal.mVerticalSpeed * dt;
    }

    // Scale by the lighting angle
    {
      Vector3 terrainPos;
      Vector3 terrainNormal;
      Environment::GetTerrain().GetLocalTerrain(thermal.mPos, terrainPos, terrainNormal, true);
      thermal.mActivity *= ClampToRange(dirToLight.Dot(terrainNormal), 0.0f, 1.0f);
    }

    // Weaken thermals over water
    if (!es.mTerrainSettings.mCollideWithPlain)
    {
      const float plainHeight = es.mTerrainSettings.mPlainHeight;
      float heightfieldHeight = Environment::GetTerrain().GetTerrainHeightFast(thermal.mPos.x, thermal.mPos.y, false);
      float transitionDepth = 2.0f;
      float scale = (heightfieldHeight - plainHeight) / transitionDepth;
      scale = ClampToRange(scale, 0.0f, 1.0f);
      thermal.mActivity *= scale;
    }

    ++it;
  }
  IW_PROFILE_STOP();
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 ThermalManager::GetThermalWindAtPosition(const Thermal& thermal, const Vector3& pos, float terrainHeight) const
{
#ifdef DEBUG_DRAW
  DebugRenderer& debugRenderer = RenderManager::GetInstance().GetDebugRenderer();
#endif

  Vector3 horDirFromThermal = pos - thermal.mPos;
  float heightAboveThermal = horDirFromThermal.z;
  horDirFromThermal.z = 0.0f;
  float horDistFromThermalSq = Square(horDirFromThermal.x) + Square(horDirFromThermal.y);
  if (horDistFromThermalSq > Square(thermal.GetRadius3()))
    return Vector3(0,0,0);
  if (horDistFromThermalSq < 0.00001f)
    return Vector3(0,0,0);
  float horDistFromThermal = sqrtf(horDistFromThermalSq);

  float thermalDepth = thermal.GetDepth();
  if (thermalDepth > thermal.mPos.z - terrainHeight)
    thermalDepth = thermal.mPos.z - terrainHeight;
  if (thermalDepth < 0.1f)
    return Vector3(0,0,0);

  float heightScale = 1.0f - Square(heightAboveThermal / thermalDepth);
  if (heightScale < 0.001f)
    return Vector3(0,0,0);
  heightScale = sqrtf(heightScale);
  horDirFromThermal *= 1.0f/horDistFromThermal;

  float updraft = 0.0f;
  if (horDistFromThermal > thermal.GetRadius2())
  {
    const float frac = (horDistFromThermal - thermal.GetRadius2()) / (thermal.GetRadius3() - thermal.GetRadius2());
    updraft = (frac - 1.0f) * thermal.GetDowndraftSpeed();
  }
  else if (horDistFromThermal > thermal.GetRadius1())
  {
    const float frac = (horDistFromThermal - thermal.GetRadius1()) / (thermal.GetRadius2() - thermal.GetRadius1());
    updraft = -frac * thermal.GetDowndraftSpeed();
  }
  else
  {
    const float frac = sqrtf(1.0f - horDistFromThermal / thermal.GetRadius1());
    updraft = thermal.GetCoreUpdraftSpeed() * frac;
#ifdef DEBUG_DRAW
    debugRenderer.DrawLine(pos, thermal.mPos, Vector3(1,1,1));
    Vector3 thermalPos = thermal.mPos;
    thermalPos.z = pos.z;
    debugRenderer.DrawLine(pos, thermalPos, Vector3(1,0,0));
    float dist = thermal.GetRadius1() - horDistFromThermal;
    debugRenderer.DrawVector(pos, horDirFromThermal * dist, Vector3(0,1,0));
#endif
  }

  updraft *= heightScale;

  Vector3 thermalWind(0,0,updraft);

  // inflow
  float inflowFracVert = -FastSin(PI * ClampToRange(heightAboveThermal / thermalDepth, -1.0f, 1.0f));
  float inflowFracHor = FastSin(PI * Minimum(sqrtf(horDistFromThermal / thermal.GetRadius3()), 1.0f));
  float inflow = 0.5f * inflowFracHor * inflowFracVert * thermal.mUpdraftSpeed;
  thermalWind -= horDirFromThermal * inflow * thermal.mActivity;

  return thermalWind;
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 ThermalManager::GetThermalWindAtPosition(const Vector3& pos, float terrainHeight) const
{
  IW_PROFILE("GetThermalWindAtPosition");

  Vector3 thermalWind(0,0,0);
  for (Thermals::const_iterator it = mThermals.begin(), itEnd = mThermals.end() ; it != itEnd ; ++it)
  {
    const Thermal& thermal = *it;
    thermalWind += GetThermalWindAtPosition(thermal, pos, terrainHeight);
  }

  return thermalWind;
}

//---------------------------------------------------------------------------------------------------------------------
void ThermalManager::RenderUpdate(class Viewport* viewport, int renderLevel)
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const Options& options = gs.mOptions;

  if (gs.mOptions.mMaxMarkersPerThermal > 0)
  {
    EnableLighting enableLighting;

    ShaderProgramModelInfo shaderInfo;

    for (int i = 0 ; i != NUM_LODS ; ++i)
      mRenderDetails[i].resize_quick(0);

    const float gravitationalAcceleration = EntityManager::GetInstance().GetDynamicsWorld().getGravity().length();
    const float flightSpeed = 10.0f;

    const Camera& camera = *viewport->GetCamera();
    float cameraAngle = camera.GetVerticalFOV();
    float convertObjectAngleToPixels = viewport->GetHeight() * gs.mOptions.mFrameworkSettings.mScreenHeight / cameraAngle;
    const Vector3 cameraLookDir = camera.GetLookDir();
    float FOV = 0.5f * Maximum(camera.GetVerticalFOV(), camera.GetHorizontalFOV());
    const float cosCameraFOV = FastCos(FOV);

    for (Thermals::const_iterator it = mThermals.begin() ; it != mThermals.end() ; ++it)
    {
      const Thermal& thermal = *it;

      int num = (int) (thermal.mActivity * (gs.mOptions.mMaxMarkersPerThermal + 1));
      num = ClampToRange(num, 1, gs.mOptions.mMaxMarkersPerThermal);
      float radii[] = {0.4f, 0.47f, 0.38f, 0.43f, 0.35f, 0.45f};

      for (int iBird = 0 ; iBird != num ; ++iBird)
      {
        float radius = thermal.GetRadius1() * radii[iBird];
        float angle = radii[iBird] + thermal.mTrackerAngleOffset + flightSpeed * thermal.mAge / radius;
        Vector3 pos = thermal.mPos;
        pos.x += radius * FastCos(angle);
        pos.y += radius * FastSin(angle);
        pos.z += iBird * 5.0f;

        // Avoid really low birds as (a) it looks silly and (b) they can intersect the hill more easily
        pos.z += 10.0f;

        Vector3 cameraToBirdDir = pos - camera.GetPosition();
        float distance = cameraToBirdDir.GetLength();
        cameraToBirdDir *= 1.0f/distance;
        if (cameraToBirdDir.Dot(cameraLookDir) < cosCameraFOV)
          continue;

        RenderDetail renderDetail;
        renderDetail.pos = pos;
        renderDetail.radius = radius;
        renderDetail.angle = angle;

        // Decide which LOD this is
        float renderRadius = 2.0f;
        float objectAngle = 2.0f * renderRadius / distance;

        float objectPixels = objectAngle * convertObjectAngleToPixels;
        int LOD;
        if (objectPixels > 80)
          LOD = 0;
        else if (objectPixels > 20)
          LOD = 1;
        else if (objectPixels > 8)
          LOD = 2;
        else 
          LOD = 3;

        if (camera.isSpherePartlyInFrustum(pos, radius + mRenderModel[LOD].GetBoundingRadius()))
        {
          mRenderDetails[LOD].push_back(renderDetail);
        }
      }

      // Now we've got all the details, loop over the lods

      for (int iLOD = 0 ; iLOD != NUM_LODS ; ++iLOD)
      {
        mRenderModel[iLOD].PartRenderPre(0, false, shaderInfo, 0, options.mSeparateSpecular);

        uint32 num = mRenderDetails[iLOD].size();
        for (uint32 iDetail = 0 ; iDetail != num ; ++iDetail)
        {
          const RenderDetail& renderDetail = mRenderDetails[iLOD][iDetail];

          float centripetalAcc = Square(flightSpeed) / renderDetail.radius;
          float roll = asinf(centripetalAcc / gravitationalAcceleration);

          esPushMatrix();
          esTranslatef(renderDetail.pos.x, renderDetail.pos.y, renderDetail.pos.z);

          esRotatef(renderDetail.angle * 180.0f / PI, 0, 0, 1);
          ROTATE_90_X;
          esRotatef(roll * 180.0f / PI,  0, 0, 1);

          mRenderModel[iLOD].PartRender(0, false, shaderInfo, 0);

          esPopMatrix();
        }
        mRenderModel[1].PartRenderPost(0, false, 0);
      }
    }

    glDisable(GL_COLOR_MATERIAL);
  }

  if (gs.mOptions.mDrawThermalWindField)
  {
    float deltaTime = 20.0f;
    float bestDist = 0.0f;
    const Camera& camera = *viewport->GetCamera();
    const CameraTarget* cameraTarget = camera.GetCameraTarget();
    Vector3 cameraPosition = camera.GetPosition();
    float radius, junk;
    const Thermal* thermal = GetNearestThermal(
      cameraTarget ? cameraTarget->GetCameraTargetPosition(cameraPosition, 0, radius, junk) : cameraPosition, bestDist);
    if (thermal)
    {
      DebugRenderer& debugRenderer = RenderManager::GetInstance().GetDebugRenderer();

      //Vector3 side = camera.GetLookDir().Cross(Vector3(0,0,1)).GetNormalised();
      //Vector3 side(1,0,0);
      Vector3 side = Environment::GetInstance().GetWindAtPosition(thermal->mPos, Environment::WIND_TYPE_SMOOTH);
      side.z = 0.0f; side.Normalise();
      const size_t numPts = 20;
      const size_t numHeights = 10;
      for (size_t iHeight = 0 ; iHeight != numHeights ; ++iHeight)
      {
        float heightFrac = 2.0f * (iHeight / (numHeights - 1.0f)) - 1.0f;
        float dz = 0.9f * heightFrac * thermal->GetDepth();
        for (size_t i = 0 ; i != numPts ; ++i)
        {
          float radiusFrac = 2.0f * (i / (numPts - 1.0f)) - 1.0f;
          float r = radiusFrac * thermal->GetRadius3();
          Vector3 pos = thermal->mPos + side * r;
          pos.z += dz;
          float terrainHeight = Environment::GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true);
          Vector3 thermalWind = GetThermalWindAtPosition(*thermal, pos, terrainHeight);
          Vector3 colour = thermalWind.z > 0.0f ? Vector3(0,1,0) : Vector3(1,0,0);
          if (i == 0)
            colour = Vector3(0,0,1);
          debugRenderer.DrawArrow(pos, pos + thermalWind * deltaTime, colour);
        }
      }
    }
  }
}

