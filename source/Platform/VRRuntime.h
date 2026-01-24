#ifndef VR_RUNTIME_H
#define VR_RUNTIME_H

#ifdef PICASIM_VR_SUPPORT

#include <string>
#include <cstdint>

// Forward declarations - actual types defined in Helpers.h
class Transform;
class Vector3;

// GLM for matrix and quaternion types
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

//==============================================================================
// VR eye identifiers
//==============================================================================
enum VREye
{
    VR_EYE_LEFT = 0,
    VR_EYE_RIGHT = 1,
    VR_EYE_COUNT = 2
};

//==============================================================================
// VR session state
//==============================================================================
enum VRSessionState
{
    VR_SESSION_UNKNOWN = 0,
    VR_SESSION_IDLE,          // Session created but not running
    VR_SESSION_READY,         // Ready to begin
    VR_SESSION_SYNCHRONIZED,  // Synchronized with runtime
    VR_SESSION_VISIBLE,       // App is visible but not focused
    VR_SESSION_FOCUSED,       // App has input focus
    VR_SESSION_STOPPING,      // Session ending
    VR_SESSION_LOSS_PENDING,  // Session loss imminent
    VR_SESSION_EXITING        // Session ended
};

//==============================================================================
// Per-eye view information
//==============================================================================
struct VRViewInfo
{
    glm::mat4 projectionMatrix;  // Projection matrix for this eye
    glm::mat4 viewMatrix;        // View matrix for this eye (inverse of pose)
    glm::vec3 position;          // Eye position in local space
    glm::quat orientation;       // Eye orientation as quaternion
    float fovLeft;               // Field of view angles (radians)
    float fovRight;
    float fovUp;
    float fovDown;
    int recommendedWidth;        // Recommended render target size
    int recommendedHeight;
};

//==============================================================================
// VR frame timing information
//==============================================================================
struct VRFrameInfo
{
    int64_t predictedDisplayTime;  // When this frame will be displayed (nanoseconds)
    bool shouldRender;             // Whether to render this frame
    VRViewInfo views[VR_EYE_COUNT];
};

//==============================================================================
// Abstract VR runtime interface
//
// This interface abstracts the VR runtime (OpenXR, SteamVR, etc.) from the
// rest of the application. Implementations handle the runtime-specific
// initialization, session management, and frame lifecycle.
//==============================================================================
class VRRuntime
{
public:
    virtual ~VRRuntime() {}

    //--------------------------------------------------------------------------
    // Lifecycle
    //--------------------------------------------------------------------------

    // Initialize the VR runtime. Returns true on success.
    virtual bool Initialize() = 0;

    // Shutdown the VR runtime and release all resources.
    virtual void Shutdown() = 0;

    // Check if the runtime is initialized.
    virtual bool IsInitialized() const = 0;

    // Check if a headset is connected and available.
    virtual bool IsHMDConnected() const = 0;

    //--------------------------------------------------------------------------
    // Session management
    //--------------------------------------------------------------------------

    // Create a VR session. Must be called after Initialize().
    // graphicsBinding is platform-specific (e.g., HDC/HGLRC on Windows).
    virtual bool CreateSession() = 0;

    // Destroy the current session.
    virtual void DestroySession() = 0;

    // Get the current session state.
    virtual VRSessionState GetSessionState() const = 0;

    // Begin the VR session (start rendering to headset).
    virtual bool BeginSession() = 0;

    // End the VR session (stop rendering to headset).
    virtual void EndSession() = 0;

    //--------------------------------------------------------------------------
    // Frame lifecycle
    //--------------------------------------------------------------------------

    // Wait for the optimal time to begin rendering the next frame.
    // Populates frameInfo with timing and view information.
    virtual bool WaitFrame(VRFrameInfo& frameInfo) = 0;

    // Begin rendering a frame. Call after WaitFrame.
    virtual bool BeginFrame() = 0;

    // End the frame and submit to the compositor.
    virtual bool EndFrame(const VRFrameInfo& frameInfo) = 0;

    //--------------------------------------------------------------------------
    // Swapchain management
    //--------------------------------------------------------------------------

    // Create swapchains for rendering. Called after CreateSession.
    virtual bool CreateSwapchains() = 0;

    // Destroy swapchains.
    virtual void DestroySwapchains() = 0;

    // Acquire the next swapchain image for rendering.
    // Returns the image index in imageIndex.
    virtual bool AcquireSwapchainImage(VREye eye, uint32_t& imageIndex) = 0;

    // Wait for the swapchain image to be available for rendering.
    virtual bool WaitSwapchainImage(VREye eye) = 0;

    // Release the swapchain image after rendering.
    virtual void ReleaseSwapchainImage(VREye eye) = 0;

    // Get the OpenGL texture ID for a swapchain image.
    virtual uint32_t GetSwapchainTexture(VREye eye, uint32_t imageIndex) const = 0;

    // Get the swapchain dimensions.
    virtual int GetSwapchainWidth(VREye eye) const = 0;
    virtual int GetSwapchainHeight(VREye eye) const = 0;

    //--------------------------------------------------------------------------
    // Head tracking
    //--------------------------------------------------------------------------

    // Get the current head pose (position and orientation).
    // The pose is in the local reference space (seated origin).
    virtual bool GetHeadPose(glm::vec3& position, glm::quat& orientation) const = 0;

    // Get the pose for a specific eye (includes IPD offset).
    virtual bool GetEyePose(VREye eye, glm::vec3& position, glm::quat& orientation) const = 0;

    //--------------------------------------------------------------------------
    // Projection
    //--------------------------------------------------------------------------

    // Get the projection matrix for an eye.
    virtual glm::mat4 GetProjectionMatrix(VREye eye, float nearClip, float farClip) const = 0;

    // Get the view matrix for an eye (inverse of eye pose).
    virtual glm::mat4 GetViewMatrix(VREye eye) const = 0;

    //--------------------------------------------------------------------------
    // Runtime info
    //--------------------------------------------------------------------------

    // Get the name of the VR runtime (e.g., "Oculus", "SteamVR").
    virtual const char* GetRuntimeName() const = 0;

    // Get the name of the connected headset.
    virtual const char* GetSystemName() const = 0;

    // Get recommended render target size for an eye.
    virtual void GetRecommendedRenderTargetSize(VREye eye, int& width, int& height) const = 0;
};

//==============================================================================
// Factory function to create the appropriate VR runtime
// Returns nullptr if no VR runtime is available.
//==============================================================================
VRRuntime* CreateVRRuntime();

#endif // PICASIM_VR_SUPPORT

#endif // VR_RUNTIME_H
