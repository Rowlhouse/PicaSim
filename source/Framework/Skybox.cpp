#include "Skybox.h"

#include <filesystem>

#include "RenderManager.h"
#include "LoadingScreenHelper.h"
#include "Graphics.h"
#include "Helpers.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "Viewport.h"
#include "Trace.h"

//======================================================================================================================
Skybox::Skybox() :
    mOffset(0.0f),
    mExtendBelowHorizon(1.0f),
    mInitialised(false),
    mVRParallaxEnabled(false)
{}

//======================================================================================================================
Skybox::~Skybox()
{
}

//======================================================================================================================
bool Skybox::Init(const char* skyboxPath, bool use16BitTextures, int maxDetail, LoadingScreenHelper* loadingScreen)
{
    TRACE_METHOD_ONLY(1);
    if (loadingScreen) loadingScreen->Update("Skybox");

    if (mInitialised)
        Terminate();

    char filename[256];
    char loadingTxt[256];

    const char* sideNames[] = {"up", "front", "left", "back", "right", "down"};

    int detail = maxDetail;
    while (detail >= 1)
    {
        sprintf(filename, "%s/%d/front1.jpg", skyboxPath, detail);
        if (std::filesystem::exists(filename))
        {
            break;
        }
        --detail;
    }

    if (detail < 1)
    {
        TRACE_FILE_IF(1) TRACE("Failed to find Skybox files for %s", skyboxPath);
        return false;
    }

    for (int iSide = 0 ; iSide != NUM_SIDES ; ++iSide)
    {
        for (int iImage = 1 ; ; ++iImage)
        {
            sprintf(filename, "%s/%d/%s%d.jpg", skyboxPath, detail, sideNames[iSide], iImage);
            if (!std::filesystem::exists(filename))
            {
                break;
            }

            sprintf(loadingTxt, "Image %s%d", sideNames[iSide], iImage);
            if (loadingScreen) loadingScreen->Update(loadingTxt);

            Texture* texture = new Texture;
            mTextures[iSide].push_back(texture);

            LoadTextureFromFile(*texture, filename);
            if (loadingScreen) loadingScreen->Update();
            texture->SetMipMapping(false);
            texture->SetModifiable(false);
            if (use16BitTextures && texture->GetWidth() > 512)
                texture->SetFormatHW(CIwImage::RGB_565);
            texture->Upload();
            TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", filename, texture->mHWID);

            if (texture->GetFlags() & Texture::UPLOADED_F)
            {
                // Letting Marmalade handle the filtering/clamping doesn't seem to work... in particular clamping doesn't, so 
                // force it manually and re-upload
                glBindTexture(GL_TEXTURE_2D, texture->mHWID);

                // filtering
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if (loadingScreen) loadingScreen->Update();
                texture->Upload();
                TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", filename, texture->mHWID);
            }

        }

    }
    if (loadingScreen) loadingScreen->Update();

    RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_SKYBOX);

    mInitialised = true;
    return true;
}

//======================================================================================================================
void Skybox::Terminate()
{
    TRACE_METHOD_ONLY(1);
    if (mInitialised)
        RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_SKYBOX);

    for (int iSide = 0 ; iSide != NUM_SIDES ; ++iSide)
    {
        while (!mTextures[iSide].empty())
        {
            delete mTextures[iSide].back();
            mTextures[iSide].pop_back();
        }
    }
    mInitialised = false;
}

//======================================================================================================================
// Avoids clipping with the near plane
float scale = 100.0f;
static GLfloat pts[] = {
    scale,  scale,  scale,
    scale, -scale,  scale, 
    scale, -scale, -scale, 
    scale,  scale, -scale, 
};

static GLfloat uvs[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};

