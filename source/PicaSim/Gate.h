#ifndef GATE_H
#define GATE_H

#include "RenderModel.h"

//======================================================================================================================
class GatePost : public RenderObject
{
public:
    void Init(const Vector3& pos, bool draw, const Vector4& colour, float height, const Vector4& targetColour);
    void Terminate();

    void RenderUpdate(Viewport* viewport, int renderLevel) OVERRIDE;
    const Transform& GetTM() const OVERRIDE {return mTM;}
    float GetRenderBoundingRadius() const OVERRIDE {return mRenderModel->GetBoundingRadius();}

    void SetIsTarget(bool isTarget) {mIsTarget = isTarget;}

private:
    static RenderModel* mRenderModel;
    static int mRenderModelReferenceCount;
    Transform mTM;
    bool mIsTarget;
    bool mDraw;
    Vector4 mTargetColour;
    Vector4 mColour;
};

//======================================================================================================================
class PhysicalGate : public RenderObject
{
public:
    void Init(const Vector3& pos1, const Vector3& pos2, float height, const Vector4& colour);
    void Terminate();

    void RenderUpdate(Viewport* viewport, int renderLevel) OVERRIDE;

    const Transform& GetTM() const OVERRIDE {return mTM;}
    float GetRenderBoundingRadius() const OVERRIDE {return mRenderModel.GetBoundingRadius();}

    void SetBlendColour(const Vector4& blendColour, float amount);
    float GetBlendAmount() const {return mBlendAmount;}
    void SetBlendAmount(float amount) {mBlendAmount = amount;}

private:
    RenderModel mRenderModel;
    Transform mTM;

    btBoxShape* mPostShapes[3];
    btRigidBody* mPostBodies[3];

    Vector4 mColour;
    Vector4 mBlendColour;
    float mBlendAmount;
};

#endif
