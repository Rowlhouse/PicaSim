#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "Camera.h"
#include "RenderManager.h"

#include <set>

//======================================================================================================================
// This is passed to render objects to let them know how to display themselves, especially when there are 
// multiple viewpoints (for stereoscopic display) 
struct DisplayConfig
{
    int mViewpointIndex;
    int mLeft;
    int mBottom;
    int mWidth;
    int mHeight;
};

class Viewport : public RenderGxObject
{
public:
    /// Is enabled on creation
    Viewport(float left, float bottom, float width, float height, Camera* camera);
    ~Viewport();

    void GxRender(int renderLevel, DisplayConfig& displayConfig) override;

    void SetDrawFrame(bool drawFrame) {mDrawFrame = drawFrame;} 

    Camera* GetCamera() {return mCamera;}
    const Camera* GetCamera() const {return mCamera;}

    /// create/start the viewport rendering process and clears the viewport
    void SetupViewport(const struct DisplayConfig& displayConfig, bool resetting = false);
    void ReSetupViewport();

    /// Returns viewport width/height - only valid after SetupViewport has been called
    float GetAspectRatio() const {return mAspectRatio;}

    float GetWidth() const {return mWidth;}
    float GetHeight() const {return mHeight;}

    void Resize(float left, float bottom, float width, float height);

    void Enable(bool enable) {mEnabled = enable;}

    bool GetEnabled() const {return mEnabled;}

    void AddRenderObject(const class RenderObject* renderObject);
    void RemoveRenderObject(const class RenderObject* renderObject);
    void ClearRenderObjects();
    bool GetShouldRenderObject(const class RenderObject* renderObject) const;

private:
    typedef std::set<const class RenderObject*> RenderObjects;
    RenderObjects mRenderObjects;

    Camera* mCamera;
    DisplayConfig mDisplayConfig;
    float mLeft;
    float mBottom;
    float mWidth;
    float mHeight;
    float mAspectRatio;
    bool mDrawFrame;
    bool mEnabled;
};

#endif