//======================================================================================================================
void Skybox::DrawSide(Side side, int mvpLoc) const
{
    int numPerSide = std::lround(std::sqrt(mTextures[side].size()));

    float imageScale = 1.0f / numPerSide;
    esScalef(1.0f, imageScale, imageScale);

    size_t index = 0;
    for (int j = 0 ; j != numPerSide ; ++j)
    {
        for (int i = 0 ; i != numPerSide ; ++i)
        {
            if (index < mTextures[side].size())
            {
                Texture* texture = mTextures[side][index];
                if (texture && texture->GetFlags() & Texture::UPLOADED_F)
                {
                    glBindTexture(GL_TEXTURE_2D, texture->mHWID);
                    float y = scale * (numPerSide - (i * 2 + 1.0f));
                    float z = scale * (numPerSide - (j * 2 + 1.0f));
                    esPushMatrix();
                    esTranslatef(0.0f, y, z);
                    esSetModelViewProjectionMatrix(mvpLoc);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    esPopMatrix();
                }
            }
            ++index;
        }
    }

}

//======================================================================================================================
void Skybox::RenderUpdate(class Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(2);
    esPushMatrix();

    glFrontFace(GL_CW);

    DisableDepthMask disableDepthMask;
    DisableDepthTest disableDepthTest;

    DisableFog disableFog;

    const SkyboxShader* skyboxShader = (SkyboxShader*) ShaderManager::GetInstance().GetShader(SHADER_SKYBOX);
    if (gGLVersion == 1)
    {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        skyboxShader->Use();
        glUniform1i(skyboxShader->u_texture, 0);
    }

    Vector3 pos = viewport->GetCamera()->GetPosition();
    esTranslatef(pos.x, pos.y, pos.z);

    esRotatef(-mOffset, 0, 0, 1);
      
    glActiveTexture(GL_TEXTURE0);

    if (gGLVersion == 1)
    {
        glVertexPointer(3, GL_FLOAT, 0, pts);
        glTexCoordPointer(2, GL_FLOAT, 0, uvs);
    }
    else
    {
        // Load the vertex position
        glVertexAttribPointer(skyboxShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glEnableVertexAttribArray(skyboxShader->a_position);

        glVertexAttribPointer(skyboxShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
        glEnableVertexAttribArray(skyboxShader->a_texCoord);
    }

#if 1
    // front
    {
        esPushMatrix();
        DrawSide(FRONT, skyboxShader->u_mvpMatrix);
        esPopMatrix();
    }
#endif

#if 1
    // right
    {
        esPushMatrix();
        ROTATE_270_Z;
        DrawSide(RIGHT, skyboxShader->u_mvpMatrix);
        esPopMatrix();
    }
#endif

#if 1
    // back
    {
        esPushMatrix();
        ROTATE_180_Z;
        DrawSide(BACK, skyboxShader->u_mvpMatrix);
        esPopMatrix();
    }
#endif

#if 1
    // left
    {
        esPushMatrix();
        ROTATE_90_Z;
        DrawSide(LEFT, skyboxShader->u_mvpMatrix);
        esPopMatrix();
    }
#endif

#if 1
    // up
    {
        esPushMatrix();
        ROTATE_270_Y;
        DrawSide(UP, skyboxShader->u_mvpMatrix);
        esPopMatrix();
    }
#endif

#if 1
    // down
    {
        esPushMatrix();
        ROTATE_90_Y;
        DrawSide(DOWN, skyboxShader->u_mvpMatrix);
        esPopMatrix();
    }
#endif

    if (gGLVersion == 1)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        glDisableVertexAttribArray(skyboxShader->a_position);
        glDisableVertexAttribArray(skyboxShader->a_texCoord);
    }

    glFrontFace(GL_CCW);

    esPopMatrix();
}

//======================================================================================================================
void Skybox::DrawSideVRParallax(Side side, const SkyboxVRParallaxShader* shader,
                                float parallaxDirX, float parallaxDirY, int faceType) const
{
    int numPerSide = std::lround(std::sqrt(mTextures[side].size()));

    // Set per-face uniforms
    glUniform2f(shader->u_parallaxDir, parallaxDirX, parallaxDirY);
    glUniform1f(shader->u_tileScale, (float)numPerSide);
    glUniform1i(shader->u_faceType, faceType);

    float imageScale = 1.0f / numPerSide;
    esScalef(1.0f, imageScale, imageScale);

    size_t index = 0;
    for (int j = 0 ; j != numPerSide ; ++j)
    {
        for (int i = 0 ; i != numPerSide ; ++i)
        {
            if (index < mTextures[side].size())
            {
                Texture* texture = mTextures[side][index];
                if (texture && texture->GetFlags() & Texture::UPLOADED_F)
                {
                    // Bind skybox texture to unit 0
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture->mHWID);

                    float y = scale * (numPerSide - (i * 2 + 1.0f));
                    float z = scale * (numPerSide - (j * 2 + 1.0f));
                    esPushMatrix();
                    esTranslatef(0.0f, y, z);
                    esSetModelViewProjectionMatrix(shader->u_mvpMatrix);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    esPopMatrix();
                }
            }
            ++index;
        }
    }
}

