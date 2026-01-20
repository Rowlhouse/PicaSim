#include "AudioManager.h"
#include "Trace.h"

#include <limits>
#include <cstring>
#include <algorithm>
#include <cstdio>

AudioManager* AudioManager::mInstance = 0;

//======================================================================================================================
static int16 ClipToInt16(int32 sval)
{
    if (sval > INT16_MAX)
        return INT16_MAX;
    if (sval < INT16_MIN)
        return INT16_MIN;
    return (int16)sval;
}

//======================================================================================================================
void AudioManager::Init()
{
    TRACE_FILE_IF(1) TRACE("AudioManager::Init()");
    IwAssert(ROWLHOUSE, mInstance == 0);
    mInstance = new AudioManager;
}

//======================================================================================================================
void AudioManager::Terminate()
{
    TRACE_FILE_IF(1) TRACE("AudioManager::Terminate()");
    IwAssert(ROWLHOUSE, mInstance);
    delete mInstance;
    mInstance = 0;
}

//======================================================================================================================
AudioManager::AudioManager()
    : mALDevice(nullptr)
    , mALContext(nullptr)
    , mNumAvailableChannels(0)
    , mChannels(nullptr)
    , mVolScale(1.0f)
{
    mTM.SetIdentity();
    mVelocity = Vector3(0,0,0);

    // Open default audio device
    mALDevice = alcOpenDevice(nullptr);
    if (!mALDevice)
    {
        TRACE_FILE_IF(1) TRACE("Failed to open OpenAL device");
        return;
    }

    // Create and activate context
    mALContext = alcCreateContext(mALDevice, nullptr);
    if (!mALContext)
    {
        TRACE_FILE_IF(1) TRACE("Failed to create OpenAL context");
        alcCloseDevice(mALDevice);
        mALDevice = nullptr;
        return;
    }
    alcMakeContextCurrent(mALContext);

    // Configure distance model to match original 1/r attenuation
    // AL_INVERSE_DISTANCE_CLAMPED: gain = ref_dist / (ref_dist + rolloff * (dist - ref_dist))
    // With rolloff=1 and ref_dist=soundSourceRadius, this approximates 1/r for dist > ref_dist
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    // Configure Doppler effect to match original 330 m/s speed of sound
    alSpeedOfSound(330.0f);
    alDopplerFactor(1.0f);

    // Pre-allocate channels (OpenAL sources)
    mNumAvailableChannels = 32;
    mChannels = new Channel[mNumAvailableChannels];

    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        alGenSources(1, &mChannels[i].mALSource);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            TRACE_FILE_IF(1) TRACE("Failed to create OpenAL source %d, error %d", i, error);
            mNumAvailableChannels = i;
            break;
        }
        mChannels[i].mInUse = false;
        mChannels[i].mFrequencyScale = 1.0f;
        mChannels[i].mVolumeScale = 0.0f;
    }

    TRACE_FILE_IF(1) TRACE("AudioManager initialized with %d channels", mNumAvailableChannels);
}

//======================================================================================================================
AudioManager::~AudioManager()
{
    StopAllChannels();

    // Delete all OpenAL sources
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        if (mChannels[i].mALSource != 0)
        {
            alDeleteSources(1, &mChannels[i].mALSource);
        }
    }
    delete[] mChannels;

    // Delete all sounds (and their OpenAL buffers)
    for (Sounds::iterator it = mSounds.begin(); it != mSounds.end(); ++it)
    {
        Sound* sound = *it;
        delete sound;
    }

    // Destroy OpenAL context and close device
    if (mALContext)
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(mALContext);
    }
    if (mALDevice)
    {
        alcCloseDevice(mALDevice);
    }
}

//======================================================================================================================
void AudioManager::Update(float deltaTime)
{
    if (!mALContext)
        return;

    // Update listener position and orientation
    Vector3 listenerPos = mTM.GetTrans();
    Vector3 listenerVel = mVelocity;
    Vector3 listenerFwd = mTM.RowX();  // Forward direction
    Vector3 listenerUp = mTM.RowZ();   // Up direction

    alListener3f(AL_POSITION, listenerPos.x, listenerPos.y, listenerPos.z);
    alListener3f(AL_VELOCITY, listenerVel.x, listenerVel.y, listenerVel.z);

    ALfloat orientation[6] = {
        listenerFwd.x, listenerFwd.y, listenerFwd.z,  // "at" vector
        listenerUp.x, listenerUp.y, listenerUp.z      // "up" vector
    };
    alListenerfv(AL_ORIENTATION, orientation);

    // Update each active channel
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        if (mChannels[i].mInUse)
            UpdateChannel(i, deltaTime);
    }
}

