#ifndef RUNWAY_H
#define RUNWAY_H

#include "Framework.h"
#include "../Platform/S3ECompat.h"
#include <vector>

class Runway : public RenderObject
{
public:
    enum Mode {RUNWAY, CIRCLE};

    // If mode is RUNWAY then length and width are as expected
    // If mode is CIRCLE then length is the radius and width is the width
    Runway(const Transform& tm, float length, float width, const char* textureFile, Mode mode);
    ~Runway();

    void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

    bool ContainsPoint(const Vector3& point) const;

private:
    void MakeRow(
        const Vector3& start, const Transform& tm, const Vector3& offset, 
        float x, float dx, 
        float uv, float dUV, 
        float thisAlpha, float nextAlpha, 
        float frac, float heightOffset);

    void SetUpRunway();
    void SetUpCircle();

    typedef std::vector<Vector3> Points;
    typedef std::vector<Vector2> UVs;
    typedef std::vector<Vector4> Colours;

    Mode mMode;

    Points  mPoints;
    UVs     mUVs;
    Colours mColours;

    Transform   mTM;
    float       mLength;
    float       mWidth;
    Texture     mTexture;
};

#endif

