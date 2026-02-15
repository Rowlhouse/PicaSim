#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include "Camera.h"
#include "FrameworkSettings.h"
#include "Graphics.h"
#include "FrameBufferObject.h"
#include "../Platform/FontRenderer.h"

#include <vector>
#include <map>
#include <memory>

#ifdef PICASIM_VR_SUPPORT
struct VRFrameInfo;
#endif

class RenderObject;
class RenderOverlayObject;
class DebugRenderer;
class Viewport;
class Skybox;
struct DisplayConfig;

enum RenderLevels
{
    RENDER_LEVEL_ANY = -9999,  // Only used for queries
    RENDER_LEVEL_SKYBOX  =            0,
    RENDER_LEVEL_PLAIN   =           10,
    RENDER_LEVEL_TERRAIN =           20,
    RENDER_LEVEL_OBJECTS_BACKGROUND = 25,  // Invisible objects (collision proxies for panoramas)
    RENDER_LEVEL_OBJECTS =           30,
    RENDER_LEVEL_TERRAIN_SHADOW =    40,
    RENDER_LEVEL_DEBUG   =           50,
};

//======================================================================================================================
struct ShadowCaster
{
    RenderObject*      mRenderObject;
    Texture*           mTexture;
    FrameBufferObject* mFrameBufferObject;
};

//======================================================================================================================
class RenderGxObject
{
public:
    virtual ~RenderGxObject() {}
    virtual void GxRender(int renderLevel, DisplayConfig& displayConfig) = 0;
};

//======================================================================================================================
class RenderManager
{
public:
    typedef std::vector<RenderObject*> ShadowCasterObjects;

    /// Gets the singleton
    static RenderManager& GetInstance(); 

    static bool GetExists() { return mInstance != nullptr; }

    /// Creates the singleton, and initially no viewports and cameras will exist
    static void Init(FrameworkSettings& frameworkSettings, class LoadingScreenHelper* loadingScreen);

    /// Destroys the singleton. Note that any created viewports and cameras will be destroyed
    static void Terminate();

    ~RenderManager();

    /// realDeltaTime is the real time since the last render update. gameDeltaTime is the game time - 
    /// i.e. zero when paused, and scaled according to time scale - so for effects that should follow slow mo.
    void Update(float realDeltaTime, float gameDeltaTime);

    void RenderUpdate();

    /// Renders the game without swapping buffers - use this when you want to overlay
    /// ImGui or other UI on top of the game before swapping
    void RenderWithoutSwap();

    DebugRenderer& GetDebugRenderer() {return *mDebugRenderer;}

    /// Register for a render callback. The ordering is determined by the level, smaller 
    /// numbers happen earlier, with ranges suggested by RenderLevels
    void RegisterRenderObject(RenderObject* renderObject, int renderLevel);
    void UnregisterRenderObject(RenderObject* renderObject, int renderLevel);
    bool IsRenderObjectRegistered(const RenderObject* renderObject, int renderLevel = RENDER_LEVEL_ANY) const;

    /// Register for a render overlay callback. The ordering is determined by the level, smaller 
    /// numbers happen earlier, with ranges suggested by RenderLevels
    void RegisterRenderOverlayObject(RenderOverlayObject* renderOverlayObject, int renderLevel);
    void UnregisterRenderOverlayObject(RenderOverlayObject* renderOverlayObject, int renderLevel);
    bool IsRenderOverlayObjectRegistered(const RenderOverlayObject* renderOverlayObject, int renderLevel) const;

    void RegisterShadowCasterObject(RenderObject* renderObject);
    void UnregisterShadowCasterObject(RenderObject* renderObject);
    bool IsShadowCasterObjectRegistered(const RenderObject* renderObject) const;

    size_t GetNumShadowCasterObjects() const {return mShadowCasterObjects.size();}
    RenderObject& GetShadowCasterObject(size_t i) {return *mShadowCasterObjects[i];}
    const RenderObject& GetShadowCasterObject(size_t i) const {return *mShadowCasterObjects[i];}

    /// call this to register (or clear) a IwGx renderer - used to overlay text etc
    void RegisterRenderGxObject(RenderGxObject* renderGxObject, int renderLevel);
    void UnregisterRenderGxObject(RenderGxObject* renderGxObject, int renderLevel);
    bool IsRenderGxObjectRegistered(const RenderGxObject* renderObject, int renderLevel) const;

    /// Creates and returns a viewport that is associated with a camera (that can be shared between 
    /// viewports). Currently viewports are rendered in the order they are created, so the most 
    /// recently created viewport renders on top of the others.
    Viewport* CreateViewport(float left, float bottom, float right, float top, Camera* camera);

    /// Destroys the viewport
    void DestroyViewport(Viewport* viewport);

    /// Create and return a camera. 
    Camera* CreateCamera();

    /// Destroys the camera
    void DestroyCamera(Camera* camera);

    size_t GetNumCameras() const {return mCameras.size();}

    Camera* GetCamera(size_t index) {return mCameras[index];}

