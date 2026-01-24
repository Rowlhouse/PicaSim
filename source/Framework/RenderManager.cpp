#include "RenderManager.h"

#include "DebugRenderer.h"
#include "RenderObject.h"
#include "RenderOverlayObject.h"
#include "Trace.h"
#include "LoadingScreenHelper.h"
#include "Viewport.h"
#include "Graphics.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "../Platform/S3ECompat.h"
#include "../Platform/Window.h"

#ifdef PICASIM_VR_SUPPORT
#include "../Platform/VRManager.h"
#include "../Platform/VRRuntime.h"
#include <glm/gtc/matrix_transform.hpp>
#endif

RenderManager* RenderManager::mInstance = 0;


//======================================================================================================================
RenderManager& RenderManager::GetInstance()
{
    IwAssert(ROWLHOUSE, mInstance != 0);
    return *mInstance;
}

//======================================================================================================================
void RenderManager::SetLightingDirection(float bearing, float elevation)
{
    mLightingDirection = Vector3(-1.0f, 0.0f, 0.0f);
    Quat q(Vector3(0,1,0), -DegreesToRadians(elevation));
    mLightingDirection = q.RotateVector(mLightingDirection);
    q = Quat(Vector3(0,0,1), -DegreesToRadians(bearing));
    mLightingDirection = q.RotateVector(mLightingDirection);
}


//======================================================================================================================
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


//======================================================================================================================
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

    IwGxPrintSetColour(128, 128, 128);

    mInstance->mDebugRenderer->Init();
}

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
Vector3 CalculateCameraOffset(const Camera& camera, const DisplayConfig& displayConfig, int numViewpoints, float stereoSeparation)
{
    if (numViewpoints == 1)
        return Vector3(0,0,0);
    Vector3 leftDir = camera.GetTransform().RowY();
    return displayConfig.mViewpointIndex ? leftDir * (-0.5f * stereoSeparation) : leftDir * (0.5f * stereoSeparation);
}

//======================================================================================================================
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
            glViewport( displayConfig.mLeft, displayConfig.mBottom, displayConfig.mWidth, displayConfig.mHeight );

            esMatrixMode(GL_PROJECTION);
            esLoadIdentity();
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
        IwGxFlush();
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

//======================================================================================================================
void RenderManager::RenderWithoutSwap()
{
    TRACE_METHOD_ONLY(2);

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
            glViewport( displayConfig.mLeft, displayConfig.mBottom, displayConfig.mWidth, displayConfig.mHeight );

            esMatrixMode(GL_PROJECTION);
            esLoadIdentity();
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
    // Note: This method does NOT render GxRender objects or swap buffers
    // The caller is expected to render ImGui or other UI overlays and then swap
}

//======================================================================================================================
void RenderManager::RegisterRenderObject(RenderObject* renderObject, int renderLevel)
{
    IwAssertMsg(ROWLHOUSE, !IsRenderObjectRegistered(renderObject, renderLevel), ("RenderObject should be unregistered before registration")); 
    mRenderObjects.insert(std::make_pair(renderLevel, renderObject));
}

//======================================================================================================================
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

//======================================================================================================================
bool RenderManager::IsRenderObjectRegistered(const RenderObject* renderObject, int renderLevel) const
{
    for (RenderObjects::const_iterator it = mRenderObjects.begin() ; it != mRenderObjects.end() ; ++it)
    {
        if (it->second == renderObject && (renderLevel == RENDER_LEVEL_ANY || it->first == renderLevel))
            return true;
    }
    return false;
}

//======================================================================================================================
void RenderManager::RegisterRenderGxObject(RenderGxObject* renderGxObject, int renderLevel)
{
    IwAssertMsg(ROWLHOUSE, !IsRenderGxObjectRegistered(renderGxObject, renderLevel), ("RenderGxObject should be unregistered before registration")); 
    mRenderGxObjects.insert(std::make_pair(renderLevel, renderGxObject));
}

//======================================================================================================================
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

//======================================================================================================================
bool RenderManager::IsRenderGxObjectRegistered(const RenderGxObject* renderGxObject, int renderLevel) const
{
    for (RenderGxObjects::const_iterator it = mRenderGxObjects.begin() ; it != mRenderGxObjects.end() ; ++it)
    {
        if (it->second == renderGxObject && (renderLevel == RENDER_LEVEL_ANY || it->first == renderLevel))
            return true;
    }
    return false;
}