//======================================================================================================================
void Skybox::RenderVRParallax(Viewport* viewport,
                               const Vector3& skyboxCenter,
                               float eyeOffset, float ipd,
                               GLuint depthTexture,
                               int screenWidth, int screenHeight,
                               float nearPlane, float farPlane,
                               float skyDistance, float parallaxScale)
{
    TRACE_METHOD_ONLY(2);

    if (!mInitialised)
        return;

    esPushMatrix();

    glFrontFace(GL_CW);

    DisableDepthMask disableDepthMask;
    DisableDepthTest disableDepthTest;
    DisableFog disableFog;

    const SkyboxVRParallaxShader* vrShader = (SkyboxVRParallaxShader*) ShaderManager::GetInstance().GetShader(SHADER_SKYBOX_VR_PARALLAX);
    vrShader->Use();

    // Set up uniforms
    glUniform1i(vrShader->u_skyboxTexture, 0);  // Texture unit 0
    glUniform1i(vrShader->u_depthTexture, 1);   // Texture unit 1
    glUniform1f(vrShader->u_eyeOffset, eyeOffset);
    glUniform1f(vrShader->u_ipd, ipd);
    glUniform1f(vrShader->u_nearPlane, nearPlane);
    glUniform1f(vrShader->u_farPlane, farPlane);
    glUniform1f(vrShader->u_skyDistance, skyDistance);
    glUniform2f(vrShader->u_screenSize, (float)screenWidth, (float)screenHeight);
    glUniform1f(vrShader->u_parallaxScale, parallaxScale);

    // Bind depth texture to unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    // Use provided skybox center (base camera position, not VR head position)
    // This prevents the skybox from moving when the head position changes
    esTranslatef(skyboxCenter.x, skyboxCenter.y, skyboxCenter.z);
    esRotatef(-mOffset, 0, 0, 1);

    // Set up vertex attributes
    glVertexAttribPointer(vrShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glEnableVertexAttribArray(vrShader->a_position);

    glVertexAttribPointer(vrShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    glEnableVertexAttribArray(vrShader->a_texCoord);

    // Render all sides with face-specific parallax directions
    // Side faces: parallax is horizontal (in UV.x direction)
    // Up/Down faces: parallax is complex (disabled, faceType=1)
    {
        esPushMatrix();
        DrawSideVRParallax(FRONT, vrShader, 1.0f, 0.0f, 0);
        esPopMatrix();
    }
    {
        esPushMatrix();
        ROTATE_270_Z;
        DrawSideVRParallax(RIGHT, vrShader, 1.0f, 0.0f, 0);
        esPopMatrix();
    }
    {
        esPushMatrix();
        ROTATE_180_Z;
        DrawSideVRParallax(BACK, vrShader, 1.0f, 0.0f, 0);
        esPopMatrix();
    }
    {
        esPushMatrix();
        ROTATE_90_Z;
        DrawSideVRParallax(LEFT, vrShader, 1.0f, 0.0f, 0);
        esPopMatrix();
    }
    {
        esPushMatrix();
        ROTATE_270_Y;
        DrawSideVRParallax(UP, vrShader, 0.0f, 0.0f, 1);     // disabled
        esPopMatrix();
    }
    {
        esPushMatrix();
        ROTATE_90_Y;
        DrawSideVRParallax(DOWN, vrShader, 0.0f, 0.0f, 1);   // disabled
        esPopMatrix();
    }

    glDisableVertexAttribArray(vrShader->a_position);
    glDisableVertexAttribArray(vrShader->a_texCoord);

    // Reset to texture unit 0
    glActiveTexture(GL_TEXTURE0);

    glFrontFace(GL_CCW);

    esPopMatrix();
}

