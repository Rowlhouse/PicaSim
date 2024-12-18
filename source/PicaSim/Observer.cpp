#include "Observer.h"
#include "Environment.h"
#include "PicaSim.h"
#include "Controller.h"
#include "AeroplanePhysics.h"

const float observerHeight = 1.65f;

static const float maxLookPitchAngle = PI * 0.5f;
static const float maxLookYawAngle = PI;

//---------------------------------------------------------------------------------------------------------------------
void Observer::Init(const Vector3& point, CameraTarget* target)
{
  TRACE_METHOD_ONLY(1);
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();

  mCameraOffset = Vector3(0,0,observerHeight);
  mTarget = target;
  Transform tm;
  tm.SetIdentity();
  tm.SetTrans(point);
  SetTransform(tm);
  mLookYaw = mLookPitch = mLookYawRate = mLookPitchRate = 0.0f;
  EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_PRE_PHYSICS);

  CLoad3DS load3DS;
  ThreeDSModel model;
  load3DS.Import3DS(&model, "SystemData/Objects/LaunchMarker.3ds");
  float modelScale = 0.00272f;
  mRenderModel.Init(model, Vector3(0,0,0), 0.0f, Vector3(modelScale, modelScale, modelScale), true, Colour(0, 0, 0, 0));

  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);
  mSkyGrid.Init();
}

//---------------------------------------------------------------------------------------------------------------------
void Observer::Terminate()
{
  TRACE_METHOD_ONLY(1);
  EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_PRE_PHYSICS);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);
  mRenderModel.Terminate();
  mSkyGrid.Terminate();
}

//---------------------------------------------------------------------------------------------------------------------
void Observer::SetTransform(const Transform& tm)
{
  EnvironmentSettings& es = PicaSim::GetInstance().GetSettings().mEnvironmentSettings;

  mTM = tm;
  if (es.mTerrainSettings.mType != TerrainSettings::TYPE_PANORAMA)
    Environment::GetTerrain().GetTerrainHeight(mTM.t, true);
  es.mObserverPosition = mTM.GetTrans();
}

//---------------------------------------------------------------------------------------------------------------------
Transform Observer::GetCameraTransform(void* cameraUserData) const
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const EnvironmentSettings& es = gs.mEnvironmentSettings;

  Transform tm(mTM);
  if (es.mTerrainSettings.mType != TerrainSettings::TYPE_PANORAMA)
    tm.t += mTM.RotateVec(mCameraOffset);

  float yaw = mLookYaw * maxLookYawAngle + DegreesToRadians(gs.mOptions.mGroundViewYawOffset);
  float pitch = mLookPitch * maxLookPitchAngle + DegreesToRadians(gs.mOptions.mGroundViewPitchOffset);
  Transform yawTM;
  yawTM.SetRotZ(yaw);
  Transform pitchTM;
  pitchTM.SetRotY(pitch);
  return pitchTM * yawTM * tm;
}

