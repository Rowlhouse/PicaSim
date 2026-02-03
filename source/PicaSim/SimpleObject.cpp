#include "SimpleObject.h"

#include "PicaSim.h"
#include "ShaderManager.h"
#include "Framework.h"
#include "Shaders.h"

#include "../Platform/S3ECompat.h"
#include <cmath>

//======================================================================================================================
BoxObject::BoxObject(const Vector3& extents,
                     const Transform& tm,
                     const Vector3& colour,
                     const char* textureFile,
                     float mass,
                     bool enableRender,
                     bool enableShadows,
                     bool visible,
                     bool shadowVisible)
    : 
    mExtents(extents), mTM(tm), mInitialTM(tm), mColour(colour), mWireframe(false), mTextureFile(textureFile ? textureFile : ""), 
    mMass(mass), mRenderEnabled(enableRender), mShadowsEnabled(enableShadows), mVisible(visible), mShadowVisible(shadowVisible)
{
    if (textureFile)
    {
        LoadTextureFromFile(mTexture, textureFile);
        mTexture.Upload();
        TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", textureFile, mTexture.mHWID);
    }

    // Create the physics representation

    mCollisionShape = new btBoxShape(Vector3ToBulletVector3(extents * 0.5f));

    btVector3 localInertia(0,0,0);
    if (mass > 0.0f)
        mCollisionShape->calculateLocalInertia(mass, localInertia);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,0,mCollisionShape,localInertia);
    rbInfo.m_startWorldTransform = TransformToBulletTransform(tm);
    mRigidBody = new btRigidBody(rbInfo);

    mRigidBody->setFlags(BT_ENABLE_GYROSCOPIC_FORCE_IMPLICIT_CATTO); // Same as BODY

    EntityManager::GetInstance().GetDynamicsWorld().addRigidBody(mRigidBody);

    SetExtents(mExtents);

    if (mRenderEnabled)
    {
        int level = mVisible ? RENDER_LEVEL_OBJECTS : RENDER_LEVEL_OBJECTS_BACKGROUND;
        RenderManager::GetInstance().RegisterRenderObject(this, level);
    }
    if (mShadowsEnabled)
        RenderManager::GetInstance().RegisterShadowCasterObject(this);
    if (mMass > 0.0f)
        EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
}

//======================================================================================================================
BoxObject::~BoxObject()
{
    if (mRenderEnabled)
    {
        int level = mVisible ? RENDER_LEVEL_OBJECTS : RENDER_LEVEL_OBJECTS_BACKGROUND;
        RenderManager::GetInstance().UnregisterRenderObject(this, level);
    }
    if (mShadowsEnabled)
        RenderManager::GetInstance().UnregisterShadowCasterObject(this);
    if (mMass > 0.0f)
        EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);

    delete mRigidBody->getMotionState();
    EntityManager::GetInstance().GetDynamicsWorld().removeCollisionObject(mRigidBody);
    delete mRigidBody;
    delete mCollisionShape;
}

//======================================================================================================================
void BoxObject::SetVisible(bool visible) 
{
    if (visible == mVisible || !mRenderEnabled)
        return;

    int oldLevel = mVisible ? RENDER_LEVEL_OBJECTS : RENDER_LEVEL_OBJECTS_BACKGROUND;
    RenderManager::GetInstance().UnregisterRenderObject(this, oldLevel);

    mVisible = visible;

    int newLevel = mVisible ? RENDER_LEVEL_OBJECTS : RENDER_LEVEL_OBJECTS_BACKGROUND;
    RenderManager::GetInstance().RegisterRenderObject(this, newLevel);
}

// points for the top face
static const float d = 0.5f;
static GLfloat boxPoints[] = {
    -d, -d,  d,
      d, -d,  d, 
      d,  d,  d, 
    -d,  d,  d, 
};

static GLfloat uvs[] = {
    0, 0,
    1, 0,
    1, 1,
    0, 1,
};