//======================================================================================================================
void RenderManager::RegisterRenderOverlayObject(RenderOverlayObject* renderOverlayObject, int renderLevel)
{
    IwAssertMsg(ROWLHOUSE, !IsRenderOverlayObjectRegistered(renderOverlayObject, renderLevel), ("RenderOverlayObject should be unregistered before registration")); 
    mRenderOverlayObjects.insert(std::make_pair(renderLevel, renderOverlayObject));
}

//======================================================================================================================
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

//======================================================================================================================
bool RenderManager::IsRenderOverlayObjectRegistered(const RenderOverlayObject* renderOverlayObject, int renderLevel) const
{
    for (RenderOverlayObjects::const_iterator it = mRenderOverlayObjects.begin() ; it != mRenderOverlayObjects.end() ; ++it)
    {
        if (it->second == renderOverlayObject && it->first == renderLevel)
            return true;
    }
    return false;
}

//======================================================================================================================
void RenderManager::RegisterShadowCasterObject(RenderObject* renderObject)
{
    IwAssertMsg(ROWLHOUSE, !IsShadowCasterObjectRegistered(renderObject), ("ShadowCasterObject should be unregistered before registration")); 
    mShadowCasterObjects.push_back(renderObject);
}

//======================================================================================================================
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

//======================================================================================================================
bool RenderManager::IsShadowCasterObjectRegistered(const RenderObject* renderObject) const
{
    for (ShadowCasterObjects::const_iterator it = mShadowCasterObjects.begin() ; it != mShadowCasterObjects.end() ; ++it)
    {
        if (*it == renderObject)
            return true;
    }
    return false;
}

//======================================================================================================================
Viewport* RenderManager::CreateViewport(float left, float bottom, float right, float top, Camera* camera)
{
    Viewport *viewport = new Viewport(left, bottom, right - left, top - bottom, camera);
    mViewports.push_back(viewport);
    return viewport;
}

//======================================================================================================================
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

//======================================================================================================================
Camera* RenderManager::CreateCamera()
{
    Camera* camera = new Camera(mFrameworkSettings);
    mCameras.push_back(camera);
    return camera;
}

//======================================================================================================================
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

