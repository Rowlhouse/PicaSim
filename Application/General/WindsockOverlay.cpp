#include "WindsockOverlay.h"
#include "Entete.h"
#include "Graphics.h"
#include "Shaders.h"
#include "Trace.h"
#include "Viewport.h"
#include "WindsockOverlay.h"
#include "RenderManager.h"
#include "ShaderManager.h"

// #include <s3e.h>

//---------------------------------------------------------------------------------------------------------------------
WindsockOverlay::WindsockOverlay(
  const char* imageFile, float size, 
  float x, float y, 
  GLubyte alpha, float angle)
  : mSize(size), mX(x), mY(y), mAlpha(alpha), mAngle(angle)
{
  LoadTextureFromFile(mTexture, imageFile);
  mTexture.SetMipMapping(true);
  mTexture.SetFiltering(true);
  mTexture.SetClamping(true);
  mTexture.SetFormatHW(CIwImage::RGBA_4444);
  mTexture.Upload();
  TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", imageFile, mTexture.m_HWID);

  // nice mipmapping
  if (mTexture.GetFlags() & Texture::UPLOADED_F)
  {
    glBindTexture(GL_TEXTURE_2D, mTexture.m_HWID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mTexture.Upload();
  }

  mImageAspectRatio = mTexture.GetHeight() / (float) mTexture.GetWidth();
  RenderManager::GetInstance().RegisterRenderOverlayObject(this, 0);
}

//---------------------------------------------------------------------------------------------------------------------
WindsockOverlay::~WindsockOverlay()
{
  RenderManager::GetInstance().UnregisterRenderOverlayObject(this, 0);
}

static GLfloat uvs[] = {
  1, 0,
  1, 1,
  0, 1,
  0, 0,
};

//---------------------------------------------------------------------------------------------------------------------
void WindsockOverlay::RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig)
{
  if (mAlpha == 0)
    return;

  if (!(mTexture.GetFlags() & Texture::UPLOADED_F))
    return;

  // s is the full size in pixels
  float s = mSize * Maximum(displayConfig.mWidth, displayConfig.mHeight);
  float s2 = s * 0.5f; // half width

  float x0, x1, y0, y1;

  // midpoint of the button
  float x = displayConfig.mLeft + mX * displayConfig.mWidth;
  float y = displayConfig.mBottom + mY * displayConfig.mHeight;
  x0 = x - s2 * mImageAspectRatio;
  x1 = x0 + s * mImageAspectRatio;
  y0 = y - s2;
  y1 = y0 + s;

  DisableDepthMask disableDepthMask;
  DisableDepthTest disableDepthTest;
  EnableBlend enableBlend;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLfloat pts[] = {
    x0, y0, 0,
    x1, y0, 0,
    x1, y1, 0,
    x0, y1, 0,
  };

  const OverlayShader* overlayShader = (OverlayShader*) ShaderManager::GetInstance().GetShader(SHADER_OVERLAY);

  if (gGLVersion == 1)
  {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, uvs);
    glVertexPointer(3, GL_FLOAT, 0, pts);

    glColor4ub(255,255,255,mAlpha);
  }
  else
  {
    overlayShader->Use();

    glUniform1i(overlayShader->u_texture, 0);

    glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glEnableVertexAttribArray(overlayShader->a_position);

    glVertexAttribPointer(overlayShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    glEnableVertexAttribArray(overlayShader->a_texCoord);

    glUniform4f(overlayShader->u_colour, 1.0f, 1.0f, 1.0f, mAlpha/255.0f);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTexture.m_HWID);

  esPushMatrix();
  esTranslatef(x, y, 0.0f);

  GLfloat sh = -0.9f;
  GLfloat shear[] = { 
    1, 0, 0, 0, 
    0, .3f, 0, 0.005f,
    0, 0, 1, 0,
    0, 0, 0, 1 
  };
  esMultMatrixf(shear);
  esRotatef(mAngle, 0, 0, 1.0f);
  esTranslatef(-x, -y, 0.0f);

  esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  esPopMatrix();

  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}
