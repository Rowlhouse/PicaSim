#include "AudioManager.h"
#include "Helpers.h"
#include "Trace.h"
#include "FonctionsMarmelade.h"
#include "File.h"
#include "SoundMarmelade.h"

//#include <IwUtil.h>
//#include <s3eSound.h>
//#include <s3eAtomic.h>
#include <unistd.h>
#include <limits>

#define S3E_INLINE inline

AudioManager* AudioManager::mInstance = 0;

static float gPlaybackFrequency = 1.0f;

static float filterAmount = 0.7f;
#define USE_FILTERx

//---------------------------------------------------------------------------------------------------------------------
S3E_INLINE int16_t ClipToInt16(int32_t sval)
{
  enum
  {
    minval =  INT16_MIN,
    maxval =  INT16_MAX,
    allbits = UINT16_MAX
  };

  // quick overflow test, the addition moves valid range to 0-allbits
  if ((sval-minval) & ~allbits)
  {
    // we overflowed.
    if (sval > maxval)
      sval = maxval;
    else
      if (sval < minval)
        sval = minval;
  }
  return (int16_t)sval;
}

//---------------------------------------------------------------------------------------------------------------------
int32_t AudioManager::AudioCallbackMonoSource(void* sys, void* user)
{
  s3eSoundGenAudioInfo* info = (s3eSoundGenAudioInfo*)sys;
  AudioManager::Channel* channel = (AudioManager::Channel*) user;

  info->m_EndSample = S3E_FALSE;
  int16_t* target = (int16_t*)info->m_Target;

  int outputSampleSize = info->m_Stereo ? 2 : 1;
  int outputSamplesPlayed = 0;

  if ((!channel->mInUse || channel->mVolumeScale < 0.0001f) && channel->mSound->mLoop && info->m_Mix)
  {
    return info->m_NumSamples;
  }

  float origInvDeltaPlaybackPos = Maximum(gPlaybackFrequency / (channel->mSound->mSampleFrequency * channel->mCurrentFrequencyMultiplierF), 0.000001f);
  float targetInvDeltaPlaybackPos = Maximum(gPlaybackFrequency / (channel->mSound->mSampleFrequency * channel->mPlaybackFrequencyMultiplierF), 0.000001f);
  channel->mCurrentFrequencyMultiplierF = channel->mPlaybackFrequencyMultiplierF;

  float origLeftVol = channel->mCurrentLeftVolF;
  float origRightVol = channel->mCurrentRightVolF;
  float targetLeftVol = channel->mLeftVolF;
  float targetRightVol = channel->mRightVolF;
  channel->mCurrentLeftVolF = channel->mLeftVolF;
  channel->mCurrentRightVolF = channel->mRightVolF;

  float numSoundSamplesF = float(channel->mSound->mSoundSamples);
  IwAssert(ROWLHOUSE, channel->mPlaybackPosF >= 0.0f && channel->mPlaybackPosF == channel->mPlaybackPosF);

  // Loop through samples (mono) or sample-pairs (stereo) required.
  // If stereo, we copy the 16bit sample for each l/r channel and do per
  // left/right channel processing on each sample for the pair. i needs
  // scaling when comparing to input sample count as that is always 16bit.
  for (uint i = 0; i != info->m_NumSamples; ++i)
  {
    const float updateFraction = info->m_NumSamples > 1 ? (i / (info->m_NumSamples - 1.0f)) : 0.0f;
    // linearly interpolate in the period, not the frequency
    float deltaPlaybackPos = 1.0f / Interpolate(origInvDeltaPlaybackPos, targetInvDeltaPlaybackPos, updateFraction);

    channel->mPlaybackPosF += deltaPlaybackPos;

    int inputPlaybackPos = (int) channel->mPlaybackPosF;

    if (inputPlaybackPos >= channel->mSound->mSoundSamples && !channel->mSound->mLoop)
    {
      info->m_EndSample = S3E_TRUE;
      break;
    }

    float inputPlayBackPosFrac = channel->mPlaybackPosF - (int) channel->mPlaybackPosF;

    inputPlaybackPos = inputPlaybackPos % channel->mSound->mSoundSamples;

    int inputPlaybackPosNext = (inputPlaybackPos + 1 ) % channel->mSound->mSoundSamples;

    IwAssert(ROWLHOUSE, inputPlaybackPos >= 0 && inputPlaybackPos < channel->mSound->mSoundSamples);
    IwAssert(ROWLHOUSE, inputPlaybackPosNext >= 0 && inputPlaybackPosNext < channel->mSound->mSoundSamples);

    // copy 16bit sample directly from input buffer
    int16_t yInput  = channel->mSound->mSoundData[inputPlaybackPos];
    int16_t yInputNext  = channel->mSound->mSoundData[inputPlaybackPosNext];

#ifdef USE_FILTER
    float origYF = Interpolate(float(yInput), float(yInputNext), inputPlayBackPosFrac);
    float yF = filterAmount * channel->mLastOutputLeftF + (1.0f - filterAmount) * origYF;
    channel->mLastOutputLeftF = origYF;
#else
    float yF = Interpolate(float(yInput), float(yInputNext), inputPlayBackPosFrac);
#endif

    float leftVol = Interpolate(origLeftVol, targetLeftVol, updateFraction);
    float rightVol = Interpolate(origRightVol, targetRightVol, updateFraction);

    if (info->m_Stereo)
    {
      int32_t yLeft = (int32_t) (yF * leftVol);
      int32_t yRight = (int32_t) (yF * rightVol);
      int16_t origYOutputLeft = info->m_Mix ? *target : 0;
      int16_t origYOutputRight =  info->m_Mix ? *(target+1) : 0;
      *target++ = ClipToInt16(yLeft + origYOutputLeft);
      *target++ = ClipToInt16(yRight + origYOutputRight);
    }
    else
    {
      float vol = (leftVol + rightVol) * 0.5f;
      int32_t y = (int32_t) (yF * vol);
      int16_t origYOutput = info->m_Mix ? *target : 0;
      *target++ = ClipToInt16(origYOutput + y);
    }

    ++outputSamplesPlayed;
  }

  IwAssert(ROWLHOUSE, channel->mPlaybackPosF == channel->mPlaybackPosF);
  if (channel->mSound->mLoop && channel->mPlaybackPosF >= numSoundSamplesF)
    channel->mPlaybackPosF -= numSoundSamplesF;
  IwAssert(ROWLHOUSE, channel->mPlaybackPosF == channel->mPlaybackPosF);

  // Inform s3e sound how many samples we played
  return outputSamplesPlayed;
}

