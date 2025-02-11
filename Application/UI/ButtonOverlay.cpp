#include "ButtonOverlay.h"
#include "FonctionsMarmelade.h"
#include "FonctionsPointerKeyboardMarmelade.h"
#include "Graphics.h"
#include "RenderManager.h"
#include "Trace.h"
#include "Helpers.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "Viewport.h"

//#include <s3e.h>
//#include <IwGx.h>
//#include <IwGxFont.h>

//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::Init(
  float size, float paddingFraction,
  AnchorH anchorH, AnchorV anchorV, 
  float x, float y, 
  GLubyte alpha, bool enabled, bool enableText)
{
  mSize = size; 
  mPaddingFraction = paddingFraction; 
  mAnchorH = anchorH; mAnchorV = anchorV; 
  mText.clear(); 
  mX = x; mY = y; mX0 = 0; mY0 = 0; mX1 = 0; mY1 = 0; 
  mEnabled = enabled; 
  mEnableText = enableText;
  mColour[0] = mColour[1] = mColour[2] = 255;
  mColour[3] = alpha;
  mTextColour = Vector4(1,1,1,1);
  mFont = 0;
  mTextAnchorV = ANCHOR_V_BOT;
  RenderManager::GetInstance().RegisterRenderOverlayObject(this, 0);

  int pointerType = s3ePointerGetInt(S3E_POINTER_TYPE);
  if (pointerType == S3E_POINTER_TYPE_MOUSE)
    mPaddingFraction = 0.0f;

  if (enabled)
    mAlpha = alpha/255.0f;
  else
    mAlpha = 0.0f;
  mAlphaRate = 0.0f;

  if (mEnableText)
    RenderManager::GetInstance().RegisterRenderGxObject(this, 0);
}

//---------------------------------------------------------------------------------------------------------------------
ButtonOverlay::ButtonOverlay(
  const char* imageFile, float size, float paddingFraction,
  AnchorH anchorH, AnchorV anchorV, 
  float x, float y, 
  GLubyte alpha, bool enabled, bool enableText)
{
  mOwnsTexture = true;
  mTexture = new Texture;
  LoadTextureFromFile(*mTexture, imageFile);
  mTexture->SetFormatHW(CIwImage::RGBA_4444);
  mTexture->Upload();
  TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", imageFile, mTexture->m_HWID);
  Init(size, paddingFraction, anchorH, anchorV, x, y, alpha, enabled, enableText);
}

//---------------------------------------------------------------------------------------------------------------------
ButtonOverlay::ButtonOverlay(
  Texture* texture, float size, float paddingFraction,
  AnchorH anchorH, AnchorV anchorV, 
  float x, float y, 
  GLubyte alpha, bool enabled, bool enableText)
{
  mOwnsTexture = false;
  mTexture = texture;
  Init(size, paddingFraction, anchorH, anchorV, x, y, alpha, enabled, enableText);
}

//---------------------------------------------------------------------------------------------------------------------
ButtonOverlay::~ButtonOverlay()
{
  RenderManager::GetInstance().UnregisterRenderOverlayObject(this, 0);
  if (mEnableText)
    RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
  if (mOwnsTexture)
    delete mTexture;
}