//======================================================================================================================
void AudioManager::UpdateChannel(SoundChannel soundChannel, float deltaTime)
{
    Channel& channel = mChannels[soundChannel];
    IwAssert(ROWLHOUSE, channel.mInUse);

    // Volume ramping - smooth transition to target volume
    float targetVol = channel.mTargetVolumeScaleF * mVolScale;
    float deltaVol = targetVol - channel.mVolumeScale;
    float maxVolChange = channel.mTargetVolumeScaleFRate * deltaTime;
    if (deltaVol > maxVolChange)
        deltaVol = maxVolChange;
    else if (deltaVol < -maxVolChange)
        deltaVol = -maxVolChange;
    channel.mVolumeScale += deltaVol;

    // Apply volume to OpenAL source
    alSourcef(channel.mALSource, AL_GAIN, channel.mVolumeScale);

    // Apply pitch (frequency scale)
    // OpenAL pitch range is typically 0.5 to 2.0, but we allow wider range
    float pitch = Maximum(channel.mFrequencyScale, 0.1f);
    alSourcef(channel.mALSource, AL_PITCH, pitch);

    // Update 3D position and velocity if using 3D audio
    if (channel.mUse3D)
    {
        Vector3 pos = channel.mSourcePosition;
        Vector3 vel = channel.mSourceVelocity;
        alSource3f(channel.mALSource, AL_POSITION, pos.x, pos.y, pos.z);
        alSource3f(channel.mALSource, AL_VELOCITY, vel.x, vel.y, vel.z);
    }

    TRACE_FILE_IF(4) TRACE("AudioManager::UpdateChannel(%d): vol = %f, freq = %f",
        soundChannel, channel.mVolumeScale, channel.mFrequencyScale);
}

//======================================================================================================================
void AudioManager::SetAllChannelsToZeroVolume()
{
    TRACE_FILE_IF(1) TRACE("AudioManager::SetAllChannelsToZeroVolume()");
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        SetChannelTargetVolumeScale(i, 0.0f);
    }
}

//======================================================================================================================
void AudioManager::StopAllChannels()
{
    TRACE_FILE_IF(1) TRACE("AudioManager::StopAllChannels()");
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        ReleaseSoundChannel(i);
    }
}

//======================================================================================================================
AudioManager::Channel::Channel()
    : mInUse(false)
    , mUse3D(false)
    , mSound(nullptr)
    , mALSource(0)
    , mVolumeScale(0.0f)
    , mTargetVolumeScaleF(1.0f)
    , mTargetVolumeScaleFRate(2.0f)
    , mFrequencyScale(1.0f)
    , mSoundSourceRadius(1.0f)
    , mSourcePosition(0,0,0)
    , mSourceVelocity(0,0,0)
{
}

//======================================================================================================================
AudioManager::Sound::Sound(const char* soundFile, int sampleFrequency, bool stereo, bool loop, bool normaliseOnLoad)
    : mRefCount(1)
    , mSoundData(nullptr)
    , mSoundSamples(0)
    , mALBuffer(0)
{
    TRACE_FILE_IF(1) TRACE("Sound::Sound(%s) = %p", soundFile, this);
    strncpy(mName, soundFile, sizeof(mName) / sizeof(mName[0]));
    mName[sizeof(mName) - 1] = '\0';
    mSampleFrequency = sampleFrequency;
    mStereo = stereo;
    mLoop = loop;
    mNormaliseOnLoad = normaliseOnLoad;

    // Load raw PCM file
    FILE* file = fopen(soundFile, "rb");
    if (!file)
    {
        TRACE_FILE_IF(1) TRACE("Failed to open sound file: %s", soundFile);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileNumBytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    TRACE_FILE_IF(1) TRACE("Reading sound file - %ld bytes", fileNumBytes);
    mSoundSamples = (int)(fileNumBytes / 2); // reading 16 bit values
    mSoundData = new int16[mSoundSamples];

    size_t bytesRead = fread(mSoundData, 1, fileNumBytes, file);
    fclose(file);

    if (bytesRead != (size_t)fileNumBytes)
    {
        TRACE_FILE_IF(1) TRACE("Error reading sound file: %s", soundFile);
        delete[] mSoundData;
        mSoundData = nullptr;
        mSoundSamples = 0;
        return;
    }

    // Normalize if requested
    if (normaliseOnLoad)
    {
        int16 maxLevel = 0;
        for (int i = 0; i < mSoundSamples; ++i)
        {
            int16 absVal = mSoundData[i] < 0 ? -mSoundData[i] : mSoundData[i];
            if (absVal > maxLevel)
                maxLevel = absVal;
        }
        if (maxLevel > 0)
        {
            float scale = (float)INT16_MAX / (float)maxLevel;
            for (int i = 0; i < mSoundSamples; ++i)
            {
                mSoundData[i] = ClipToInt16((int)(mSoundData[i] * scale));
            }
        }
    }

    // Create OpenAL buffer
    alGenBuffers(1, &mALBuffer);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        TRACE_FILE_IF(1) TRACE("Failed to create OpenAL buffer, error %d", error);
        delete[] mSoundData;
        mSoundData = nullptr;
        mSoundSamples = 0;
        return;
    }

    // Determine format
    ALenum format = stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

    // Upload data to OpenAL buffer
    alBufferData(mALBuffer, format, mSoundData, mSoundSamples * sizeof(int16), sampleFrequency);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        TRACE_FILE_IF(1) TRACE("Failed to upload audio data to OpenAL buffer, error %d", error);
        alDeleteBuffers(1, &mALBuffer);
        mALBuffer = 0;
        delete[] mSoundData;
        mSoundData = nullptr;
        mSoundSamples = 0;
        return;
    }

    TRACE_FILE_IF(1) TRACE("Created OpenAL buffer %u for %s (%d samples, %d Hz, %s)",
        mALBuffer, soundFile, mSoundSamples, sampleFrequency, stereo ? "stereo" : "mono");
}

