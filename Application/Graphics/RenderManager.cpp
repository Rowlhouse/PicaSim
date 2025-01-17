#include "RenderManager.h"
#include "FonctionsMarmelade.h"
#include "Graphics.h"
#include "Helpers.h"

#include "DebugRenderer.h"
//#include "RenderObject.h"
//#include "RenderOverlayObject.h"
#include "Trace.h"
#include "Viewport.h"
//#include "LoadingScreenHelper.h"

//#include <IwGL.h>
//#include <IwGx.h>

RenderManager* RenderManager::mInstance = 0;


//---------------------------------------------------------------------------------------------------------------------
RenderManager& RenderManager::GetInstance()
{
  IwAssert(ROWLHOUSE, mInstance != 0);
  return *mInstance;
}

//---------------------------------------------------------------------------------------------------------------------
void RenderManager::SetLightingDirection(float bearing, float elevation)
{
  mLightingDirection = Vector3(-1.0f, 0.0f, 0.0f);
  Quat q(Vector3(0,1,0), DegreesToRadians(elevation));
  mLightingDirection = q.RotateVector(mLightingDirection);
  q = Quat(Vector3(0,0,1), DegreesToRadians(bearing));
  mLightingDirection = q.RotateVector(mLightingDirection);
}


//---------------------------------------------------------------------------------------------------------------------
RenderManager::RenderManager(FrameworkSettings& frameworkSettings)
  : mFrameworkSettings(frameworkSettings)
{
  mDebugRenderer = new DebugRenderer;
  mLightingAmbientColour = Vector3(0.3f, 0.3f, 0.3f);
  mLightingDiffuseColour = Vector3(1.0f, 1.0f, 1.0f);
  mShadowStrength = 0.2f;
  mShadowDecayHeight = 40.0f;
  mShadowSizeScale = 1.3f;
  mEnableStereoscopy = false;
  mStereoSeparation = 0.0f;

  float lightBearing   = 0.0f;
  float lightElevation = 0.0f;
  SetLightingDirection(lightBearing, lightElevation);
}

RenderManager::~RenderManager()
{
  delete mDebugRenderer;
}


//---------------------------------------------------------------------------------------------------------------------
void RenderManager::Init(FrameworkSettings& frameworkSettings, LoadingScreenHelper* loadingScreen)
{
  TRACE_FUNCTION_ONLY(1);
  IwAssert(ROWLHOUSE, !mInstance);
  mInstance = new RenderManager(frameworkSettings);

  // Get dimensions from IwGL
  int width =  s3eSurfaceGetInt(S3E_SURFACE_DEVICE_WIDTH);
  int height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
  GLint depthBits = 0;
  glGetIntegerv( GL_DEPTH_BITS, &depthBits);
  TRACE_FILE_IF(1) TRACE("Screen BPP  : %d\n", s3eSurfaceGetInt(S3E_SURFACE_PIXEL_TYPE) & S3E_SURFACE_PIXEL_SIZE_MASK);
  TRACE_FILE_IF(1) TRACE("Screen Size : %dx%d\n", width, height);
  TRACE_FILE_IF(1) TRACE("\n");
  TRACE_FILE_IF(1) TRACE( "Vendor     : %s\n", (const char*)glGetString( GL_VENDOR ) );
  TRACE_FILE_IF(1) TRACE( "Renderer   : %s\n", (const char*)glGetString( GL_RENDERER ) );
  TRACE_FILE_IF(1) TRACE( "Version    : %s\n", (const char*)glGetString( GL_VERSION ) );
  TRACE_FILE_IF(1) TRACE( "Extensions : %s\n", (const char*)glGetString( GL_EXTENSIONS ) );
  TRACE_FILE_IF(1) TRACE( "Depth bits : %d\n", depthBits );
  TRACE_FILE_IF(1) TRACE("\n");

  //////////////////////////////////////////////////////////////////////////////////////////////////IwGxPrintSetColour(128, 128, 128);

  mInstance->mDebugRenderer->Init();
}

