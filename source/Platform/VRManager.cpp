#include "VRManager.h"

#ifdef PICASIM_VR_SUPPORT

#include "VRRuntime.h"
#include "../Framework/Helpers.h"
#include "../Framework/Graphics.h"
#include "../Framework/Trace.h"

#include <glad/glad.h>

//==============================================================================
// Static member initialization
//==============================================================================
VRManager* VRManager::mInstance = nullptr;

//==============================================================================
// Singleton access
//==============================================================================
bool VRManager::Init()
{
    if (mInstance)
    {
        TRACE_FILE_IF(1) TRACE("VRManager::Init - Already initialized");
        return true;
    }

    mInstance = new VRManager();
    if (!mInstance->InitializeRuntime())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::Init - Failed to initialize VR runtime");
        delete mInstance;
        mInstance = nullptr;
        return false;
    }

    TRACE_FILE_IF(1) TRACE("VRManager::Init - Initialized successfully");
    return true;
}

//------------------------------------------------------------------------------
void VRManager::Terminate()
{
    if (mInstance)
    {
        mInstance->ShutdownRuntime();
        delete mInstance;
        mInstance = nullptr;
        TRACE_FILE_IF(1) TRACE("VRManager::Terminate - Shutdown complete");
    }
}

//------------------------------------------------------------------------------
bool VRManager::IsAvailable()
{
    return mInstance != nullptr && mInstance->mRuntime != nullptr;
}

//------------------------------------------------------------------------------
VRManager& VRManager::GetInstance()
{
    IwAssert(ROWLHOUSE, mInstance != nullptr);
    return *mInstance;
}

//==============================================================================
// Constructor/Destructor
//==============================================================================
VRManager::VRManager()
    : mRuntime(nullptr)
    , mVREnabled(false)
    , mInVRFrame(false)
    , mHeadPosition(0.0f)
    , mHeadOrientation(1.0f, 0.0f, 0.0f, 0.0f)  // Identity quaternion
    , mReferencePosition(0.0f)
    , mReferenceYaw(0.0f)
    , mManualAzimuthOffset(0.0f)
    , mTargetAzimuth(0.0f)
    , mCalibrated(false)
{
    for (int i = 0; i < VR_EYE_COUNT; ++i)
    {
        mEyeFramebuffers[i] = 0;
        mEyeColorTextures[i] = 0;
        mEyeDepthRenderbuffers[i] = 0;
        mEyeWidth[i] = 0;
        mEyeHeight[i] = 0;
    }
}

//------------------------------------------------------------------------------
VRManager::~VRManager()
{
    DestroyEyeFramebuffers();
}

//==============================================================================
// Runtime initialization
//==============================================================================
bool VRManager::InitializeRuntime()
{
    mRuntime = CreateVRRuntime();
    if (!mRuntime)
    {
        TRACE_FILE_IF(1) TRACE("VRManager::InitializeRuntime - No VR runtime available");
        return false;
    }

    if (!mRuntime->Initialize())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::InitializeRuntime - Failed to initialize runtime");
        delete mRuntime;
        mRuntime = nullptr;
        return false;
    }

    TRACE_FILE_IF(1) TRACE("VRManager::InitializeRuntime - Runtime: %s, System: %s",
        mRuntime->GetRuntimeName(), mRuntime->GetSystemName());

    return true;
}

//------------------------------------------------------------------------------
void VRManager::ShutdownRuntime()
{
    if (mVREnabled)
    {
        DisableVR();
    }

    if (mRuntime)
    {
        mRuntime->Shutdown();
        delete mRuntime;
        mRuntime = nullptr;
    }
}