//======================================================================================================================
AudioManager::Sound::~Sound()
{
    TRACE_FILE_IF(1) TRACE("Sound::~Sound()");
    if (mALBuffer != 0)
    {
        alDeleteBuffers(1, &mALBuffer);
    }
    delete[] mSoundData;
}

//======================================================================================================================
AudioManager::Sounds::iterator AudioManager::FindLoadedSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop)
{
    TRACE_FILE_IF(1) TRACE("AudioManager::FindLoadedSound %s", soundFile);
    for (Sounds::iterator it = mSounds.begin(); it != mSounds.end(); ++it)
    {
        Sound* sound = *it;
        if (strcmp(sound->mName, soundFile) == 0 &&
                sampleFrequency == sound->mSampleFrequency &&
                stereo == sound->mStereo &&
                loop == sound->mLoop)
        {
            TRACE_FILE_IF(1) TRACE("Found sound");
            return it;
        }
    }
    TRACE_FILE_IF(1) TRACE("Didn't find already loaded sound");
    return mSounds.end();
}

//======================================================================================================================
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

    if (sound->mALBuffer != 0)
    {
        mSounds.push_back(sound);
        return sound;
    }
    else
    {
        delete sound;
        return nullptr;
    }
}

//======================================================================================================================
void AudioManager::UnloadSound(Sound* sound)
{
    IwAssert(ROWLHOUSE, sound);
    if (!sound)
        return;

    for (Sounds::iterator it = mSounds.begin(); it != mSounds.end(); ++it)
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

//======================================================================================================================
AudioManager::SoundChannel AudioManager::AllocateSoundChannel(float soundSourceRadius, bool use3D)
{
    TRACE_FILE_IF(1) TRACE("AudioManager::AllocateSoundChannel");

    // Find a free channel
    SoundChannel soundChannel = -1;
    for (SoundChannel i = 0; i < mNumAvailableChannels; ++i)
    {
        if (!mChannels[i].mInUse)
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

    // Initialize channel
    Channel& channel = mChannels[soundChannel];
    IwAssert(ROWLHOUSE, channel.mInUse == false);
    channel.mInUse = true;
    channel.mUse3D = use3D;
    channel.mSound = nullptr;
    channel.mSoundSourceRadius = soundSourceRadius;
    channel.mVolumeScale = 0.0f;
    channel.mTargetVolumeScaleF = 1.0f;
    channel.mTargetVolumeScaleFRate = 2.0f;
    channel.mFrequencyScale = 1.0f;
    channel.mSourcePosition = Vector3(0,0,0);
    channel.mSourceVelocity = Vector3(0,0,0);

    // Configure OpenAL source for 3D or 2D mode
    ALuint source = channel.mALSource;
    if (use3D)
    {
        // 3D sound: position in world space
        alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
        alSourcef(source, AL_REFERENCE_DISTANCE, soundSourceRadius);
        alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
        alSourcef(source, AL_MAX_DISTANCE, 10000.0f);
    }
    else
    {
        // 2D sound: position relative to listener (at origin = listener position)
        alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    }

    alSourcef(source, AL_GAIN, 0.0f);
    alSourcef(source, AL_PITCH, 1.0f);

    TRACE_FILE_IF(1) TRACE("Allocated sound channel %d", soundChannel);
    return soundChannel;
}

//======================================================================================================================
void AudioManager::ReleaseSoundChannel(AudioManager::SoundChannel soundChannel)
{
    TRACE_FILE_IF(1) TRACE("AudioManager::ReleaseSoundChannel(%d)", soundChannel);

    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: ReleaseSoundChannel called with invalid channel %d", soundChannel);
        return;
    }

    Channel& channel = mChannels[soundChannel];
    if (channel.mInUse)
    {
        channel.mInUse = false;

        // Stop playback and detach buffer
        alSourceStop(channel.mALSource);
        alSourcei(channel.mALSource, AL_BUFFER, 0);
    }
    else
    {
        TRACE_FILE_IF(1) TRACE("Channel not in use");
    }
}

//======================================================================================================================
void AudioManager::StartSoundOnChannel(SoundChannel soundChannel, Sound* sound, bool loop)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: StartSoundOnChannel called with invalid channel %d", soundChannel);
        return;
    }
    TRACE_FILE_IF(1) TRACE("AudioManager::StartSoundOnChannel(%d, %s) sound = %p",
        soundChannel, sound->mName, sound);
    IwAssert(ROWLHOUSE, soundChannel >= 0);

    Channel& channel = mChannels[soundChannel];
    IwAssert(ROWLHOUSE, channel.mInUse);
    channel.mSound = sound;
    channel.mVolumeScale = 0.0f;

    ALuint source = channel.mALSource;

    // Stop any current playback
    alSourceStop(source);

    // Attach buffer to source
    alSourcei(source, AL_BUFFER, sound->mALBuffer);

    // Set looping mode
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);

    // Reset gain and pitch
    alSourcef(source, AL_GAIN, 0.0f);
    alSourcef(source, AL_PITCH, 1.0f);

    // Start playback
    alSourcePlay(source);
}

