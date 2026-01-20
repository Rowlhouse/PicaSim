#ifndef WINDSOCKOVERLAY_H
#define WINDSOCKOVERLAY_H

#include "RenderOverlayObject.h"

#include "Graphics.h"

class WindsockOverlay : public RenderOverlayObject
{
public:
    /// Registers with render manager on creation
    WindsockOverlay(const char* imageFile, float size, float x, float y, GLubyte alpha, float degrees);
    /// Deregisters on deletion
    ~WindsockOverlay();

    void RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

    void SetAlpha(GLubyte alpha) {mAlpha = alpha;}

    void SetAngle(float degrees) {mAngle = degrees;}

    void SetSize(float size) {mSize = size;}

    void SetPosition(float x, float y) {mX = x; mY = y;}

private:
    float mAngle;            ///< degrees
    float mImageAspectRatio; ///< Height / width
    Texture mTexture;
    float mSize; // As a fraction of the average width and height
    GLubyte mAlpha;
    float mX, mY;
};


#endif

