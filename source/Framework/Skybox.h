#ifndef SKYBOX_H
#define SKYBOX_H

#include "Helpers.h"
#include "Graphics.h"
#include "RenderObject.h"

#include <vector>

class Skybox : public RenderObject
{
public:
    Skybox();
    ~Skybox();

    bool Init(const char* skyboxPath, bool use16BitTextures, int maxDetail, class LoadingScreenHelper* loadingScreen);
    void Terminate();

    void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

    void SetOffset(float degrees) {mOffset = degrees;}
    void SetExtendBelowHorizon(float amount) {mExtendBelowHorizon = amount;}

    // VR depth-based parallax support
    void SetVRParallaxEnabled(bool enabled) { mVRParallaxEnabled = enabled; }
    bool IsVRParallaxEnabled() const { return mVRParallaxEnabled; }

    // Render skybox with depth-based parallax for VR
    // skyboxCenter: world position to center the skybox (use base camera position, not VR head position)
    // eyeRightDir: eye separation direction in world space (from left eye to right eye)
    // eyeOffset: -1.0 for left eye, +1.0 for right eye
    // ipd: interpupillary distance in world units
    // depthTexture: texture containing the depth buffer
    // nearPlane, farPlane: clipping plane distances
    void RenderVRParallax(class Viewport* viewport,
                          const Vector3& skyboxCenter,
                          const Vector3& eyeRightDir,
                          float eyeOffset, float ipd,
                          GLuint depthTexture,
                          int screenWidth, int screenHeight,
                          float nearPlane, float farPlane);

private:
    enum Side {UP, FRONT, LEFT, BACK, RIGHT, DOWN, NUM_SIDES};

    void DrawSide(Side side, int mvpLoc) const;
    void DrawSideVRParallax(Side side, const class SkyboxVRParallaxShader* shader,
                            float parallaxDirX, float parallaxDirY) const;

    typedef std::vector<Texture*> Textures;
    Textures mTextures[NUM_SIDES];
    float mOffset;
    float mExtendBelowHorizon;
    bool mInitialised;
    bool mVRParallaxEnabled;
};

#endif
