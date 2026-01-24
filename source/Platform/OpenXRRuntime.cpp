#include "OpenXRRuntime.h"

#ifdef PICASIM_VR_SUPPORT

#include "Window.h"
#include "../Framework/Trace.h"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include <cstring>
#include <algorithm>

//======================================================================================================================
// Factory function implementation
//======================================================================================================================
VRRuntime* CreateVRRuntime()
{
    return new OpenXRRuntime();
}

//======================================================================================================================
// Helper macros
//======================================================================================================================
#define XR_CHECK(call) \
    do { \
        XrResult result = (call); \
        if (XR_FAILED(result)) { \
            TRACE_FILE_IF(1) TRACE("OpenXR error: %s returned %d", #call, (int)result); \
            return false; \
        } \
    } while (0)

#define XR_CHECK_VOID(call) \
    do { \
        XrResult result = (call); \
        if (XR_FAILED(result)) { \
            TRACE_FILE_IF(1) TRACE("OpenXR error: %s returned %d", #call, (int)result); \
            return; \
        } \
    } while (0)

//======================================================================================================================
// Constructor/Destructor
//======================================================================================================================
OpenXRRuntime::OpenXRRuntime()
    : mInstance(XR_NULL_HANDLE)
    , mSystemId(XR_NULL_SYSTEM_ID)
    , mSession(XR_NULL_HANDLE)
    , mLocalSpace(XR_NULL_HANDLE)
    , mSessionState(XR_SESSION_STATE_UNKNOWN)
    , mIsInitialized(false)
    , mIsRunning(false)
    , mShouldRender(false)
    , mPosesValid(false)
{
    memset(&mFrameState, 0, sizeof(mFrameState));
    mFrameState.type = XR_TYPE_FRAME_STATE;

    memset(&mHeadPose, 0, sizeof(mHeadPose));
    mHeadPose.orientation.w = 1.0f;

    for (int i = 0; i < VR_EYE_COUNT; ++i)
    {
        mSwapchains[i].swapchain = XR_NULL_HANDLE;
        mSwapchains[i].width = 0;
        mSwapchains[i].height = 0;
        mSwapchains[i].currentImageIndex = 0;
        mSwapchains[i].imageAcquired = false;

        memset(&mEyePoses[i], 0, sizeof(mEyePoses[i]));
        mEyePoses[i].orientation.w = 1.0f;

        memset(&mEyeFovs[i], 0, sizeof(mEyeFovs[i]));
    }
}

//----------------------------------------------------------------------------------------------------------------------
OpenXRRuntime::~OpenXRRuntime()
{
    Shutdown();
}

//======================================================================================================================
// Lifecycle
//======================================================================================================================
bool OpenXRRuntime::Initialize()
{
    if (mIsInitialized)
    {
        return true;
    }

    if (!CreateInstance())
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::Initialize - Failed to create instance");
        return false;
    }

    if (!GetSystem())
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::Initialize - Failed to get system");
        DestroyInstance();
        return false;
    }

    mIsInitialized = true;
    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::Initialize - Success: %s on %s",
        mRuntimeName.c_str(), mSystemName.c_str());
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::Shutdown()
{
    if (!mIsInitialized)
    {
        return;
    }

    if (mSession != XR_NULL_HANDLE)
    {
        DestroySession();
    }

    DestroyInstance();
    mIsInitialized = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::IsInitialized() const
{
    return mIsInitialized;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::IsHMDConnected() const
{
    return mIsInitialized && mSystemId != XR_NULL_SYSTEM_ID;
}

//======================================================================================================================
// Instance creation
//======================================================================================================================
bool OpenXRRuntime::CreateInstance()
{
    // Query available extensions
    uint32_t extensionCount = 0;
    XR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr));

    std::vector<XrExtensionProperties> extensions(extensionCount);
    for (auto& ext : extensions)
    {
        ext.type = XR_TYPE_EXTENSION_PROPERTIES;
        ext.next = nullptr;
    }
    XR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensions.data()));

    // Check for OpenGL extension
    bool hasOpenGL = false;
    for (const auto& ext : extensions)
    {
        if (strcmp(ext.extensionName, XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) == 0)
        {
            hasOpenGL = true;
            break;
        }
    }

    if (!hasOpenGL)
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::CreateInstance - OpenGL extension not available");
        return false;
    }

    // Required extensions
    const char* enabledExtensions[] = {
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
    };

    // Create instance
    XrInstanceCreateInfo createInfo = {};
    createInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;
    createInfo.next = nullptr;
    createInfo.createFlags = 0;
    strcpy(createInfo.applicationInfo.applicationName, "PicaSim");
    createInfo.applicationInfo.applicationVersion = 1;
    strcpy(createInfo.applicationInfo.engineName, "PicaSim");
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    createInfo.enabledApiLayerCount = 0;
    createInfo.enabledApiLayerNames = nullptr;
    createInfo.enabledExtensionCount = 1;
    createInfo.enabledExtensionNames = enabledExtensions;

    XR_CHECK(xrCreateInstance(&createInfo, &mInstance));

    // Get runtime info
    XrInstanceProperties instanceProps = {};
    instanceProps.type = XR_TYPE_INSTANCE_PROPERTIES;
    if (XR_SUCCEEDED(xrGetInstanceProperties(mInstance, &instanceProps)))
    {
        mRuntimeName = instanceProps.runtimeName;
    }
    else
    {
        mRuntimeName = "Unknown Runtime";
    }

    TRACE_FILE_IF(2) TRACE("OpenXRRuntime::CreateInstance - Runtime: %s", mRuntimeName.c_str());
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::DestroyInstance()
{
    if (mInstance != XR_NULL_HANDLE)
    {
        xrDestroyInstance(mInstance);
        mInstance = XR_NULL_HANDLE;
    }
    mSystemId = XR_NULL_SYSTEM_ID;
}