//---------------------------------------------------------------------------------------------------------------------
void RenderManager::Terminate()
{
  TRACE_FUNCTION_ONLY(1);
  IwAssert(ROWLHOUSE, mInstance);

  for (Viewports::iterator it = mInstance->mViewports.begin(); it != mInstance->mViewports.end() ; ++it)
    delete *it;
  mInstance->mViewports.clear();

  for (Cameras::iterator it = mInstance->mCameras.begin() ; it != mInstance->mCameras.end() ; ++it)
    delete *it;
  mInstance->mCameras.clear();
  mInstance->mDebugRenderer->Terminate();

  delete mInstance;
  mInstance = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void RenderManager::Update(float realDeltaTime, float gameDeltaTime)
{
  for (Cameras::iterator it = mCameras.begin() ; it != mCameras.end() ; ++it)
  {
    (*it)->RenderUpdate(gameDeltaTime);
  }
  for (RenderOverlayObjects::iterator it = mRenderOverlayObjects.begin() ; it != mRenderOverlayObjects.end() ; ++it)
  {
    (it->second)->RenderOverlayUpdate(realDeltaTime);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void RenderManager::SetupLighting()
{
  esMatrixMode( GL_MODELVIEW );

  GLfloat zeros[] = {0, 0, 0, 0};
  GLfloat diffuseColour[]  = {mLightingDiffuseColour.x, mLightingDiffuseColour.y, mLightingDiffuseColour.z, 1.0f};
  GLfloat ambientColour[]  = {mLightingAmbientColour.x, mLightingAmbientColour.y, mLightingAmbientColour.z, 1.0f};
  GLfloat specularColour[] = {mLightingDiffuseColour.x, mLightingDiffuseColour.y, mLightingDiffuseColour.z, 1.0f};

  // Jitter the light position otherwise OpenGL/Marmalade doesn't register it has changed - 
  // even though it has if the modelview matrix has changed!
  float t = gGLVersion == 1 ? 0.001f * rand()/float(RAND_MAX) : 0.0f;
  GLfloat lightPos[] = {-mLightingDirection.x + t, -mLightingDirection.y, -mLightingDirection.z, 0.0f};

  // set the light position (especially) after setting the viewpoint,
  // so that it is fixed
  if (gGLVersion == 1)
  {
    glEnable(GL_LIGHTING);

    GLint numLights = 1;
    glGetIntegerv(GL_MAX_LIGHTS, &numLights);
    for (int i = 0 ; i != numLights ; ++i)
      glDisable(GL_LIGHT0 + i);

    glEnable(GL_LIGHT0);
    if (mFrameworkSettings.mUseMultiLights)
    {
      for (int i = 1 ; i != 5 ; ++i)
        glEnable(GL_LIGHT0 + i);
    }
  }
  esSetLightPos(GL_LIGHT0, lightPos);
  esSetLightDiffuseColour(GL_LIGHT0, diffuseColour);
  esSetLightSpecularColour(GL_LIGHT0, specularColour);
  if (mFrameworkSettings.mUseMultiLights)
  {
    esSetLightAmbientColour(GL_LIGHT0, zeros);
    float underneathFrac = 1.1f;
    GLfloat sideAmbient[]= {underneathFrac * ambientColour[0], underneathFrac * ambientColour[1], underneathFrac * ambientColour[2], 1.0f};
    for (int i = 1 ; i != 4 ; ++i)
    {
      float angle = ( i / 3.0f) * TWO_PI;
      GLfloat pos[] = {sinf(angle), cosf(angle), -0.2f + t, 0.0f};
      esSetLightPos(GL_LIGHT0+i, pos);
      esSetLightDiffuseColour(GL_LIGHT0+i, sideAmbient);
      esSetLightAmbientColour(GL_LIGHT0+i, zeros);
      esSetLightSpecularColour(GL_LIGHT0+i, zeros);
    }
    {
      float topScale = 0.9f;
      GLfloat topAmbient[]= {ambientColour[0] * topScale, ambientColour[1] * topScale, ambientColour[2] * topScale, 1.0f};
      GLfloat pos[] = {0.0f + t, 0.0f, 1.0f, 0.0f};
      esSetLightPos(GL_LIGHT4, pos);
      esSetLightDiffuseColour(GL_LIGHT4, topAmbient);
      esSetLightAmbientColour(GL_LIGHT4, zeros);
      esSetLightSpecularColour(GL_LIGHT4, zeros);
    }
  }
  else
  {
    esSetLightAmbientColour(GL_LIGHT0, ambientColour);
    for (int i = 1 ; i != 5 ; ++i)
    {
      esSetLightPos(GL_LIGHT0+i, zeros);
      esSetLightDiffuseColour(GL_LIGHT0+i, zeros);
      esSetLightAmbientColour(GL_LIGHT0+i, zeros);
      esSetLightSpecularColour(GL_LIGHT0+i, zeros);
    }
  }

  if (gGLVersion == 1)
  {
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0f);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
    if (mFrameworkSettings.mUseMultiLights)
    {
      for (int i = 1 ; i != 5 ; ++i)
      {
        glLightf(GL_LIGHT0+i, GL_SPOT_EXPONENT, 0.0f);
        glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, 180.0f);
        glLightf(GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0+i, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION, 0.0f);
      }
    }

    // disable the default global ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zeros);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0f);

    // Disable lighting by default
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL); // Fixes colour on aeroplane
  }
}