#ifdef PICASIM_VR_SUPPORT
//======================================================================================================================
void RenderManager::RenderUpdateVR(VRFrameInfo& frameInfo)
{
    TRACE_METHOD_ONLY(2);

    if (!VRManager::IsAvailable() || !VRManager::GetInstance().IsVREnabled())
    {
        // Fall back to normal rendering
        RenderUpdate();
        return;
    }

    VRManager& vrManager = VRManager::GetInstance();

    // Must be in a valid VR frame (between BeginVRFrame and EndVRFrame)
    if (!vrManager.IsInVRFrame())
    {
        // No VR frame active - fall back to normal rendering
        RenderUpdate();
        return;
    }

    // Get the actual frame info from VRManager (the parameter may be empty/dummy)
    const VRFrameInfo& actualFrameInfo = vrManager.GetCurrentFrameInfo();

    // Check if we should actually render this frame
    if (!actualFrameInfo.shouldRender)
    {
        // VR runtime says don't render (e.g., headset not in view)
        return;
    }

    VRRuntime* runtime = vrManager.GetRuntime();
    if (!runtime)
    {
        RenderUpdate();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    if (gGLVersion == 1)
    {
        glShadeModel(GL_SMOOTH);
        glDisable(GL_FOG);
    }

    // Render each eye
    for (int eye = 0; eye < VR_EYE_COUNT; ++eye)
    {
        const VRViewInfo& viewInfo = actualFrameInfo.views[eye];

        // Acquire VR swapchain image
        uint32_t imageIndex;
        if (!runtime->AcquireSwapchainImage((VREye)eye, imageIndex))
        {
            continue;
        }
        runtime->WaitSwapchainImage((VREye)eye);

        // Bind to VR swapchain texture via FBO
        int eyeWidth = runtime->GetSwapchainWidth((VREye)eye);
        int eyeHeight = runtime->GetSwapchainHeight((VREye)eye);

        // Create a temporary FBO to render to the swapchain texture
        GLuint eyeFBO;
        glGenFramebuffers(1, &eyeFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, eyeFBO);

        GLuint swapchainTex = runtime->GetSwapchainTexture((VREye)eye, imageIndex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, swapchainTex, 0);

        // Create depth buffer for this eye
        GLuint depthRB;
        glGenRenderbuffers(1, &depthRB);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

        glViewport(0, 0, eyeWidth, eyeHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create display config for this eye
        DisplayConfig displayConfig;
        displayConfig.mLeft = 0;
        displayConfig.mBottom = 0;
        displayConfig.mWidth = eyeWidth;
        displayConfig.mHeight = eyeHeight;
        displayConfig.mViewpointIndex = eye;


        
        for (Viewports::iterator iViewport = mViewports.begin(); iViewport != mViewports.end(); ++iViewport)
        {
            Viewport* viewport = *iViewport;
            if (!viewport->GetEnabled())
                continue;

            esMatrixMode(GL_TEXTURE);
            esLoadIdentity();

            // Use VR projection matrix
            esMatrixMode(GL_PROJECTION);
            esLoadIdentity();

            glm::mat4 projMatrix = runtime->GetProjectionMatrix((VREye)eye,
                mFrameworkSettings.mNearClipPlaneDistance,
                mFrameworkSettings.mFarClipPlaneDistance);
            esMultMatrixf(&projMatrix[0][0]);

            // Combine VR head pose with game camera position
            esMatrixMode(GL_MODELVIEW);
            esLoadIdentity();

            // Get the base camera transform (pilot position in world)
            Camera& camera = *viewport->GetCamera();
            Transform baseTM = camera.GetTransform();

            // Get VR view matrix and combine with base transform
            glm::mat4 vrViewMatrix = runtime->GetViewMatrix((VREye)eye);

            // Apply yaw offset to align headset forward with target direction
            float yawOffset = vrManager.GetTotalYawOffset();
            glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), yawOffset, glm::vec3(0.0f, 0.0f, 1.0f));

            // Apply position offset relative to reference position
            glm::vec3 posOffset = vrManager.GetHeadPosition() - vrManager.GetReferencePosition();
            // Rotate the position offset by the yaw to keep it consistent
            glm::vec3 rotatedPosOffset = glm::vec3(yawRotation * glm::vec4(posOffset, 0.0f));
            glm::mat4 posTranslation = glm::translate(glm::mat4(1.0f), -rotatedPosOffset);

            // Combine: yaw rotation * view matrix (applied to view space)
            vrViewMatrix = vrViewMatrix * glm::inverse(yawRotation);

            // The VR view matrix needs to be combined with the game world position
            // First, convert base transform to a matrix
            glm::mat4 baseMatrix = glm::mat4(1.0f);
            baseMatrix[0] = glm::vec4(baseTM.m[0][0], baseTM.m[1][0], baseTM.m[2][0], 0.0f);
            baseMatrix[1] = glm::vec4(baseTM.m[0][1], baseTM.m[1][1], baseTM.m[2][1], 0.0f);
            baseMatrix[2] = glm::vec4(baseTM.m[0][2], baseTM.m[1][2], baseTM.m[2][2], 0.0f);
            baseMatrix[3] = glm::vec4(baseTM.t.x, baseTM.t.y, baseTM.t.z, 1.0f);

            // Combine: VR view * base world position
            glm::mat4 combinedView = vrViewMatrix * glm::inverse(baseMatrix);
            esMultMatrixf(&combinedView[0][0]);

            // Update frustum planes for culling (shadows use this)
            camera.UpdateFrustumPlanes();

            SetupLighting();

            // Render all objects (skip frustum culling for VR as it needs recalculation)
            for (RenderObjects::iterator it = mRenderObjects.begin(); it != mRenderObjects.end(); ++it)
            {
                RenderObject* renderObject = it->second;
                if (!viewport->GetShouldRenderObject(renderObject))
                    continue;

                int renderLevel = it->first;
                renderObject->RenderUpdate(viewport, renderLevel);
            }
        }

        // Copy rendered content to VRManager's mirror texture (for desktop display)
        // We do this while the swapchain texture is still bound
        uint32_t mirrorTex = vrManager.GetEyeColorTexture((VREye)eye);
        if (mirrorTex != 0)
        {
            int mirrorWidth, mirrorHeight;
            vrManager.GetEyeRenderTargetSize((VREye)eye, mirrorWidth, mirrorHeight);

            // Bind the swapchain FBO as read, mirror texture as draw
            GLuint mirrorFBO;
            glGenFramebuffers(1, &mirrorFBO);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, eyeFBO);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mirrorFBO);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTex, 0);

            // Blit from swapchain to mirror texture
            glBlitFramebuffer(0, 0, eyeWidth, eyeHeight,
                              0, 0, mirrorWidth, mirrorHeight,
                              GL_COLOR_BUFFER_BIT, GL_LINEAR);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &mirrorFBO);
        }

        // Cleanup eye FBO resources
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteRenderbuffers(1, &depthRB);
        glDeleteFramebuffers(1, &eyeFBO);

        // Release swapchain image
        runtime->ReleaseSwapchainImage((VREye)eye);
    }
}

