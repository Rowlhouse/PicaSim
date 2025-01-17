#include "Camera.h"
#include "Helpers.h"
#include "Graphics.h"

//#include <IwGx.h>
//#include <IwUtil.h>
//#include <MarmaladeVersion.h>
//#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(const FrameworkSettings& frameworkSettings)
  : mFrameworkSettings(frameworkSettings)
{
  mCameraTarget = 0;
  mCameraTransform = 0;

  mPosition       = Vector3(-3, 0, 2);
  mTargetPosition = Vector3(0, 0, 0);
  mUpDirection    = Vector3(0, 0, 1);
  for (int i = 0 ; i != 6 ; ++i)
    mFrustumPlanes[i] = Plane(Vector3(0,0,0), 0.0f);
  mTM.SetIdentity();

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

    mPosition = mTM.t;
    mTargetPosition = mPosition + mTM.RowX() * 50.0f;
    mUpDirection = mTM.RowZ();
  }
  if (mCameraTarget)
  {
    float radius = 0.0f;
    float closestDistanceToCamera = 0.0f;
    mTargetPosition = mCameraTarget->GetCameraTargetPosition(mTM.GetTrans(), mUserData, radius, closestDistanceToCamera);
    mUpDirection = Vector3(0,0,1);

    float dist = (mTargetPosition - mPosition).GetLength() + 0.00001f;
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
  mFrustumPlanes[0] = Plane( Vector3(mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8]), mvp[15]-mvp[12] );
  // Left clipping plane.
  mFrustumPlanes[1] = Plane( Vector3(mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8]), mvp[15]+mvp[12] );
  // Bottom clipping plane.
  mFrustumPlanes[2] = Plane( Vector3(mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9]), mvp[15]+mvp[13] );
  // Top clipping plane.
  mFrustumPlanes[3] = Plane( Vector3(mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9]), mvp[15]-mvp[13] );
  // Far clipping plane.
  mFrustumPlanes[4] = Plane( Vector3(mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10]), mvp[15]-mvp[14] );
  // Near clipping plane.
  mFrustumPlanes[5] = Plane( Vector3(mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10]), mvp[15]+mvp[14] );

  for( unsigned int i = 0; i != 6; i++ )
  {
    mFrustumPlanes[i].Normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::SetupCameraView(const Vector3& positionOffset)
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
  mTM.Transpose();
  mTM.t = mPosition;

  // OpenGL camera looks along its negative z axis and up is the y axis, so fix up
  float t0 = mTM.m[0][0];
  float t1 = mTM.m[0][1];
  float t2 = mTM.m[0][2];
  mTM.m[0][0] = -mTM.m[2][0];
  mTM.m[0][1] = -mTM.m[2][1];
  mTM.m[0][2] = -mTM.m[2][2];
  mTM.m[2][0] =  mTM.m[1][0];
  mTM.m[2][1] =  mTM.m[1][1];
  mTM.m[2][2] =  mTM.m[1][2];
  mTM.m[1][0] = -t0;
  mTM.m[1][1] = -t1;
  mTM.m[1][2] = -t2;

  UpdateFrustumPlanes();
}

//----------------------------------------------------------------------------------------------------------------------
bool Camera::isPointInFrustum(const Vector3& pt) const
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
bool Camera::isSphereFullyInFrustum(const Vector3& pt, float radius) const
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
bool Camera::isSpherePartlyInFrustum(const Vector3& pt, float radius) const
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
