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
#include "../Platform/Platform.h"

#ifdef PICASIM_VR_SUPPORT
#include "../Platform/VRManager.h"
#include "../Platform/VRRuntime.h"
#include "Camera.h"  // For VROrientationMode
#include "Skybox.h"  // For VR parallax skybox
#include <glm/gtc/matrix_transform.hpp>
#endif

std::unique_ptr<RenderManager> RenderManager::mInstance;


//======================================================================================================================
RenderManager& RenderManager::GetInstance()
{
    IwAssert(ROWLHOUSE, mInstance != nullptr);
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
    mDebugRenderer = std::make_unique<DebugRenderer>();
    mLightingAmbientColour = Vector3(0.3f, 0.3f, 0.3f);
    mLightingDiffuseColour = Vector3(1.0f, 1.0f, 1.0f);
    mShadowStrength = 0.2f;
    mShadowDecayHeight = 40.0f;
    mShadowSizeScale = 1.3f;
    mShadowBlur = 0.0f;

#ifdef PICASIM_VR_SUPPORT
    mVRSkybox = nullptr;
    mVRPanoramaDepthEnabled = false;
    mVROverlayDistance = 2.0f;
    mVROverlayScale = 0.75f;
    mVROverlayVisible = true;
    mVROverlayShowCursor = false;
#endif

    float lightBearing   = 0.0f;
    float lightElevation = 0.0f;
    SetLightingDirection(lightBearing, lightElevation);
}

//======================================================================================================================
RenderManager::~RenderManager()
{
}

//======================================================================================================================
void RenderManager::Init(FrameworkSettings& frameworkSettings, LoadingScreenHelper* loadingScreen)
{
    TRACE_FUNCTION_ONLY(ONCE_1);
    IwAssert(ROWLHOUSE, !mInstance);
    mInstance.reset(new RenderManager(frameworkSettings));

    // Get dimensions from IwGL
    int width =  Platform::GetDisplayWidth();
    int height = Platform::GetDisplayHeight();
    GLint depthBits = 0;
    glGetIntegerv( GL_DEPTH_BITS, &depthBits);
    TRACE_FILE_IF(ONCE_1) TRACE("Screen Size : %dx%d", width, height);
    TRACE_FILE_IF(ONCE_2) TRACE( "Vendor     : %s", (const char*)glGetString( GL_VENDOR ) );
    TRACE_FILE_IF(ONCE_2) TRACE( "Renderer   : %s", (const char*)glGetString( GL_RENDERER ) );
    TRACE_FILE_IF(ONCE_2) TRACE( "Version    : %s", (const char*)glGetString( GL_VERSION ) );
    TRACE_FILE_IF(ONCE_2) TRACE( "Extensions : %s", (const char*)glGetString( GL_EXTENSIONS ) );
    TRACE_FILE_IF(ONCE_2) TRACE( "Depth bits : %d", depthBits );

    mInstance->mDebugRenderer->Init();
}

//======================================================================================================================
void RenderManager::Terminate()
{
    TRACE_FUNCTION_ONLY(ONCE_1);
    IwAssert(ROWLHOUSE, mInstance);

    for (Viewports::iterator it = mInstance->mViewports.begin(); it != mInstance->mViewports.end() ; ++it)
        delete *it;
    mInstance->mViewports.clear();

    for (Cameras::iterator it = mInstance->mCameras.begin() ; it != mInstance->mCameras.end() ; ++it)
        delete *it;
    mInstance->mCameras.clear();
    mInstance->mDebugRenderer->Terminate();

    mInstance.reset();
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

    GLfloat lightPos[] = {-mLightingDirection.x, -mLightingDirection.y, -mLightingDirection.z, 0.0f};
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
            GLfloat pos[] = {sinf(angle), cosf(angle), -0.2f, 0.0f};
            esSetLightPos(GL_LIGHT0+i, pos);
            esSetLightDiffuseColour(GL_LIGHT0+i, sideAmbient);
            esSetLightAmbientColour(GL_LIGHT0+i, zeros);
            esSetLightSpecularColour(GL_LIGHT0+i, zeros);
        }
        {
            float topScale = 0.9f;
            GLfloat topAmbient[]= {ambientColour[0] * topScale, ambientColour[1] * topScale, ambientColour[2] * topScale, 1.0f};
            GLfloat pos[] = {0.0f, 0.0f, 1.0f, 0.0f};
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

}

//======================================================================================================================
DisplayConfig GetDisplayConfig(int screenWidth, int screenHeight)
{
    DisplayConfig result;
    result.mLeft = 0;
    result.mBottom = 0;
    result.mWidth = screenWidth;
    result.mHeight = screenHeight;
    result.mViewpointIndex = 0;
    return result;
}

