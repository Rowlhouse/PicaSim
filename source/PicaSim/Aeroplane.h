#ifndef AEROPLANE_H
#define AEROPLANE_H

#include "Framework.h"
#include "GameSettings.h"
#include "Rope.h"
#include <string>
#include <memory>

class AeroplaneGraphics;
class AeroplanePhysics;
class Controller;
class AIControllerTug;
class IncomingConnection;

/// Aeroplane contains the graphical and physical parts separately so that it's 
/// possible to have a non-graphical simulation, or a graphical-only representation 
/// (e.g. for animated aeroplanes, or for networking), or an AI/human pilot. Common 
/// functionality like the position can get stored in this container class.
/// Common functionality (like 
class Aeroplane : public CameraTarget, public CameraTransform, public Entity
{
public:
    enum LaunchMode {LAUNCHMODE_NONE, LAUNCHMODE_BUNGEE, LAUNCHMODE_AEROTOW};

    Aeroplane(Controller& controller);
    ~Aeroplane();

    /// If basicLaunchPos is zero then the aeroplane will not be moved
    void Init(const AeroplaneSettings& as, 
                        const Vector3* basicLaunchPos, 
                        class LoadingScreenHelper* loadingScreen);
    void Terminate();

    void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

    // Note that this transform may be interpolated between two physics steps.
    const Transform& GetTransform() const {return mTM;}

    /// Note that setting the transform will also set the physics transform, resetting any interpolation etc
    void SetTransform(const Transform& tm, const Vector3& vel, const Vector3& angVel);

    const Vector3& GetVelocity() const {return mVelocity;}
    const Vector3& GetAngularVelocity() const {return mAngularVelocity;}

    void Launch(const Vector3& basicLaunchPos);

    Transform GetCameraTransform(void* cameraUserData) const OVERRIDE;
    Vector3 GetCameraTargetPosition(
        const Vector3& cameraPosition, 
        const void*    cameraUserData,  
        float&         targetRadius,
        float&         closestDistanceToCamera) const OVERRIDE;

    AeroplaneGraphics* GetGraphics() {return mGraphics.get();}
    const AeroplaneGraphics* GetGraphics() const {return mGraphics.get();}

    AeroplanePhysics* GetPhysics() {return mPhysics.get();}
    const AeroplanePhysics* GetPhysics() const {return mPhysics.get();}

    void SetController(Controller* controller) {mController = controller;}
    const Controller& GetController() const {return *mController;}
    Controller& GetController() {return *mController;}

    void SetIncomingConnection(IncomingConnection* incomingConnection) { mIncomingConnection = incomingConnection; }

    const AeroplaneSettings& GetAeroplaneSettings() const {return mAeroplaneSettings;}
    void SetAeroplaneSettings(const AeroplaneSettings& as) {mAeroplaneSettings = as;}

    /// Total time in the air since the last launch
    float GetFlightTime() const {return mFlightTime;}

    uint32 GetChecksum() const {return mChecksum;}

    bool IsUsingBungee(Vector3& bungeePosition, Vector3& hookPosition) const;
    LaunchMode GetLaunchMode() const {return mLaunchMode;}
    void SetLaunchMode(LaunchMode launchMode) {mLaunchMode = launchMode;}

    const AIControllerTug* GetTugController() const {return mTugController.get();}

    int GetDebugAerofoilIndex() const {return mDebugAerofoilIndex;}
    void IncrementDebugAerofoilIndex() const {++mDebugAerofoilIndex;}

    enum CrashFlag {CRASHFLAG_AIRFRAME = 1 << 0, CRASHFLAG_PROPELLER = 1 << 1, CRASHFLAG_UNDERCARRIAGE = 1 << 2};
    int GetCrashFlags() const {return mCrashFlags;}
    bool GetCrashed() const {return mCrashFlags != 0;}
    bool GetCrashed(CrashFlag crashFlag) const {return mCrashFlags & crashFlag ? true : false;}
    void SetCrashFlag(CrashFlag crashFlag) {mCrashFlags |= crashFlag;}
    void ClearCrashFlags() {mCrashFlags = 0;}