//======================================================================================================================
// System (HMD) discovery
//======================================================================================================================
bool OpenXRRuntime::GetSystem()
{
    XrSystemGetInfo systemInfo = {};
    systemInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemInfo.next = nullptr;
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrResult result = xrGetSystem(mInstance, &systemInfo, &mSystemId);
    if (XR_FAILED(result))
    {
        if (result == XR_ERROR_FORM_FACTOR_UNAVAILABLE)
        {
            TRACE_FILE_IF(1) TRACE("OpenXRRuntime::GetSystem - No HMD connected");
        }
        else
        {
            TRACE_FILE_IF(1) TRACE("OpenXRRuntime::GetSystem - xrGetSystem failed: %d", (int)result);
        }
        return false;
    }

    // Get system properties
    XrSystemProperties systemProps = {};
    systemProps.type = XR_TYPE_SYSTEM_PROPERTIES;
    if (XR_SUCCEEDED(xrGetSystemProperties(mInstance, mSystemId, &systemProps)))
    {
        mSystemName = systemProps.systemName;
    }
    else
    {
        mSystemName = "Unknown HMD";
    }

    // Get view configuration views (eye resolution)
    uint32_t viewCount = 0;
    XR_CHECK(xrEnumerateViewConfigurationViews(mInstance, mSystemId,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr));

    mConfigViews.resize(viewCount);
    for (auto& view : mConfigViews)
    {
        view.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        view.next = nullptr;
    }

    XR_CHECK(xrEnumerateViewConfigurationViews(mInstance, mSystemId,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, viewCount, &viewCount, mConfigViews.data()));

    mViews.resize(viewCount);
    for (auto& view : mViews)
    {
        view.type = XR_TYPE_VIEW;
        view.next = nullptr;
    }

    TRACE_FILE_IF(2) TRACE("OpenXRRuntime::GetSystem - %s, %d views", mSystemName.c_str(), (int)viewCount);
    for (size_t i = 0; i < mConfigViews.size(); ++i)
    {
        TRACE_FILE_IF(2) TRACE("  View %d: %dx%d recommended", (int)i,
            mConfigViews[i].recommendedImageRectWidth,
            mConfigViews[i].recommendedImageRectHeight);
    }

    return true;
}

