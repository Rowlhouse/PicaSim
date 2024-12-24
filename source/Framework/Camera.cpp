#include "Camera.h"
#include "Graphics.h"

//#include <IwGx.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
//#include <IwUtil.h>
//#include <MarmaladeVersion.h>
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(const FrameworkSettings& frameworkSettings)
  : mFrameworkSettings(frameworkSettings)
{
  mCameraTarget = 0;
  mCameraTransform = 0;

  mPosition       = glm::vec3(-3, 0, 2);
  mTargetPosition = glm::vec3(0, 0, 0);
  mUpDirection    = glm::vec3(0, 0, 1);
  for (int i = 0 ; i != 6 ; ++i)
    mFrustumPlanes[i] = Plane(glm::vec3(0,0,0), 0.0f);
  mTM = glm::mat4(1.0f);

  mBasicVerticalFOV = mActualVerticalFOV = mActualHorizontalFOV = DegreesToRadians(60.0f);

  mZoomedObjectSize = 1.0f;
  mAutoZoom = 0.0f;

  mUserData = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::SetupCameraProjection(float aspectRatio)
{
  mActualVerticalFOV = mBasicVerticalFOV;
  mActualHorizontalFOV = mActualVerticalFOV * aspectRatio;

  float nearClipPlaneDistance = mFrameworkSettings.mNearClipPlaneDistance;

  if (mCameraTransform)
  {
    mTM = mCameraTransform->GetCameraTransform(mUserData);

    mPosition = glm::vec3(mTM[3][0], mTM[3][1], mTM[3][2]);
    mTargetPosition = mPosition + glm::vec3(mTM[0]) * 50.0f;
    mUpDirection = glm::vec3(mTM[2]);
  }
  if (mCameraTarget)
  {
    float radius = 0.0f;
    float closestDistanceToCamera = 0.0f;
    mTargetPosition = mCameraTarget->GetCameraTargetPosition(glm::vec3(mTM[3]), mUserData, radius, closestDistanceToCamera);
    mUpDirection = glm::vec3(0,0,1);

    float dist = glm::length(mTargetPosition - mPosition) + 0.00001f;
    if (mAutoZoom > 0.0f)
    {
      float autoZoomFOV = 2.0f * atanf(radius / dist);
      mActualHorizontalFOV = mActualHorizontalFOV * (1.0f - mAutoZoom) + autoZoomFOV * mAutoZoom;
      mActualVerticalFOV = mActualVerticalFOV * (1.0f - mAutoZoom) + autoZoomFOV * mAutoZoom;
    }
    nearClipPlaneDistance = ClampToRange(closestDistanceToCamera, mFrameworkSettings.mNearClipPlaneDistance* 0.5f, mFrameworkSettings.mNearClipPlaneDistance);
  }

  if (aspectRatio < 1.0f)
  {
    float halfHorFOV = mActualHorizontalFOV * 0.5f;
    mActualVerticalFOV = mActualHorizontalFOV / aspectRatio;
    mNearWidth = 2.0f * nearClipPlaneDistance * tanf(halfHorFOV);
    mNearHeight = mNearWidth / aspectRatio;
  }
  else
  {
    float halfVerFOV = mActualVerticalFOV * 0.5f;
    mActualHorizontalFOV = mActualVerticalFOV * aspectRatio;
    mNearHeight = 2.0f * nearClipPlaneDistance * tanf(halfVerFOV);
    mNearWidth = mNearHeight * aspectRatio;
  }

  // Set the projection
  esMatrixMode( GL_PROJECTION );
  /*if (IsMarmaladeVersionLessThan(6,2,0))
  {
    // Workaround Marmalade rotation bug
    if (mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
    {
      if (s3eSurfaceGetInt(S3E_SURFACE_DEVICE_BLIT_DIRECTION) == S3E_SURFACE_BLIT_DIR_ROT90)
        glRotatef(-90.0f, 0, 0, 1);
      else
        glRotatef(90.0f, 0, 0, 1);
    }
  }*/

  esFrustumf(-mNearWidth*0.5f, mNearWidth*0.5f, -mNearHeight*0.5f, mNearHeight*0.5f, 
    nearClipPlaneDistance, mFrameworkSettings.mFarClipPlaneDistance);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::UpdateFrustumPlanes()
{
  GLMat44 mvpMatrix;
  GLMat44 mvMatrix; esGetMatrix(mvMatrix, GL_MODELVIEW);
  GLMat44 pMatrix; esGetMatrix(pMatrix, GL_PROJECTION);

  esMatrixMultiply(mvpMatrix, mvMatrix, pMatrix);

  //if (gGLVersion == 2)
  //  esMatrixTranspose(mvpMatrix, mvpMatrix);

  float* mvp = &mvpMatrix[0][0];

  // Right clipping plane.
  mFrustumPlanes[0] = Plane( glm::vec3(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8]), mvp[15]-mvp[12] );
  // Left clipping plane.
  mFrustumPlanes[1] = Plane( glm::vec3(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8]), mvp[15]+mvp[12] );
  // Bottom clipping plane.
  mFrustumPlanes[2] = Plane( glm::vec3(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9]), mvp[15]+mvp[13] );
  // Top clipping plane.
  mFrustumPlanes[3] = Plane( glm::vec3(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9]), mvp[15]-mvp[13] );
  // Far clipping plane.
  mFrustumPlanes[4] = Plane( glm::vec3(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10]), mvp[15]-mvp[14] );
  // Near clipping plane.
  mFrustumPlanes[5] = Plane( glm::vec3(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10]), mvp[15]+mvp[14] );

  for( unsigned int i = 0; i != 6; i++ )
  {
    mFrustumPlanes[i].Normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::SetupCameraView(const glm::vec3& positionOffset)
{
  // Set the view matrix 
  esMatrixMode( GL_MODELVIEW );

  GLMat44 viewMatrix;
  esMatrixLoadIdentity(viewMatrix);
  LookAt(
    viewMatrix,
    mPosition.x + positionOffset.x, mPosition.y + positionOffset.y, mPosition.z + positionOffset.z,
    mTargetPosition.x, mTargetPosition.y, mTargetPosition.z,
    mUpDirection.x, mUpDirection.y, mUpDirection.z);

  ConvertGLMat44ToTransform(viewMatrix, mTM);
  glm::transpose(mTM);
  mTM[3] = glm::vec4(mPosition, 1.0f);

  // OpenGL camera looks along its negative z axis and up is the y axis, so fix up
  float t0 = mTM[0][0];
  float t1 = mTM[0][1];
  float t2 = mTM[0][2];
  mTM[0][0] = -mTM[2][0];
  mTM[0][1] = -mTM[2][1];
  mTM[0][2] = -mTM[2][2];
  mTM[2][0] =  mTM[1][0];
  mTM[2][1] =  mTM[1][1];
  mTM[2][2] =  mTM[1][2];
  mTM[1][0] = -t0;
  mTM[1][1] = -t1;
  mTM[1][2] = -t2;

  UpdateFrustumPlanes();
}

//----------------------------------------------------------------------------------------------------------------------
bool Camera::isPointInFrustum(const glm::vec3& pt) const
{
  for (int i = 0 ; i != 6 ; ++i)
  {
    const Plane& plane = mFrustumPlanes[i];
    if (plane.GetDistanceToPoint(pt) < 0.0f)
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Camera::isSphereFullyInFrustum(const glm::vec3& pt, float radius) const
{
  for (int i = 0 ; i != 6 ; ++i)
  {
    const Plane& plane = mFrustumPlanes[i];
    if (plane.GetDistanceToPoint(pt) < radius)
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Camera::isSpherePartlyInFrustum(const glm::vec3& pt, float radius) const
{
  if (radius >= FLT_MAX)
    return true;

  for (int i = 0 ; i != 6 ; ++i)
  {
    const Plane& plane = mFrustumPlanes[i];
    if (plane.GetDistanceToPoint(pt) < -radius)
      return false;
  }
  return true;
}