//======================================================================================================================
void BoxObject::RenderUpdate(class Viewport* viewport, int renderLevel)
{
    if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW && (!mVisible || !mShadowVisible))
        return;

    EnableCullFace enableCullFace(GL_BACK);

    if (gGLVersion == 1 && mVisible) 
    {
        glEnable(GL_NORMALIZE);
        if (renderLevel != RENDER_LEVEL_TERRAIN_SHADOW)
            glEnable(GL_LIGHTING);
    }

    if (!mVisible)
    {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }

    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    const ModelShader* modelShader = (ModelShader*) ShaderManager::GetInstance().GetShader(SHADER_MODEL);
    if (gGLVersion == 1)
    {
        if (renderLevel != RENDER_LEVEL_TERRAIN_SHADOW)
        {
            float spec = 0.5f;
            GLfloat mat[] = {spec, spec, spec, 1.0f};
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f); // smooth surface = large numbers = small highlights

            // Overwrites the actual material for ambient and diffuse, front and back
            glEnable(GL_COLOR_MATERIAL);
        }

        glEnableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        modelShader->Use();
        glUniform1f(modelShader->u_specularExponent, 100.0f);

        glDisableVertexAttribArray(modelShader->a_normal);

        glEnableVertexAttribArray(modelShader->a_position);
        glDisableVertexAttribArray(modelShader->a_colour);

        esSetLighting(modelShader->lightShaderInfo);
        if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW)
        {
            for (int i = 0 ; i != 5 ; ++i)
            {
                if (i == 0)
                    glUniform4f(modelShader->lightShaderInfo[i].u_lightAmbientColour, 1.0f, 1.0f, 1.0f, 1.0f);
                else
                    glUniform4f(modelShader->lightShaderInfo[i].u_lightAmbientColour, 0.0f, 0.0f, 0.0f, 1.0f);
                glUniform4f(modelShader->lightShaderInfo[i].u_lightDiffuseColour, 0.0f, 0.0f, 0.0f, 1.0f);
                glUniform4f(modelShader->lightShaderInfo[i].u_lightSpecularColour, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
    }

    PushMatrix pushMatrix;

    GLMat44 glTM;
    ConvertTransformToGLMat44(mTM, glTM);
    esMultMatrixf(&glTM[0][0]);

    esScalef(mExtents.x, mExtents.y, mExtents.z);

    if (gGLVersion == 1)
    {
        glVertexPointer(3, GL_FLOAT, 0, boxPoints);
        glNormal3f(0, 0, mExtents.z);
    }
    else
    {
        glVertexAttribPointer(modelShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) boxPoints);
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/mExtents.z);
    }


    // draw the top and then round the sides
    if (gGLVersion == 1)
    {
        if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW)
        {
            float c = ClampToRange(1.0f - mShadowAmount, 0.0f, 1.0f);
            glColor4f(c, c, c, 1.0f);
        }
        else
        {
            glColor4f(mColour.x, mColour.y, mColour.z, 1.0f);
        }
    }
    else
    {
        if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW)
        {
            float c = ClampToRange(1.0f - mShadowAmount, 0.0f, 1.0f);
            glVertexAttrib4f(modelShader->a_colour, c, c, c, 1.0f);
        }
        else
        {
            glVertexAttrib4f(modelShader->a_colour, mColour.x, mColour.y, mColour.z, 1.0f);
        }
    }
    esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Sides
    if (gGLVersion == 1)
        glNormal3f(0, 0, mExtents.y);
    else
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/mExtents.y);

    ROTATE_90_X;
    esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (gGLVersion == 1)
        glNormal3f(0, 0, mExtents.x);
    else
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/mExtents.x);

    ROTATE_90_Y;
    esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (gGLVersion == 1)
        glNormal3f(0, 0, mExtents.y);
    else
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/mExtents.y);

    ROTATE_90_Y;
    esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (gGLVersion == 1)
        glNormal3f(0, 0, mExtents.x);
    else
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/mExtents.x);

    ROTATE_90_Y;
    esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (gGLVersion == 1)
        glNormal3f(0, 0, mExtents.z);
    else
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/mExtents.z);

    ROTATE_90_X;
    esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (gGLVersion == 1)
    {
        glDisable(GL_COLOR_MATERIAL);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (mWireframe)
    {
        RenderManager::GetInstance().GetDebugRenderer().DrawBox(mTM, mExtents);
    }

    if (gGLVersion == 1) 
    {
        glDisable(GL_NORMALIZE);
        glDisable(GL_LIGHTING);
    }

    if (!mVisible)
    {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

//======================================================================================================================
void BoxObject::EntityUpdate(float deltaTime, int entityLevel)
{
    btTransform tm = mRigidBody->getWorldTransform();
    mTM = BulletTransformToTransform(tm);
}

//======================================================================================================================
void BoxObject::SetTM(const Transform& tm)
{
    mTM = tm;
    mRigidBody->setWorldTransform(TransformToBulletTransform(tm));
}

//======================================================================================================================
void BoxObject::SetExtents(const Vector3& extents)
{
    mExtents = extents;
    Vector3 margin = Vector3(1,1,1) * mCollisionShape->getMargin();
    mCollisionShape->setImplicitShapeDimensions(Vector3ToBulletVector3(extents * 0.5f - margin));
    mRenderBoundingRadius = 1.3f * (0.5f * extents).GetLength();

    if (GetMass() > 0.0f)
    {
        btVector3 localInertia(0,0,0);
        mCollisionShape->calculateLocalInertia(GetMass(), localInertia);
        mRigidBody->setMassProps(GetMass(), localInertia);
    }
}
