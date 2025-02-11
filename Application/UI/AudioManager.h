#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

//#include <Helpers.h>

//#include <s3eTypes.h>

//#include <vector>

#include "Entete.h"
// #include "FonctionsMarmelade.h"
#include "Vecteurs.h"
// #include <cstdint>

class AudioManager
{
public:
  /// A sound that has been loaded
  struct Sound
  {
    Sound(const char* soundFile, int sampleFrequency, bool stereo, bool loop, bool normalise);
    ~Sound();
    char mName[256];      // Name (for debugging)
    int     mRefCount;
    int16_t*  mSoundData;        // Buffer that holds the sound data
    int     mSoundSamples;     // Length of sound data in 16-bit samples
    int     mSampleFrequency;
    bool    mStereo;
    bool    mLoop;
    bool    mNormaliseOnLoad;
  };

  /// Used to refer to a channel 
  typedef int SoundChannel;

  static void Init();
  static void Terminate();

  static AudioManager& GetInstance() {IwAssert(ROWLHOUSE, mInstance); return *mInstance;}

  void Update(float dt);

  /// Sets the position of the listener
  void SetTransformAndVelocity(const Transform& tm, const Vector3& vel) {mTM = tm; mVelocity = vel;}

  /// This stops playing all sounds and marks them all as not in use.
  void StopAllChannels();

  /// Just sets all the volumes to zero - will be restored on the next update
  void SetAllChannelsToZeroVolume();

  /// Sets the overall volume (without smoothing). Note that volScale can be > 1
  void SetVolume(float volScale) {mVolScale = volScale;}

  /// Returns a free sound channel (if one exists). This marks the channel as in-use. 
  /// Once a sound starts playing on a channel, it becomes "owned" by the caller,
  /// even if the sound is non-loooping and it finishes. The sound needs to be stopped 
  /// if that channel is to become available for re-use.
  /// Returns -1 if no free channel is found
  SoundChannel AllocateSoundChannel(float soundSourceRadius, bool use3D);
  void ReleaseSoundChannel(SoundChannel soundChannel);

  void StartSoundOnChannel(SoundChannel soundChannel, Sound* sound, bool loop);
  /// Pauses playing the sound but doesn't mark it as not in-use
  void PauseSoundOnChannel(SoundChannel soundChannel);

  bool IsSoundPlayingOnChannel(SoundChannel soundChannel);

  void SetChannelPositionAndVelocity(SoundChannel soundChannel, const Vector3& pos, const Vector3& velocity);
  void SetChannelFrequencyScale(SoundChannel soundChannel, float freqScale);

  void SetChannelTargetVolumeScale(SoundChannel soundChannel, float targetVolScale, float volScaleRate = 2.0f);
  /// Sets the volume directly. Use the target if possible as that will be less likely to cause popping
  void SetChannelVolumeScale(SoundChannel soundChannel, float volScale);

  /// Sounds should all be loaded in one go to prevent stalls. Sounds will be reference counted, 
  /// so this will only actually load if necessary.
  Sound* LoadSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop, bool normalise);

  void UnloadSound(Sound* sound);
private:
  struct Channel
  {
    Channel();
    bool         mInUse;
    bool         mUse3D;
    const Sound* mSound;
    /// This is how many samples, or left/right pairs of samples we've played. 
    /// Multiply by 2 if stereo to get the offset. Float so that we can handle 
    /// sample rate scaling
    float         mPlaybackPosF;

    /// Derived values used in the sound callback - so these will be updated (ideally atomically!)
    float         mVolumeScale;
    float         mLeftVolF;
    float         mRightVolF;
    float         mPlaybackFrequencyMultiplierF;

    /// These are set by the game
    float         mSoundSourceRadius;
    Vector3       mSourcePosition;
    Vector3       mSourceVelocity;
    float         mFrequencyScale;
    float         mTargetVolumeScaleF; // The target overall volume - there's a fade into/out of this
    float         mTargetVolumeScaleFRate; // The rate of change of the overall volume

    /// Only used in the callback
    float         mCurrentLeftVolF;
    float         mCurrentRightVolF;
    float         mCurrentFrequencyMultiplierF;
    float         mLastOutputLeftF;
    float         mLastOutputRightF;
  };

  typedef std::vector<Sound*> Sounds;

  AudioManager();
  ~AudioManager();

  // returns the 
  Sounds::iterator FindLoadedSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop);

  /// Callback for playing the audio when the source is stereo
  static int32_t AudioCallbackStereoSource(void* sys, void* user);
  static int32_t AudioCallbackMonoSource(void* sys, void* user);

  static AudioManager* mInstance;

  /// Updates the derived quantities based on the listener and channel pos etc
  void UpdateChannel(SoundChannel soundChannel, float deltaTime);

  /// Details on the channels available
  int mNumAvailableChannels;
  Channel* mChannels;

  /// All the loaded sounds. On the heap so that push_back doesn't invalidate 
  /// references to already loaded sounds.
  Sounds mSounds;

  Transform mTM;
  Vector3 mVelocity;

  float mVolScale;
};

#endif