    float GetAirDensity() const {return mAirDensity;}

    Vector3 GetLastLaunchPosition() const {return mLastLaunchPos;}

private:
    void EntityUpdatePostPhysics(float deltaTime);
    void EntityUpdateLoopPrePhysics(float deltaTime);
    void EntityUpdateLoopPostPhysics(float deltaTime);

    void LaunchNormal(const Vector3& basicLaunchPos);
    // Returns false if not possible
    bool LaunchBungee(const Vector3& basicLaunchPos);
    // Returns false if not possible
    bool LaunchAerotow(const Vector3& basicLaunchPos);

    void LaunchAtPosition(const Vector3& pos, const Vector3& dir);

    void ResetControls();

    float GetHeightForNoGroundPenetration(const Vector3& launchPos) const;

    AeroplaneSettings  mAeroplaneSettings;

    std::unique_ptr<AeroplaneGraphics> mGraphics;
    std::unique_ptr<AeroplanePhysics>  mPhysics;

    Rope    mBungeeRope;
    Vector3 mBungeeEnd;
    Vector3 mBungeeVel;
    float   mBungeeAmount;

    int mSmokeIDs[AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE];

    Controller*  mController;

    IncomingConnection* mIncomingConnection;

    AIControllersSettings::AIControllerSetting mTugControllerSetting;
    std::unique_ptr<AIControllerTug> mTugController;

    uint32 mChecksum;

    mutable int mDebugAerofoilIndex;

    // The object TM - may be interpolated from physics
    Transform mTM;
    Vector3 mVelocity;
    Vector3 mAngularVelocity;

    int mCrashFlags; // set of CrashFlags
    float mAirDensity;

    // For the camera
    Vector3 mSmoothedCameraVelocity;
    Vector3 mSmoothedCameraVelocityRate;

    Vector3 mSmoothedVelocityForCamera;
    Vector3 mSmoothedVelocityForCameraRate;

    Vector3 mSmoothedCameraTargetPos;
    Vector3 mSmoothedCameraTargetPosRate;

    LaunchMode mLaunchMode;

    Vector3 mLastLaunchPos;

    // For the bungee launch
    Vector3 mBungeePosition;

    // For lookaround
    float mLookYaw;
    float mLookPitch;
    float mLookYawRate;
    float mLookPitchRate;

    float mFlightTime;

    struct SoundSetting
    {
        SoundSetting()
            : mSound(0), mSoundChannel(-1), mMinSpeedRelAir(0), mFreqScalePerSpeed(0)
            , mMinFreqScale(0), mMaxFreqScale(0), mVolScalePerSpeed(0), mVolPow(1), mMaxVolume(0) {}
        AudioManager::Sound* mSound;
        AudioManager::SoundChannel mSoundChannel;

        float mMinSpeedRelAir;
        float mFreqScalePerSpeed;
        float mMinFreqScale;
        float mMaxFreqScale;

        float mVolScalePerSpeed;
        float mVolPow;
        float mMaxVolume;
    };

    typedef std::vector<SoundSetting> SoundSettings;
    SoundSettings mSoundSettings;

    struct VarioSoundSetting
    {
        VarioSoundSetting() : mSound(0), mSoundChannel(-1), mMinAscentRate(0.03f), mFreqScalePerSpeed(0), mMinFreqScale(0), mMaxFreqScale(0) {}
        AudioManager::Sound* mSound;
        AudioManager::SoundChannel mSoundChannel;

        float mMinAscentRate;
        float mFreqScalePerSpeed;
        float mMinFreqScale;
        float mMaxFreqScale;
    };

    VarioSoundSetting mVarioSoundSetting;
};

typedef std::vector<Aeroplane*> Aeroplanes; 

#endif