//---------------------------------------------------------------------------------------------------------------------
void RemoveDeadZone(float&v, float d)
{
  if (v > 0.0f)
  {
    if (v < d)
      v = 0.0f;
    else
      v = (v - d) / (1.0f - d);
  }
  else 
  {
    if (v > -d)
      v = 0.0f;
    else
      v = (v + d) / (1.0f - d);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void Observer::EntityUpdate(float deltaTime, int entityLevel)
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();
  EnvironmentSettings& es = gs.mEnvironmentSettings;
  if (
    PicaSim::GetInstance().GetMode() != PicaSim::MODE_WALK ||
    PicaSim::GetInstance().GetStatus() != PicaSim::STATUS_PAUSED ||
    !gs.mOptions.mEnableWalkabout
    )
  {
    float windBearing = es.mWindBearing;
    mTM.SetRotZ(DegreesToRadians(-windBearing), false, true);
    mTM.SetTrans(es.mObserverPosition);
    if (es.mTerrainSettings.mType != TerrainSettings::TYPE_PANORAMA)
      Environment::GetTerrain().GetTerrainHeight(mTM.t, true);

    // Smooth lookaround
    float realTimeDelta = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
    float yaw = PicaSim::GetInstance().GetPlayerAeroplane()->GetController().GetControl(Controller::CHANNEL_LOOKYAW);
    float pitch = -PicaSim::GetInstance().GetPlayerAeroplane()->GetController().GetControl(Controller::CHANNEL_LOOKPITCH);
    SmoothCD(mLookYaw, mLookYawRate, realTimeDelta, yaw, 0.5f);
    SmoothCD(mLookPitch, mLookPitchRate, realTimeDelta, pitch, 0.5f);
  }
  else
  {
    const Controller& controller = PicaSim::GetInstance().GetPlayerAeroplane()->GetController();
    float x = controller.GetControl(Controller::CHANNEL_AILERONS);
    float y = controller.GetControl(Controller::CHANNEL_ELEVATOR);
    float z = controller.GetControl(Controller::CHANNEL_LOOKYAW);
    float p = controller.GetControl(Controller::CHANNEL_LOOKPITCH);

    RemoveDeadZone(x, 0.2f);
    RemoveDeadZone(y, 0.2f);
    RemoveDeadZone(z, 0.2f);

    x = x * fabsf(x);
    y = y * fabsf(y);
    z = 0.3f * z * fabsf(z);

    // Allow movement even when paused
    deltaTime = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();

    const float speed = 2.0f;
    float d = speed * deltaTime;
    Vector3 delta = mTM.RowX() * y - mTM.RowY() * z;
    mTM.t += delta * speed;

    const float angSpeed = 0.1f;
    Transform tm;
    tm.SetAxisAngle(Vector3(0,0,1), -angSpeed * x);
    mTM = tm.PostMult(mTM);
    mTM.Normalise();

    Vector3 faceDir = mTM.RowX();
    faceDir.z = 0.0f;
    if (faceDir.GetLength() > 0.01f)
    {
      faceDir.Normalise();
      faceDir.z = p;
      faceDir.Normalise();
      Vector3 leftDir = mTM.RowY();
      leftDir.z = 0.0f;
      leftDir.Normalise();
      Vector3 upDir = faceDir.Cross(leftDir).GetNormalised();
      SetRowX(mTM, faceDir);
      SetRowY(mTM, leftDir);
      SetRowZ(mTM, upDir);
    }

    Environment::GetTerrain().GetTerrainHeight(mTM.t, true);

    // Set the wind to be blowing in the opposite direction
    if (
      gs.mOptions.mSetWindDirectionOnWalkabout && 
      gs.mChallengeSettings.mAllowEnvironmentSettings
      )
    {
      float windBearing = RadiansToDegrees(atan2f(mTM.RowX().y, mTM.RowX().x));
      es.mWindBearing = windBearing;
    }
    es.mObserverPosition = mTM.GetTrans();
  }
}

//---------------------------------------------------------------------------------------------------------------------
void Observer::RenderUpdate(Viewport* viewport, int renderLevel)
{
  TRACE_METHOD_ONLY(2);
  const Options& options = PicaSim::GetInstance().GetSettings().mOptions;

  mSkyGrid.Enable(false);
  if (PicaSim::GetInstance().GetMode() == PicaSim::MODE_GROUND)
  {
    if (viewport == &PicaSim::GetInstance().GetMainViewport() && options.mSkyGridOverlay != Options::SKYGRID_NONE)
    {
      float alignmentAngle = 0.0f;
      switch (options.mSkyGridAlignment)
      {
      case Options::SKYGRIDALIGN_ALONGWIND:
        {
          Vector3 alignmentDir = Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH);
          alignmentAngle = btAtan2Fast(alignmentDir.x, alignmentDir.y);
        }
        break;
      case Options::SKYGRIDALIGN_CROSSWIND:
        {
          Vector3 alignmentDir = Environment::GetInstance().GetWindDirection(Environment::WIND_TYPE_SMOOTH).Cross(Vector3(0,0,1));
          alignmentAngle = btAtan2Fast(alignmentDir.x, alignmentDir.y);
        }
        break;
      case Options::SKYGRIDALIGN_ALONGRUNWAY:
        alignmentAngle = DegreesToRadians(90.0f + PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mRunwayAngle);
        break;
      case Options::SKYGRIDALIGN_CROSSRUNWAY:
        alignmentAngle = DegreesToRadians(PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mRunwayAngle);
        break;
      }
      Transform tm;
      tm.SetAxisAngle(Vector3(0,0,1), -alignmentAngle);
      Vector3 pos = mTM.GetTrans();
      const Aeroplane* playerAeroplane = PicaSim::GetInstance().GetPlayerAeroplane();
      if (playerAeroplane->GetPhysics()->GetTetherHandlePos(pos))
      {
        pos.z = Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x,pos.y, true);
      }
      tm.SetTrans(pos);
      mSkyGrid.SetTransform(tm);
      mSkyGrid.Enable(true);
      if (options.mSkyGridOverlay == Options::SKYGRID_SPHERE)
        mSkyGrid.SetSphere(options.mSkyGridDistance);
      else if (options.mSkyGridOverlay == Options::SKYGRID_BOX)
        mSkyGrid.SetBox(options.mSkyGridDistance);
    }
    return;
  }

  if (!options.mDrawLaunchMarker)
    return;

#if 0
  RenderManager::GetInstance().GetDebugRenderer().DrawPoint(mTM.GetTrans(), observerHeight, Vector3(1,0,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawPoint(mTM.GetTrans() + Vector3(0,0,observerHeight), observerHeight, Vector3(0,1,0));
#endif

  if (mRenderModel.IsCreated())
  {
    EnableLighting enableLighting;
    glDisable(GL_BLEND);

    Transform TM = mTM;
    TM.SetTrans(TM.GetTrans() + Vector3(0,0,-0.25f));

    esPushMatrix();
    GLMat44 glTM;
    ConvertTransformToGLMat44(TM, glTM);
    esMultMatrixf(&glTM[0][0]);

    ROTATE_90_X;
    ROTATE_90_Y;

    mRenderModel.Render(0, false, options.mSeparateSpecular);

    esPopMatrix();
  }
}

//---------------------------------------------------------------------------------------------------------------------
float Observer::GetRenderBoundingRadius() const
{
  return observerHeight;
}
