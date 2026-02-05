#include "Gate.h"
#include "GameSettings.h"
#include "PicaSim.h"
#include "Environment.h"

RenderModel* GatePost::mRenderModel = 0;
int GatePost::mRenderModelReferenceCount = 0;

//======================================================================================================================
void GatePost::Init(const Vector3& pos, bool draw, const Vector4& colour, float height, const Vector4& targetColour)
{
    TRACE_METHOD_ONLY(1);
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    mTM.SetIdentity();
    Vector3 p = pos;
    Environment::GetInstance().GetTerrain().GetTerrainHeight(p, true);
    mTM.SetTrans(p);

    mDraw = draw;
    mColour = colour;
    mTargetColour = targetColour;

    if (mDraw)
    {
        if (mRenderModelReferenceCount == 0)
        {
            IwAssert(ROWLHOUSE, !mRenderModel);
            mRenderModel = new RenderModel;

            TRACE_FILE_IF(1) TRACE("GatePost::Init - creating 3D model");
            ACModel model;
            if (ACLoadModel(model, "SystemData/Objects/Tower.ac"))
            {
                if (height == 0.0f)
                    height = 20.0f;
                float modelScale = height;
                mRenderModel->Init(
                    model, "SystemData/Objects/Tower.ac", Vector3(0,0,0), 0.0f, Vector3(modelScale, modelScale, modelScale), true, true, 0);
            }
        }
        ++mRenderModelReferenceCount;
        TRACE_FILE_IF(1) TRACE("GatePost::Init %d", mRenderModelReferenceCount);

        RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);
    }

    mIsTarget = false;
}

//======================================================================================================================
void GatePost::Terminate()
{
    TRACE_METHOD_ONLY(1);
    if (mDraw)
    {
        RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);

        --mRenderModelReferenceCount;
        if (mRenderModelReferenceCount == 0)
        {
            mRenderModel->Terminate();
            delete mRenderModel;
            mRenderModel = 0;
        }
    }
}

//======================================================================================================================
void GatePost::RenderUpdate(Viewport* viewport, int renderLevel)
{
    if (!mDraw)
        return;
    TRACE_METHOD_ONLY(2);

    if (mRenderModel->IsCreated())
    {
        const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
        glDisable(GL_BLEND);

        Transform TM = mTM;

        esPushMatrix();
        GLMat44 glTM;
        ConvertTransformToGLMat44(TM, glTM);
        esMultMatrixf(&glTM[0][0]);

        ROTATE_90_X;
        ROTATE_90_Y;

        if (mIsTarget)
            mRenderModel->Render(&mTargetColour, false, options.mSeparateSpecular);
        else
            mRenderModel->Render(&mColour, false, options.mSeparateSpecular);

        esPopMatrix();
    }
}

//======================================================================================================================
void PhysicalGate::Init(const Vector3& pos1, const Vector3& pos2, float height, const Vector4& colour)
{
    TRACE_METHOD_ONLY(1);
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();

    mColour = colour;
    mBlendColour = Vector4(0,0,0, 1.0f);
    mBlendAmount = 0.0f;

    mTM.SetIdentity();
    Vector3 p = (pos1 + pos2) * 0.5f;
    mTM.SetTrans(p);

    Vector3 up(0,0,1);

    Vector3 left = pos1 - pos2;
    left.z = 0.0f;
    float width = left.GetLength();
    left.Normalise();

    Vector3 dir = left.Cross(up);
    dir.Normalise();

    SetRowX(mTM, dir);
    SetRowY(mTM, left);
    SetRowZ(mTM, up);

    float poleWidth = 0.1f;
    float hpw = poleWidth * 0.5f;

    RenderModel::Box posts[3];

    posts[0].mColour = colour;
    posts[0].mExtents.x = posts[0].mExtents.y = poleWidth;
    posts[0].mExtents.z = height;
    posts[0].mTM.SetIdentity();
    posts[0].mTM.SetTrans(Vector3(0.0f, hpw + width*0.5f, height*0.5f));

    posts[1].mColour = colour;
    posts[1].mExtents.x = posts[1].mExtents.y = poleWidth;
    posts[1].mExtents.z = height;
    posts[1].mTM.SetIdentity();
    posts[1].mTM.SetTrans(Vector3(0.0f, -(hpw+width*0.5f), height*0.5f));

    posts[2].mColour = colour;
    posts[2].mExtents.x = posts[2].mExtents.z = poleWidth;
    posts[2].mExtents.y = width + 2.0f * poleWidth;
    posts[2].mTM.SetIdentity();
    posts[2].mTM.SetTrans(Vector3(0.0f, 0.0f, hpw+height));

    RenderModel::Boxes boxes;
    boxes.push_back(posts[0]);
    boxes.push_back(posts[1]);
    boxes.push_back(posts[2]);

    // Physics
    for (size_t i = 0 ; i != 3 ; ++i)
    {
        mPostShapes[i] = new btBoxShape(Vector3ToBulletVector3(posts[i].mExtents * 0.5f));

        btTransform tm = TransformToBulletTransform(posts[i].mTM * mTM);

        btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, 0, mPostShapes[i]);
        rbInfo.m_startWorldTransform = tm;
        mPostBodies[i] = new btRigidBody(rbInfo);
        EntityManager::GetInstance().GetDynamicsWorld().addRigidBody(mPostBodies[i]);
    }

    TRACE_FILE_IF(1) TRACE("Gate::Init - creating model");

    mRenderModel.Init(boxes);

    RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);
}

//======================================================================================================================
void PhysicalGate::Terminate()
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);

    mRenderModel.Terminate();

    for (size_t i = 0 ; i != 3 ; ++i)
    {
        delete mPostBodies[i]->getMotionState();
        EntityManager::GetInstance().GetDynamicsWorld().removeCollisionObject( mPostBodies[i]);
        delete mPostBodies[i];
        delete mPostShapes[i];

        mPostBodies[i] = 0;
        mPostShapes[i] = 0;
    }
}

//======================================================================================================================
void PhysicalGate::RenderUpdate(Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(2);
    if (mRenderModel.IsCreated())
    {
        const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
        glDisable(GL_BLEND);

        Vector4 colour = mColour * (1.0f - mBlendAmount) + mBlendColour * mBlendAmount;

        esPushMatrix();
        GLMat44 glTM;
        ConvertTransformToGLMat44(mTM, glTM);
        esMultMatrixf(&glTM[0][0]);

        mRenderModel.Render(&colour, false, options.mSeparateSpecular);

        esPopMatrix();
    }
}

//======================================================================================================================
void PhysicalGate::SetBlendColour(const Vector4& blendColour, float amount)
{
    mBlendAmount = amount;
    mBlendColour = blendColour;
}

