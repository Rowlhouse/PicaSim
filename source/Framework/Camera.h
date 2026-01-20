#ifndef CAMERA_H
#define CAMERA_H

#include "Helpers.h"
#include "FrameworkSettings.h"
#include "Graphics.h"

/// This is used to make the camera look at an object. The position and up direction should be set manually on the camera.
class CameraTarget
{
public:
    virtual ~CameraTarget() {}
    virtual Vector3 GetCameraTargetPosition(
        const Vector3& cameraPosition, 
        const void*    cameraUserData, 
        float&         targetRadius,
        float&         closestDistanceToCamera) const = 0;
};

/// This is used to lock the camera to an object - the camera TM is obtained directly - any manual sets are ignored/overridden.
/// The transform is such that when its identity, rowX is the look direction (north), rowY is the left direction (west) and rowZ is up
class CameraTransform
{
public:
    virtual ~CameraTransform() {}
    virtual Transform GetCameraTransform(void* cameraUserData) const = 0;
};

class Camera
{
public:
    Camera(const FrameworkSettings& frameworkSettings);

    /// Update to allow the camera to move etc
    void RenderUpdate(float deltaTime) {}

    // Set up the perspective transform and the modelview matrix for the camera. aspectRatio is width/height
    void SetupCameraProjection(float aspectRatio);
    void SetupCameraView(const Vector3& positionOffset = Vector3(0,0,0));

    void SetCameraTarget(CameraTarget* cameraTarget) {mCameraTarget = cameraTarget;}
    const CameraTarget* GetCameraTarget() const {return mCameraTarget;}

    void SetCameraTransform(CameraTransform* cameraTransform) {mCameraTransform = cameraTransform;}
    const CameraTransform* GetCameraTransform() const {return mCameraTransform;}

    /// Returns the camera transform. Note that this is only updated during the SetupCamera call.
    const Transform& GetTransform() const {return mTM;}

    void SetPosition(const Vector3& pos) {mPosition = pos;}
    const Vector3& GetPosition() const {return mPosition;}

    void SetUpDirection(const Vector3& upDir) {mUpDirection = upDir;}
    const Vector3& GetUpdirection() const {return mUpDirection;}

    void SetTargetPosition(const Vector3& targetPosition) {mTargetPosition = targetPosition;}
    const Vector3& GetTargetPosition() const {return mTargetPosition;}
    Vector3 GetLookDir() const {return (mTargetPosition - mPosition).GetNormalised();}

    /// Returns the width of the near mClipToPlanes mClipPlanes
    float GetNearClipPlaneWidth() const {return mNearWidth;}
    float GetNearClipPlaneHeight() const {return mNearHeight;}

    float GetNearClipPlaneDistance() const {return mFrameworkSettings.mNearClipPlaneDistance;}
    float GetFarClipPlaneDistance() const {return mFrameworkSettings.mFarClipPlaneDistance;}

    /// If autoZoom is 1 then the fov will just cover zoomedObjectSize for a camera target 
    void SetAutoZoom(float autoZoom, float zoomedObjectSize) {mAutoZoom = autoZoom; mZoomedObjectSize = zoomedObjectSize;}
    void DisableAutoZoom() {SetAutoZoom(0.0f, 1.0f);}

    /// The full field of view (in radians)
    float GetVerticalFOV() const {return mActualVerticalFOV;}
    float GetHorizontalFOV() const {return mActualHorizontalFOV;}
    void SetVerticalFOV(float fov) {mBasicVerticalFOV = mActualVerticalFOV = fov;}

    /// User data is passed to CameraTarget
    void SetUserData(void* userData) {mUserData = userData;}
    void* GetUserData() const {return mUserData;}

    const FrameworkSettings& GetFrameworkSettings() const {return mFrameworkSettings;}

    bool isPointInFrustum(const Vector3& pt) const;
    bool isSphereFullyInFrustum(const Vector3& pt, float radius) const;
    bool isSpherePartlyInFrustum(const Vector3& pt, float radius) const;

private:
    void UpdateFrustumPlanes();

    const FrameworkSettings& mFrameworkSettings;

    CameraTarget* mCameraTarget;
    CameraTransform* mCameraTransform;

    Vector3 mPosition;
    Vector3 mTargetPosition;
    Vector3 mUpDirection;

    Plane mFrustumPlanes[6];

    Transform mTM;

    float mNearClip;
    float mFarClip;
    float mNearWidth;
    float mNearHeight;

    float mBasicVerticalFOV;
    float mActualVerticalFOV; // modified when autozoom is used
    float mActualHorizontalFOV;

    float mZoomedObjectSize;
    float mAutoZoom;

    void* mUserData;
};

#endif
