#ifndef SIMPLEOBJECT_H
#define SIMPLEOBJECT_H

#include "Framework.h"
#include "../Platform/S3ECompat.h"

class BoxObject : public Entity, public RenderObject
{
public:
    /// Pass in the full extents
    /// If mass is zero, then the shape will be static
    BoxObject(
        const Vector3& extents, const Transform& tm, const Vector3& colour, const char* textureFile, 
        float mass, bool enableRender, bool enableShadows, bool visible, bool shadowVisible);
    ~BoxObject();

    void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

    void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

    float GetRenderBoundingRadius() const OVERRIDE {return mRenderBoundingRadius;}

    bool GetShadowVisible() const OVERRIDE {return mShadowVisible;}

    const Transform& GetTM() const OVERRIDE {return mTM;}
    void SetTM(const Transform& tm);

    const Transform& GetInitialTM() const {return mInitialTM;}
    void SetInitialTM(const Transform& tm) {mInitialTM = tm;}

    const Vector3& GetExtents() const {return mExtents;}
    void SetExtents(const Vector3& extents);

    const Vector3& GetColour() const {return mColour;}
    void SetColour(const Vector3& colour) {mColour = colour;}

    void SetWireframe(bool enable) {mWireframe = enable;}
    bool GetWireframe() const {return mWireframe;}

    void SetVisible(bool visible) {mVisible = visible;}
    void SetShadowVisible(bool shadowVisible) {mShadowVisible = shadowVisible;}

    const std::string& GetTextureFile() const {return mTextureFile;}
    float GetMass() const {return mMass;}

    void forceAwake() {mRigidBody->setDeactivationTime(0.0f);}

private:
    // Initialisation data and constants
    Transform      mInitialTM;
    Vector3        mExtents;
    std::string    mTextureFile;
    float          mMass;
    bool           mRenderEnabled;
    bool           mShadowsEnabled;
    bool           mVisible;
    bool           mShadowVisible;

    // Things that do not update and are not initialisation
    Texture     mTexture;
    Vector3     mColour;
    float       mRenderBoundingRadius;
    bool        mWireframe;

    // Updated each frame
    Transform      mTM;
    btBoxShape*    mCollisionShape;
    btRigidBody*   mRigidBody;
};

#endif

