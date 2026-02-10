#include "WindsockOverlay.h"
#include "RenderManager.h"
#include "Framework.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "../Platform/S3ECompat.h"

//======================================================================================================================
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
    TRACE_FILE_IF(ONCE_2) TRACE("Uploaded texture %s id %d", imageFile, mTexture.mHWID);

    // nice mipmapping
    if (mTexture.GetFlags() & Texture::UPLOADED_F)
    {
        glBindTexture(GL_TEXTURE_2D, mTexture.mHWID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        mTexture.Upload();
    }

    mImageAspectRatio = mTexture.GetHeight() / (float) mTexture.GetWidth();
    RenderManager::GetInstance().RegisterRenderOverlayObject(this, 0);
}

//======================================================================================================================
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

//======================================================================================================================
void WindsockOverlay::RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig)
{
    if (mAlpha == 0)
        return;

    if (!(mTexture.GetFlags() & Texture::UPLOADED_F))
        return;

    // size is the full size in pixels
    float size = mSize * Minimum(displayConfig.mWidth, displayConfig.mHeight) * 2.0f;
    float s2 = size * 0.5f; // half width

    float x0, x1, y0, y1;

    // midpoint of the button
    float x = displayConfig.mLeft + mX * displayConfig.mWidth;
    float y = displayConfig.mBottom + mY * displayConfig.mHeight;
    x0 = x - s2 * mImageAspectRatio;
    x1 = x0 + size * mImageAspectRatio;
    y0 = y - s2;
    y1 = y0 + size;

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

    overlayShader->Use();

    glUniform1i(overlayShader->u_texture, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Ensure no VBO bound before client-side arrays

    glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glEnableVertexAttribArray(overlayShader->a_position);

    glVertexAttribPointer(overlayShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    glEnableVertexAttribArray(overlayShader->a_texCoord);

    glUniform4f(overlayShader->u_colour, 1.0f, 1.0f, 1.0f, mAlpha/255.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture.mHWID);

    esPushMatrix();
    esTranslatef(x, y, 0.0f);

    GLfloat shear[] = { 
        1, 0, 0, 0, 
        0, .3f, 0, 1.0f/size,
        0, 0, 1, 0,
        0, 0, 0, 1 
    };
    esMultMatrixf(shear);
    esRotatef(mAngle, 0, 0, 1.0f);
    esTranslatef(-x, -y, 0.0f);

    esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    esPopMatrix();

    glDisableVertexAttribArray(overlayShader->a_position);
    glDisableVertexAttribArray(overlayShader->a_texCoord);
}
