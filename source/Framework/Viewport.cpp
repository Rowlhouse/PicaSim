#include "Viewport.h"
#include "RenderManager.h"

#include <IwGL.h>
#include <IwGx.h>
#include <Iw2D.h>

//----------------------------------------------------------------------------------------------------------------------
Viewport::Viewport(float left, float bottom, float width, float height, Camera* camera)
{
  mCamera = camera;
  Resize(left, bottom, width, height);
  mAspectRatio = 1.0f;
  mEnabled = true;
  mDrawFrame = false;

  RenderManager::GetInstance().RegisterRenderGxObject(this, 10);
}

//----------------------------------------------------------------------------------------------------------------------
Viewport::~Viewport()
{
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 10);
}

//----------------------------------------------------------------------------------------------------------------------
void Viewport::Resize(float left, float bottom, float width, float height)
{
  mLeft = left;
  mBottom = bottom;
  mWidth = width;
  mHeight = height;
}

//----------------------------------------------------------------------------------------------------------------------
void Viewport::AddRenderObject(const class RenderObject* renderObject)
{
  mRenderObjects.insert(renderObject);
}

//----------------------------------------------------------------------------------------------------------------------
void Viewport::RemoveRenderObject(const class RenderObject* renderObject)
{
  mRenderObjects.erase(renderObject);
}

//----------------------------------------------------------------------------------------------------------------------
void Viewport::ClearRenderObjects()
{
  mRenderObjects.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool Viewport::GetShouldRenderObject(const class RenderObject* renderObject) const
{
  if (mRenderObjects.empty())
    return true;
  if (mRenderObjects.find(renderObject) != mRenderObjects.end())
    return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void Viewport::ReSetupViewport()
{
  SetupViewport(mDisplayConfig);
}

//----------------------------------------------------------------------------------------------------------------------
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

  // Workaround Marmalade bug where it doesn't rotate the OpenGL surface
  if (IsMarmaladeVersionLessThan(6,2,0) && fs.mOS == S3E_OS_ID_IPHONE)
  {
    if (s3eSurfaceGetInt(S3E_SURFACE_DEVICE_BLIT_DIRECTION) == S3E_SURFACE_BLIT_DIR_ROT90)
      glViewport( y, displayConfig.mWidth - (x+w), h, w );
    else
      glViewport( displayConfig.mHeight - (y+h), x, h, w );
  }
  else
  {
    glViewport( x, y, w, h );
  }

  mAspectRatio = (displayConfig.mWidth * mWidth) / (displayConfig.mHeight * mHeight);

  if (!resetting)
  {
    if (IsMarmaladeVersionLessThan(6,2,0) && fs.mOS == S3E_OS_ID_IPHONE)
    {
      if (s3eSurfaceGetInt(S3E_SURFACE_DEVICE_BLIT_DIRECTION) == S3E_SURFACE_BLIT_DIR_ROT90)
        glScissor( y, displayConfig.mWidth - (x+w), h, w );
      else
        glScissor( displayConfig.mHeight - (y+h), x, h, w );
    }
    else
    {
      glScissor( x, y, w, h );
    }
    glEnable(GL_SCISSOR_TEST);
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
    glClear(GL_DEPTH_BUFFER_BIT); 
    glDisable(GL_SCISSOR_TEST);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Viewport::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
  if (!mDrawFrame || !mEnabled)
    return;

  Iw2DSetColour(0x60606060); // AABBGGRR
  float x0 = mLeft * displayConfig.mWidth;
  float y0 = (1.0f - (mBottom + mHeight)) * displayConfig.mHeight;

  float w = mWidth * displayConfig.mWidth;
  float h = mHeight * displayConfig.mHeight;

  Iw2DDrawRect(CIwFVec2(x0, y0), CIwFVec2(w, h));
}
