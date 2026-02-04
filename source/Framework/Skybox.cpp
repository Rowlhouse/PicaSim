#include "Skybox.h"
#include "TileExpansion.h"

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

#define DEBUG_SAVE_EXPANDED_TEXTURES 0

//======================================================================================================================
// Transform eye separation direction from world space to face-local coordinates
// side: skybox face (0=UP, 1=FRONT, 2=LEFT, 3=BACK, 4=RIGHT, 5=DOWN)
// eyeRightDir: eye separation direction in world space (points from left eye to right eye)
// Returns: eye direction in face-local coords (x=forward into face, y/z match UV axes)
//======================================================================================================================
static glm::vec3 CalculateFaceEyeRightLocal(int side, const glm::vec3& eyeRightDir)
{
    // Face coordinate system: x = forward (into face), y/z = tangent directions
    // These must match how the shader interprets skyboxPos (x=forward, y/z for UV calculation)
    glm::vec3 faceForward, faceY, faceZ;

    switch (side)
    {
    // For each face, define the world-space directions that map to face-local x, y, z
    // Face-local y should align with the direction that increases U
    // Face-local z should align with the direction that increases V
    case FACE_FRONT: faceForward = glm::vec3(1,0,0);  faceY = glm::vec3(0,1,0);  faceZ = glm::vec3(0,0,1);  break;
    case FACE_RIGHT: faceForward = glm::vec3(0,-1,0); faceY = glm::vec3(1,0,0);  faceZ = glm::vec3(0,0,1);  break;
    case FACE_BACK:  faceForward = glm::vec3(-1,0,0); faceY = glm::vec3(0,-1,0); faceZ = glm::vec3(0,0,1);  break;
    case FACE_LEFT:  faceForward = glm::vec3(0,1,0);  faceY = glm::vec3(-1,0,0); faceZ = glm::vec3(0,0,1);  break;
    case FACE_UP:    faceForward = glm::vec3(0,0,1);  faceY = glm::vec3(0,1,0);  faceZ = glm::vec3(-1,0,0); break;
    case FACE_DOWN:  faceForward = glm::vec3(0,0,-1); faceY = glm::vec3(0,1,0);  faceZ = glm::vec3(1,0,0);  break;
    default:         faceForward = glm::vec3(1,0,0);  faceY = glm::vec3(0,1,0);  faceZ = glm::vec3(0,0,1);  break;
    }

    // Project eyeRightDir onto face coordinate system
    return glm::vec3(
        glm::dot(eyeRightDir, faceForward),
        glm::dot(eyeRightDir, faceY),
        glm::dot(eyeRightDir, faceZ)
    );
}

//======================================================================================================================
Skybox::Skybox() :
    mOffset(0.0f),
    mExtendBelowHorizon(1.0f),
    mPanoramaExtension(0.0f),
    mInitialised(false),
    mVRParallaxEnabled(false)
{}



//======================================================================================================================
Skybox::~Skybox()
{
}

