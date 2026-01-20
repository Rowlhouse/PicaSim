#ifndef SKYGRID_H
#define SKYGRID_H

#include "RenderObject.h"
#include "RenderManager.h"
#include "Helpers.h"

#include <vector>

class SkyGrid : public RenderObject
{
public:
    void Init();
    void Terminate();

    void SetTransform(const Transform& tm) {mTM = tm;}

    void SetSphere(float radius);
    void SetBox(float distance);
    void Enable(bool enable) {mEnable = enable;}

private:
    typedef std::vector<Vector3> LinePoints;
    typedef std::vector<Vector4> LinePointColours;
    typedef std::vector<Vector4> LineStripColours;
    typedef std::vector<Vector3>   LineStrip;
    typedef std::vector<LineStrip> LineStrips;

    enum Type {SPHERE, BOX};

    void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;
    void AddCircle(const Vector3& centre, const Vector3& axis, float radius, const Vector4& colour, int divisions);

    Transform mTM;
    Type mType;
    float mDistance; // box distance or sphere radius
    bool mEnable;

    LinePoints            mLinePoints;
    LinePointColours      mLinePointColours;

    LineStrips            mLineStrips;
    LineStripColours      mLineStripColours;
};

#endif
