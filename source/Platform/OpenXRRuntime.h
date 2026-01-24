#ifndef OPENXR_RUNTIME_H
#define OPENXR_RUNTIME_H

#ifdef PICASIM_VR_SUPPORT

#include "VRRuntime.h"

// OpenXR headers
#define XR_USE_GRAPHICS_API_OPENGL
#ifdef _WIN32
#define XR_USE_PLATFORM_WIN32
#include <windows.h>
#endif
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <vector>
#include <string>

//======================================================================================================================
// OpenXRRuntime - OpenXR implementation of VRRuntime
//
// Implements VR functionality using the OpenXR standard. Works with Oculus,
// SteamVR, Windows Mixed Reality, and other OpenXR-compatible runtimes.
//======================================================================================================================
class OpenXRRuntime : public VRRuntime
{
public:
    OpenXRRuntime();
    virtual ~OpenXRRuntime();

    //----------------------------------------------------------------------------------------------------------------------
    // VRRuntime interface implementation
    //----------------------------------------------------------------------------------------------------------------------

    // Lifecycle
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override;
    bool IsHMDConnected() const override;

    // Session management
    bool CreateSession() override;
    void DestroySession() override;
    VRSessionState GetSessionState() const override;
    bool IsSessionRunning() const override;
    void PollEvents() override;
    bool BeginSession() override;
    void EndSession() override;

    // Frame lifecycle
    bool WaitFrame(VRFrameInfo& frameInfo) override;
    bool BeginFrame() override;
    bool EndFrame(const VRFrameInfo& frameInfo) override;

    // Swapchain management
    bool CreateSwapchains() override;
    void DestroySwapchains() override;
    bool AcquireSwapchainImage(VREye eye, uint32_t& imageIndex) override;
    bool WaitSwapchainImage(VREye eye) override;
    void ReleaseSwapchainImage(VREye eye) override;
    uint32_t GetSwapchainTexture(VREye eye, uint32_t imageIndex) const override;
    int GetSwapchainWidth(VREye eye) const override;
    int GetSwapchainHeight(VREye eye) const override;

    // Head tracking
    bool GetHeadPose(glm::vec3& position, glm::quat& orientation) const override;
    bool GetEyePose(VREye eye, glm::vec3& position, glm::quat& orientation) const override;

    // Projection
    glm::mat4 GetProjectionMatrix(VREye eye, float nearClip, float farClip) const override;
    glm::mat4 GetViewMatrix(VREye eye) const override;

    // Runtime info
    const char* GetRuntimeName() const override;
    const char* GetSystemName() const override;
    void GetRecommendedRenderTargetSize(VREye eye, int& width, int& height) const override;

private:
    //----------------------------------------------------------------------------------------------------------------------
    // Internal helper methods
    //----------------------------------------------------------------------------------------------------------------------

    bool CreateInstance();
    void DestroyInstance();

    bool GetSystem();

    bool CreateSessionInternal();
    void DestroySessionInternal();

    bool CreateReferenceSpace();
    void DestroyReferenceSpace();

    bool CreateSwapchainsInternal();
    void DestroySwapchainsInternal();

    void HandleSessionStateChange(XrSessionState newState);

    bool LocateViews(XrTime displayTime);

    // Convert OpenXR pose to glm types
    static glm::vec3 XrVec3ToGlm(const XrVector3f& v);
    static glm::quat XrQuatToGlm(const XrQuaternionf& q);
    static glm::mat4 CreateProjectionFov(const XrFovf& fov, float nearClip, float farClip);

    //----------------------------------------------------------------------------------------------------------------------
    // OpenXR handles
    //----------------------------------------------------------------------------------------------------------------------

    XrInstance mInstance;
    XrSystemId mSystemId;
    XrSession mSession;
    XrSpace mLocalSpace;      // Seated experience reference space

    //----------------------------------------------------------------------------------------------------------------------
    // View configuration
    //----------------------------------------------------------------------------------------------------------------------

    std::vector<XrViewConfigurationView> mConfigViews;
    std::vector<XrView> mViews;

    //----------------------------------------------------------------------------------------------------------------------
    // Swapchain data
    //----------------------------------------------------------------------------------------------------------------------

    struct SwapchainData
    {
        XrSwapchain swapchain;
        int width;
        int height;
        std::vector<XrSwapchainImageOpenGLKHR> images;
        uint32_t currentImageIndex;
        bool imageAcquired;
    };
    SwapchainData mSwapchains[VR_EYE_COUNT];

    //----------------------------------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------------------------------

    XrSessionState mSessionState;
    XrFrameState mFrameState;
    bool mIsInitialized;
    bool mIsRunning;
    bool mShouldRender;

    // Runtime info
    std::string mRuntimeName;
    std::string mSystemName;

    // Cached head pose from last frame
    mutable XrPosef mHeadPose;
    mutable XrPosef mEyePoses[VR_EYE_COUNT];
    mutable XrFovf mEyeFovs[VR_EYE_COUNT];
    mutable bool mPosesValid;
};

#endif // PICASIM_VR_SUPPORT

#endif // OPENXR_RUNTIME_H
