#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

//#include "Graphics.h"

//#include <vector>

#include "Camera.h"
#include "ClassesMarmelade.h"
#include "FrameBufferObject.h"
#include "FrameworkSettings.h"
#include <map>

class RenderObject;
class RenderOverlayObject;
class DebugRenderer;
class Viewport;
struct DisplayConfig;

enum RenderLevels
{
  RENDER_LEVEL_ANY = -9999,  // Only used for queries
  RENDER_LEVEL_SKYBOX  =            0,
  RENDER_LEVEL_PLAIN   =           10,
  RENDER_LEVEL_TERRAIN =           20,
  RENDER_LEVEL_OBJECTS =           30,
  RENDER_LEVEL_TERRAIN_SHADOW =    40,
  RENDER_LEVEL_DEBUG   =           50,
};

//---------------------------------------------------------------------------------------------------------------------
struct ShadowCaster
{
  RenderObject*      mRenderObject;
  Texture*           mTexture;
  FrameBufferObject* mFrameBufferObject;
};

//---------------------------------------------------------------------------------------------------------------------
class RenderGxObject
{
public:
  virtual ~RenderGxObject() {}
  virtual void GxRender(int renderLevel, DisplayConfig& displayConfig) = 0;
};

//---------------------------------------------------------------------------------------------------------------------
class RenderManager
{
public:
  typedef std::vector<RenderObject*> ShadowCasterObjects;

  /// Gets the singleton
  static RenderManager& GetInstance(); 

  /// Creates the singleton, and initially no viewports and cameras will exist
  static void Init(FrameworkSettings& frameworkSettings, class LoadingScreenHelper* loadingScreen);

  /// Destroys the singleton. Note that any created viewports and cameras will be destroyed
  static void Terminate();

  /// realDeltaTime is the real time since the last render update. gameDeltaTime is the game time - 
  /// i.e. zero when paused, and scaled according to time scale - so for effects that should follow slow mo.
  void Update(float realDeltaTime, float gameDeltaTime);

  void RenderUpdate();

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
  float GetShadowStrength() const {return mShadowStrength;}
  float GetShadowDecayHeight() const {return mShadowDecayHeight;}
  float GetShadowSizeScale() const {return mShadowSizeScale;}

  void SetLightingDirection(const Vector3& direction) {mLightingDirection = direction;}
  void SetLightingDirection(float bearing, float elevation);

  void EnableStereoscopy(bool enable) {mEnableStereoscopy = enable;}
  void SetStereoSeparation(float separation) {mStereoSeparation = separation;}

  const struct FrameworkSettings& GetFrameworkSettings() const {return mFrameworkSettings;}

private:
  /// Key into RenderObjects is the "level", so smaller numbers get rendered first.
  typedef std::multimap<int, RenderObject*> RenderObjects;
  typedef std::multimap<int, RenderOverlayObject*> RenderOverlayObjects;
  typedef std::multimap<int, RenderGxObject*> RenderGxObjects;
  typedef std::vector<Viewport*>  Viewports;
  typedef std::vector<Camera*>    Cameras;

  RenderManager(FrameworkSettings& frameworkSettings);
  ~RenderManager();

  void RenderFPS();
  void SetupLighting();

  static RenderManager* mInstance;

  Viewports mViewports;
  Cameras mCameras;
  RenderObjects mRenderObjects;
  RenderGxObjects mRenderGxObjects;
  RenderOverlayObjects mRenderOverlayObjects;
  ShadowCasterObjects mShadowCasterObjects;

  DebugRenderer* mDebugRenderer;

  Vector3 mLightingDiffuseColour;
  Vector3 mLightingAmbientColour;
  Vector3 mLightingDirection;
  float   mShadowStrength;
  float   mShadowDecayHeight;
  float   mShadowSizeScale;

  bool    mEnableStereoscopy;
  float   mStereoSeparation;
  bool    mUseMultiLights;

  FrameworkSettings& mFrameworkSettings;
};

#endif