//======================================================================================================================
void RenderManager::RenderUpdate()
{
    TRACE_METHOD_ONLY(ONCE_2);
    GLenum gl_error;

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    int w =  mFrameworkSettings.mScreenWidth;
    int h  = mFrameworkSettings.mScreenHeight;

    DisplayConfig displayConfig = GetDisplayConfig(w, h);

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
        camera.SetupCameraView(Vector3(0, 0, 0));

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

    // IwGx rendering
    if (!mRenderGxObjects.empty())
    {
        DisableDepthMask disableDepthMask;
        DisableDepthTest disableDepthTest;

        for (RenderGxObjects::iterator it = mRenderGxObjects.begin() ; it != mRenderGxObjects.end() ; ++it)
        {
            (it->second)->GxRender(it->first, displayConfig);
        }
    }

    if (!mRenderGxObjects.empty())
    {
        DisableDepthMask disableDepthMask;
        DisableDepthTest disableDepthTest;
        glFlush();
        IwGxSwapBuffers();
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
    TRACE_METHOD_ONLY(ONCE_2);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    int w =  mFrameworkSettings.mScreenWidth;
    int h  = mFrameworkSettings.mScreenHeight;

    DisplayConfig displayConfig = GetDisplayConfig(w, h);

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
        camera.SetupCameraView(Vector3(0, 0, 0));

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
        if (it->second == renderObject && (renderLevel == RENDER_LEVEL_ANY || it->first == renderLevel))
        {
            mRenderObjects.erase(it);
            if (renderLevel != RENDER_LEVEL_ANY)
                return;
        }
    }
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
    TRACE_METHOD_ONLY(ONCE_2);

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

    // DEBUG: Force reset critical GL state before VR rendering
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);
    glDisable(GL_SCISSOR_TEST);
    GLenum preErr = glGetError();
    if (preErr != GL_NO_ERROR)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("VR pre-render GL error: 0x%x", preErr);
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
        int msaaSamples = vrManager.GetMSAASamples();

        // Create FBO for the swapchain texture (non-MSAA, for final output)
        GLuint swapchainFBO;
        glGenFramebuffers(1, &swapchainFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, swapchainFBO);

        GLuint swapchainTex = runtime->GetSwapchainTexture((VREye)eye, imageIndex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, swapchainTex, 0);

        // Create depth buffer for swapchain FBO
        GLuint depthRB;
        glGenRenderbuffers(1, &depthRB);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

        // If MSAA is enabled, create MSAA renderbuffers and FBO for rendering
        GLuint msaaFBO = 0;
        GLuint msaaColorRB = 0;
        GLuint msaaDepthRB = 0;
        GLuint renderFBO = swapchainFBO;  // Default to swapchain FBO

        if (msaaSamples > 1)
        {
            // Create MSAA color renderbuffer
            glGenRenderbuffers(1, &msaaColorRB);
            glBindRenderbuffer(GL_RENDERBUFFER, msaaColorRB);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA8, eyeWidth, eyeHeight);

            // Create MSAA depth renderbuffer
            glGenRenderbuffers(1, &msaaDepthRB);
            glBindRenderbuffer(GL_RENDERBUFFER, msaaDepthRB);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Create MSAA FBO
            glGenFramebuffers(1, &msaaFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, msaaFBO);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorRB);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, msaaDepthRB);

            renderFBO = msaaFBO;  // Render to MSAA FBO
        }

        // Bind the render FBO (either MSAA or swapchain)
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

        // DEBUG: Check FBO completeness
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("VR FBO incomplete for eye %d: 0x%x", eye, fboStatus);
        }

        glViewport(0, 0, eyeWidth, eyeHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DEBUG: Check for errors after clear
        GLenum clearErr = glGetError();
        if (clearErr != GL_NO_ERROR)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("VR clear GL error for eye %d: 0x%x", eye, clearErr);
        }

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

            // Skip zoom viewport in VR - it's only for the desktop view
            // CAMERA_ZOOM = 3 (defined in PicaSim.h)
            const int CAMERA_ZOOM_ID = 3;
            if ((size_t)viewport->GetCamera()->GetUserData() == CAMERA_ZOOM_ID)
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

            // Get fresh camera transform by calling the callback
            // For chase/cockpit modes this updates position relative to plane
            Transform baseTM;
            if (camera.GetCameraTransform())
            {
                baseTM = camera.GetCameraTransform()->GetCameraTransform(camera.GetUserData());
            }
            else
            {
                baseTM = camera.GetTransform();
            }

            // Get VR view matrix (VRManager handles panoramic scene mode)
            glm::mat4 vrViewMatrix = vrManager.GetViewMatrixForScene((VREye)eye);

            // Apply yaw offset to align headset forward with target direction
            float yawOffset = vrManager.GetTotalYawOffset();
            glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), yawOffset, glm::vec3(0.0f, 0.0f, 1.0f));

            // Calculate head position offset relative to reference position
            // In panoramic mode, GetHeadPosition() returns reference position, so offset is zero
            glm::vec3 posOffset = vrManager.GetHeadPosition() - vrManager.GetReferencePosition();

            // Rotate the position offset by the yaw to keep it consistent
            glm::vec3 rotatedPosOffset = glm::vec3(yawRotation * glm::vec4(posOffset, 0.0f));

            // Combine: yaw rotation * view matrix (applied to view space)
            vrViewMatrix = vrViewMatrix * glm::inverse(yawRotation);

            // The VR view matrix needs to be combined with the game world position
            // Determine how to build base matrix based on VR orientation mode
            bool vrOverridesOrientation = (camera.GetVROrientationMode() == Camera::VROrientationMode::Override);

            // Build base matrix - differs based on VR orientation mode
            glm::mat4 baseMatrix = glm::mat4(1.0f);
            if (vrOverridesOrientation)
            {
                // Override mode: Use only position, with identity orientation
                // This lets VR headset + yaw offset fully control view direction
                baseMatrix[3] = glm::vec4(baseTM.t.x, baseTM.t.y, baseTM.t.z, 1.0f);
            }
            else
            {
                // Inherit mode: Use full transform (orientation from camera)
                // Transform is row-major (m[row][col]), glm is column-major
                // Each row of Transform becomes a column in glm (transpose)
                baseMatrix[0] = glm::vec4(baseTM.m[0][0], baseTM.m[0][1], baseTM.m[0][2], 0.0f);
                baseMatrix[1] = glm::vec4(baseTM.m[1][0], baseTM.m[1][1], baseTM.m[1][2], 0.0f);
                baseMatrix[2] = glm::vec4(baseTM.m[2][0], baseTM.m[2][1], baseTM.m[2][2], 0.0f);
                baseMatrix[3] = glm::vec4(baseTM.t.x, baseTM.t.y, baseTM.t.z, 1.0f);
            }

            // Combine: VR view * base world position
            glm::mat4 combinedView = vrViewMatrix * glm::inverse(baseMatrix);
            esMultMatrixf(&combinedView[0][0]);

            // Update camera position to include VR head offset (needed for skybox rendering)
            // The actual eye position is base position plus head offset rotated appropriately
            glm::vec3 eyePos;
            if (vrOverridesOrientation)
            {
                // Override mode: head offset rotated by yaw only
                eyePos = glm::vec3(baseTM.t.x, baseTM.t.y, baseTM.t.z) + rotatedPosOffset;
            }
            else
            {
                // Inherit mode: head offset rotated by base orientation and yaw
                glm::mat3 baseRot(baseMatrix);
                eyePos = glm::vec3(baseTM.t.x, baseTM.t.y, baseTM.t.z) + baseRot * rotatedPosOffset;
            }

            // Compute camera world transform from combined view matrix (for terrain LOD/clipping)
            // Camera transform = inverse(view matrix)
            glm::mat4 cameraWorldMatrix = glm::inverse(combinedView);

            // Convert from OpenGL to PicaSim coordinate system
            // OpenGL: -Z forward, +X right, +Y up
            // PicaSim: +X forward, -Y right (left=+Y), +Z up
            Transform vrCameraTM;
            // Row 0 (forward in PicaSim) = -Z in OpenGL (column 2, negated)
            vrCameraTM.m[0][0] = -cameraWorldMatrix[2][0];
            vrCameraTM.m[0][1] = -cameraWorldMatrix[2][1];
            vrCameraTM.m[0][2] = -cameraWorldMatrix[2][2];
            // Row 1 (left in PicaSim) = -X in OpenGL (column 0, negated)
            vrCameraTM.m[1][0] = -cameraWorldMatrix[0][0];
            vrCameraTM.m[1][1] = -cameraWorldMatrix[0][1];
            vrCameraTM.m[1][2] = -cameraWorldMatrix[0][2];
            // Row 2 (up in PicaSim) = +Y in OpenGL (column 1)
            vrCameraTM.m[2][0] = cameraWorldMatrix[1][0];
            vrCameraTM.m[2][1] = cameraWorldMatrix[1][1];
            vrCameraTM.m[2][2] = cameraWorldMatrix[1][2];
            // Translation
            vrCameraTM.t = Vector3(eyePos.x, eyePos.y, eyePos.z);

            camera.SetTransform(vrCameraTM);

            // Update frustum planes for culling (shadows use this)
            camera.UpdateFrustumPlanes();

            SetupLighting();

            // Render objects - if VR panorama depth enabled, split into two passes:
            // Pass 1: Background objects (for depth-based parallax)
            // Pass 2: Foreground objects (rendered after parallax skybox)
            for (RenderObjects::iterator it = mRenderObjects.begin(); it != mRenderObjects.end(); ++it)
            {
                RenderObject* renderObject = it->second;
                if (!viewport->GetShouldRenderObject(renderObject))
                    continue;

                int renderLevel = it->first;

                // Skip skybox when VR panorama depth is enabled - rendered with parallax
                if (mVRPanoramaDepthEnabled && renderLevel == RENDER_LEVEL_SKYBOX)
                    continue;

                // Skip visible objects (RENDER_LEVEL_OBJECTS and above) in first pass
                // They will be rendered after the parallax skybox
                if (mVRPanoramaDepthEnabled && renderLevel >= RENDER_LEVEL_OBJECTS)
                    continue;

                renderObject->RenderUpdate(viewport, renderLevel);
            }
        }

        // If MSAA was used, resolve from MSAA FBO to swapchain FBO
        if (msaaSamples > 1)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, swapchainFBO);

            // When VR panorama depth is enabled, DON'T resolve color yet - we'll continue
            // rendering to MSAA FBO and do final color resolve at the end
            if (!mVRPanoramaDepthEnabled)
            {
                glBlitFramebuffer(0, 0, eyeWidth, eyeHeight,
                                  0, 0, eyeWidth, eyeHeight,
                                  GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }

            // Resolve depth for VR panorama depth feature (needed for parallax shader)
            if (mVRPanoramaDepthEnabled)
            {
                glBlitFramebuffer(0, 0, eyeWidth, eyeHeight,
                                  0, 0, eyeWidth, eyeHeight,
                                  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            }
        }

        // VR Panorama Depth: Render skybox with depth-based parallax
        // Must happen AFTER MSAA resolve so we read from resolved depth buffer
        if (mVRPanoramaDepthEnabled && mVRSkybox)
        {
            // Bind swapchain FBO (non-MSAA) for depth copy and skybox render
            glBindFramebuffer(GL_FRAMEBUFFER, swapchainFBO);

            // Create a depth texture by copying the current depth buffer
            GLuint depthTexture;
            glGenTextures(1, &depthTexture);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Copy depth buffer to texture (now from resolved swapchain FBO)
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 0, 0, eyeWidth, eyeHeight, 0);

            // Rebind MSAA FBO for skybox and foreground rendering (preserves anti-aliasing)
            if (msaaSamples > 1)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, msaaFBO);
            }

            // Calculate eye offset (-1 for left, +1 for right)
            float eyeOffset = (eye == VR_EYE_LEFT) ? -1.0f : 1.0f;

            // Get IPD from VR runtime
            float ipd = runtime->GetIPD();

            // Get the first enabled viewport for rendering
            Viewport* vrViewport = nullptr;
            for (Viewports::iterator iViewport = mViewports.begin(); iViewport != mViewports.end(); ++iViewport)
            {
                if ((*iViewport)->GetEnabled())
                {
                    vrViewport = *iViewport;
                    break;
                }
            }

            if (vrViewport)
            {
                // Get the base camera position (pilot position, not VR head position)
                // This ensures the skybox doesn't move when the head position changes
                Camera& vrCamera = *vrViewport->GetCamera();
                Transform baseTM;
                if (vrCamera.GetCameraTransform())
                {
                    baseTM = vrCamera.GetCameraTransform()->GetCameraTransform(vrCamera.GetUserData());
                }
                else
                {
                    baseTM = vrCamera.GetTransform();
                }
                Vector3 skyboxCenter = baseTM.t;

                // Get the eye-to-eye direction from the VR view matrix
                // The view matrix is already in PicaSim coordinates (X=forward, Y=left, Z=up)
                // The "right" direction is the first row of the view matrix (camera's local X axis in world space)
                glm::mat4 vrView = vrManager.GetViewMatrixForScene(VR_EYE_LEFT);

                // Extract right vector from view matrix (first row = camera's right axis in world space)
                // GLM is column-major: mat[col][row], so first row is elements [0][0], [1][0], [2][0]
                glm::vec3 eyeRight(vrView[0][0], vrView[1][0], vrView[2][0]);

                // The scene is rendered with yaw offset applied to the view matrix.
                // To match the skybox (which is in the virtual world), we need to rotate
                // the eye direction by the yaw offset.
                float yawOffset = vrManager.GetTotalYawOffset();
                float cosYaw = cos(yawOffset);
                float sinYaw = sin(yawOffset);
                Vector3 eyeRightDir(
                    eyeRight.x * cosYaw - eyeRight.y * sinYaw,
                    eyeRight.x * sinYaw + eyeRight.y * cosYaw,
                    eyeRight.z
                );

                // Get FOV tangents for depth correction
                float tanLeft, tanRight, tanUp, tanDown;
                if (!runtime->GetFovTangents((VREye)eye, tanLeft, tanRight, tanUp, tanDown))
                {
                    // Fallback: assume symmetric 90 degree FOV
                    tanLeft = tanDown = -1.0f;
                    tanRight = tanUp = 1.0f;
                }

                // Render skybox with depth-based parallax
                mVRSkybox->RenderVRParallax(
                    vrViewport,
                    skyboxCenter,
                    eyeRightDir,
                    eyeOffset,
                    ipd,
                    depthTexture,
                    eyeWidth, eyeHeight,
                    mFrameworkSettings.mNearClipPlaneDistance,
                    mFrameworkSettings.mFarClipPlaneDistance,
                    tanLeft, tanRight, tanUp, tanDown
                );
            }

            // Clean up depth texture
            glDeleteTextures(1, &depthTexture);

            // Pass 2: Render foreground objects (visible objects like plane, crates)
            // These render ON TOP of the parallax skybox
            for (Viewports::iterator iViewport = mViewports.begin(); iViewport != mViewports.end(); ++iViewport)
            {
                Viewport* viewport = *iViewport;
                if (!viewport->GetEnabled())
                    continue;

                // Skip zoom viewport in VR
                const int CAMERA_ZOOM_ID = 3;
                if ((size_t)viewport->GetCamera()->GetUserData() == CAMERA_ZOOM_ID)
                    continue;

                Camera& camera = *viewport->GetCamera();

                // Restore matrices for rendering
                esMatrixMode(GL_PROJECTION);
                esLoadIdentity();

                glm::mat4 projMatrix = runtime->GetProjectionMatrix((VREye)eye,
                    mFrameworkSettings.mNearClipPlaneDistance,
                    mFrameworkSettings.mFarClipPlaneDistance);
                esMultMatrixf(&projMatrix[0][0]);

                esMatrixMode(GL_MODELVIEW);
                esLoadIdentity();

                // Get base camera transform
                Transform baseTM;
                if (camera.GetCameraTransform())
                {
                    baseTM = camera.GetCameraTransform()->GetCameraTransform(camera.GetUserData());
                }
                else
                {
                    baseTM = camera.GetTransform();
                }

                // Get VR view matrix (VRManager handles panoramic scene mode)
                glm::mat4 vrViewMatrix = vrManager.GetViewMatrixForScene((VREye)eye);

                float yawOffset = vrManager.GetTotalYawOffset();
                glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), yawOffset, glm::vec3(0.0f, 0.0f, 1.0f));

                vrViewMatrix = vrViewMatrix * glm::inverse(yawRotation);

                bool vrOverridesOrientation = (camera.GetVROrientationMode() == Camera::VROrientationMode::Override);

                glm::mat4 baseMatrix = glm::mat4(1.0f);
                if (vrOverridesOrientation)
                {
                    baseMatrix[3] = glm::vec4(baseTM.t.x, baseTM.t.y, baseTM.t.z, 1.0f);
                }
                else
                {
                    baseMatrix[0] = glm::vec4(baseTM.m[0][0], baseTM.m[0][1], baseTM.m[0][2], 0.0f);
                    baseMatrix[1] = glm::vec4(baseTM.m[1][0], baseTM.m[1][1], baseTM.m[1][2], 0.0f);
                    baseMatrix[2] = glm::vec4(baseTM.m[2][0], baseTM.m[2][1], baseTM.m[2][2], 0.0f);
                    baseMatrix[3] = glm::vec4(baseTM.t.x, baseTM.t.y, baseTM.t.z, 1.0f);
                }

                glm::mat4 combinedView = vrViewMatrix * glm::inverse(baseMatrix);
                esMultMatrixf(&combinedView[0][0]);

                SetupLighting();

                // Render foreground objects only
                for (RenderObjects::iterator it = mRenderObjects.begin(); it != mRenderObjects.end(); ++it)
                {
                    RenderObject* renderObject = it->second;
                    if (!viewport->GetShouldRenderObject(renderObject))
                        continue;

                    int renderLevel = it->first;

                    // Only render visible objects (RENDER_LEVEL_OBJECTS and above)
                    if (renderLevel < RENDER_LEVEL_OBJECTS)
                        continue;

                    renderObject->RenderUpdate(viewport, renderLevel);
                }
            }
        }

        // Final color resolve for VR panorama depth mode (skybox + foreground were rendered to MSAA FBO)
        if (mVRPanoramaDepthEnabled && msaaSamples > 1)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, swapchainFBO);
            glBlitFramebuffer(0, 0, eyeWidth, eyeHeight,
                              0, 0, eyeWidth, eyeHeight,
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        // Render 2D overlays (buttons, windsock, challenge text) into the swapchain FBO
        {
            glBindFramebuffer(GL_FRAMEBUFFER, swapchainFBO);

            // Compute stereo pixel shift for overlay depth
            float eyeSign = (eye == VR_EYE_LEFT) ? 1.0f : -1.0f;
            glm::mat4 projMatrix = runtime->GetProjectionMatrix((VREye)eye,
                mFrameworkSettings.mNearClipPlaneDistance,
                mFrameworkSettings.mFarClipPlaneDistance);
            float ipd = runtime->GetIPD();
            float ndcShift = projMatrix[0][0] * (ipd * 0.5f) / mVROverlayDistance;
            float stereoPixelShift = ndcShift * float(eyeWidth) * 0.5f * eyeSign;

            RenderOverlaysForVREye(eyeWidth, eyeHeight, stereoPixelShift);
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
            glBindFramebuffer(GL_READ_FRAMEBUFFER, swapchainFBO);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mirrorFBO);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTex, 0);

            // Blit from swapchain to mirror texture
            glBlitFramebuffer(0, 0, eyeWidth, eyeHeight,
                              0, 0, mirrorWidth, mirrorHeight,
                              GL_COLOR_BUFFER_BIT, GL_LINEAR);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &mirrorFBO);
        }

        // Cleanup MSAA resources if used
        if (msaaSamples > 1)
        {
            glDeleteRenderbuffers(1, &msaaColorRB);
            glDeleteRenderbuffers(1, &msaaDepthRB);
            glDeleteFramebuffers(1, &msaaFBO);
        }

        // Cleanup swapchain FBO resources
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteRenderbuffers(1, &depthRB);
        glDeleteFramebuffers(1, &swapchainFBO);

        // Release swapchain image
        runtime->ReleaseSwapchainImage((VREye)eye);
    }
}