//======================================================================================================================
void RenderManager::RenderMirrorWindow()
{
    if (!VRManager::IsAvailable() || !VRManager::GetInstance().IsVREnabled())
    {
        return;
    }

    VRManager& vrManager = VRManager::GetInstance();

    // Get the left eye texture to display
    uint32_t eyeTexture = vrManager.GetEyeColorTexture(VR_EYE_LEFT);
    if (eyeTexture == 0)
    {
        return;
    }

    int eyeWidth, eyeHeight;
    vrManager.GetEyeRenderTargetSize(VR_EYE_LEFT, eyeWidth, eyeHeight);

    // Bind the default framebuffer (desktop window)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mFrameworkSettings.mScreenWidth, mFrameworkSettings.mScreenHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate letterboxed/pillarboxed rectangle to maintain aspect ratio
    float eyeAspect = (float)eyeWidth / (float)eyeHeight;
    float screenAspect = (float)mFrameworkSettings.mScreenWidth / (float)mFrameworkSettings.mScreenHeight;

    float x0, y0, x1, y1;
    if (eyeAspect > screenAspect)
    {
        // Eye is wider - letterbox (black bars top/bottom)
        float height = mFrameworkSettings.mScreenWidth / eyeAspect;
        float yOffset = (mFrameworkSettings.mScreenHeight - height) * 0.5f;
        x0 = 0;
        x1 = (float)mFrameworkSettings.mScreenWidth;
        y0 = yOffset;
        y1 = yOffset + height;
    }
    else
    {
        // Eye is taller - pillarbox (black bars left/right)
        float width = mFrameworkSettings.mScreenHeight * eyeAspect;
        float xOffset = (mFrameworkSettings.mScreenWidth - width) * 0.5f;
        x0 = xOffset;
        x1 = xOffset + width;
        y0 = 0;
        y1 = (float)mFrameworkSettings.mScreenHeight;
    }

    // Set up orthographic projection for 2D rendering
    esMatrixMode(GL_PROJECTION);
    esPushMatrix();
    esLoadIdentity();
    esOrthof(0, (float)mFrameworkSettings.mScreenWidth, 0, (float)mFrameworkSettings.mScreenHeight, -1, 1);

    esMatrixMode(GL_MODELVIEW);
    esPushMatrix();
    esLoadIdentity();

    // Vertex positions for fullscreen quad
    GLfloat pts[] = {
        x0, y0, 0,
        x1, y0, 0,
        x1, y1, 0,
        x0, y1, 0,
    };

    // Texture coordinates (flip V since OpenGL textures are bottom-up)
    GLfloat uvs[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
    };

    // Disable depth test and blending for simple blit
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Use the overlay shader
    const OverlayShader* overlayShader = (OverlayShader*)ShaderManager::GetInstance().GetShader(SHADER_OVERLAY);
    overlayShader->Use();

    glUniform1i(overlayShader->u_texture, 0);

    glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glEnableVertexAttribArray(overlayShader->a_position);

    glVertexAttribPointer(overlayShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    glEnableVertexAttribArray(overlayShader->a_texCoord);

    glUniform4f(overlayShader->u_colour, 1.0f, 1.0f, 1.0f, 1.0f);
    esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);

    // Bind the eye texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);

    // Draw the quad
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Clean up
    glDisableVertexAttribArray(overlayShader->a_position);
    glDisableVertexAttribArray(overlayShader->a_texCoord);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore matrices
    esMatrixMode(GL_PROJECTION);
    esPopMatrix();
    esMatrixMode(GL_MODELVIEW);
    esPopMatrix();

    // Swap buffers for mirror window
    IwGLSwapBuffers();
}
#endif // PICASIM_VR_SUPPORT
