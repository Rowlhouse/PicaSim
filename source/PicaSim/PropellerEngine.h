#ifndef PROPELLERENGINE_H
#define PROPELLERENGINE_H

#include "Framework.h"
#include "Controller.h"
#include "GameSettings.h"
#include "Engine.h"

class PropellerEngine : public Engine
{
public:
    PropellerEngine();

    void Init(class TiXmlElement* engineElement, class TiXmlHandle& aerodynamicsHandle, class Aeroplane* aeroplane) OVERRIDE;
    void Terminate() OVERRIDE;
    void Launched() OVERRIDE;

    void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

    Transform GetTM() const OVERRIDE {return mTM;}

    /// Opportunity to apply forces
    void UpdatePrePhysics(float deltaTime, const struct TurbulenceData& turbulenceData) OVERRIDE;

    /// Update position etc after the physics step
    void UpdatePostPhysics(float deltaTime) OVERRIDE;

    void GetRotation(float& angle, float& angVel) const OVERRIDE {angle = mAngle; angVel = mW;}

    virtual float GetRadius() const {return mRadius;}

    float GetPropSpeed() const OVERRIDE {return mW;}

private:
    // Data struct for XML values that need post-processing (position, rotation, audio)
    struct EngineData
    {
        EngineData() : position(0,0,0), rotation(0,0,0), roll(0), pitch(0), yaw(0),
            audioSampleRate(0), audioRadius(0), audioMinVolume(0), audioMaxVolume(0),
            audioMinFreqScale(1), audioMaxFreqScale(1), colour(1,1,1,0.1f) {}
        Vector3 position;
        Vector3 rotation;
        float roll, pitch, yaw;
        // Audio settings
        std::string audioFile;
        int audioSampleRate;
        float audioRadius;
        float audioMinVolume, audioMaxVolume;
        float audioMinFreqScale, audioMaxFreqScale;
        // Visual
        Vector4 colour;
    };

    // For reading engine properties from XML, used for copy functionality
    void ReadFromXML(class TiXmlElement* engineElement, EngineData& engineData);

    class Aeroplane* mAeroplane;

    // The position/orientation of the middle of the engine
    Transform mTMLocal;
    Transform mTM;
    Vector3 mVel;

    int   mNumBlades;
    int   mNumRings;
    float mRadius;
    float mPitch;
    float mBladeChord;
    float mCL0;
    float mCLPerRadian;
    float mCD0;
    float mCDInducedMultiplier;
    float mStallAngle;
    float mInertia;
    float mMaxTorque;
    float mMaxW;
    float mMinW;
    float mFrictionTorque;
    float mWashRotationFraction;
    float mAeroTorqueScale;
    float mPropDiskAreaScale;
    float mDirectionMultiplier;
    float mTransitionalLiftSpeed;
    float mTransitionalLiftAmount;
    float mPropPredictionTime;
    float mPropPredictionMaxAngSpeed;
    bool  mIsVariable;

    float mControlExp;
    float mControlRate;
    int mChannelForMode;

    // Calculated
    float mPropSolidity;

    // The state
    enum Mode
    {
        MODE_THROTTLE,
        MODE_VPP
    };
    Mode mMode;
    float mControl;
    float mW; // prop angular vel
    float mAngle; // Prop angle

    float mControlPerChannel[Controller::MAX_CHANNELS];
    float mPitchAnglePerChannel[Controller::MAX_CHANNELS];
    float mYawAnglePerChannel[Controller::MAX_CHANNELS];

    enum {MAX_GYROS = 4};
    struct GyroControl
    {
        GyroControl() : mControl(0.0f) {}
        std::string mName;
        float mControl;
    };
    GyroControl mGyroControls[MAX_GYROS];

    enum {MAX_ACCELEROMETERS = 4};
    struct AccelerometerControl
    {
        AccelerometerControl() : mControl(0.0f) {}
        std::string mName;
        float mControl;
    };
    AccelerometerControl mAccelerometerControls[MAX_ACCELEROMETERS];

    struct SoundSetting
    {
        SoundSetting() : mSound(0), mSoundChannel(-1), mRadius(1.0f), mMinVolume(0), mMaxVolume(0), mMinFreqScale(1), mMaxFreqScale(1) {}
        AudioManager::Sound* mSound;
        AudioManager::SoundChannel mSoundChannel;

        float mRadius;
        float mMinVolume;
        float mMaxVolume;
        float mMinFreqScale;
        float mMaxFreqScale;
    };

    typedef std::vector<SoundSetting> SoundSettings;
    SoundSettings mSoundSettings;

};

#endif