//======================================================================================================================
// Session management
//======================================================================================================================
bool OpenXRRuntime::CreateSession()
{
    if (mSession != XR_NULL_HANDLE)
    {
        return true;
    }

    return CreateSessionInternal();
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::DestroySession()
{
    DestroyReferenceSpace();
    DestroySessionInternal();
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::CreateSessionInternal()
{
#ifdef _WIN32
    // Get Windows-specific handles from SDL
    Window* window = gWindow;
    if (!window || !window->GetSDLWindow())
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::CreateSessionInternal - No window available");
        return false;
    }

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(window->GetSDLWindow(), &wmInfo))
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::CreateSessionInternal - Failed to get window info");
        return false;
    }

    // Get the OpenGL function to retrieve graphics requirements
    PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR = nullptr;
    XR_CHECK(xrGetInstanceProcAddr(mInstance, "xrGetOpenGLGraphicsRequirementsKHR",
        (PFN_xrVoidFunction*)&xrGetOpenGLGraphicsRequirementsKHR));

    // Check graphics requirements
    XrGraphicsRequirementsOpenGLKHR graphicsReqs = {};
    graphicsReqs.type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR;
    XR_CHECK(xrGetOpenGLGraphicsRequirementsKHR(mInstance, mSystemId, &graphicsReqs));

    TRACE_FILE_IF(2) TRACE("OpenXRRuntime::CreateSessionInternal - OpenGL requirements: %d.%d to %d.%d",
        XR_VERSION_MAJOR(graphicsReqs.minApiVersionSupported),
        XR_VERSION_MINOR(graphicsReqs.minApiVersionSupported),
        XR_VERSION_MAJOR(graphicsReqs.maxApiVersionSupported),
        XR_VERSION_MINOR(graphicsReqs.maxApiVersionSupported));

    // Create graphics binding
    XrGraphicsBindingOpenGLWin32KHR graphicsBinding = {};
    graphicsBinding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR;
    graphicsBinding.next = nullptr;
    graphicsBinding.hDC = GetDC(wmInfo.info.win.window);
    graphicsBinding.hGLRC = (HGLRC)window->GetGLContext();

    // Create session
    XrSessionCreateInfo sessionInfo = {};
    sessionInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionInfo.next = &graphicsBinding;
    sessionInfo.createFlags = 0;
    sessionInfo.systemId = mSystemId;

    XR_CHECK(xrCreateSession(mInstance, &sessionInfo, &mSession));

    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::CreateSessionInternal - Session created");

    // Create reference space
    if (!CreateReferenceSpace())
    {
        DestroySessionInternal();
        return false;
    }

    return true;
