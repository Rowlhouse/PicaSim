/*
    Sss - a slope soaring simulater.
    Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/

#include "ParticleEmitter.h"
#include "Trace.h"
#include "Graphics.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "Viewport.h"

//======================================================================================================================
Particle::Particle(
    const Vector3& initialPos, 
    const Vector3& initialVel,
    const Vector3& targetVel,
    const Vector3& colour,
    float initialSize,
    float finalSize,
    float initialAlpha,
    float lifetime,
    float rotation,
    float rotationRate)
    :
    mPos(initialPos),
    mVel(initialVel),
    mTargetVel(targetVel),
    mColour(colour),
    mInitialSize(initialSize),
    mFinalSize(finalSize),
    mInitialAlpha(initialAlpha),
    mTimeLeft(lifetime),
    mLifetime(lifetime),
    mRotation(rotation),
    mRotationRate(rotationRate)
{
}
    
//======================================================================================================================
void Particle::Update(float dt, float E, float dampingTime)
{
    //mPos += (mTargetVel - mVel) * dampingTime * E - (mTargetVel - mVel) * dampingTime + mTargetVel * dt;
    mVel = mTargetVel + (mVel - mTargetVel) * E;
    mPos += dt * mVel;
    mTimeLeft -= Maximum(dt, 0.0f);
    mRotation += mRotationRate * dt;
}

//======================================================================================================================
void Particle::Draw(const float* rotationMatrix)
{
    esPushMatrix();
    esTranslatef(mPos.x, mPos.y, mPos.z);
    esMultMatrixf(&rotationMatrix[0]); // TODO move this up to the caller
    esRotatef(mRotation, 1.0f, 0.0f, 0.0f);

    float frac = 1.0f - mTimeLeft/mLifetime;

    float size = mInitialSize + (mFinalSize - mInitialSize) * frac;
    float alpha = mInitialAlpha * (mTimeLeft/mLifetime);

    float s2 = size * 0.5f;

    GLfloat pts[] = {
        0, s2, -s2,
        0, -s2, -s2,
        0, -s2, s2,
        0, s2, s2,
    };

    GLfloat uvs[] = {
        0, 1,
        1, 1,
        1, 0,
        0, 0,
    };

    glVertexPointer(3, GL_FLOAT, 0, pts);
    glTexCoordPointer(2, GL_FLOAT, 0, uvs);
    glColor4f(mColour.x, mColour.y, mColour.z, alpha);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    esPopMatrix();
}

//======================================================================================================================
ParticleEmitter::ParticleEmitter(
    int            maxNumParticles,
    const Vector3& emitterPos,
    const Vector3& particleVel,
    const Vector3& targetVel,
    const Vector3& colour,
    const Texture* texture,
    float          initialAlpha,
    float          initialSize,
    float          finalSize,
    float          lifetime,
    float          dampingTime,
    float          rate)
    : 
    mEmitterPos(emitterPos),
    mParticleVel(particleVel),
    mTargetVel(targetVel),
    mColour(colour),
    mTexture(texture),
    mMaxNum(maxNumParticles),
    mInitialSize(initialSize),
    mFinalSize(finalSize),
    mInitialAlpha(initialAlpha),
    mLifetime(lifetime),
    mDampingTime(dampingTime),
    mRate(rate),
    mNum(0),
    mNumberToAdd(0),
    mSuicidal(false),
    mDead(false),
    mViewportToUse(0)
{
    TRACE_METHOD_ONLY(2);
    RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_TERRAIN_SHADOW+1);
}

//======================================================================================================================
ParticleEmitter::~ParticleEmitter()
{
    RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_TERRAIN_SHADOW+1);
}

//======================================================================================================================
void ParticleEmitter::Update(float          dt,
                                                          const Vector3& emitterPos,
                                                          const Vector3& particleVel,
                                                          const Vector3& targetVel,
                                                          float          jitterVelMagnitude)
{
    if (mDead || mSuicidal)
        return;
    
    mNumberToAdd += dt * mRate;

    int numberToAddNow = Minimum((int) mNumberToAdd, mMaxNum - mNum);
    if (numberToAddNow > 0)
    {
        mNumberToAdd -= numberToAddNow;
        Vector3 posDiff = emitterPos - mEmitterPos;
        Vector3 velDiff = particleVel - mParticleVel;
        Vector3 targetDiff = targetVel - mTargetVel;
        for (int i = 0 ; i < numberToAddNow ; ++i)
        {
            // add in a gradual sense... i.e. spread the particles out
            float frac = float(i) / numberToAddNow;
            Vector3 pos = mEmitterPos + frac * posDiff;
            Vector3 target = mTargetVel + frac * targetDiff;
            Vector3 jitterVel(
                RangedRandom(-jitterVelMagnitude, jitterVelMagnitude), 
                RangedRandom(-jitterVelMagnitude, jitterVelMagnitude), 
                RangedRandom(-jitterVelMagnitude, jitterVelMagnitude)
                );
            Vector3 vel = mParticleVel + frac * velDiff + jitterVel;
            mParticles.push_back(
                Particle(pos,
                                  vel,
                                  target,
                                  mColour,
                                  mInitialSize,
                                  mFinalSize,
                                  mInitialAlpha,
                                  mLifetime,
                                  RangedRandom(0.0f, 360.0f),
                                  RangedRandom(-40.0f, 40.0f)));
            --numberToAddNow;
            ++mNum;
        }
    }
    // just update the source pos etc
    mEmitterPos = emitterPos;
    mParticleVel = particleVel;
    mTargetVel = targetVel;
}

static GLfloat pts[] = {
    0.0f,  0.5f,  0.5f,
    0.0f, -0.5f,  0.5f, 
    0.0f, -0.5f, -0.5f, 
    0.0f,  0.5f, -0.5f, 
};

static GLfloat uvs[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};


//======================================================================================================================
void ParticleEmitter::RenderUpdate(class Viewport* viewport, int renderLevel)
{
    if (mParticles.empty())
        return;

    if (!mTexture || !mTexture->GetFlags() & Texture::UPLOADED_F)
        return;

    if (mViewportToUse && mViewportToUse != viewport)
        return;

    glBindTexture(GL_TEXTURE_2D, mTexture->mHWID);

    DisableDepthMask disableDepthMask;
    DisableFog disableFog;
    EnableBlend enableBlend;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const SmokeShader* smokeShader = (SmokeShader*) ShaderManager::GetInstance().GetShader(SHADER_SMOKE);
    if (gGLVersion == 1)
    {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glActiveTexture(GL_TEXTURE0);
        glVertexPointer(3, GL_FLOAT, 0, &pts[0]);
        glTexCoordPointer(2, GL_FLOAT, 0, &uvs[0]);
    }
    else
    {
        smokeShader->Use();
        glUniform1i(smokeShader->u_texture, 0);

        glEnableVertexAttribArray(smokeShader->a_position);
        glEnableVertexAttribArray(smokeShader->a_texCoord);

        glVertexAttribPointer(smokeShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glVertexAttribPointer(smokeShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    }

    const Transform cameraTM = viewport->GetCamera()->GetTransform();
    const Vector3 lookDir = cameraTM.RowX();
    const Vector3 cameraPos = cameraTM.GetTrans();
    Transform cameraRot(cameraTM);
    cameraRot.SetTrans(Vector3(0,0,0));

    GLMat44 cameraRotGL;
    ConvertTransformToGLMat44(cameraRot, cameraRotGL);

    for (ParticleList::iterator it = mParticles.begin(); it != mParticles.end(); ++it)
    {
        const Particle& particle = *it;

        const Vector3 pos = particle.mPos;
        // Never render anything behind the camera
        if ((pos - cameraPos).Dot(lookDir) < 0.0f)
            continue;

        float frac = 1.0f - particle.mTimeLeft / particle.mLifetime;
        float size = particle.mInitialSize + (particle.mFinalSize - particle.mInitialSize) * frac;

        float alpha = particle.mInitialAlpha * (1.0f - frac);
        if (gGLVersion == 1)
            glColor4f(particle.mColour.x, particle.mColour.y, particle.mColour.z, alpha);
        else
            glUniform4f(smokeShader->u_colour, particle.mColour.x, particle.mColour.y, particle.mColour.z, alpha);

        esPushMatrix();
        esTranslatef(pos.x, pos.y, pos.z);
        esMultMatrixf(&cameraRotGL[0][0]);
        esScalef(size, size, size);
        esRotatef(particle.mRotation, 1.0f, 0.0f, 0.0f);
        esSetModelViewProjectionMatrix(smokeShader->u_mvpMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        esPopMatrix();
    }

    if (gGLVersion == 1)
    {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        glDisableVertexAttribArray(smokeShader->a_position);
        glDisableVertexAttribArray(smokeShader->a_texCoord);
    }

#if 0
    for (ParticleList::iterator it = mParticles.begin(); it != mParticles.end(); ++it)
    {
        const Particle& p = *it;
        float frac = 1.0f - p.mTimeLeft / p.mLifetime;
        float size = p.mInitialSize + (p.mFinalSize - p.mInitialSize) * frac;
        RenderManager::GetInstance().GetDebugRenderer().DrawPoint(p.mPos, size, Vector3(p.mColour.x, p.mColour.y, p.mColour.z));
    }

#endif
}

//======================================================================================================================
void ParticleEmitter::MoveParticles(float dt)
{
    float dampingFrac = mDampingTime > 0.0f ? expf(-dt / mDampingTime) : 0.0f;
    for (ParticleList::iterator it = mParticles.begin(); it != mParticles.end();  )
    {
        it->Update(dt, dampingFrac, mDampingTime);
        if (it->GetTimeLeft() <= 0.0f)
        {
            mParticles.erase(it++);
            --mNum;
        }
        else
        {
            ++it;
        }
    }
    if (mSuicidal && mParticles.empty())
        mDead = true;
}

