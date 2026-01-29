#include "Viewport.h"
#include "RenderManager.h"
#include "Graphics.h"
#include "../Platform/S3ECompat.h"

//======================================================================================================================
Viewport::Viewport(float left, float bottom, float width, float height, Camera* camera)
{
    mCamera = camera;
    Resize(left, bottom, width, height);
    mAspectRatio = 1.0f;
    mEnabled = true;
    mDrawFrame = false;

    RenderManager::GetInstance().RegisterRenderGxObject(this, 10);
}

//======================================================================================================================
Viewport::~Viewport()
{
    RenderManager::GetInstance().UnregisterRenderGxObject(this, 10);
}

//======================================================================================================================
void Viewport::Resize(float left, float bottom, float width, float height)
{
    mLeft = left;
    mBottom = bottom;
    mWidth = width;
    mHeight = height;
}

//======================================================================================================================
void Viewport::AddRenderObject(const class RenderObject* renderObject)
{
    mRenderObjects.insert(renderObject);
}

//======================================================================================================================
void Viewport::RemoveRenderObject(const class RenderObject* renderObject)
{
    mRenderObjects.erase(renderObject);
}

//======================================================================================================================
void Viewport::ClearRenderObjects()
{
    mRenderObjects.clear();
}

//======================================================================================================================
bool Viewport::GetShouldRenderObject(const class RenderObject* renderObject) const
{
    if (mRenderObjects.empty())
        return true;
    if (mRenderObjects.find(renderObject) != mRenderObjects.end())
        return true;
    return false;
}

//======================================================================================================================
void Viewport::ReSetupViewport()
{
    SetupViewport(mDisplayConfig);
}

//======================================================================================================================
void Viewport::SetupViewport(const DisplayConfig& displayConfig, bool resetting)
{
    if (!resetting)
        mDisplayConfig = displayConfig;

    if (!mEnabled)
        return;

    const FrameworkSettings& fs = mCamera->GetFrameworkSettings();

    GLint x = (GLint) (displayConfig.mLeft + mLeft * displayConfig.mWidth);
    GLint y = (GLint) (displayConfig.mBottom + mBottom * displayConfig.mHeight);
    GLint w = (GLint) (mWidth * displayConfig.mWidth);
    GLint h = (GLint) (mHeight * displayConfig.mHeight);
    glViewport( x, y, w, h );

    mAspectRatio = (displayConfig.mWidth * mWidth) / (displayConfig.mHeight * mHeight);

    if (!resetting)
    {
        glScissor( x, y, w, h );
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }
}

//======================================================================================================================
void Viewport::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
    // Frame drawing removed - was using Iw2D stubs
}