//---------------------------------------------------------------------------------------------------------------------
int32_t AudioManager::AudioCallbackStereoSource(void* sys, void* user)
{
  AudioManager::Channel* channel = (AudioManager::Channel*) user;
  if (!channel->mSound->mStereo)
    return AudioCallbackMonoSource(sys, user);

  s3eSoundGenAudioInfo* info = (s3eSoundGenAudioInfo*)sys;

  info->m_EndSample = S3E_FALSE;
  int16_t* target = (int16_t*)info->m_Target;

  int outputSampleSize = info->m_Stereo ? 2 : 1;
  int outputSamplesPlayed = 0;

  // For stereo output, info->m_NumSamples is number of l/r pairs (each sample is 32bit)
  // info->m_OrigNumSamples always measures the total number of 16 bit samples,
  // regardless of whether input was mono or stereo.

  if ((!channel->mInUse || channel->mVolumeScale < 0.0001f) && channel->mSound->mLoop && info->m_Mix)
  {
    return info->m_NumSamples;
  }

  float origInvDeltaPlaybackPos = Maximum(gPlaybackFrequency / (channel->mSound->mSampleFrequency * channel->mCurrentFrequencyMultiplierF), 0.000001f);
  float targetInvDeltaPlaybackPos = Maximum(gPlaybackFrequency / (channel->mSound->mSampleFrequency * channel->mPlaybackFrequencyMultiplierF), 0.000001f);
  channel->mCurrentFrequencyMultiplierF = channel->mPlaybackFrequencyMultiplierF;

  float origLeftVol = channel->mCurrentLeftVolF;
  float origRightVol = channel->mCurrentRightVolF;
  float targetLeftVol = channel->mLeftVolF;
  float targetRightVol = channel->mRightVolF;
  channel->mCurrentLeftVolF = channel->mLeftVolF;
  channel->mCurrentRightVolF = channel->mRightVolF;

  float numSoundSamplesF = float(channel->mSound->mSoundSamples);
  IwAssert(ROWLHOUSE, channel->mPlaybackPosF >= 0.0f && channel->mPlaybackPosF == channel->mPlaybackPosF);

  // Loop through samples (mono) or sample-pairs (stereo) required.
  // If stereo, we copy the 16bit sample for each l/r channel and do per
  // left/right channel processing on each sample for the pair. i needs
  // scaling when comparing to input sample count as that is always 16bit.
  for (uint i = 0; i != info->m_NumSamples; ++i)
  {
    const float updateFraction = info->m_NumSamples > 1 ? (i / (info->m_NumSamples - 1.0f)) : 0.0f;
    // linearly interpolate in the period, not the frequency
    float deltaPlaybackPos = 1.0f / Interpolate(origInvDeltaPlaybackPos, targetInvDeltaPlaybackPos, updateFraction);

    channel->mPlaybackPosF += deltaPlaybackPos;

    int inputPlaybackPosLeft = ((int) channel->mPlaybackPosF) * 2;
    int inputPlaybackPosRight = 1 + inputPlaybackPosLeft;

    if (!channel->mSound->mLoop && inputPlaybackPosLeft >= channel->mSound->mSoundSamples/2)
    {
      info->m_EndSample = S3E_TRUE;
      break;
    }

    float inputPlayBackPosFrac = channel->mPlaybackPosF - (int) channel->mPlaybackPosF;

    inputPlaybackPosLeft = inputPlaybackPosLeft % channel->mSound->mSoundSamples;
    inputPlaybackPosRight = inputPlaybackPosRight % channel->mSound->mSoundSamples;

    int inputPlaybackPosLeftNext = (inputPlaybackPosLeft + 2 ) % channel->mSound->mSoundSamples;
    int inputPlaybackPosRightNext = (inputPlaybackPosRight + 2) % channel->mSound->mSoundSamples;

    IwAssert(ROWLHOUSE, inputPlaybackPosLeft >= 0 && inputPlaybackPosLeft < channel->mSound->mSoundSamples);
    IwAssert(ROWLHOUSE, inputPlaybackPosRight >= 0 && inputPlaybackPosRight < channel->mSound->mSoundSamples);

    // copy left (and right) 16bit sample directly from input buffer
    int16_t yInputLeft  = channel->mSound->mSoundData[inputPlaybackPosLeft];
    int16_t yInputRight = channel->mSound->mSoundData[inputPlaybackPosRight];

    int16_t yInputLeftNext  = channel->mSound->mSoundData[inputPlaybackPosLeftNext];
    int16_t yInputRightNext = channel->mSound->mSoundData[inputPlaybackPosRightNext];

#ifdef USE_FILTER
    float origYLeftF = Interpolate(float(yInputLeft), float(yInputLeftNext), inputPlayBackPosFrac);
    float origYRightF = Interpolate(float(yInputRight), float(yInputRightNext), inputPlayBackPosFrac);
    float yLeftF  = filterAmount * channel->mLastOutputLeftF + (1.0f - filterAmount) * origYLeftF;
    float yRightF = filterAmount * channel->mLastOutputRightF + (1.0f - filterAmount) * origYRightF;
    channel->mLastOutputLeftF = origYLeftF;
    channel->mLastOutputRightF = origYRightF;
#else
    float yLeftF = Interpolate(float(yInputLeft), float(yInputLeftNext), inputPlayBackPosFrac);
    float yRightF = Interpolate(float(yInputRight), float(yInputRightNext), inputPlayBackPosFrac);
#endif

    float leftVol = Interpolate(origLeftVol, targetLeftVol, updateFraction);
    float rightVol = Interpolate(origRightVol, targetRightVol, updateFraction);

    if (info->m_Stereo)
    {
      int32_t yLeft = (int32_t) (yLeftF * leftVol);
      int32_t yRight = (int32_t) (yRightF * rightVol);
      int16_t origYOutputLeft = info->m_Mix ? *target : 0;
      int16_t origYOutputRight =  info->m_Mix ? *(target+1) : 0;
      *target++ = ClipToInt16(yLeft + origYOutputLeft);
      *target++ = ClipToInt16(yRight + origYOutputRight);
    }
    else
    {
      float vol = (leftVol + rightVol) * 0.5f;
      int32_t y = (int32_t) ((yLeftF + yRightF) * vol);
      int16_t origYOutput = info->m_Mix ? *target : 0;
      *target++ = ClipToInt16(origYOutput + y);
    }

    ++outputSamplesPlayed;
  }

  IwAssert(ROWLHOUSE, channel->mPlaybackPosF == channel->mPlaybackPosF);
  if (channel->mSound->mLoop && channel->mPlaybackPosF >= numSoundSamplesF)
    channel->mPlaybackPosF -= numSoundSamplesF;
  IwAssert(ROWLHOUSE, channel->mPlaybackPosF == channel->mPlaybackPosF);

  //static int count = 0;
  //TRACE_FILE_IF(1) TRACE("%d Played %d stereo source samples on sound channel %p mix = %d stereo = %d buffer = %p, pos = %f deltaPlaybackPos = %f", 
  //  count++, outputSamplesPlayed, channel, info->m_Mix, info->m_Stereo, channel->mSound, channel->mPlaybackPosF, deltaPlaybackPos);

  // Inform s3e sound how many samples we played
  return outputSamplesPlayed;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::Init()
{
  TRACE_FILE_IF(1) TRACE("AudioManager::Init()");
  IwAssert(ROWLHOUSE, mInstance == 0);
  mInstance = new AudioManager;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::Terminate()
{
  TRACE_FILE_IF(1) TRACE("AudioManager::Terminate()");
  IwAssert(ROWLHOUSE, mInstance);
  delete mInstance;
  mInstance = 0;
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::AudioManager()
{
  mVolScale = 1.0f;

  float playbackFrequency = (float) s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ);
  if (playbackFrequency > 0.0f)
    gPlaybackFrequency = playbackFrequency;

  mTM.SetIdentity();
  mVelocity = Vector3(0,0,0);

  mNumAvailableChannels = s3eSoundGetInt(S3E_SOUND_NUM_CHANNELS);
  if (mNumAvailableChannels <= 0)
  {
    TRACE_FILE_IF(1) TRACE("No audio channels available\n");
    mNumAvailableChannels = 0;
    mChannels = 0;
  }
  else
  {
    mChannels = new Channel[mNumAvailableChannels];
    for (int i = 0 ; i != mNumAvailableChannels ; ++i)
    {
      mChannels[i].mInUse = false;
      mChannels[i].mFrequencyScale = 1.0f;
      mChannels[i].mVolumeScale = 0.0f;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::~AudioManager()
{
  StopAllChannels();

  for (Sounds::iterator it = mSounds.begin() ; it != mSounds.end() ; ++it)
  {
    Sound* sound = *it;
    delete sound;
  }

  delete [] mChannels;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::Update(float deltaTime)
{
  float playbackFrequency = (float) s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ);
  if (playbackFrequency > 0.0f)
    gPlaybackFrequency = playbackFrequency;

  for (int i = 0 ; i != mNumAvailableChannels ; ++i)
  {
    if (mChannels[i].mInUse)
      UpdateChannel(i, deltaTime);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::UpdateChannel(SoundChannel soundChannel, float deltaTime)
{
  Channel& channel = mChannels[soundChannel];
  IwAssert(ROWLHOUSE, channel.mInUse);

  bool setVolumeToZero = false;
  if (s3eSoundGetInt(S3E_SOUND_AVAILABLE) == 0)
    setVolumeToZero = true;
  else if (s3eSoundGetInt(S3E_SOUND_VOLUME) == 0)
    setVolumeToZero = true;

  if (setVolumeToZero)
  {
    channel.mVolumeScale = 0.0f;
    channel.mLeftVolF = channel.mRightVolF = 0.0f;
  }
  else if (channel.mUse3D)
  {
    Vector3 deltaToSource = channel.mSourcePosition - mTM.GetTrans();
    float distToSource = deltaToSource.GetLength();
    Vector3 dirToSource;
    if (distToSource > 0.0000001f)
      dirToSource = deltaToSource / distToSource;
    else
      dirToSource = mTM.RowX();

    // Fade in/out
    float deltaVol = (channel.mTargetVolumeScaleF * mVolScale) - channel.mVolumeScale;
    float maxVolChange = channel.mTargetVolumeScaleFRate * deltaTime;
    if (deltaVol > maxVolChange)
      deltaVol = maxVolChange;
    else if (deltaVol < -maxVolChange)
      deltaVol = -maxVolChange;
    // Needs atomic
    channel.mVolumeScale += deltaVol;

    // Volume drop off with distance
    // 1/r law for amplitude (squared for power)
    float amplitudeScale = distToSource > channel.mSoundSourceRadius ? 
      channel.mSoundSourceRadius / distToSource : 1.0f;
    float leftVol = channel.mVolumeScale * amplitudeScale;
    float rightVol = channel.mVolumeScale * amplitudeScale;

    // Left/right amplitude
    float sourceOnLeft = dirToSource.Dot(mTM.RowY());
    leftVol *= sqrtf(1.0f + sourceOnLeft);
    rightVol *= sqrtf(1.0f - sourceOnLeft);

    // Needs atomic
    channel.mLeftVolF = Minimum(leftVol, 1.0f);
    channel.mRightVolF = Minimum(rightVol, 1.0f);

    // Doppler. Listener looks along X
    float sourceSpeed = channel.mSourceVelocity.Dot(mTM.RowX());
    float listenerSpeed = mVelocity.Dot(mTM.RowX());
    const float c = 330.0f;
    const float maxSourceSpeed = c * 0.9f;
    if (fabsf(sourceSpeed) > maxSourceSpeed)
      sourceSpeed *= maxSourceSpeed / fabsf(sourceSpeed);

    // Needs atomic
    float freqScale = channel.mFrequencyScale * (c + listenerSpeed) / (c + sourceSpeed);
    IwAssert(ROWLHOUSE, freqScale > 0.0f);
    freqScale = Maximum(freqScale, 0.1f);
    channel.mPlaybackFrequencyMultiplierF = freqScale;

    TRACE_FILE_IF(4) TRACE("AudioManager::UpdateChannel(%d): vol = %f (%f, %f), freqMult = %f (l speed = %f, s speed = %f), pos = %f", 
      soundChannel,
      channel.mVolumeScale, channel.mLeftVolF, channel.mRightVolF,
      channel.mPlaybackFrequencyMultiplierF,
      listenerSpeed, sourceSpeed,
      channel.mPlaybackPosF);
  }
  else
  {
    // Fade in/out
    float deltaVol = (channel.mTargetVolumeScaleF * mVolScale) - channel.mVolumeScale;
    float maxVolChange = channel.mTargetVolumeScaleFRate * deltaTime;
    if (deltaVol > maxVolChange)
      deltaVol = maxVolChange;
    else if (deltaVol < -maxVolChange)
      deltaVol = -maxVolChange;
    channel.mVolumeScale += deltaVol;

    channel.mLeftVolF = channel.mVolumeScale;
    channel.mRightVolF = channel.mVolumeScale;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::SetAllChannelsToZeroVolume()
{
  TRACE_FILE_IF(1) TRACE("AudioManager::SetAllChannelsToZeroVolume()");
  for (int i = 0 ; i != mNumAvailableChannels ; ++i)
  {
    SetChannelTargetVolumeScale(i, 0.0f);
  }
}


//---------------------------------------------------------------------------------------------------------------------
void AudioManager::StopAllChannels()
{
  TRACE_FILE_IF(1) TRACE("AudioManager::StopAllChannels()");
  for (int i = 0 ; i != mNumAvailableChannels ; ++i)
  {
    ReleaseSoundChannel(i);
  }
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::Channel::Channel() 
{
  memset(this, 0, sizeof(*this));
  mCurrentFrequencyMultiplierF = 1.0f;
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::Sound::Sound(const char* soundFile, int sampleFrequency, bool stereo, bool loop, bool normaliseOnLoad)
  : mRefCount(1)
{
  TRACE_FILE_IF(1) TRACE("Sound::Sound(%s) = %p", soundFile, this);
  strncpy(mName, soundFile, sizeof(mName) / sizeof(mName[0]));
  mSampleFrequency = sampleFrequency;
  mStereo = stereo;
  mLoop = loop;
  mSoundData = 0;
  mNormaliseOnLoad = normaliseOnLoad;

  s3eFile *fileHandle = s3eFileOpen(soundFile, "rb");
  IwAssert(ROWLHOUSE, fileHandle);
  if (fileHandle)
  {
    int32_t fileNumBytes = s3eFileGetSize(fileHandle);
    TRACE_FILE_IF(1) TRACE("Reading sound file - %d bytes", fileNumBytes);
    mSoundSamples = fileNumBytes/2; // reading 16 bit values
    mSoundData = new int16_t[mSoundSamples];
    memset(mSoundData, 0, fileNumBytes);
    unsigned char* buffer = (unsigned char*) mSoundData;
    int32_t result = s3eFileRead(buffer, fileNumBytes, 1, fileHandle);
    if (result != 1)
    {
      s3eFileError error = s3eFileGetError();
      TRACE_FILE_IF(1) TRACE("Error loading %s error = %d", soundFile, error);
    }
    s3eFileClose(fileHandle);

    // Now apply a low pass filter to remove frequencies above half the Nyquist frequency
    int16_t nSamples = (int16_t) (0.5f + mSampleFrequency / gPlaybackFrequency);
    if (nSamples > 0)
    {
      int16_t *filteredSoundData = new int16_t[mSoundSamples];
      for (int32_t i = 0 ; i != mSoundSamples ; ++i)
      {
        int total = mSoundData[i];
        if (stereo)
        {
          for (int32_t j = 1 ; j != nSamples ; ++j)
          {
            total += mSoundData[(mSoundSamples + (i - j)*2) % mSoundSamples];
          }
        }
        else
        {
          for (int32_t j = 1 ; j != nSamples ; ++j)
          {
            total += mSoundData[(mSoundSamples + i - j) % mSoundSamples];
          }
        }
        total /= nSamples;
        filteredSoundData[i] = ClipToInt16(total);
      }
      std::swap(mSoundData, filteredSoundData);
      delete [] filteredSoundData;
    }

    // Normalise
    if (normaliseOnLoad)
    {
      int16_t maxLevel = 0;
      for (int32_t i = 0 ; i != mSoundSamples ; ++i)
      {
        if (mSoundData[i] > maxLevel)
          maxLevel = mSoundData[i];
        else if (mSoundData[i] < -maxLevel)
          maxLevel = -mSoundData[i];
      }
      if (maxLevel > 0)
      {
        float scale = std::numeric_limits<int16_t>::max() / (float) maxLevel;
        for (int32_t i = 0 ; i != mSoundSamples ; ++i)
        {
          mSoundData[i] = ClipToInt16((int) (mSoundData[i] * scale));
        }
      }
    }

    static bool output = false;
    if (output)
    {
      int32_t maxSamples = 2048;
      for (int32_t i = 0 ; i != std::min(mSoundSamples, maxSamples) ; ++i)
      {
        int16_t left = mSoundData[i];
        if (stereo)
        {
          int16_t right = mSoundData[++i];
          TRACE_FILE_IF(1) TRACE("%d %d", left, right);
        }
        else
        {
          TRACE_FILE_IF(1) TRACE("%d", left);
        }
      }
    }
    output = false;

  }
  else
  {
    TRACE_FILE_IF(1) TRACE("Failed to open sound file");
  }
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::Sound::~Sound()
{
  TRACE_FILE_IF(1) TRACE("Sound::~Sound()");
  delete [] mSoundData;
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::Sounds::iterator AudioManager::FindLoadedSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop)
{
  TRACE_FILE_IF(1) TRACE("AudioManager::FindLoadedSound %s", soundFile);
  for (Sounds::iterator it = mSounds.begin() ; it != mSounds.end() ; ++it)
  {
    Sound* sound = *it;
    // For now demand an exact match - may not be necessary really
    if (
      strcmp(sound->mName, soundFile) == 0 &&
      sampleFrequency == sound->mSampleFrequency &&
      stereo == sound->mStereo &&
      loop == sound->mLoop
      )
    {
      TRACE_FILE_IF(1) TRACE("Found sound");
      return it;
    }
  }
  TRACE_FILE_IF(1) TRACE("Didn't find already loaded sound");
  return mSounds.end();
}


//---------------------------------------------------------------------------------------------------------------------
AudioManager::Sound* AudioManager::LoadSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop, bool normalise)
{
  TRACE_FILE_IF(1) TRACE("AudioManager::LoadSound %s", soundFile);
  Sounds::iterator it = FindLoadedSound(soundFile, sampleFrequency, stereo, loop);
  
  if (it != mSounds.end())
  {
    Sound* sound = *it;
    ++sound->mRefCount;
    return sound;
  }
    
  Sound* sound = new Sound(soundFile, sampleFrequency, stereo, loop, normalise);

  if (sound->mSoundData)
  {
    mSounds.push_back(sound);
    return sound;
  }
  else
  {
    delete sound;
    return 0;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::UnloadSound(Sound* sound)
{
  IwAssert(ROWLHOUSE, sound);
  if (!sound)
    return;

  for (Sounds::iterator it = mSounds.begin() ; it != mSounds.end() ; ++it)
  {
    if (*it == sound)
    {
      --sound->mRefCount;

      if (sound->mRefCount == 0)
      {
        delete sound;
        mSounds.erase(it);
      }
      return;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
AudioManager::SoundChannel AudioManager::AllocateSoundChannel(float soundSourceRadius, bool use3D)
{
  TRACE_FILE_IF(1) TRACE("AudioManager::AllocateSoundChannel");
  // Don't use s3eSoundGetFreeChannel as it will not work for one-shot sounds
  SoundChannel soundChannel = -1;
  for (SoundChannel i = 0 ; i != mNumAvailableChannels ; ++i)
  {
    if (mChannels[i].mInUse == false)
    {
      soundChannel = i;
      break;
    }
  }
  if (soundChannel == -1)
  {
    TRACE_FILE_IF(1) TRACE("Failed to get a sound channel");
    return -1;
  }
  IwAssert(ROWLHOUSE, soundChannel < mNumAvailableChannels);

  // Prepare our data
  Channel& channel = mChannels[soundChannel];
  IwAssert(ROWLHOUSE, channel.mInUse == false);
  channel.mInUse = true;
  channel.mUse3D = use3D;
  channel.mPlaybackPosF = 0.0f;
  channel.mSound = 0;
  channel.mSoundSourceRadius = soundSourceRadius;
  channel.mVolumeScale = 0.0f;
  channel.mFrequencyScale = 1.0f;
  channel.mVolumeScale = 0.0f;
  channel.mLeftVolF = 0.0f;
  channel.mRightVolF = 0.0f;
  channel.mSourcePosition = Vector3(0,0,0);
  channel.mPlaybackFrequencyMultiplierF = 1.0f;
  SetChannelTargetVolumeScale(soundChannel, 1.0f);
  SetChannelPositionAndVelocity(soundChannel, Vector3(0,0,0), Vector3(0,0,0));
  UpdateChannel(soundChannel, 0.0f);

  // Set the volume to 0 so that no click is audible after unregistering the callbacks later
  s3eSoundChannelSetInt(soundChannel, S3E_CHANNEL_VOLUME, 0);

  s3eSoundChannelRegister(soundChannel, S3E_CHANNEL_GEN_AUDIO, AudioCallbackStereoSource, &channel);
  s3eSoundChannelRegister(soundChannel, S3E_CHANNEL_GEN_AUDIO_STEREO, AudioCallbackStereoSource, &channel);

  TRACE_FILE_IF(1) TRACE("Sound channel %p = %d", &channel, soundChannel);

  return soundChannel;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::ReleaseSoundChannel(AudioManager::SoundChannel soundChannel)
{
  TRACE_FILE_IF(1) TRACE("AudioManager::ReleaseSoundChannel(%d)", soundChannel);
  IwAssert(ROWLHOUSE, soundChannel >= 0 && soundChannel < mNumAvailableChannels);

  if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    return;

  Channel& channel = mChannels[soundChannel];
  if (channel.mInUse)
  {
    channel.mInUse = false;

    s3eSoundChannelStop(soundChannel);

    // Delay in order to (hopefully) let the callback flush through
    s3eDeviceYield(100);
  }
  else
  {
    TRACE_FILE_IF(1) TRACE("Channel not in use");
  }
}

//---------------------------------------------------------------------------------------------------------------------
// Note that one-shot sounds may be started multiple times
void AudioManager::StartSoundOnChannel(SoundChannel soundChannel, Sound* sound, bool loop)
{
  TRACE_FILE_IF(1) TRACE("AudioManager::StartSoundOnChannel(%d, %s, %p) sound = %p", 
    soundChannel, sound->mName, &mChannels[soundChannel], sound);
  IwAssert(ROWLHOUSE, soundChannel >= 0);

  Channel& channel = mChannels[soundChannel];
  IwAssert(ROWLHOUSE, channel.mInUse);
  channel.mSound = sound;
  channel.mPlaybackPosF = 0.0f;
  channel.mVolumeScale = 0.0f;
  channel.mLeftVolF = 0;
  channel.mRightVolF = 0;
  channel.mPlaybackFrequencyMultiplierF = 1.0f;

  channel.mCurrentFrequencyMultiplierF = 1.0f;
  channel.mCurrentLeftVolF = 0.0f;
  channel.mCurrentRightVolF = 0.0f;
  channel.mLastOutputLeftF = 0.0f;
  channel.mLastOutputRightF = 0.0f;

  s3eSoundChannelPlay(soundChannel, channel.mSound->mSoundData, channel.mSound->mSoundSamples, loop ? 0 : 1, 0);
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::SetChannelPositionAndVelocity(SoundChannel soundChannel, const Vector3& pos, const Vector3& velocity)
{
  Channel& channel = mChannels[soundChannel];
  channel.mSourcePosition = pos;
  channel.mSourceVelocity = velocity;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::SetChannelFrequencyScale(SoundChannel soundChannel, float freqScale)
{
  freqScale = Maximum(freqScale, FLT_MIN);
  Channel& channel = mChannels[soundChannel];
  channel.mFrequencyScale = freqScale;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::SetChannelVolumeScale(SoundChannel soundChannel, float volScale)
{
  volScale = Maximum(volScale, 0.0f);
  Channel& channel = mChannels[soundChannel];
  channel.mVolumeScale = volScale;
  channel.mTargetVolumeScaleF = volScale;
  if (!channel.mUse3D)
    channel.mLeftVolF = channel.mRightVolF = volScale;
}

//---------------------------------------------------------------------------------------------------------------------
void AudioManager::SetChannelTargetVolumeScale(SoundChannel soundChannel, float volScale, float volScaleRate)
{
  volScale = Maximum(volScale, 0.0f);
  Channel& channel = mChannels[soundChannel];
  channel.mTargetVolumeScaleF = volScale;
  channel.mTargetVolumeScaleFRate = volScaleRate;
}
