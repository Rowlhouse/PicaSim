#ifndef VR_MANAGER_H
#define VR_MANAGER_H

#ifdef PICASIM_VR_SUPPORT

#include "VRRuntime.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Forward declarations
struct SDL_Window;
class Transform;

//==============================================================================
// VRManager - Singleton that coordinates VR state with the application
//
// Responsibilities:
// - Manages VR runtime lifecycle
// - Provides easy access to VR state and head tracking
// - Coordinates VR frame timing with the render loop
// - Manages eye framebuffers for VR rendering
//==============================================================================
class VRManager
{
public:
    //--------------------------------------------------------------------------
    // Singleton access
    //--------------------------------------------------------------------------

    static bool Init();
    static void Terminate();
    static bool IsAvailable();
    static VRManager& GetInstance();

    //--------------------------------------------------------------------------
    // VR mode control
    //--------------------------------------------------------------------------

    // Enable VR mode. Returns true if VR was successfully enabled.
    bool EnableVR();

    // Disable VR mode and return to normal rendering.
    void DisableVR();

    // Check if VR mode is currently enabled and active.
    bool IsVREnabled() const { return mVREnabled && mRuntime != nullptr; }

    // Check if VR is ready for rendering (session is focused).
    bool IsVRReady() const;

    //--------------------------------------------------------------------------
    // Frame management
    //--------------------------------------------------------------------------

    // Begin a VR frame. Returns true if rendering should proceed.
    // Populates frameInfo with timing and view data.
    bool BeginVRFrame(VRFrameInfo& frameInfo);

    // End the current VR frame and submit to compositor.
    void EndVRFrame(const VRFrameInfo& frameInfo);

    // Check if we're currently in a VR frame.
    bool IsInVRFrame() const { return mInVRFrame; }

    // Get the current frame info (only valid when IsInVRFrame is true).
    const VRFrameInfo& GetCurrentFrameInfo() const { return mCurrentFrameInfo; }

    //--------------------------------------------------------------------------
    // Rendering helpers
    //--------------------------------------------------------------------------

    // Bind the framebuffer for rendering to a specific eye.
    void BindEyeFramebuffer(VREye eye);

    // Unbind the eye framebuffer (restore default framebuffer).
    void UnbindEyeFramebuffer();

    // Get the dimensions of the eye render target.
    void GetEyeRenderTargetSize(VREye eye, int& width, int& height) const;

    //--------------------------------------------------------------------------
    // Head tracking
    //--------------------------------------------------------------------------

    // Get the current head position in local space.
    glm::vec3 GetHeadPosition() const;

    // Get the current head orientation as a quaternion.
    glm::quat GetHeadOrientation() const;

    // Get the head transform (combined position and orientation).
    // Returns the transform in PicaSim's coordinate system (Z-up).
    bool GetHeadTransform(Transform& transform) const;

    // Get the view matrix for a specific eye.
    glm::mat4 GetEyeViewMatrix(VREye eye) const;

    // Get the projection matrix for a specific eye.
    glm::mat4 GetEyeProjectionMatrix(VREye eye, float nearClip, float farClip) const;

    //--------------------------------------------------------------------------
    // Runtime access
    //--------------------------------------------------------------------------

    VRRuntime* GetRuntime() { return mRuntime; }
    const VRRuntime* GetRuntime() const { return mRuntime; }

    // Get runtime/headset info for display
    const char* GetRuntimeName() const;
    const char* GetSystemName() const;

private:
    VRManager();
    ~VRManager();

    // Non-copyable
    VRManager(const VRManager&) = delete;
    VRManager& operator=(const VRManager&) = delete;

    bool InitializeRuntime();
    void ShutdownRuntime();

    // Create framebuffers for eye rendering
    bool CreateEyeFramebuffers();
    void DestroyEyeFramebuffers();

    static VRManager* mInstance;

    VRRuntime* mRuntime;
    bool mVREnabled;
    bool mInVRFrame;

    // Eye framebuffers (for rendering before copying to VR swapchain)
    uint32_t mEyeFramebuffers[VR_EYE_COUNT];
    uint32_t mEyeColorTextures[VR_EYE_COUNT];
    uint32_t mEyeDepthRenderbuffers[VR_EYE_COUNT];
    int mEyeWidth[VR_EYE_COUNT];
    int mEyeHeight[VR_EYE_COUNT];

    // Cached head pose from last WaitFrame
    glm::vec3 mHeadPosition;
    glm::quat mHeadOrientation;

    // Current frame info (valid during VR frame)
    VRFrameInfo mCurrentFrameInfo;
};

#endif // PICASIM_VR_SUPPORT

#endif // VR_MANAGER_H