    const Vector3& GetLightingAmbientColour() const {return mLightingAmbientColour;}
    const Vector3& GetLightingDiffuseColour() const {return mLightingDiffuseColour;}
    const Vector3& GetLightingDirection() const {return mLightingDirection;}

    void SetLightingAmbientColour(const Vector3& colour) {mLightingAmbientColour = colour;}
    void SetLightingDiffuseColour(const Vector3& colour) {mLightingDiffuseColour = colour;}

    void SetShadowStrength(float shadowStrength) {mShadowStrength = shadowStrength;}
    void SetShadowDecayHeight(float shadowDecayHeight) {mShadowDecayHeight = shadowDecayHeight;}
    void SetShadowSizeScale(float shadowSizeScale) {mShadowSizeScale = shadowSizeScale;}
    void SetShadowBlur(float shadowBlur) {mShadowBlur = shadowBlur;}
    float GetShadowStrength() const {return mShadowStrength;}
    float GetShadowDecayHeight() const {return mShadowDecayHeight;}
    float GetShadowSizeScale() const {return mShadowSizeScale;}
    float GetShadowBlur() const {return mShadowBlur;}

    void SetLightingDirection(const Vector3& direction) {mLightingDirection = direction;}
    void SetLightingDirection(float bearing, float elevation);

#ifdef PICASIM_VR_SUPPORT
    // VR mirror window display mode
    enum VRMirrorMode
    {
        VR_MIRROR_LEFT_EYE = 0,         // Show left eye only
        VR_MIRROR_RIGHT_EYE = 1,        // Show right eye only
        VR_MIRROR_BOTH_EYES = 2,        // Show both eyes side by side (left-right)
        VR_MIRROR_BOTH_EYES_CROSSED = 3 // Show both eyes crossed (right-left) for cross-eyed viewing
    };

    // VR rendering - renders to VR headset swapchains
    void RenderUpdateVR(VRFrameInfo& frameInfo);

    // Render to mirror window after VR frame
    void RenderMirrorWindow(VRMirrorMode mode = VR_MIRROR_LEFT_EYE);

    // VR skybox with depth-based parallax
    void SetVRSkybox(Skybox* skybox) { mVRSkybox = skybox; }
    void SetVRPanoramaDepthEnabled(bool enabled) { mVRPanoramaDepthEnabled = enabled; }
    void SetVROverlayDistance(float distance) { mVROverlayDistance = distance; }
    void SetVROverlayScale(float scale) { mVROverlayScale = scale; }
    void SetVROverlayVisible(bool visible) { mVROverlayVisible = visible; }
    void SetVROverlayShowCursor(bool show) { mVROverlayShowCursor = show; }

    // VR overlay coordinate mapping for input hit testing
    struct VROverlayMapping {
        bool active = false;
        int virtualWidth = 0;
        int virtualHeight = 0;
        float invScale = 1.0f;
    };
    const VROverlayMapping& GetVROverlayMapping() const { return mVROverlayMapping; }
    void ResetVROverlayMapping() { mVROverlayMapping.active = false; }
#endif

    const struct FrameworkSettings& GetFrameworkSettings() const {return mFrameworkSettings;}

private:
    /// Key into RenderObjects is the "level", so smaller numbers get rendered first.
    typedef std::multimap<int, RenderObject*> RenderObjects;
    typedef std::multimap<int, RenderOverlayObject*> RenderOverlayObjects;
    typedef std::multimap<int, RenderGxObject*> RenderGxObjects;
    typedef std::vector<Viewport*>  Viewports;
    typedef std::vector<Camera*>    Cameras;

    RenderManager(FrameworkSettings& frameworkSettings);

    void RenderFPS();
    void SetupLighting();

    static std::unique_ptr<RenderManager> mInstance;

    Viewports mViewports;
    Cameras mCameras;
    RenderObjects mRenderObjects;
    RenderGxObjects mRenderGxObjects;
    RenderOverlayObjects mRenderOverlayObjects;
    ShadowCasterObjects mShadowCasterObjects;

    std::unique_ptr<DebugRenderer> mDebugRenderer;

    Vector3 mLightingDiffuseColour;
    Vector3 mLightingAmbientColour;
    Vector3 mLightingDirection;
    float   mShadowStrength;
    float   mShadowDecayHeight;
    float   mShadowSizeScale;
    float   mShadowBlur;

    bool    mUseMultiLights;

#ifdef PICASIM_VR_SUPPORT
    Skybox* mVRSkybox;
    bool    mVRPanoramaDepthEnabled;
    float   mVROverlayDistance;
    float   mVROverlayScale;
    bool    mVROverlayVisible;
    bool    mVROverlayShowCursor;
    VROverlayMapping mVROverlayMapping;
    void RenderOverlaysForVREye(int eyeWidth, int eyeHeight, float stereoPixelShift);
    void DrawVRCursor(float cursorX, float cursorY);
#endif

    FrameworkSettings& mFrameworkSettings;
};

#endif