//======================================================================================================================
void RenderManager::RenderOverlaysForVREye(int eyeWidth, int eyeHeight, float stereoPixelShift)
{
    if (!mVROverlayVisible)
        return;

    // Use a fixed virtual screen so overlay sizes are independent of both
    // the desktop window size and the headset resolution.
    // virtualHeight = 720 matches FontRenderer's baseline (GetDisplayScale() == 1.0).
    // virtualWidth preserves the eye aspect ratio.
    const int virtualHeight = 720;
    int virtualWidth = (int)(720.0f * float(eyeWidth) / float(eyeHeight));

    // Tell FontRenderer to use the virtual height for scaling and Y-flip
    FontRenderer::SetDisplayHeightOverride(virtualHeight);

    DisplayConfig displayConfig;
    displayConfig.mViewpointIndex = 0;
    displayConfig.mLeft = 0;
    displayConfig.mBottom = 0;
    displayConfig.mWidth = virtualWidth;
    displayConfig.mHeight = virtualHeight;

    // Viewport covers the full eye texture — the ortho projection maps
    // virtual coords onto this area.
    glViewport(0, 0, eyeWidth, eyeHeight);

    // Compute ortho with scale margins + stereo shift.
    float invScale = 1.0f / mVROverlayScale;
    float marginX = float(virtualWidth)  * (invScale - 1.0f) * 0.5f;
    float marginY = float(virtualHeight) * (invScale - 1.0f) * 0.5f;
    float totalOrthoW = float(virtualWidth) * invScale;
    // stereoPixelShift is in eye pixels — convert to ortho (virtual) units
    float orthoShift = stereoPixelShift * totalOrthoW / float(eyeWidth);

    esMatrixMode(GL_PROJECTION);
    esLoadIdentity();
    esOrthof(
        -marginX - orthoShift,
        float(virtualWidth) + marginX - orthoShift,
        -marginY,
        float(virtualHeight) + marginY,
        1.0f, -1.0f);

    esMatrixMode(GL_MODELVIEW);
    esLoadIdentity();

    // Render overlay objects (buttons, windsock, etc.)
    for (RenderOverlayObjects::iterator it = mRenderOverlayObjects.begin(); it != mRenderOverlayObjects.end(); ++it)
    {
        (it->second)->RenderOverlayUpdate(it->first, displayConfig);
    }

    // Render Gx objects (challenge text, viewport frame, etc.) with depth disabled
    if (!mRenderGxObjects.empty())
    {
        DisableDepthMask disableDepthMask;
        DisableDepthTest disableDepthTest;

        for (RenderGxObjects::iterator it = mRenderGxObjects.begin(); it != mRenderGxObjects.end(); ++it)
        {
            (it->second)->GxRender(it->first, displayConfig);
        }
    }

    // Store VR overlay mapping for input hit testing (ButtonOverlay::IsPressed)
    mVROverlayMapping.active = true;
    mVROverlayMapping.virtualWidth = virtualWidth;
    mVROverlayMapping.virtualHeight = virtualHeight;
    mVROverlayMapping.invScale = invScale;

    // Draw mouse cursor in VR overlay when paused
    if (mVROverlayShowCursor)
    {
        int mx = s3ePointerGetTouchX(0);
        int my = s3ePointerGetTouchY(0);
        float dxFrac = (float)mx / (float)IwGxGetDisplayWidth();
        float dyFrac = (float)my / (float)IwGxGetDisplayHeight();

        // Map desktop [0,1] to ortho space (matching the projection set up above)
        float cursorX = -marginX + dxFrac * float(virtualWidth) * invScale;
        float cursorY = float(virtualHeight) + marginY - dyFrac * float(virtualHeight) * invScale;

        DrawVRCursor(cursorX, cursorY);
    }

    // Clear the override so desktop rendering is unaffected
    FontRenderer::SetDisplayHeightOverride(0);
}