//---------------------------------------------------------------------------------------------------------------------
bool ButtonOverlay::IsPressed(uint32_t pressMask) const
{
  if (!mEnabled)
    return false;

  uint32_t displayWidth = IwGxGetScreenWidth(); 
  uint32_t displayHeight = IwGxGetScreenHeight(); 

  float paddingX = mPaddingFraction * (mX1 - mX0);
  float paddingY = mPaddingFraction * (mY1 - mY0);

  for (uint32_t iTouch = 0 ; iTouch != S3E_POINTER_TOUCH_MAX ; ++iTouch)
  {
    if (
      ( (pressMask & PRESS_DOWN) && s3ePointerGetTouchState(iTouch) & S3E_POINTER_STATE_DOWN) ||
      ( (pressMask & PRESS_CLICKED) && s3ePointerGetTouchState(iTouch) & S3E_POINTER_STATE_PRESSED)
      )
    {
      int32_t touchX = s3ePointerGetTouchX(iTouch);
      int32_t touchY = s3ePointerGetTouchY(iTouch);

      float x = (float) touchX / displayWidth;
      float y = 1.0f - (float) touchY / displayHeight;

      if (
        x > (mX0 -paddingX) && x < (mX1 + paddingX) && 
        y > (mY0 - paddingY) && y < (mY1 + paddingY)
        )
      {
        return true;
      }
    }
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::RenderOverlayUpdate(float deltaTime)
{
  float smoothTime = 0.2f;
  if (!mEnabled)
    SmoothCD(mAlpha, mAlphaRate, deltaTime, 0.0f, smoothTime);
    //mAlpha = 0.0f;
  else
    SmoothCD(mAlpha, mAlphaRate, deltaTime, mColour[3]/255.0f, smoothTime);
}

static GLfloat uvs[] = {
  0, 1,
  1, 1,
  1, 0,
  0, 0,
};

//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig)
{
  // Need to calculate the touch positions so don't early out if disabled yet

  // s is the full size in pixels
  // The buttons are arranged horizontally so it's more useful for the size to be the horizontal size - so they can be set to fill the space.
  float s = mSize * Maximum(displayConfig.mWidth, displayConfig.mHeight);
  float s2 = s * 0.5f; // half width

  float x0, x1, y0, y1;

  // midpoint of the button
  float x = displayConfig.mLeft + mX * displayConfig.mWidth;
  float y = displayConfig.mBottom + mY * displayConfig.mHeight;
  x0 = x - mAnchorH * s2;
  x1 = x0 + s;
  y0 = y - mAnchorV * s2;
  y1 = y0 + s;

  if (displayConfig.mViewpointIndex == 0)
  {
    const FrameworkSettings& fs = RenderManager::GetInstance().GetFrameworkSettings();
    mX0 = x0/fs.mScreenWidth;
    mY0 = y0/fs.mScreenHeight;
    mX1 = x1/fs.mScreenWidth;
    mY1 = y1/fs.mScreenHeight;
  }

  if (!mEnabled)
    return;

  if (mAlpha < 0.001f)
    return;

  if (!(mTexture->GetFlags() & Texture::UPLOADED_F))
    return;

  DisableDepthTest disableDepthTest;
  DisableDepthMask disableDepthMask;
  EnableBlend enableBlend;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLfloat pts[] = {
    x0, y0, 0,
    x1, y0, 0,
    x1, y1, 0,
    x0, y1, 0,
  };

  int mvpLoc = -1;

  if (gGLVersion == 1)
  {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pts);
    glTexCoordPointer(2, GL_FLOAT, 0, uvs);

    glColor4ub(mColour[0], mColour[1], mColour[2], (GLubyte) (mAlpha * 255));
  }
  else
  {
    const OverlayShader* overlayShader = (OverlayShader*) ShaderManager::GetInstance().GetShader(SHADER_OVERLAY);
    overlayShader->Use();

    // Get the variable locations
    glUniform1i(overlayShader->u_texture, 0);

    glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glEnableVertexAttribArray(overlayShader->a_position);

    glVertexAttribPointer(overlayShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    glEnableVertexAttribArray(overlayShader->a_texCoord);

    glUniform4f(overlayShader->u_colour, mColour[0]/255.0f, mColour[1]/255.0f, mColour[2]/255.0f, mAlpha);
    esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTexture->m_HWID);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::SetText(const char* txt, CIwGxFont* font, AnchorH textAnchorH, AnchorV textAnchorV, float offsetX, float offsetY)
{
  mText = txt;
  mFont = font;
  mTextAnchorH = textAnchorH;
  mTextAnchorV = textAnchorV;
  mTextOffsetX = offsetX;
  mTextOffsetY = offsetY;
}

//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::UpdateText(const char* txt)
{
  mText = txt;
}


//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::SetTextColour(const Vector4& colour)
{
  mTextColour = colour;
}


//---------------------------------------------------------------------------------------------------------------------
void ButtonOverlay::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
  if (mText.empty() || !mFont || !mEnabled)
    return;

  IwGxFontSetFont(mFont);

  Colour origColour = IwGxFontGetCol();
  CIwColour colour;
  colour.r = ClampToRange((int) (mTextColour.x * 255), 0, 255);
  colour.g = ClampToRange((int) (mTextColour.y * 255), 0, 255);
  colour.b = ClampToRange((int) (mTextColour.z * 255), 0, 255);
  colour.a = ClampToRange((int) (mTextColour.w * 255), 0, 255);
  IwGxFontSetCol(colour);
  IwGxLightingOn();
  uint16_t fontHeight = mFont->GetHeight();

  float X1 = displayConfig.mLeft + mX1 * displayConfig.mWidth;
  float Y1 = displayConfig.mBottom + mY1 * displayConfig.mHeight;

  int x, y;
  int w = (int) displayConfig.mWidth;
  int h = (int) ((mY1 - mY0) * displayConfig.mHeight);

  if (mTextAnchorH == ANCHOR_H_LEFT)
  {
    IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT);
    x = (int) (displayConfig.mLeft + mX0 * displayConfig.mWidth);
  }
  else if (mTextAnchorH == ANCHOR_H_RIGHT)
  {
    IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_RIGHT);
    x = (int) (displayConfig.mLeft + mX0 * displayConfig.mWidth - w);
  }
  else
  {
    IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE);
    x = (int) (displayConfig.mLeft + mX0 * displayConfig.mWidth - w/2);
  }

  if (mTextAnchorV == ANCHOR_V_TOP)
  {
    IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_BOTTOM);
    y = (int) ((1.0f-mY1) * displayConfig.mHeight) - h;
  }
  else if (mTextAnchorV == ANCHOR_V_BOT)
  {
    IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_TOP);
    y = (int) ((1.0f-mY1) * displayConfig.mHeight) + h;
  }
  else
  {
    IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE);
    y = (int) ((1.0f-mY1) * displayConfig.mHeight) + h/2;
  }

  float s = mSize * Maximum(displayConfig.mWidth, displayConfig.mHeight);
  x += (int) (mTextOffsetX * s);
  y -= (int) (mTextOffsetY * s);

  IwGxFontSetRect(CIwRect(x,y,w,h));
  IwGxFontDrawText(mText.c_str());

  IwGxFontSetCol(origColour);
  IwGxLightingOff();
}

