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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Local constants matching Skybox::Side enum
namespace {
    enum SkyboxSide { SKY_UP = 0, SKY_FRONT, SKY_LEFT, SKY_BACK, SKY_RIGHT, SKY_DOWN };
}

//======================================================================================================================
// Calculate parallax direction in UV space for a given skybox face
// side: skybox face (0=UP, 1=FRONT, 2=LEFT, 3=BACK, 4=RIGHT, 5=DOWN)
// eyeRightDir: eye separation direction in world space (points from left eye to right eye)
// Returns: direction in UV space for parallax offset
//======================================================================================================================
static glm::vec2 CalculateFaceParallaxDir(int side, const glm::vec3& eyeRightDir)
{
    // UV axes in world space for each face (PicaSim coords: +X forward, +Y left, +Z up)
    // The base face (FRONT) has vertices at x=scale, spanning y and z
    // UV.x increases in -Y direction, UV.y increases in +Z direction
    glm::vec3 uAxis, vAxis;

    switch (side)
    {
    // UV axes derived from FRONT face vertex data, then transformed by each face's rotation
    // FRONT base: U along -Y, V along -Z (V=0 at top, V=1 at bottom)
    case SKY_FRONT: uAxis = glm::vec3(0, -1, 0); vAxis = glm::vec3(0, 0, -1); break;  // no rotation
    case SKY_RIGHT: uAxis = glm::vec3(-1, 0, 0); vAxis = glm::vec3(0, 0, -1); break;  // ROTATE_270_Z (90° CW)
    case SKY_BACK:  uAxis = glm::vec3(0, 1, 0);  vAxis = glm::vec3(0, 0, -1); break;  // ROTATE_180_Z
    case SKY_LEFT:  uAxis = glm::vec3(1, 0, 0);  vAxis = glm::vec3(0, 0, -1); break;  // ROTATE_90_Z (90° CCW)
    // Up/Down faces use rotations around Y axis
    case SKY_UP:    uAxis = glm::vec3(0, -1, 0); vAxis = glm::vec3(-1, 0, 0); break;  // ROTATE_270_Y
    case SKY_DOWN:  uAxis = glm::vec3(0, -1, 0); vAxis = glm::vec3(1, 0, 0);  break;  // ROTATE_90_Y
    default:        uAxis = glm::vec3(0, -1, 0); vAxis = glm::vec3(0, 0, -1); break;
    }

    // Project eyeRightDir onto face plane by taking dot products with UV axes
    float u = glm::dot(eyeRightDir, uAxis);
    float v = glm::dot(eyeRightDir, vAxis);

    glm::vec2 dir(u, v);
    float len = glm::length(dir);
    if (len > 0.001f)
        dir /= len;  // normalize

    return dir;
}

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
                               const Vector3& eyeRightDir,
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

    // Convert eye right direction to glm for calculations
    glm::vec3 eyeRight(eyeRightDir.x, eyeRightDir.y, eyeRightDir.z);

    // The skybox is rotated by -mOffset around Z axis (see esRotatef call above).
    // To align the eye direction with the un-rotated UV axes, we need to rotate
    // it by +mOffset (the inverse rotation).
    float offsetRadians = glm::radians(mOffset);
    float cosOffset = cos(offsetRadians);
    float sinOffset = sin(offsetRadians);
    glm::vec3 rotatedEyeRight(
        eyeRight.x * cosOffset - eyeRight.y * sinOffset,
        eyeRight.x * sinOffset + eyeRight.y * cosOffset,
        eyeRight.z
    );
    eyeRight = rotatedEyeRight;

    // Render all sides with calculated parallax directions based on eye separation
    glm::vec2 parallaxDir;

    // FRONT - looks at +X
    parallaxDir = CalculateFaceParallaxDir(SKY_FRONT, eyeRight);
    {
        // TRACE("Front: %.3f %.3f", parallaxDir.x, parallaxDir.y);
        esPushMatrix();
        DrawSideVRParallax(FRONT, vrShader, parallaxDir.x, parallaxDir.y, 0);
        esPopMatrix();
    }

    // RIGHT - looks at -Y
    parallaxDir = CalculateFaceParallaxDir(SKY_RIGHT, eyeRight);
    {
        // TRACE("Right: %.3f %.3f", parallaxDir.x, parallaxDir.y);
        esPushMatrix();
        ROTATE_270_Z;
        DrawSideVRParallax(RIGHT, vrShader, parallaxDir.x, parallaxDir.y, 0);
        esPopMatrix();
    }

    // BACK - looks at -X
    parallaxDir = CalculateFaceParallaxDir(SKY_BACK, eyeRight);
    {
        // TRACE("Back: %.3f %.3f", parallaxDir.x, parallaxDir.y);
        esPushMatrix();
        ROTATE_180_Z;
        DrawSideVRParallax(BACK, vrShader, parallaxDir.x, parallaxDir.y, 0);
        esPopMatrix();
    }

    // LEFT - looks at +Y
    parallaxDir = CalculateFaceParallaxDir(SKY_LEFT, eyeRight);
    {
        // TRACE("Left: %.3f %.3f", parallaxDir.x, parallaxDir.y);
        esPushMatrix();
        ROTATE_90_Z;
        DrawSideVRParallax(LEFT, vrShader, parallaxDir.x, parallaxDir.y, 0);
        esPopMatrix();
    }

    // UP - looks at +Z
    parallaxDir = CalculateFaceParallaxDir(SKY_UP, eyeRight);
    {
        esPushMatrix();
        ROTATE_270_Y;
        DrawSideVRParallax(UP, vrShader, parallaxDir.x, parallaxDir.y, 0);
        esPopMatrix();
    }

    // DOWN - looks at -Z
    parallaxDir = CalculateFaceParallaxDir(SKY_DOWN, eyeRight);
    {
        esPushMatrix();
        ROTATE_90_Y;
        DrawSideVRParallax(DOWN, vrShader, parallaxDir.x, parallaxDir.y, 0);
        esPopMatrix();
    }

    glDisableVertexAttribArray(vrShader->a_position);
    glDisableVertexAttribArray(vrShader->a_texCoord);

    // Reset to texture unit 0
    glActiveTexture(GL_TEXTURE0);

    glFrontFace(GL_CCW);

    esPopMatrix();
}