//==============================================================================
// VR mode control
//==============================================================================
bool VRManager::EnableVR()
{
    if (mVREnabled)
    {
        return true;
    }

    if (!mRuntime || !mRuntime->IsHMDConnected())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::EnableVR - No HMD connected");
        return false;
    }

    // Create VR session
    if (!mRuntime->CreateSession())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::EnableVR - Failed to create session");
        return false;
    }

    // Create swapchains
    if (!mRuntime->CreateSwapchains())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::EnableVR - Failed to create swapchains");
        mRuntime->DestroySession();
        return false;
    }

    // Create eye framebuffers
    if (!CreateEyeFramebuffers())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::EnableVR - Failed to create eye framebuffers");
        mRuntime->DestroySwapchains();
        mRuntime->DestroySession();
        return false;
    }

    // Begin the session
    if (!mRuntime->BeginSession())
    {
        TRACE_FILE_IF(1) TRACE("VRManager::EnableVR - Failed to begin session");
        DestroyEyeFramebuffers();
        mRuntime->DestroySwapchains();
        mRuntime->DestroySession();
        return false;
    }

    mVREnabled = true;
    TRACE_FILE_IF(1) TRACE("VRManager::EnableVR - VR mode enabled");
    return true;
}

//------------------------------------------------------------------------------
void VRManager::DisableVR()
{
    if (!mVREnabled)
    {
        return;
    }

    if (mInVRFrame)
    {
        // Force end the current frame
        VRFrameInfo dummyFrame;
        dummyFrame.shouldRender = false;
        EndVRFrame(dummyFrame);
    }

    if (mRuntime)
    {
        mRuntime->EndSession();
        DestroyEyeFramebuffers();
        mRuntime->DestroySwapchains();
        mRuntime->DestroySession();
    }

    mVREnabled = false;
    TRACE_FILE_IF(1) TRACE("VRManager::DisableVR - VR mode disabled");
}

//------------------------------------------------------------------------------
bool VRManager::IsVRReady() const
{
    if (!mVREnabled || !mRuntime)
    {
        return false;
    }

    VRSessionState state = mRuntime->GetSessionState();
    return state == VR_SESSION_FOCUSED || state == VR_SESSION_VISIBLE;
}

//==============================================================================
// Frame management
//==============================================================================
bool VRManager::BeginVRFrame(VRFrameInfo& frameInfo)
{
    if (!mVREnabled || !mRuntime)
    {
        return false;
    }

    if (mInVRFrame)
    {
        TRACE_FILE_IF(1) TRACE("VRManager::BeginVRFrame - Already in VR frame");
        return false;
    }

    // Wait for optimal frame timing
    if (!mRuntime->WaitFrame(frameInfo))
    {
        return false;
    }

    // Update cached head pose
    mRuntime->GetHeadPose(mHeadPosition, mHeadOrientation);

    // Begin the frame
    if (!mRuntime->BeginFrame())
    {
        return false;
    }

    mInVRFrame = true;
    mCurrentFrameInfo = frameInfo;  // Store for access by RenderUpdateVR
    // Return true to indicate frame was begun - caller must call EndVRFrame!
    // Check frameInfo.shouldRender to decide whether to actually render.
    return true;
}

//------------------------------------------------------------------------------
void VRManager::EndVRFrame(const VRFrameInfo& frameInfo)
{
    if (!mInVRFrame || !mRuntime)
    {
        return;
    }

    mRuntime->EndFrame(frameInfo);
    mInVRFrame = false;
}