#else
    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::CreateSessionInternal - Platform not supported yet");
    return false;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::DestroySessionInternal()
{
    if (mSession != XR_NULL_HANDLE)
    {
        xrDestroySession(mSession);
        mSession = XR_NULL_HANDLE;
    }
    mSessionState = XR_SESSION_STATE_UNKNOWN;
    mIsRunning = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::CreateReferenceSpace()
{
    // Create LOCAL reference space (seated experience)
    XrReferenceSpaceCreateInfo spaceInfo = {};
    spaceInfo.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
    spaceInfo.next = nullptr;
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    spaceInfo.poseInReferenceSpace.orientation.w = 1.0f;  // Identity

    XR_CHECK(xrCreateReferenceSpace(mSession, &spaceInfo, &mLocalSpace));

    TRACE_FILE_IF(2) TRACE("OpenXRRuntime::CreateReferenceSpace - LOCAL space created");
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::DestroyReferenceSpace()
{
    if (mLocalSpace != XR_NULL_HANDLE)
    {
        xrDestroySpace(mLocalSpace);
        mLocalSpace = XR_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
VRSessionState OpenXRRuntime::GetSessionState() const
{
    switch (mSessionState)
    {
        case XR_SESSION_STATE_UNKNOWN: return VR_SESSION_UNKNOWN;
        case XR_SESSION_STATE_IDLE: return VR_SESSION_IDLE;
        case XR_SESSION_STATE_READY: return VR_SESSION_READY;
        case XR_SESSION_STATE_SYNCHRONIZED: return VR_SESSION_SYNCHRONIZED;
        case XR_SESSION_STATE_VISIBLE: return VR_SESSION_VISIBLE;
        case XR_SESSION_STATE_FOCUSED: return VR_SESSION_FOCUSED;
        case XR_SESSION_STATE_STOPPING: return VR_SESSION_STOPPING;
        case XR_SESSION_STATE_LOSS_PENDING: return VR_SESSION_LOSS_PENDING;
        case XR_SESSION_STATE_EXITING: return VR_SESSION_EXITING;
        default: return VR_SESSION_UNKNOWN;
    }
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::IsSessionRunning() const
{
    return mIsRunning;
}

// Forward declaration (defined later in this file)
static const char* GetSessionStateName(XrSessionState state);

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::BeginSession()
{
    if (mSession == XR_NULL_HANDLE)
    {
        return false;
    }

    // Already running?
    if (mIsRunning)
    {
        return true;
    }

    // Poll events to check for state changes
    // Note: This may trigger HandleSessionStateChange which can start the session
    PollEvents();

    // Check again if session was started by HandleSessionStateChange
    if (mIsRunning)
    {
        return true;
    }

    // If not in READY state yet, that's OK - frame loop will retry later
    // This allows VR to be enabled before the headset is worn
    if (mSessionState != XR_SESSION_STATE_READY)
    {
        TRACE_FILE_IF(2) TRACE("OpenXRRuntime::BeginSession - Waiting for READY state (current: %s)",
            GetSessionStateName(mSessionState));
        return true;  // Not an error, just not ready yet
    }

    // Start the session (this case handles when READY was already set before PollEvents)
    XrSessionBeginInfo beginInfo = {};
    beginInfo.type = XR_TYPE_SESSION_BEGIN_INFO;
    beginInfo.next = nullptr;
    beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    XrResult result = xrBeginSession(mSession, &beginInfo);
    if (XR_FAILED(result))
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::BeginSession - xrBeginSession failed: %d", (int)result);
        return false;
    }

    mIsRunning = true;
    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::BeginSession - Session started successfully");
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::EndSession()
{
    if (mSession == XR_NULL_HANDLE || !mIsRunning)
    {
        return;
    }

    xrRequestExitSession(mSession);

    // Poll until we reach STOPPING state
    while (mSessionState != XR_SESSION_STATE_STOPPING &&
           mSessionState != XR_SESSION_STATE_EXITING)
    {
        PollEvents();
    }

    xrEndSession(mSession);
    mIsRunning = false;
    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::EndSession - Session ended");
}

//======================================================================================================================
// Event handling
//======================================================================================================================
void OpenXRRuntime::PollEvents()
{
    XrEventDataBuffer eventData = {};
    eventData.type = XR_TYPE_EVENT_DATA_BUFFER;

    while (xrPollEvent(mInstance, &eventData) == XR_SUCCESS)
    {
        switch (eventData.type)
        {
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
            {
                XrEventDataSessionStateChanged* stateEvent =
                    reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
                HandleSessionStateChange(stateEvent->state);
                break;
            }
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
            {
                TRACE_FILE_IF(1) TRACE("OpenXRRuntime::PollEvents - Instance loss pending");
                break;
            }
            default:
                break;
        }

        eventData = {};
        eventData.type = XR_TYPE_EVENT_DATA_BUFFER;
    }
}

//----------------------------------------------------------------------------------------------------------------------
static const char* GetSessionStateName(XrSessionState state)
{
    switch (state)
    {
        case XR_SESSION_STATE_UNKNOWN: return "UNKNOWN";
        case XR_SESSION_STATE_IDLE: return "IDLE";
        case XR_SESSION_STATE_READY: return "READY";
        case XR_SESSION_STATE_SYNCHRONIZED: return "SYNCHRONIZED";
        case XR_SESSION_STATE_VISIBLE: return "VISIBLE";
        case XR_SESSION_STATE_FOCUSED: return "FOCUSED";
        case XR_SESSION_STATE_STOPPING: return "STOPPING";
        case XR_SESSION_STATE_LOSS_PENDING: return "LOSS_PENDING";
        case XR_SESSION_STATE_EXITING: return "EXITING";
        default: return "INVALID";
    }
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::HandleSessionStateChange(XrSessionState newState)
{
    TRACE_FILE_IF(1) TRACE("OpenXR session state: %s -> %s",
        GetSessionStateName(mSessionState), GetSessionStateName(newState));
    mSessionState = newState;

    switch (newState)
    {
        case XR_SESSION_STATE_READY:
            // Auto-begin session when ready (needed because PollEvents is called
            // separately from the frame loop)
            if (!mIsRunning)
            {
                // Call xrBeginSession directly here to avoid recursion through BeginSession()
                XrSessionBeginInfo beginInfo = {};
                beginInfo.type = XR_TYPE_SESSION_BEGIN_INFO;
                beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
                XrResult result = xrBeginSession(mSession, &beginInfo);
                if (XR_SUCCEEDED(result))
                {
                    mIsRunning = true;
                    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::HandleSessionStateChange - Session started");
                }
                else
                {
                    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::HandleSessionStateChange - xrBeginSession failed: %d", (int)result);
                }
            }
            break;
        case XR_SESSION_STATE_STOPPING:
            // Must call xrEndSession when receiving STOPPING state
            if (mIsRunning)
            {
                XrResult result = xrEndSession(mSession);
                if (XR_SUCCEEDED(result))
                {
                    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::HandleSessionStateChange - Session ended");
                }
                else
                {
                    TRACE_FILE_IF(1) TRACE("OpenXRRuntime::HandleSessionStateChange - xrEndSession failed: %d", (int)result);
                }
                mIsRunning = false;
            }
            break;
        default:
            break;
    }
}

//======================================================================================================================
// Frame lifecycle
//======================================================================================================================
bool OpenXRRuntime::WaitFrame(VRFrameInfo& frameInfo)
{
    if (mSession == XR_NULL_HANDLE)
    {
        return false;
    }

    // Poll events
    PollEvents();

    // Handle session state transitions
    if (mSessionState == XR_SESSION_STATE_READY && !mIsRunning)
    {
        BeginSession();
    }

    if (!mIsRunning)
    {
        frameInfo.shouldRender = false;
        return true;
    }

    // Wait for frame
    XrFrameWaitInfo waitInfo = {};
    waitInfo.type = XR_TYPE_FRAME_WAIT_INFO;
    waitInfo.next = nullptr;

    mFrameState = {};
    mFrameState.type = XR_TYPE_FRAME_STATE;

    XrResult result = xrWaitFrame(mSession, &waitInfo, &mFrameState);
    if (XR_FAILED(result))
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::WaitFrame - xrWaitFrame failed: %d", (int)result);
        return false;
    }

    frameInfo.predictedDisplayTime = mFrameState.predictedDisplayTime;
    frameInfo.shouldRender = mFrameState.shouldRender == XR_TRUE;
    mShouldRender = frameInfo.shouldRender;

    // Locate views for this frame
    if (frameInfo.shouldRender && LocateViews(mFrameState.predictedDisplayTime))
    {
        for (int eye = 0; eye < VR_EYE_COUNT && eye < (int)mViews.size(); ++eye)
        {
            frameInfo.views[eye].position = XrVec3ToGlm(mViews[eye].pose.position);
            frameInfo.views[eye].orientation = XrQuatToGlm(mViews[eye].pose.orientation);
            frameInfo.views[eye].fovLeft = mViews[eye].fov.angleLeft;
            frameInfo.views[eye].fovRight = mViews[eye].fov.angleRight;
            frameInfo.views[eye].fovUp = mViews[eye].fov.angleUp;
            frameInfo.views[eye].fovDown = mViews[eye].fov.angleDown;
            frameInfo.views[eye].recommendedWidth = mSwapchains[eye].width;
            frameInfo.views[eye].recommendedHeight = mSwapchains[eye].height;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::BeginFrame()
{
    if (mSession == XR_NULL_HANDLE || !mIsRunning)
    {
        return false;
    }

    XrFrameBeginInfo beginInfo = {};
    beginInfo.type = XR_TYPE_FRAME_BEGIN_INFO;
    beginInfo.next = nullptr;

    XrResult result = xrBeginFrame(mSession, &beginInfo);
    if (XR_FAILED(result))
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::BeginFrame - xrBeginFrame failed: %d", (int)result);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::EndFrame(const VRFrameInfo& frameInfo)
{
    if (mSession == XR_NULL_HANDLE || !mIsRunning)
    {
        return false;
    }

    // Prepare projection views
    std::vector<XrCompositionLayerProjectionView> projectionViews(VR_EYE_COUNT);
    for (int eye = 0; eye < VR_EYE_COUNT; ++eye)
    {
        projectionViews[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
        projectionViews[eye].next = nullptr;
        projectionViews[eye].pose = mEyePoses[eye];
        projectionViews[eye].fov = mEyeFovs[eye];
        projectionViews[eye].subImage.swapchain = mSwapchains[eye].swapchain;
        projectionViews[eye].subImage.imageRect.offset = {0, 0};
        projectionViews[eye].subImage.imageRect.extent = {
            mSwapchains[eye].width,
            mSwapchains[eye].height
        };
        projectionViews[eye].subImage.imageArrayIndex = 0;
    }

    XrCompositionLayerProjection layer = {};
    layer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
    layer.next = nullptr;
    layer.layerFlags = 0;
    layer.space = mLocalSpace;
    layer.viewCount = (uint32_t)projectionViews.size();
    layer.views = projectionViews.data();

    const XrCompositionLayerBaseHeader* layers[] = { (XrCompositionLayerBaseHeader*)&layer };

    XrFrameEndInfo endInfo = {};
    endInfo.type = XR_TYPE_FRAME_END_INFO;
    endInfo.next = nullptr;
    endInfo.displayTime = mFrameState.predictedDisplayTime;
    endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    endInfo.layerCount = mShouldRender ? 1 : 0;
    endInfo.layers = mShouldRender ? layers : nullptr;

    XrResult result = xrEndFrame(mSession, &endInfo);
    if (XR_FAILED(result))
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::EndFrame - xrEndFrame failed: %d", (int)result);
        return false;
    }

    return true;
}

//======================================================================================================================
// View location
//======================================================================================================================
bool OpenXRRuntime::LocateViews(XrTime displayTime)
{
    XrViewState viewState = {};
    viewState.type = XR_TYPE_VIEW_STATE;

    XrViewLocateInfo locateInfo = {};
    locateInfo.type = XR_TYPE_VIEW_LOCATE_INFO;
    locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    locateInfo.displayTime = displayTime;
    locateInfo.space = mLocalSpace;

    uint32_t viewCount = (uint32_t)mViews.size();
    XrResult result = xrLocateViews(mSession, &locateInfo, &viewState, viewCount, &viewCount, mViews.data());
    if (XR_FAILED(result))
    {
        TRACE_FILE_IF(1) TRACE("OpenXRRuntime::LocateViews - xrLocateViews failed: %d", (int)result);
        mPosesValid = false;
        return false;
    }

    mPosesValid = (viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) &&
                  (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT);

    if (mPosesValid)
    {
        // Cache head pose (average of eye poses for now)
        if (mViews.size() >= 2)
        {
            mHeadPose.position.x = (mViews[0].pose.position.x + mViews[1].pose.position.x) * 0.5f;
            mHeadPose.position.y = (mViews[0].pose.position.y + mViews[1].pose.position.y) * 0.5f;
            mHeadPose.position.z = (mViews[0].pose.position.z + mViews[1].pose.position.z) * 0.5f;
            // Use left eye orientation as head orientation (close enough)
            mHeadPose.orientation = mViews[0].pose.orientation;
        }

        for (int eye = 0; eye < VR_EYE_COUNT && eye < (int)mViews.size(); ++eye)
        {
            mEyePoses[eye] = mViews[eye].pose;
            mEyeFovs[eye] = mViews[eye].fov;
        }
    }

    return mPosesValid;
}

//======================================================================================================================
// Swapchain management
//======================================================================================================================
bool OpenXRRuntime::CreateSwapchains()
{
    return CreateSwapchainsInternal();
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::DestroySwapchains()
{
    DestroySwapchainsInternal();
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::CreateSwapchainsInternal()
{
    if (mSession == XR_NULL_HANDLE)
    {
        return false;
    }

    // Enumerate swapchain formats
    uint32_t formatCount = 0;
    XR_CHECK(xrEnumerateSwapchainFormats(mSession, 0, &formatCount, nullptr));

    std::vector<int64_t> formats(formatCount);
    XR_CHECK(xrEnumerateSwapchainFormats(mSession, formatCount, &formatCount, formats.data()));

    // Prefer SRGB, fall back to RGBA8
    int64_t selectedFormat = GL_RGBA8;
    for (int64_t fmt : formats)
    {
        if (fmt == GL_SRGB8_ALPHA8)
        {
            selectedFormat = GL_SRGB8_ALPHA8;
            break;
        }
    }

    TRACE_FILE_IF(2) TRACE("OpenXRRuntime::CreateSwapchainsInternal - Using format: 0x%X", (int)selectedFormat);

    // Create swapchain for each eye
    for (int eye = 0; eye < VR_EYE_COUNT && eye < (int)mConfigViews.size(); ++eye)
    {
        XrSwapchainCreateInfo createInfo = {};
        createInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
        createInfo.next = nullptr;
        createInfo.createFlags = 0;
        createInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
        createInfo.format = selectedFormat;
        createInfo.sampleCount = 1;
        createInfo.width = mConfigViews[eye].recommendedImageRectWidth;
        createInfo.height = mConfigViews[eye].recommendedImageRectHeight;
        createInfo.faceCount = 1;
        createInfo.arraySize = 1;
        createInfo.mipCount = 1;

        XR_CHECK(xrCreateSwapchain(mSession, &createInfo, &mSwapchains[eye].swapchain));

        mSwapchains[eye].width = createInfo.width;
        mSwapchains[eye].height = createInfo.height;

        // Get swapchain images
        uint32_t imageCount = 0;
        XR_CHECK(xrEnumerateSwapchainImages(mSwapchains[eye].swapchain, 0, &imageCount, nullptr));

        mSwapchains[eye].images.resize(imageCount);
        for (auto& img : mSwapchains[eye].images)
        {
            img.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;
            img.next = nullptr;
        }

        XR_CHECK(xrEnumerateSwapchainImages(mSwapchains[eye].swapchain, imageCount, &imageCount,
            (XrSwapchainImageBaseHeader*)mSwapchains[eye].images.data()));

        TRACE_FILE_IF(2) TRACE("OpenXRRuntime::CreateSwapchainsInternal - Eye %d: %dx%d, %d images",
            eye, mSwapchains[eye].width, mSwapchains[eye].height, imageCount);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::DestroySwapchainsInternal()
{
    for (int eye = 0; eye < VR_EYE_COUNT; ++eye)
    {
        if (mSwapchains[eye].swapchain != XR_NULL_HANDLE)
        {
            xrDestroySwapchain(mSwapchains[eye].swapchain);
            mSwapchains[eye].swapchain = XR_NULL_HANDLE;
        }
        mSwapchains[eye].images.clear();
        mSwapchains[eye].width = 0;
        mSwapchains[eye].height = 0;
        mSwapchains[eye].currentImageIndex = 0;
        mSwapchains[eye].imageAcquired = false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::AcquireSwapchainImage(VREye eye, uint32_t& imageIndex)
{
    if (eye < 0 || eye >= VR_EYE_COUNT || mSwapchains[eye].swapchain == XR_NULL_HANDLE)
    {
        return false;
    }

    if (mSwapchains[eye].imageAcquired)
    {
        imageIndex = mSwapchains[eye].currentImageIndex;
        return true;
    }

    XrSwapchainImageAcquireInfo acquireInfo = {};
    acquireInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;

    XR_CHECK(xrAcquireSwapchainImage(mSwapchains[eye].swapchain, &acquireInfo, &imageIndex));

    mSwapchains[eye].currentImageIndex = imageIndex;
    mSwapchains[eye].imageAcquired = true;

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::WaitSwapchainImage(VREye eye)
{
    if (eye < 0 || eye >= VR_EYE_COUNT || mSwapchains[eye].swapchain == XR_NULL_HANDLE)
    {
        return false;
    }

    XrSwapchainImageWaitInfo waitInfo = {};
    waitInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
    waitInfo.timeout = XR_INFINITE_DURATION;

    XR_CHECK(xrWaitSwapchainImage(mSwapchains[eye].swapchain, &waitInfo));

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::ReleaseSwapchainImage(VREye eye)
{
    if (eye < 0 || eye >= VR_EYE_COUNT || mSwapchains[eye].swapchain == XR_NULL_HANDLE)
    {
        return;
    }

    if (!mSwapchains[eye].imageAcquired)
    {
        return;
    }

    XrSwapchainImageReleaseInfo releaseInfo = {};
    releaseInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;

    xrReleaseSwapchainImage(mSwapchains[eye].swapchain, &releaseInfo);

    mSwapchains[eye].imageAcquired = false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OpenXRRuntime::GetSwapchainTexture(VREye eye, uint32_t imageIndex) const
{
    if (eye < 0 || eye >= VR_EYE_COUNT || imageIndex >= mSwapchains[eye].images.size())
    {
        return 0;
    }
    return mSwapchains[eye].images[imageIndex].image;
}

//----------------------------------------------------------------------------------------------------------------------
int OpenXRRuntime::GetSwapchainWidth(VREye eye) const
{
    if (eye < 0 || eye >= VR_EYE_COUNT)
    {
        return 0;
    }
    return mSwapchains[eye].width;
}

//----------------------------------------------------------------------------------------------------------------------
int OpenXRRuntime::GetSwapchainHeight(VREye eye) const
{
    if (eye < 0 || eye >= VR_EYE_COUNT)
    {
        return 0;
    }
    return mSwapchains[eye].height;
}

//======================================================================================================================
// Head tracking
//======================================================================================================================
bool OpenXRRuntime::GetHeadPose(glm::vec3& position, glm::quat& orientation) const
{
    if (!mPosesValid)
    {
        return false;
    }

    position = XrVec3ToGlm(mHeadPose.position);
    orientation = XrQuatToGlm(mHeadPose.orientation);
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpenXRRuntime::GetEyePose(VREye eye, glm::vec3& position, glm::quat& orientation) const
{
    if (!mPosesValid || eye < 0 || eye >= VR_EYE_COUNT)
    {
        return false;
    }

    position = XrVec3ToGlm(mEyePoses[eye].position);
    orientation = XrQuatToGlm(mEyePoses[eye].orientation);
    return true;
}

//======================================================================================================================
// Projection
//======================================================================================================================
glm::mat4 OpenXRRuntime::GetProjectionMatrix(VREye eye, float nearClip, float farClip) const
{
    if (eye < 0 || eye >= VR_EYE_COUNT || !mPosesValid)
    {
        return glm::mat4(1.0f);
    }

    return CreateProjectionFov(mEyeFovs[eye], nearClip, farClip);
}

//----------------------------------------------------------------------------------------------------------------------
glm::mat4 OpenXRRuntime::GetViewMatrix(VREye eye) const
{
    if (eye < 0 || eye >= VR_EYE_COUNT || !mPosesValid)
    {
        return glm::mat4(1.0f);
    }

    glm::vec3 pos = XrVec3ToGlm(mEyePoses[eye].position);
    glm::quat rot = XrQuatToGlm(mEyePoses[eye].orientation);

    // OpenXR coordinate system: X=right, Y=up, Z=back (forward is -Z)
    // PicaSim coordinate system: X=forward, Y=left, Z=up
    // Required mapping:
    //   OpenXR X (right)   -> PicaSim -Y (right)
    //   OpenXR Y (up)      -> PicaSim Z (up)
    //   OpenXR -Z (forward)-> PicaSim X (forward)
    // This is achieved by: first rotate -90° around Y, then +90° around X
    // Resulting quaternion: (w=0.5, x=0.5, y=-0.5, z=-0.5)
    glm::quat coordTransform(0.5f, 0.5f, -0.5f, -0.5f);

    // Transform position and orientation to PicaSim's coordinate system
    glm::vec3 transformedPos = coordTransform * pos;
    glm::quat transformedRot = coordTransform * rot;

    // View matrix is inverse of pose
    glm::mat4 rotation = glm::mat4_cast(glm::inverse(transformedRot));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), -transformedPos);

    return rotation * translation;
}

//======================================================================================================================
// Runtime info
//======================================================================================================================
const char* OpenXRRuntime::GetRuntimeName() const
{
    return mRuntimeName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const char* OpenXRRuntime::GetSystemName() const
{
    return mSystemName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void OpenXRRuntime::GetRecommendedRenderTargetSize(VREye eye, int& width, int& height) const
{
    if (eye >= 0 && eye < (int)mConfigViews.size())
    {
        width = mConfigViews[eye].recommendedImageRectWidth;
        height = mConfigViews[eye].recommendedImageRectHeight;
    }
    else
    {
        width = 1920;
        height = 1080;
    }
}

//======================================================================================================================
// Utility functions
//======================================================================================================================
glm::vec3 OpenXRRuntime::XrVec3ToGlm(const XrVector3f& v)
{
    return glm::vec3(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------------------------------------
glm::quat OpenXRRuntime::XrQuatToGlm(const XrQuaternionf& q)
{
    return glm::quat(q.w, q.x, q.y, q.z);
}

//----------------------------------------------------------------------------------------------------------------------
glm::mat4 OpenXRRuntime::CreateProjectionFov(const XrFovf& fov, float nearClip, float farClip)
{
    float tanLeft = std::tan(fov.angleLeft);
    float tanRight = std::tan(fov.angleRight);
    float tanDown = std::tan(fov.angleDown);
    float tanUp = std::tan(fov.angleUp);

    float tanWidth = tanRight - tanLeft;
    float tanHeight = tanUp - tanDown;

    glm::mat4 result(0.0f);

    result[0][0] = 2.0f / tanWidth;
    result[1][1] = 2.0f / tanHeight;
    result[2][0] = (tanRight + tanLeft) / tanWidth;
    result[2][1] = (tanUp + tanDown) / tanHeight;
    result[2][2] = -(farClip + nearClip) / (farClip - nearClip);
    result[2][3] = -1.0f;
    result[3][2] = -(2.0f * farClip * nearClip) / (farClip - nearClip);

    return result;
}

#endif // PICASIM_VR_SUPPORT
