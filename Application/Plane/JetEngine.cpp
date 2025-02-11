#include "JetEngine.h"

// #include "Aerofoil.h"
// #include "AerofoilParameters.h"
#include "Aeroplane.h"
#include "HelpersXML.h"
#include "Trace.h"
#include "AeroplanePhysics.h"
// #include "Controller.h"
// #include "Environment.h"
#include "DimensionalScaling.h"
#include "PicaSim.h"
#include "EntityManager.h"

//----------------------------------------------------------------------------------------------------------------------
void JetEngine::Init(class TiXmlElement* engineElement, class Aeroplane* aeroplane)
{
  TRACE_METHOD_ONLY(1);
  Engine::Init(engineElement, aeroplane);

  mAeroplane = aeroplane;
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();

  mControl = 0.0f;

  for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
  {
    mControlPerChannel[i] = 0.0f;
  }

  mMaxForce = 0.0f;
  mMinSpeed = mControlExp = 1.0f;
  mMaxSpeed = 1.0f;

  // Assume a default of 0.15s per full control
  mControlRate = 1.0f / 0.15f;

  // Read data for this engine
  mName = readStringFromXML(engineElement, "name");
  readFromXML(engineElement, "controlPerChannel0", mControlPerChannel[0]);
  readFromXML(engineElement, "controlPerChannel1", mControlPerChannel[1]);
  readFromXML(engineElement, "controlPerChannel2", mControlPerChannel[2]);
  readFromXML(engineElement, "controlPerChannel3", mControlPerChannel[3]);
  readFromXML(engineElement, "controlPerChannel4", mControlPerChannel[4]);
  readFromXML(engineElement, "controlPerChannel5", mControlPerChannel[5]);
  readFromXML(engineElement, "maxForce", mMaxForce);
  readFromXML(engineElement, "minSpeed", mMinSpeed);
  readFromXML(engineElement, "maxSpeed", mMaxSpeed);
  readFromXML(engineElement, "controlExp", mControlExp);
  readFromXML(engineElement, "controlRate", mControlRate);

  // Scale
  DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);
  ds.ScaleForce(mMaxForce);
  ds.ScaleVel(mMaxSpeed);
  ds.ScaleVel(mMinSpeed);
  mMaxForce *= as.mEngineScale;
  mMaxSpeed *= as.mEngineScale;
  mMinSpeed *= as.mEngineScale;

  Vector3 enginePosition = ds.GetScaledLength(readVector3FromXML(engineElement, "position"));
  Vector3 engineRotation = readVector3FromXML(engineElement, "rotation");

  float roll = readFloatFromXML(engineElement, "roll");
  float pitch = readFloatFromXML(engineElement, "pitch");
  float yaw = readFloatFromXML(engineElement, "yaw");
  ApplyRollPitchYawToRotationDegrees(roll, pitch, yaw, engineRotation);

  float angle = DegreesToRadians(engineRotation.GetLength());
  if (angle > 0.0f)
    engineRotation.Normalise();
  else
    engineRotation = Vector3(1,0,0);

  // Store the info about the engine
  mTMLocal.SetAxisAngle(engineRotation, angle);
  mTMLocal.SetTrans(enginePosition);

  mTM = mTMLocal * mAeroplane->GetTransform();
  mVel = Vector3(0,0,0);
  mLastWash = Vector3(0,0,0);
  mLastWashAngVel = Vector3(0,0,0);

  std::string audioFile = readStringFromXML(engineElement, "audioFile");
  if (!audioFile.empty())
  {
    SoundSetting soundSetting;

    int sampleRate = readIntFromXML(engineElement, "audioSampleRate");
    float radius = readFloatFromXML(engineElement, "audioRadius");
    bool stereo = readBoolFromXML(engineElement, "audioStereo");
    soundSetting.mSound = AudioManager::GetInstance().LoadSound(audioFile.c_str(), sampleRate, stereo, true, true);
    if (soundSetting.mSound)
    {
      soundSetting.mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(radius, true);
      if (soundSetting.mSoundChannel != -1)
      {
        AudioManager::GetInstance().StartSoundOnChannel(soundSetting.mSoundChannel, soundSetting.mSound, true);
        soundSetting.mMinVolume = readFloatFromXML(engineElement, "audioMinVolume");
        soundSetting.mMaxVolume = readFloatFromXML(engineElement, "audioMaxVolume");
        soundSetting.mMinFreqScale = readFloatFromXML(engineElement, "audioMinFreqScale");
        soundSetting.mMaxFreqScale = readFloatFromXML(engineElement, "audioMaxFreqScale");
        mSoundSettings.push_back(soundSetting);
      }
      else
      {
        AudioManager::GetInstance().UnloadSound(soundSetting.mSound);
      }
    }
  }

  EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
}