//======================================================================================================================
bool Skybox::Init(const char* skyboxPath, bool use16BitTextures, int maxDetail, LoadingScreenHelper* loadingScreen, float panoramaExtension)
{
    TRACE_METHOD_ONLY(1);
    if (loadingScreen) loadingScreen->Update("Skybox");

    if (mInitialised)
        Terminate();

    mPanoramaExtension = panoramaExtension;

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

    // If panoramaExtension > 0, we need to load all images first before expanding
    if (panoramaExtension > 0.0f)
    {
        // First pass: Load all images into memory
        std::vector<Image*> allFaceImages[NUM_SIDES];

        for (int iSide = 0; iSide != NUM_SIDES; ++iSide)
        {
            for (int iImage = 1; ; ++iImage)
            {
                sprintf(filename, "%s/%d/%s%d.jpg", skyboxPath, detail, sideNames[iSide], iImage);
                if (!std::filesystem::exists(filename))
                    break;

                sprintf(loadingTxt, "Loading %s%d", sideNames[iSide], iImage);
                if (loadingScreen) loadingScreen->Update(loadingTxt);

                Image* image = new Image();
                if (image->LoadFromFile(filename))
                {
                    allFaceImages[iSide].push_back(image);
                    TRACE_FILE_IF(1) TRACE("Loaded image %s", filename);
                }
                else
                {
                    delete image;
                    TRACE_FILE_IF(1) TRACE("Failed to load image %s", filename);
                }
            }
        }

        // Second pass: Create expanded textures with borders
        for (int iSide = 0; iSide != NUM_SIDES; ++iSide)
        {
            int numTiles = (int)allFaceImages[iSide].size();
            if (numTiles == 0) continue;

            int numPerSide = std::lround(std::sqrt(numTiles));
            int tileWidth = allFaceImages[iSide][0]->GetWidth();
            int borderPixels = (int)(tileWidth * panoramaExtension);

            for (int iTile = 0; iTile < numTiles; ++iTile)
            {
                sprintf(loadingTxt, "Expanding %s%d", sideNames[iSide], iTile + 1);
                if (loadingScreen) loadingScreen->Update(loadingTxt);

                Image* expandedImage = CreateExpandedTileImage(
                    allFaceImages[iSide], allFaceImages, iSide, iTile, numPerSide, borderPixels);

                if (expandedImage)
                {
#if DEBUG_SAVE_EXPANDED_TEXTURES
                    {
                        char debugFilename[256];
                        sprintf(debugFilename, "expanded_%s%d.png", sideNames[iSide], iTile + 1);
                        expandedImage->SavePng(debugFilename);
                        TRACE_FILE_IF(1) TRACE("Saved debug texture: %s", debugFilename);
                    }
#endif
                    Texture* texture = new Texture;
                    mTextures[iSide].push_back(texture);

                    texture->CopyFromImage(expandedImage);
                    texture->SetMipMapping(false);
                    texture->SetModifiable(false);
                    if (use16BitTextures && texture->GetWidth() > 512)
                        texture->SetFormatHW(CIwImage::RGB_565);
                    texture->Upload();
                    TRACE_FILE_IF(1) TRACE("Uploaded expanded texture %s%d id %d (size %dx%d)",
                        sideNames[iSide], iTile + 1, texture->mHWID,
                        expandedImage->GetWidth(), expandedImage->GetHeight());

                    if (texture->GetFlags() & Texture::UPLOADED_F)
                    {
                        glBindTexture(GL_TEXTURE_2D, texture->mHWID);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                        if (loadingScreen) loadingScreen->Update();
                        texture->Upload();
                    }

                    delete expandedImage;
                }
            }
        }

        // Clean up ALL original images after ALL faces have been expanded
        // (Must be done after the expansion loop since cross-face borders need access to other faces)
        for (int iSide = 0; iSide != NUM_SIDES; ++iSide)
        {
            for (Image* img : allFaceImages[iSide])
                delete img;
            allFaceImages[iSide].clear();
        }
    }
    else
    {
        // Original path: load and upload directly
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
                    glBindTexture(GL_TEXTURE_2D, texture->mHWID);
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
// These are the skybox (or tile) vertex positions. When we draw, x is forward. 
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
        glUniform1f(skyboxShader->u_panoramaExtension, mPanoramaExtension);
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
                                const glm::vec3& eyeRightLocal) const
{
    int numPerSide = std::lround(std::sqrt(mTextures[side].size()));

    // Set per-face uniforms
    glUniform3f(shader->u_eyeRightLocal, eyeRightLocal.x, eyeRightLocal.y, eyeRightLocal.z);
    glUniform1f(shader->u_tilesPerSide, (float)numPerSide);
    glUniform1f(shader->u_panoramaExtension, mPanoramaExtension);

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
                    glUniform2f(shader->u_tileOffset, y, z);

                    // Set which edges are at face boundaries (need projective warp)
                    float isLeftEdge = (i == 0) ? 1.0f : 0.0f;
                    float isRightEdge = (i == numPerSide - 1) ? 1.0f : 0.0f;
                    float isTopEdge = (j == 0) ? 1.0f : 0.0f;
                    float isBottomEdge = (j == numPerSide - 1) ? 1.0f : 0.0f;
                    glUniform4f(shader->u_tileEdgeFlags, isLeftEdge, isRightEdge, isTopEdge, isBottomEdge);

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
                               float tanFovLeft, float tanFovRight,
                               float tanFovUp, float tanFovDown)
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
    glUniform2f(vrShader->u_screenSize, (float)screenWidth, (float)screenHeight);
    glUniform2f(vrShader->u_tanFovMin, tanFovLeft, tanFovDown);
    glUniform2f(vrShader->u_tanFovMax, tanFovRight, tanFovUp);

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

    // Render all sides with eye-right direction transformed to face-local coordinates
    glm::vec3 eyeRightLocal;

    // FRONT - looks at +X
    eyeRightLocal = CalculateFaceEyeRightLocal(FACE_FRONT, eyeRight);
    {
        esPushMatrix();
        DrawSideVRParallax(FRONT, vrShader, eyeRightLocal);
        esPopMatrix();
    }

    // RIGHT - looks at -Y
    eyeRightLocal = CalculateFaceEyeRightLocal(FACE_RIGHT, eyeRight);
    {
        esPushMatrix();
        ROTATE_270_Z;
        DrawSideVRParallax(RIGHT, vrShader, eyeRightLocal);
        esPopMatrix();
    }

    // BACK - looks at -X
    eyeRightLocal = CalculateFaceEyeRightLocal(FACE_BACK, eyeRight);
    {
        esPushMatrix();
        ROTATE_180_Z;
        DrawSideVRParallax(BACK, vrShader, eyeRightLocal);
        esPopMatrix();
    }

    // LEFT - looks at +Y
    eyeRightLocal = CalculateFaceEyeRightLocal(FACE_LEFT, eyeRight);
    {
        esPushMatrix();
        ROTATE_90_Z;
        DrawSideVRParallax(LEFT, vrShader, eyeRightLocal);
        esPopMatrix();
    }

    // UP - looks at +Z
    eyeRightLocal = CalculateFaceEyeRightLocal(FACE_UP, eyeRight);
    {
        esPushMatrix();
        ROTATE_270_Y;
        DrawSideVRParallax(UP, vrShader, eyeRightLocal);
        esPopMatrix();
    }

    // DOWN - looks at -Z
    eyeRightLocal = CalculateFaceEyeRightLocal(FACE_DOWN, eyeRight);
    {
        esPushMatrix();
        ROTATE_90_Y;
        DrawSideVRParallax(DOWN, vrShader, eyeRightLocal);
        esPopMatrix();
    }

    glDisableVertexAttribArray(vrShader->a_position);
    glDisableVertexAttribArray(vrShader->a_texCoord);

    // Reset to texture unit 0
    glActiveTexture(GL_TEXTURE0);

    glFrontFace(GL_CCW);

    esPopMatrix();
}