//======================================================================================================================
void RenderManager::DrawVRCursor(float cursorX, float cursorY)
{
    const SimpleShader* shader = static_cast<const SimpleShader*>(
        ShaderManager::GetInstance().GetShader(SHADER_SIMPLE));
    shader->Use();

    // Reuse the current ortho projection (already set by RenderOverlaysForVREye)
    GLMat44 mvpMatrix;
    esGetMatrix(mvpMatrix, GL_PROJECTION);
    glUniformMatrix4fv(shader->u_mvpMatrix, 1, GL_FALSE, &mvpMatrix[0][0]);

    // Crosshair cursor — four bars around a center gap (like a + with a hole)
    // Dimensions in virtual pixels (720-tall virtual screen)
    float armLen  = 8.0f;   // length of each arm from gap edge
    float gap     = 3.0f;   // half-size of the center gap
    float thick   = 2.0f;   // half-thickness of each bar
    float outline = 1.5f;   // outline border width

    struct CursorVertex { float x, y, z, r, g, b, a; };

    // Each arm is a filled quad (6 vertices = 2 triangles).
    // Four arms: top, bottom, left, right.
    // Draw black outline first (slightly larger), then white fill.
    auto makeQuad = [](CursorVertex* out,
                       float x0, float y0, float x1, float y1,
                       float r, float g, float b, float a)
    {
        out[0] = { x0, y0, 0, r, g, b, a };
        out[1] = { x1, y0, 0, r, g, b, a };
        out[2] = { x0, y1, 0, r, g, b, a };
        out[3] = { x1, y0, 0, r, g, b, a };
        out[4] = { x1, y1, 0, r, g, b, a };
        out[5] = { x0, y1, 0, r, g, b, a };
    };

    // 4 arms x 6 vertices = 24 vertices per pass (outline + fill)
    CursorVertex outlineVerts[24];
    CursorVertex fillVerts[24];
    float o = outline;

    // Top arm
    makeQuad(&outlineVerts[0],
        cursorX - thick - o, cursorY + gap - o,
        cursorX + thick + o, cursorY + gap + armLen + o,
        0, 0, 0, 0.8f);
    makeQuad(&fillVerts[0],
        cursorX - thick, cursorY + gap,
        cursorX + thick, cursorY + gap + armLen,
        1, 1, 1, 1);

    // Bottom arm
    makeQuad(&outlineVerts[6],
        cursorX - thick - o, cursorY - gap - armLen - o,
        cursorX + thick + o, cursorY - gap + o,
        0, 0, 0, 0.8f);
    makeQuad(&fillVerts[6],
        cursorX - thick, cursorY - gap - armLen,
        cursorX + thick, cursorY - gap,
        1, 1, 1, 1);

    // Right arm
    makeQuad(&outlineVerts[12],
        cursorX + gap - o, cursorY - thick - o,
        cursorX + gap + armLen + o, cursorY + thick + o,
        0, 0, 0, 0.8f);
    makeQuad(&fillVerts[12],
        cursorX + gap, cursorY - thick,
        cursorX + gap + armLen, cursorY + thick,
        1, 1, 1, 1);

    // Left arm
    makeQuad(&outlineVerts[18],
        cursorX - gap - armLen - o, cursorY - thick - o,
        cursorX - gap + o, cursorY + thick + o,
        0, 0, 0, 0.8f);
    makeQuad(&fillVerts[18],
        cursorX - gap - armLen, cursorY - thick,
        cursorX - gap, cursorY + thick,
        1, 1, 1, 1);

    glEnableVertexAttribArray(shader->a_position);
    glEnableVertexAttribArray(shader->a_colour);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw black outline
    glVertexAttribPointer(shader->a_position, 3, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), &outlineVerts[0].x);
    glVertexAttribPointer(shader->a_colour, 4, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), &outlineVerts[0].r);
    glDrawArrays(GL_TRIANGLES, 0, 24);

    // Draw white fill
    glVertexAttribPointer(shader->a_position, 3, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), &fillVerts[0].x);
    glVertexAttribPointer(shader->a_colour, 4, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), &fillVerts[0].r);
    glDrawArrays(GL_TRIANGLES, 0, 24);

    glDisableVertexAttribArray(shader->a_position);
    glDisableVertexAttribArray(shader->a_colour);
    glEnable(GL_DEPTH_TEST);
}