//----------------------------------------------------------------------------------------------------------------------
void JetEngine::Terminate()
{
  TRACE_METHOD_ONLY(1);
  for (size_t i = 0 ; i != mSoundSettings.size() ; ++i)
  {
    if (mSoundSettings[i].mSoundChannel != -1)
      AudioManager::GetInstance().ReleaseSoundChannel(mSoundSettings[i].mSoundChannel);
    if (mSoundSettings[i].mSound)
      AudioManager::GetInstance().UnloadSound(mSoundSettings[i].mSound);
  }
  mSoundSettings.clear();

  EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
}

//---------------------------------------------------------------------------------------------------------------------
void JetEngine::Launched()
{
  mControl = 0.0f;
  mVel = Vector3(0,0,0);
}

//---------------------------------------------------------------------------------------------------------------------
void JetEngine::UpdatePrePhysics(float deltaTime, const TurbulenceData& turbulenceData)
{
  {
    const Controller& controller = mAeroplane->GetController();
    float control = 0.0f;
    for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
      control += mControlPerChannel[i] * controller.GetControl((Controller::Channel) i);

    // JetEngine control needs to be remapped, and can never be negative!
    control = ClampToRange(control, 0.0f, 1.0f);
    control = powf(control, mControlExp);

    if (mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_AIRFRAME))
      control = 0.0f;

    // Represent finite servo speed - allow infinite speed when paused
    float dt = deltaTime > 0.0f ? deltaTime : 1000.0f;
    float maxDeltaControl = mControlRate * dt;
    if (control > mControl)
      mControl += Minimum(control - mControl, maxDeltaControl);
    else 
      mControl -= Minimum(mControl - control, maxDeltaControl);
  }


  // Reduce the thrust when moving fast
  float speed = mVel.Dot(mTM.RowX());
  if (speed > mMaxSpeed)
    speed = mMaxSpeed;
  float effectiveness = (1.0f - speed / mMaxSpeed);

  float f = (speed - mMinSpeed) / (mMaxSpeed - mMinSpeed);

  effectiveness *= f;

  float force = mMaxForce * mControl * effectiveness;
  mAeroplane->GetPhysics()->ApplyWorldForceAtWorldPosition(mTM.RowX() * force, mTM.GetTrans());

  mLastWash = (mVel + mTM.RotateVec(Vector3(-mMaxSpeed,0.0f,0.0f))) * mControl;
}

//---------------------------------------------------------------------------------------------------------------------
void JetEngine::UpdatePostPhysics(float deltaTime)
{
  mTM = mTMLocal * mAeroplane->GetTransform();
  mVel = mAeroplane->GetPhysics()->GetVelAtPoint(mTM.GetTrans());
}

//---------------------------------------------------------------------------------------------------------------------
void JetEngine::EntityUpdate(float deltaTime, int entityLevel)
{
  for (size_t i = 0 ; i != mSoundSettings.size() ; ++i)
  {
    const SoundSetting& soundSetting = mSoundSettings[i];
    if (!soundSetting.mSound || soundSetting.mSoundChannel == -1)
      continue;

    float freqScale = soundSetting.mMinFreqScale + mControl * (soundSetting.mMaxFreqScale - soundSetting.mMinFreqScale);
    float volumeScale = soundSetting.mMinVolume + mControl * (soundSetting.mMaxVolume - soundSetting.mMinVolume);

    if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_PAUSED)
      volumeScale = 0.0f;

    freqScale *= PicaSim::GetInstance().GetTimeScale();

    AudioManager::GetInstance().SetChannelPositionAndVelocity(soundSetting.mSoundChannel, mTM.GetTrans(), mVel);
    AudioManager::GetInstance().SetChannelFrequencyScale(soundSetting.mSoundChannel, freqScale);
    if (PicaSim::GetInstance().GetMode() == PicaSim::MODE_GROUND)
      AudioManager::GetInstance().SetChannelTargetVolumeScale(soundSetting.mSoundChannel, PicaSim::GetInstance().GetSettings().mOptions.mOutsideAeroplaneVolume * volumeScale);
    else
      AudioManager::GetInstance().SetChannelTargetVolumeScale(soundSetting.mSoundChannel, PicaSim::GetInstance().GetSettings().mOptions.mInsideAeroplaneVolume * volumeScale);
  }
}