//======================================================================================================================
void AudioManager::PauseSoundOnChannel(SoundChannel soundChannel)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: PauseSoundOnChannel called with invalid channel %d", soundChannel);
        return;
    }
    alSourcePause(mChannels[soundChannel].mALSource);
}

//======================================================================================================================
bool AudioManager::IsSoundPlayingOnChannel(SoundChannel soundChannel)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
        return false;

    ALint state;
    alGetSourcei(mChannels[soundChannel].mALSource, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

//======================================================================================================================
void AudioManager::SetChannelPositionAndVelocity(SoundChannel soundChannel, const Vector3& pos, const Vector3& velocity)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: SetChannelPositionAndVelocity called with invalid channel %d", soundChannel);
        return;
    }
    Channel& channel = mChannels[soundChannel];
    channel.mSourcePosition = pos;
    channel.mSourceVelocity = velocity;
    // Actual OpenAL update happens in UpdateChannel()
}

//======================================================================================================================
void AudioManager::SetChannelFrequencyScale(SoundChannel soundChannel, float freqScale)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: SetChannelFrequencyScale called with invalid channel %d", soundChannel);
        return;
    }
    freqScale = Maximum(freqScale, 0.1f);
    Channel& channel = mChannels[soundChannel];
    channel.mFrequencyScale = freqScale;
}

//======================================================================================================================
void AudioManager::SetChannelVolumeScale(SoundChannel soundChannel, float volScale)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: SetChannelVolumeScale called with invalid channel %d", soundChannel);
        return;
    }
    volScale = Maximum(volScale, 0.0f);
    Channel& channel = mChannels[soundChannel];
    channel.mVolumeScale = volScale;
    channel.mTargetVolumeScaleF = volScale;
    // Immediately apply to OpenAL
    alSourcef(channel.mALSource, AL_GAIN, volScale * mVolScale);
}

//======================================================================================================================
void AudioManager::SetChannelTargetVolumeScale(SoundChannel soundChannel, float volScale, float volScaleRate)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(1) TRACE("WARNING: SetChannelTargetVolumeScale called with invalid channel %d", soundChannel);
        return;
    }
    volScale = Maximum(volScale, 0.0f);
    Channel& channel = mChannels[soundChannel];
    channel.mTargetVolumeScaleF = volScale;
    channel.mTargetVolumeScaleFRate = volScaleRate;
}