DisplayConfig GetDisplayConfig(int screenWidth, int screenHeight, int viewpointIndex, int numViewpoints)
{
  DisplayConfig result;
  if (numViewpoints == 1)
  {
    result.mLeft = 0;
    result.mBottom = 0;
    result.mWidth = screenWidth;
    result.mHeight = screenHeight;
    result.mViewpointIndex = 0;
  }
  else
  {
    result.mWidth = screenWidth / numViewpoints;
    result.mHeight = screenHeight;
    result.mLeft = (viewpointIndex * screenWidth)/numViewpoints;
    result.mBottom = 0;
    result.mViewpointIndex = viewpointIndex;
  }
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
Vector3 CalculateCameraOffset(const Camera& camera, const DisplayConfig& displayConfig, int numViewpoints, float stereoSeparation)
{
  if (numViewpoints == 1)
    return Vector3(0,0,0);
  Vector3 leftDir = camera.GetTransform().RowY();
  return displayConfig.mViewpointIndex ? leftDir * (-0.5f * stereoSeparation) : leftDir * (0.5f * stereoSeparation);
}

//---------------------------------------------------------------------------------------------------------------------
void RenderManager::RenderUpdate()
{
  TRACE_METHOD_ONLY(2);
  GLenum gl_error;

  glClearColor(0.0, 0.0, 0.0, 1.0);

  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glDisable(GL_CULL_FACE);

  if (gGLVersion == 1)
  {
    glShadeModel(GL_SMOOTH);

#ifdef FOG_ENABLED
    glEnable(GL_FOG);
    GLfloat fogColour[4] = {0.5f, 0.5f, 0.5f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColour);
    glFogf(GL_FOG_START, 0.0f);
    glFogf(GL_FOG_END, 400.0f);
#else
    glDisable(GL_FOG);
#endif
  }

  int w =  mFrameworkSettings.mScreenWidth;
  int h  = mFrameworkSettings.mScreenHeight;

  int numViewpoints = mEnableStereoscopy ? 2 : 1;

  for (int iViewpoint = 0 ; iViewpoint != numViewpoints ; ++iViewpoint)
  {
    DisplayConfig displayConfig = GetDisplayConfig(w, h, iViewpoint, numViewpoints);
    for (Viewports::iterator iViewport = mViewports.begin() ; iViewport != mViewports.end() ; ++iViewport)
    {
      // Viewport
      Viewport* viewport = *iViewport;
      if (!viewport->GetEnabled())
        continue;

      esMatrixMode( GL_TEXTURE );
      esLoadIdentity();

      esMatrixMode( GL_PROJECTION );
      esLoadIdentity();

      esMatrixMode( GL_MODELVIEW );
      esLoadIdentity();

      // This sets up the viewport and clears it
      viewport->SetupViewport(displayConfig);

      // Camera
      Camera& camera = *viewport->GetCamera();
      float aspectRatio = viewport->GetAspectRatio();

      // This changes to GL_PROJECTION, calls glFrustumf
      camera.SetupCameraProjection(aspectRatio);

      // Changes to GL_MODELVIEW, calls lookat
      Vector3 cameraOffset = CalculateCameraOffset(camera, displayConfig, numViewpoints, mStereoSeparation);
      camera.SetupCameraView(cameraOffset);

      // Changes to GL_MODELVIEW, sets lighting position etc
      SetupLighting();

      for (RenderObjects::iterator it = mRenderObjects.begin() ; it != mRenderObjects.end() ; ++it)
      {
        RenderObject* renderObject = it->second;
        if (!viewport->GetShouldRenderObject(renderObject))
          continue;

        const Transform& tm = renderObject->GetTM();
        float radius = renderObject->GetRenderBoundingRadius();
        if (!camera.isSpherePartlyInFrustum(tm.GetTrans(), radius))
          continue;

        int renderLevel = it->first;
        renderObject->RenderUpdate(viewport, renderLevel);
      }
    }

    {
      DisableFog disableFog;

      // Now the overlay - over the whole screen.
      /*if (IsMarmaladeVersionLessThan(6,2,0) && mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
        glViewport( displayConfig.mBottom, displayConfig.mLeft, displayConfig.mHeight, displayConfig.mWidth );
      else
        glViewport( displayConfig.mLeft, displayConfig.mBottom, displayConfig.mWidth, displayConfig.mHeight );*/
      glViewport( displayConfig.mLeft, displayConfig.mBottom, displayConfig.mWidth, displayConfig.mHeight );

      esMatrixMode(GL_PROJECTION);
      esLoadIdentity();
      /*if (IsMarmaladeVersionLessThan(6,2,0) && mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
      {
        if (s3eSurfaceGetInt(S3E_SURFACE_DEVICE_BLIT_DIRECTION) == S3E_SURFACE_BLIT_DIR_ROT90)
          ROTATE_270_Z;
        else
          ROTATE_90_Z;
      }*/

      esOrthof(
        float(displayConfig.mLeft), 
        float(displayConfig.mWidth + displayConfig.mLeft), 
        float(displayConfig.mBottom), 
        float(displayConfig.mHeight + displayConfig.mBottom), 
        1.0f, -1.0f);

      esMatrixMode(GL_MODELVIEW);
      esLoadIdentity();

      for (RenderOverlayObjects::iterator it = mRenderOverlayObjects.begin() ; it != mRenderOverlayObjects.end() ; ++it)
      {
        (it->second)->RenderOverlayUpdate(it->first, displayConfig);
      }
    }
  }

  for (int iViewpoint = 0 ; iViewpoint != numViewpoints ; ++iViewpoint)
  {
    DisplayConfig displayConfig = GetDisplayConfig(w, h, iViewpoint, numViewpoints);
    // IwGx rendering
    if (!mRenderGxObjects.empty())
    {
      PrepareForIwGx(false);

      DisableDepthMask disableDepthMask;
      DisableDepthTest disableDepthTest;

      for (RenderGxObjects::iterator it = mRenderGxObjects.begin() ; it != mRenderGxObjects.end() ; ++it)
      {
        (it->second)->GxRender(it->first, displayConfig);
      }
    }
  }

  if (!mRenderGxObjects.empty())
  {
    DisableDepthMask disableDepthMask;
    DisableDepthTest disableDepthTest;
    //IwGxFlush();
    IwGxSwapBuffers();
    RecoverFromIwGx(false);
  }
  else
  {
    // Finalise rendering
    IwGLSwapBuffers();
  }


  /* Check for error conditions. */
  gl_error = glGetError();

  if (gl_error != GL_NO_ERROR)
  {
    fprintf( stderr, "testgl: OpenGL error: %#x\n", gl_error );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::RegisterRenderObject(RenderObject* renderObject, int renderLevel)
{
  IwAssertMsg(ROWLHOUSE, !IsRenderObjectRegistered(renderObject, renderLevel), ("RenderObject should be unregistered before registration")); 
  mRenderObjects.insert(std::make_pair(renderLevel, renderObject));
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::UnregisterRenderObject(RenderObject* renderObject, int renderLevel)
{
  IwAssert(ROWLHOUSE, renderLevel != RENDER_LEVEL_ANY);
  // TODO make this more efficient
  for (RenderObjects::iterator it = mRenderObjects.begin() ; it != mRenderObjects.end() ; ++it)
  {
    if (it->second == renderObject && it->first == renderLevel)
    {
      mRenderObjects.erase(it);
      return;
    }
  }
  IwAssertMsg(ROWLHOUSE, false, ("RenderObject should be registered before unregistration")); 
}

//----------------------------------------------------------------------------------------------------------------------
bool RenderManager::IsRenderObjectRegistered(const RenderObject* renderObject, int renderLevel) const
{
  for (RenderObjects::const_iterator it = mRenderObjects.begin() ; it != mRenderObjects.end() ; ++it)
  {
    if (it->second == renderObject && (renderLevel == RENDER_LEVEL_ANY || it->first == renderLevel))
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::RegisterRenderGxObject(RenderGxObject* renderGxObject, int renderLevel)
{
  IwAssertMsg(ROWLHOUSE, !IsRenderGxObjectRegistered(renderGxObject, renderLevel), ("RenderGxObject should be unregistered before registration")); 
  mRenderGxObjects.insert(std::make_pair(renderLevel, renderGxObject));
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::UnregisterRenderGxObject(RenderGxObject* renderGxObject, int renderLevel)
{
  IwAssert(ROWLHOUSE, renderLevel != RENDER_LEVEL_ANY);
  // TODO make this more efficient
  for (RenderGxObjects::iterator it = mRenderGxObjects.begin() ; it != mRenderGxObjects.end() ; ++it)
  {
    if (it->second == renderGxObject && it->first == renderLevel)
    {
      mRenderGxObjects.erase(it);
      return;
    }
  }
  IwAssertMsg(ROWLHOUSE, false, ("RenderGxObject should be registered before unregistration")); 
}

//----------------------------------------------------------------------------------------------------------------------
bool RenderManager::IsRenderGxObjectRegistered(const RenderGxObject* renderGxObject, int renderLevel) const
{
  for (RenderGxObjects::const_iterator it = mRenderGxObjects.begin() ; it != mRenderGxObjects.end() ; ++it)
  {
    if (it->second == renderGxObject && (renderLevel == RENDER_LEVEL_ANY || it->first == renderLevel))
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::RegisterRenderOverlayObject(RenderOverlayObject* renderOverlayObject, int renderLevel)
{
  IwAssertMsg(ROWLHOUSE, !IsRenderOverlayObjectRegistered(renderOverlayObject, renderLevel), ("RenderOverlayObject should be unregistered before registration")); 
  mRenderOverlayObjects.insert(std::make_pair(renderLevel, renderOverlayObject));
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::UnregisterRenderOverlayObject(RenderOverlayObject* renderOverlayObject, int renderLevel)
{
  // TODO make this more efficient
  for (RenderOverlayObjects::iterator it = mRenderOverlayObjects.begin() ; it != mRenderOverlayObjects.end() ; ++it)
  {
    if (it->second == renderOverlayObject && it->first == renderLevel)
    {
      mRenderOverlayObjects.erase(it);
      return;
    }
  }
  IwAssertMsg(ROWLHOUSE, false, ("RenderOverlayObject should be registered before unregistration")); 
}

//----------------------------------------------------------------------------------------------------------------------
bool RenderManager::IsRenderOverlayObjectRegistered(const RenderOverlayObject* renderOverlayObject, int renderLevel) const
{
  for (RenderOverlayObjects::const_iterator it = mRenderOverlayObjects.begin() ; it != mRenderOverlayObjects.end() ; ++it)
  {
    if (it->second == renderOverlayObject && it->first == renderLevel)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::RegisterShadowCasterObject(RenderObject* renderObject)
{
  IwAssertMsg(ROWLHOUSE, !IsShadowCasterObjectRegistered(renderObject), ("ShadowCasterObject should be unregistered before registration")); 
  mShadowCasterObjects.push_back(renderObject);
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::UnregisterShadowCasterObject(RenderObject* renderObject)
{
  // TODO make this more efficient
  for (ShadowCasterObjects::iterator it = mShadowCasterObjects.begin() ; it != mShadowCasterObjects.end() ; ++it)
  {
    if (*it == renderObject)
    {
      mShadowCasterObjects.erase(it);
      return;
    }
  }
  IwAssertMsg(ROWLHOUSE, false, ("ShadowCasterObject should be registered before unregistration")); 
}

//----------------------------------------------------------------------------------------------------------------------
bool RenderManager::IsShadowCasterObjectRegistered(const RenderObject* renderObject) const
{
  for (ShadowCasterObjects::const_iterator it = mShadowCasterObjects.begin() ; it != mShadowCasterObjects.end() ; ++it)
  {
    if (*it == renderObject)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
Viewport* RenderManager::CreateViewport(float left, float bottom, float right, float top, Camera* camera)
{
  Viewport *viewport = new Viewport(left, bottom, right - left, top - bottom, camera);
  mViewports.push_back(viewport);
  return viewport;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::DestroyViewport(Viewport* viewport)
{
  for (Viewports::iterator it = mViewports.begin(); it != mViewports.end() ; ++it)
  {
    if (*it == viewport)
    {
      mViewports.erase(it);
      return;
    }
  }
  IwAssertMsg(ROWLHOUSE, false, ("Failed to find viewport to delete"));
}

//----------------------------------------------------------------------------------------------------------------------
Camera* RenderManager::CreateCamera()
{
  Camera* camera = new Camera(mFrameworkSettings);
  mCameras.push_back(camera);
  return camera;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderManager::DestroyCamera(Camera* camera)
{
  for (Cameras::iterator it = mCameras.begin() ; it != mCameras.end() ; ++it)
  {
    if (*it == camera)
    {
      mCameras.erase(it);
      return;
    }
  }
  IwAssertMsg(ROWLHOUSE, false, ("Failed to find camera to delete"));
}