//==============================================================================
// Eye framebuffer management
//==============================================================================
bool VRManager::CreateEyeFramebuffers()
{
    if (!mRuntime)
    {
        return false;
    }

    for (int eye = 0; eye < VR_EYE_COUNT; ++eye)
    {
        int width, height;
        mRuntime->GetRecommendedRenderTargetSize((VREye)eye, width, height);

        mEyeWidth[eye] = width;
        mEyeHeight[eye] = height;

        // Create color texture
        glGenTextures(1, &mEyeColorTextures[eye]);
        glBindTexture(GL_TEXTURE_2D, mEyeColorTextures[eye]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create depth renderbuffer
        glGenRenderbuffers(1, &mEyeDepthRenderbuffers[eye]);
        glBindRenderbuffer(GL_RENDERBUFFER, mEyeDepthRenderbuffers[eye]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Create framebuffer
        glGenFramebuffers(1, &mEyeFramebuffers[eye]);
        glBindFramebuffer(GL_FRAMEBUFFER, mEyeFramebuffers[eye]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mEyeColorTextures[eye], 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mEyeDepthRenderbuffers[eye]);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            TRACE_FILE_IF(1) TRACE("VRManager::CreateEyeFramebuffers - Framebuffer incomplete for eye %d, status: 0x%x", eye, status);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            DestroyEyeFramebuffers();
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        TRACE_FILE_IF(2) TRACE("VRManager::CreateEyeFramebuffers - Created eye %d FBO: %dx%d", eye, width, height);
    }

    return true;
}

//------------------------------------------------------------------------------
void VRManager::DestroyEyeFramebuffers()
{
    for (int eye = 0; eye < VR_EYE_COUNT; ++eye)
    {
        if (mEyeFramebuffers[eye])
        {
            glDeleteFramebuffers(1, &mEyeFramebuffers[eye]);
            mEyeFramebuffers[eye] = 0;
        }
        if (mEyeColorTextures[eye])
        {
            glDeleteTextures(1, &mEyeColorTextures[eye]);
            mEyeColorTextures[eye] = 0;
        }
        if (mEyeDepthRenderbuffers[eye])
        {
            glDeleteRenderbuffers(1, &mEyeDepthRenderbuffers[eye]);
            mEyeDepthRenderbuffers[eye] = 0;
        }
        mEyeWidth[eye] = 0;
        mEyeHeight[eye] = 0;
    }
}

//==============================================================================
// Rendering helpers
//==============================================================================
void VRManager::BindEyeFramebuffer(VREye eye)
{
    if (eye < 0 || eye >= VR_EYE_COUNT)
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mEyeFramebuffers[eye]);
    glViewport(0, 0, mEyeWidth[eye], mEyeHeight[eye]);
}

//------------------------------------------------------------------------------
void VRManager::UnbindEyeFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//------------------------------------------------------------------------------
void VRManager::GetEyeRenderTargetSize(VREye eye, int& width, int& height) const
{
    if (eye >= 0 && eye < VR_EYE_COUNT)
    {
        width = mEyeWidth[eye];
        height = mEyeHeight[eye];
    }
    else
    {
        width = 0;
        height = 0;
    }
}

//------------------------------------------------------------------------------
uint32_t VRManager::GetEyeColorTexture(VREye eye) const
{
    if (eye >= 0 && eye < VR_EYE_COUNT)
    {
        return mEyeColorTextures[eye];
    }
    return 0;
}

//==============================================================================
// Head tracking
//==============================================================================
glm::vec3 VRManager::GetHeadPosition() const
{
    return mHeadPosition;
}

//------------------------------------------------------------------------------
glm::quat VRManager::GetHeadOrientation() const
{
    return mHeadOrientation;
}

//------------------------------------------------------------------------------
bool VRManager::GetHeadTransform(Transform& transform) const
{
    if (!mVREnabled || !mRuntime)
    {
        return false;
    }

    // Convert OpenXR coordinate system (Y-up, -Z forward) to PicaSim (Z-up, X forward)
    // OpenXR: X=right, Y=up, Z=backward (looking toward -Z)
    // PicaSim: X=forward, Y=left, Z=up

    // Create rotation matrix from quaternion
    glm::mat4 rotMat = glm::mat4_cast(mHeadOrientation);

    // Apply coordinate system conversion
    // This swaps Y and Z axes and adjusts for different forward direction
    glm::mat4 conversion = glm::mat4(
        0, 0, -1, 0,  // OpenXR -Z -> PicaSim X (forward)
        -1, 0, 0, 0,  // OpenXR -X -> PicaSim Y (left)
        0, 1, 0, 0,   // OpenXR Y -> PicaSim Z (up)
        0, 0, 0, 1
    );

    glm::mat4 convertedRot = conversion * rotMat * glm::transpose(conversion);

    // Extract rotation matrix (3x3) and set in Transform
    // Transform uses row-major storage
    transform.m[0][0] = convertedRot[0][0]; transform.m[0][1] = convertedRot[1][0]; transform.m[0][2] = convertedRot[2][0];
    transform.m[1][0] = convertedRot[0][1]; transform.m[1][1] = convertedRot[1][1]; transform.m[1][2] = convertedRot[2][1];
    transform.m[2][0] = convertedRot[0][2]; transform.m[2][1] = convertedRot[1][2]; transform.m[2][2] = convertedRot[2][2];

    // Convert position
    transform.t.x = -mHeadPosition.z;  // OpenXR -Z -> PicaSim X
    transform.t.y = -mHeadPosition.x;  // OpenXR -X -> PicaSim Y
    transform.t.z = mHeadPosition.y;   // OpenXR Y -> PicaSim Z

    return true;
}

//------------------------------------------------------------------------------
glm::mat4 VRManager::GetEyeViewMatrix(VREye eye) const
{
    if (!mRuntime)
    {
        return glm::mat4(1.0f);
    }
    return mRuntime->GetViewMatrix(eye);
}

//------------------------------------------------------------------------------
glm::mat4 VRManager::GetEyeProjectionMatrix(VREye eye, float nearClip, float farClip) const
{
    if (!mRuntime)
    {
        return glm::mat4(1.0f);
    }
    return mRuntime->GetProjectionMatrix(eye, nearClip, farClip);
}

//==============================================================================
// VR View Calibration
//==============================================================================
void VRManager::ResetVRView(int cameraMode, float facingAzimuth)
{
    // Capture current headset pose as reference
    mReferencePosition = mHeadPosition;

    // Extract yaw from current headset orientation
    // mHeadOrientation is in OpenXR's coordinate system (X=right, Y=up, Z=back)
    // Apply the same coordinate transformation as GetViewMatrix to convert to PicaSim (X=forward, Y=left, Z=up)
    // This is: first -90° around Y, then +90° around X = quaternion (0.5, 0.5, -0.5, -0.5)
    glm::quat coordTransform(0.5f, 0.5f, -0.5f, -0.5f);
    glm::quat transformedRot = coordTransform * mHeadOrientation;

    // Now extract yaw (rotation around Z in the transformed Z-up system)
    glm::vec3 euler = glm::eulerAngles(transformedRot);
    mReferenceYaw = euler.z;  // Yaw component in radians (Z-up system)

    TRACE_FILE_IF(1) TRACE("VRManager::ResetVRView - Eulers: %f %f %f",
        glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z));

    // Reset manual offset
    mManualAzimuthOffset = 0.0f;

    // Set target azimuth based on camera mode
    // CAMERA_GROUND = 1 (from PicaSim.h)
    // With correct coordinate transform, VR forward maps directly to PicaSim +X (forward)
    if (cameraMode == 1)  // CAMERA_GROUND
    {
        mTargetAzimuth = facingAzimuth;
    }
    else
    {
        // For chase/cockpit, target is camera forward (azimuth 0)
        mTargetAzimuth = 0.0f;
    }

    mCalibrated = true;
    TRACE_FILE_IF(1) TRACE("VRManager::ResetVRView - Calibrated (mode=%d, refYaw=%.1f, targetAz=%.1f)",
        cameraMode, glm::degrees(mReferenceYaw), glm::degrees(mTargetAzimuth));
}

//------------------------------------------------------------------------------
void VRManager::AdjustAzimuthOffset(float deltaDegrees)
{
    mManualAzimuthOffset += glm::radians(deltaDegrees);
    TRACE_FILE_IF(2) TRACE("VRManager::AdjustAzimuthOffset - Manual offset now %.1f degrees",
        glm::degrees(mManualAzimuthOffset));
}

//------------------------------------------------------------------------------
float VRManager::GetTotalYawOffset() const
{
    // Total offset = target azimuth + manual adjustment - reference yaw
    // This makes "headset forward at calibration" point toward the target azimuth
    return mTargetAzimuth + mManualAzimuthOffset - mReferenceYaw;
}

//==============================================================================
// Runtime info
//==============================================================================
const char* VRManager::GetRuntimeName() const
{
    if (!mRuntime)
    {
        return "None";
    }
    return mRuntime->GetRuntimeName();
}

//------------------------------------------------------------------------------
const char* VRManager::GetSystemName() const
{
    if (!mRuntime)
    {
        return "No HMD";
    }
    return mRuntime->GetSystemName();
}

#endif // PICASIM_VR_SUPPORT