//======================================================================================================================
void RenderManager::RenderMirrorWindow(VRMirrorMode mode)
{
    if (!VRManager::IsAvailable() || !VRManager::GetInstance().IsVREnabled())
    {
        return;
    }

    VRManager& vrManager = VRManager::GetInstance();

    int eyeWidth, eyeHeight;
    vrManager.GetEyeRenderTargetSize(VR_EYE_LEFT, eyeWidth, eyeHeight);

    // Bind the default framebuffer (desktop window)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mFrameworkSettings.mScreenWidth, mFrameworkSettings.mScreenHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate aspect ratio for the content we're displaying
    float contentAspect;
    if (mode == VR_MIRROR_BOTH_EYES || mode == VR_MIRROR_BOTH_EYES_CROSSED)
    {
        // Both eyes side by side = double width
        contentAspect = (float)(eyeWidth * 2) / (float)eyeHeight;
    }
    else
    {
        // Single eye
        contentAspect = (float)eyeWidth / (float)eyeHeight;
    }

    float screenAspect = (float)mFrameworkSettings.mScreenWidth / (float)mFrameworkSettings.mScreenHeight;

    float x0, y0, x1, y1;
    if (contentAspect > screenAspect)
    {
        // Content is wider - letterbox (black bars top/bottom)
        float height = mFrameworkSettings.mScreenWidth / contentAspect;
        float yOffset = (mFrameworkSettings.mScreenHeight - height) * 0.5f;
        x0 = 0;
        x1 = (float)mFrameworkSettings.mScreenWidth;
        y0 = yOffset;
        y1 = yOffset + height;
    }
    else
    {
        // Content is taller - pillarbox (black bars left/right)
        float width = mFrameworkSettings.mScreenHeight * contentAspect;
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

    // Texture coordinates (same for all quads)
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
    glUniform4f(overlayShader->u_colour, 1.0f, 1.0f, 1.0f, 1.0f);

    glVertexAttribPointer(overlayShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    glEnableVertexAttribArray(overlayShader->a_texCoord);

    glActiveTexture(GL_TEXTURE0);

    if (mode == VR_MIRROR_BOTH_EYES || mode == VR_MIRROR_BOTH_EYES_CROSSED)
    {
        // For crossed mode, swap left/right textures for cross-eyed viewing
        VREye leftHalfEye = (mode == VR_MIRROR_BOTH_EYES_CROSSED) ? VR_EYE_RIGHT : VR_EYE_LEFT;
        VREye rightHalfEye = (mode == VR_MIRROR_BOTH_EYES_CROSSED) ? VR_EYE_LEFT : VR_EYE_RIGHT;

        float xMid = (x0 + x1) * 0.5f;

        GLfloat ptsLeft[] = {
            x0, y0, 0,
            xMid, y0, 0,
            xMid, y1, 0,
            x0, y1, 0,
        };

        uint32_t leftTexture = vrManager.GetEyeColorTexture(leftHalfEye);
        if (leftTexture != 0)
        {
            glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, ptsLeft);
            glEnableVertexAttribArray(overlayShader->a_position);
            esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);
            glBindTexture(GL_TEXTURE_2D, leftTexture);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        // Draw on the right half
        GLfloat ptsRight[] = {
            xMid, y0, 0,
            x1, y0, 0,
            x1, y1, 0,
            xMid, y1, 0,
        };

        uint32_t rightTexture = vrManager.GetEyeColorTexture(rightHalfEye);
        if (rightTexture != 0)
        {
            glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, ptsRight);
            glEnableVertexAttribArray(overlayShader->a_position);
            esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);
            glBindTexture(GL_TEXTURE_2D, rightTexture);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
    else
    {
        // Single eye mode
        VREye eye = (mode == VR_MIRROR_RIGHT_EYE) ? VR_EYE_RIGHT : VR_EYE_LEFT;
        uint32_t eyeTexture = vrManager.GetEyeColorTexture(eye);

        if (eyeTexture != 0)
        {
            GLfloat pts[] = {
                x0, y0, 0,
                x1, y0, 0,
                x1, y1, 0,
                x0, y1, 0,
            };

            glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
            glEnableVertexAttribArray(overlayShader->a_position);
            esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);
            glBindTexture(GL_TEXTURE_2D, eyeTexture);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }

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
