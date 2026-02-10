#include "AudioManager.h"
#include "Trace.h"

#include <limits>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

std::unique_ptr<AudioManager> AudioManager::mInstance;

// Free functions callable from Platform code without pulling in OpenAL headers
void PauseAudioDevice()
{
    if (AudioManager::IsAvailable())
        AudioManager::GetInstance().PauseDevice();
}

void ResumeAudioDevice()
{
    if (AudioManager::IsAvailable())
        AudioManager::GetInstance().ResumeDevice();
}

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
    TRACE_FILE_IF(ONCE_1) TRACE("AudioManager::Init()");
    IwAssert(ROWLHOUSE, mInstance == nullptr);
    mInstance.reset(new AudioManager);
}

//======================================================================================================================
void AudioManager::Terminate()
{
    TRACE_FILE_IF(ONCE_1) TRACE("AudioManager::Terminate()");
    IwAssert(ROWLHOUSE, mInstance);
    mInstance.reset();
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

    // Get and save default device name
    const ALCchar* defaultDeviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    if (defaultDeviceName)
    {
        mDefaultDeviceName = defaultDeviceName;
        TRACE_FILE_IF(ONCE_1) TRACE("Default audio device: %s", defaultDeviceName);
    }

#ifdef PICASIM_ANDROID
    // Point OpenAL-Soft to our config file (CWD is the extracted assets directory)
    setenv("ALSOFT_CONF", "alsoft.conf", 1);
#endif

    // Open default audio device
    mALDevice = alcOpenDevice(nullptr);
    if (!mALDevice)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to open OpenAL device");
        return;
    }

    // Store current device name
    const ALCchar* actualDeviceName = alcGetString(mALDevice, ALC_DEVICE_SPECIFIER);
    if (actualDeviceName)
    {
        mCurrentDeviceName = actualDeviceName;
    }

    // Create and activate context
    mALContext = alcCreateContext(mALDevice, nullptr);
    if (!mALContext)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to create OpenAL context");
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
    mChannels = std::make_unique<Channel[]>(mNumAvailableChannels);

    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        alGenSources(1, &mChannels[i].mALSource);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("Failed to create OpenAL source %d, error %d", i, error);
            mNumAvailableChannels = i;
            break;
        }
        mChannels[i].mInUse = false;
        mChannels[i].mFrequencyScale = 1.0f;
        mChannels[i].mVolumeScale = 0.0f;
    }

    TRACE_FILE_IF(ONCE_1) TRACE("AudioManager initialized with %d channels", mNumAvailableChannels);
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
    mChannels.reset();

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
void AudioManager::PauseDevice()
{
    if (mALDevice)
    {
        auto pauseFunc = reinterpret_cast<void(*)(ALCdevice*)>(
            alcGetProcAddress(mALDevice, "alcDevicePauseSOFT"));
        if (pauseFunc)
            pauseFunc(mALDevice);
    }
}

//======================================================================================================================
void AudioManager::ResumeDevice()
{
    if (mALDevice)
    {
        auto resumeFunc = reinterpret_cast<void(*)(ALCdevice*)>(
            alcGetProcAddress(mALDevice, "alcDeviceResumeSOFT"));
        if (resumeFunc)
            resumeFunc(mALDevice);
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

    TRACE_FILE_IF(FRAME_1) TRACE("AudioManager::UpdateChannel(%d): vol = %f, freq = %f",
        soundChannel, channel.mVolumeScale, channel.mFrequencyScale);
}

//======================================================================================================================
void AudioManager::SetAllChannelsToZeroVolume()
{
    TRACE_FILE_IF(ONCE_2) TRACE("AudioManager::SetAllChannelsToZeroVolume()");
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        SetChannelTargetVolumeScale(i, 0.0f);
    }
}

//======================================================================================================================
void AudioManager::StopAllChannels()
{
    TRACE_FILE_IF(ONCE_2) TRACE("AudioManager::StopAllChannels()");
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
    TRACE_FILE_IF(ONCE_2) TRACE("Sound::Sound(%s) = %p", soundFile, this);
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
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to open sound file: %s", soundFile);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileNumBytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    TRACE_FILE_IF(ONCE_2) TRACE("Reading sound file - %ld bytes", fileNumBytes);
    mSoundSamples = (int)(fileNumBytes / 2); // reading 16 bit values
    mSoundData = new int16[mSoundSamples];

    size_t bytesRead = fread(mSoundData, 1, fileNumBytes, file);
    fclose(file);

    if (bytesRead != (size_t)fileNumBytes)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Error reading sound file: %s", soundFile);
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
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to create OpenAL buffer, error %d", error);
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
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to upload audio data to OpenAL buffer, error %d", error);
        alDeleteBuffers(1, &mALBuffer);
        mALBuffer = 0;
        delete[] mSoundData;
        mSoundData = nullptr;
        mSoundSamples = 0;
        return;
    }

    TRACE_FILE_IF(ONCE_2) TRACE("Created OpenAL buffer %u for %s (%d samples, %d Hz, %s)",
        mALBuffer, soundFile, mSoundSamples, sampleFrequency, stereo ? "stereo" : "mono");
}

//======================================================================================================================
AudioManager::Sound::~Sound()
{
    TRACE_FILE_IF(ONCE_2) TRACE("Sound::~Sound()");
    if (mALBuffer != 0)
    {
        alDeleteBuffers(1, &mALBuffer);
    }
    delete[] mSoundData;
}

//======================================================================================================================
AudioManager::Sounds::iterator AudioManager::FindLoadedSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop)
{
    TRACE_FILE_IF(ONCE_3) TRACE("AudioManager::FindLoadedSound %s", soundFile);
    for (Sounds::iterator it = mSounds.begin(); it != mSounds.end(); ++it)
    {
        Sound* sound = *it;
        if (strcmp(sound->mName, soundFile) == 0 &&
                sampleFrequency == sound->mSampleFrequency &&
                stereo == sound->mStereo &&
                loop == sound->mLoop)
        {
            TRACE_FILE_IF(ONCE_3) TRACE("Found sound");
            return it;
        }
    }
    TRACE_FILE_IF(ONCE_3) TRACE("Didn't find already loaded sound");
    return mSounds.end();
}

//======================================================================================================================
AudioManager::Sound* AudioManager::LoadSound(const char* soundFile, int sampleFrequency, bool stereo, bool loop, bool normalise)
{
    TRACE_FILE_IF(ONCE_2) TRACE("AudioManager::LoadSound %s", soundFile);
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
    TRACE_FILE_IF(ONCE_2) TRACE("AudioManager::AllocateSoundChannel");

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
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to get a sound channel");
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

    TRACE_FILE_IF(ONCE_2) TRACE("Allocated sound channel %d", soundChannel);
    return soundChannel;
}

//======================================================================================================================
void AudioManager::ReleaseSoundChannel(AudioManager::SoundChannel soundChannel)
{
    TRACE_FILE_IF(ONCE_2) TRACE("AudioManager::ReleaseSoundChannel(%d)", soundChannel);

    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: ReleaseSoundChannel called with invalid channel %d", soundChannel);
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
        TRACE_FILE_IF(ONCE_3) TRACE("Channel not in use");
    }
}

//======================================================================================================================
void AudioManager::StartSoundOnChannel(SoundChannel soundChannel, Sound* sound, bool loop)
{
    if (soundChannel < 0 || soundChannel >= mNumAvailableChannels)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: StartSoundOnChannel called with invalid channel %d", soundChannel);
        return;
    }
    TRACE_FILE_IF(ONCE_2) TRACE("AudioManager::StartSoundOnChannel(%d, %s) sound = %p",
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
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: PauseSoundOnChannel called with invalid channel %d", soundChannel);
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
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: SetChannelPositionAndVelocity called with invalid channel %d", soundChannel);
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
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: SetChannelFrequencyScale called with invalid channel %d", soundChannel);
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
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: SetChannelVolumeScale called with invalid channel %d", soundChannel);
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
        TRACE_FILE_IF(ONCE_1) TRACE("WARNING: SetChannelTargetVolumeScale called with invalid channel %d", soundChannel);
        return;
    }
    volScale = Maximum(volScale, 0.0f);
    Channel& channel = mChannels[soundChannel];
    channel.mTargetVolumeScaleF = volScale;
    channel.mTargetVolumeScaleFRate = volScaleRate;
}

//======================================================================================================================
std::vector<std::string> AudioManager::EnumerateAudioDevices()
{
    std::vector<std::string> devices;

    // Try the "enumerate all" extension first - this gives us actual device names on Windows
    // ALC_ENUMERATE_ALL_EXT provides full device names like "Speakers (Realtek)" or "Headphones (Rift Audio)"
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT"))
    {
        const ALCchar* deviceList = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
        if (deviceList)
        {
            // Parse null-separated list
            while (*deviceList)
            {
                devices.push_back(deviceList);
                deviceList += strlen(deviceList) + 1;
            }
            return devices;
        }
    }

    // Fall back to basic enumeration
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT"))
    {
        const ALCchar* deviceList = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        if (deviceList)
        {
            while (*deviceList)
            {
                devices.push_back(deviceList);
                deviceList += strlen(deviceList) + 1;
            }
            return devices;
        }
    }

    TRACE_FILE_IF(ONCE_1) TRACE("Audio device enumeration not supported");
    return devices;
}

//======================================================================================================================
void AudioManager::RecreateALSources()
{
    // Recreate all sources (called after context recreation)
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        mChannels[i].mALSource = 0;
        alGenSources(1, &mChannels[i].mALSource);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            TRACE_FILE_IF(ONCE_2) TRACE("Failed to recreate OpenAL source %d, error %d", i, error);
        }
    }
}

//======================================================================================================================
void AudioManager::DeleteALBuffers()
{
    // Delete all OpenAL buffers before device switch
    TRACE_FILE_IF(ONCE_2) TRACE("Deleting %d sound buffers...", (int)mSounds.size());
    for (Sound* sound : mSounds)
    {
        if (sound->mALBuffer != 0)
        {
            alDeleteBuffers(1, &sound->mALBuffer);
            sound->mALBuffer = 0;
        }
    }
}

//======================================================================================================================
void AudioManager::RecreateALBuffers()
{
    // Recreate all OpenAL buffers using stored sound data
    TRACE_FILE_IF(ONCE_2) TRACE("Recreating %d sound buffers...", (int)mSounds.size());
    for (Sound* sound : mSounds)
    {
        if (sound->mSoundData == nullptr || sound->mSoundSamples == 0)
        {
            TRACE_FILE_IF(ONCE_3) TRACE("  Skipping %s - no sound data", sound->mName);
            continue;
        }

        // Create new buffer
        alGenBuffers(1, &sound->mALBuffer);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("  Failed to create buffer for %s, error %d", sound->mName, error);
            sound->mALBuffer = 0;
            continue;
        }

        // Determine format
        ALenum format = sound->mStereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

        // Upload data to OpenAL buffer
        alBufferData(sound->mALBuffer, format, sound->mSoundData,
                     sound->mSoundSamples * sizeof(int16), sound->mSampleFrequency);
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("  Failed to upload data for %s, error %d", sound->mName, error);
            alDeleteBuffers(1, &sound->mALBuffer);
            sound->mALBuffer = 0;
            continue;
        }

        TRACE_FILE_IF(ONCE_2) TRACE("  Recreated buffer %u for %s", sound->mALBuffer, sound->mName);
    }
}

//======================================================================================================================
bool AudioManager::SwitchAudioDevice(const char* deviceName)
{
    TRACE_FILE_IF(ONCE_1) TRACE("AudioManager::SwitchAudioDevice(%s) - current: %s",
        deviceName ? deviceName : "default", mCurrentDeviceName.c_str());

    // Check if already using this device
    if (deviceName && mCurrentDeviceName == deviceName)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Already using device: %s", deviceName);
        return true;
    }
    if (!deviceName && mCurrentDeviceName == mDefaultDeviceName)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("Already using default device");
        return true;
    }

    // Save channel state before stopping (so we can restore after switch)
    struct ChannelState {
        bool inUse;
        bool use3D;
        const Sound* sound;
        bool looping;
        float volumeScale;
        float targetVolumeScale;
        float targetVolumeScaleRate;
        float frequencyScale;
        float soundSourceRadius;
        Vector3 position;
        Vector3 velocity;
    };
    std::vector<ChannelState> savedChannels(mNumAvailableChannels);

    TRACE_FILE_IF(ONCE_2) TRACE("Saving state of %d channels...", mNumAvailableChannels);
    int activeCount = 0;
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        Channel& ch = mChannels[i];
        ChannelState& state = savedChannels[i];
        state.inUse = ch.mInUse;
        state.use3D = ch.mUse3D;
        state.sound = ch.mSound;
        state.volumeScale = ch.mVolumeScale;
        state.targetVolumeScale = ch.mTargetVolumeScaleF;
        state.targetVolumeScaleRate = ch.mTargetVolumeScaleFRate;
        state.frequencyScale = ch.mFrequencyScale;
        state.soundSourceRadius = ch.mSoundSourceRadius;
        state.position = ch.mSourcePosition;
        state.velocity = ch.mSourceVelocity;
        state.looping = false;

        if (ch.mInUse && ch.mALSource != 0)
        {
            ALint looping;
            alGetSourcei(ch.mALSource, AL_LOOPING, &looping);
            state.looping = (looping == AL_TRUE);
            activeCount++;
        }
    }
    TRACE_FILE_IF(ONCE_2) TRACE("Saved %d active channels", activeCount);

    TRACE_FILE_IF(ONCE_2) TRACE("Stopping all channels...");
    // Stop all playing sounds
    StopAllChannels();

    TRACE_FILE_IF(ONCE_2) TRACE("Deleting %d sources...", mNumAvailableChannels);
    // Delete all sources (must be done before destroying context)
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        if (mChannels[i].mALSource != 0)
        {
            alDeleteSources(1, &mChannels[i].mALSource);
            mChannels[i].mALSource = 0;
        }
    }

    // Delete all buffers (must be done before destroying context)
    DeleteALBuffers();

    // Destroy context
    TRACE_FILE_IF(ONCE_2) TRACE("Destroying context...");
    if (mALContext)
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(mALContext);
        mALContext = nullptr;
    }

    // Close current device
    TRACE_FILE_IF(ONCE_2) TRACE("Closing device...");
    if (mALDevice)
    {
        if (!alcCloseDevice(mALDevice))
        {
            TRACE_FILE_IF(ONCE_1) TRACE("WARNING: alcCloseDevice failed!");
        }
        mALDevice = nullptr;
    }

    // Open new device
    TRACE_FILE_IF(ONCE_2) TRACE("Opening device: %s", deviceName ? deviceName : "(default)");
    mALDevice = alcOpenDevice(deviceName);
    if (!mALDevice)
    {
        ALCenum err = alcGetError(nullptr);
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to open audio device: %s, error: %d", deviceName ? deviceName : "default", err);
        // Try to fall back to default
        if (deviceName)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("Trying fallback to default device...");
            mALDevice = alcOpenDevice(nullptr);
            if (mALDevice)
            {
                TRACE_FILE_IF(ONCE_1) TRACE("Fell back to default audio device");
            }
        }
        if (!mALDevice)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("Failed to open any audio device!");
            return false;
        }
    }
    TRACE_FILE_IF(ONCE_2) TRACE("Device opened successfully");

    // Store current device name (use full name if available)
    const ALCchar* actualDeviceName = nullptr;
    if (alcIsExtensionPresent(mALDevice, "ALC_ENUMERATE_ALL_EXT"))
    {
        actualDeviceName = alcGetString(mALDevice, ALC_ALL_DEVICES_SPECIFIER);
    }
    if (!actualDeviceName)
    {
        actualDeviceName = alcGetString(mALDevice, ALC_DEVICE_SPECIFIER);
    }
    if (actualDeviceName)
    {
        mCurrentDeviceName = actualDeviceName;
        TRACE_FILE_IF(ONCE_1) TRACE("Now using audio device: %s", actualDeviceName);
    }

    // Create new context
    TRACE_FILE_IF(ONCE_2) TRACE("Creating context...");
    mALContext = alcCreateContext(mALDevice, nullptr);
    if (!mALContext)
    {
        ALCenum err = alcGetError(mALDevice);
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to create OpenAL context, error: %d", err);
        alcCloseDevice(mALDevice);
        mALDevice = nullptr;
        return false;
    }
    if (!alcMakeContextCurrent(mALContext))
    {
        ALCenum err = alcGetError(mALDevice);
        TRACE_FILE_IF(ONCE_1) TRACE("Failed to make context current, error: %d", err);
    }
    TRACE_FILE_IF(ONCE_2) TRACE("Context created and made current");

    // Reconfigure audio settings
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    alSpeedOfSound(330.0f);
    alDopplerFactor(1.0f);

    // Recreate all sources
    TRACE_FILE_IF(ONCE_2) TRACE("Recreating %d sources...", mNumAvailableChannels);
    RecreateALSources();

    // Recreate all buffers with the stored sound data
    RecreateALBuffers();

    // Restore channel states and restart sounds
    TRACE_FILE_IF(ONCE_2) TRACE("Restoring %d channel states...", mNumAvailableChannels);
    int restoredCount = 0;
    for (int i = 0; i < mNumAvailableChannels; ++i)
    {
        const ChannelState& state = savedChannels[i];
        if (!state.inUse || state.sound == nullptr)
            continue;

        // Check that the sound's buffer was successfully recreated
        if (state.sound->mALBuffer == 0)
        {
            TRACE_FILE_IF(ONCE_3) TRACE("  Channel %d: skipping - sound buffer not available", i);
            continue;
        }

        Channel& ch = mChannels[i];
        ch.mInUse = true;
        ch.mUse3D = state.use3D;
        ch.mSound = state.sound;
        ch.mVolumeScale = state.volumeScale;
        ch.mTargetVolumeScaleF = state.targetVolumeScale;
        ch.mTargetVolumeScaleFRate = state.targetVolumeScaleRate;
        ch.mFrequencyScale = state.frequencyScale;
        ch.mSoundSourceRadius = state.soundSourceRadius;
        ch.mSourcePosition = state.position;
        ch.mSourceVelocity = state.velocity;

        // Configure the source
        ALuint source = ch.mALSource;
        if (state.use3D)
        {
            alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
            alSourcef(source, AL_REFERENCE_DISTANCE, state.soundSourceRadius);
            alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
            alSourcef(source, AL_MAX_DISTANCE, 10000.0f);
            alSource3f(source, AL_POSITION, state.position.x, state.position.y, state.position.z);
            alSource3f(source, AL_VELOCITY, state.velocity.x, state.velocity.y, state.velocity.z);
        }
        else
        {
            alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
            alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
            alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        }

        // Attach buffer and start playback
        alSourcei(source, AL_BUFFER, state.sound->mALBuffer);
        alSourcei(source, AL_LOOPING, state.looping ? AL_TRUE : AL_FALSE);
        alSourcef(source, AL_GAIN, state.volumeScale * mVolScale);
        alSourcef(source, AL_PITCH, Maximum(state.frequencyScale, 0.1f));
        alSourcePlay(source);

        restoredCount++;
        TRACE_FILE_IF(ONCE_2) TRACE("  Channel %d: restored sound %s", i, state.sound->mName);
    }
    TRACE_FILE_IF(ONCE_2) TRACE("Restored %d active channels", restoredCount);

    TRACE_FILE_IF(ONCE_2) TRACE("Audio device switch complete - now using: %s", mCurrentDeviceName.c_str());
    return true;
}

//======================================================================================================================
std::string AudioManager::FindMatchingVRAudioDevice(const char* headsetSystemName)
{
    if (!headsetSystemName)
        return "";

    // Get list of available devices
    auto devices = EnumerateAudioDevices();
    if (devices.empty())
        return "";

    // Convert headset name to lowercase for matching
    std::string headsetLower = headsetSystemName;
    for (char& c : headsetLower)
        c = (char)tolower(c);

    // Keywords to look for in VR headset audio device names
    const char* vrKeywords[] = {
        "rift", "oculus", "quest",  // Meta/Oculus
        "index",                     // Valve Index
        "vive",                      // HTC Vive
        "reverb",                    // HP Reverb (Windows MR)
        "pimax",                     // Pimax
        "virtual audio",             // Oculus Virtual Audio Device
        nullptr
    };

    // First, try to find a device that matches the headset name
    for (const auto& device : devices)
    {
        std::string deviceLower = device;
        for (char& c : deviceLower)
            c = (char)tolower(c);

        // Check if device name contains any part of headset name
        // or if headset name contains any VR keywords that match the device
        for (const char** keyword = vrKeywords; *keyword; ++keyword)
        {
            if (headsetLower.find(*keyword) != std::string::npos &&
                deviceLower.find(*keyword) != std::string::npos)
            {
                return device;
            }
        }
    }

    // Second pass: look for any device with VR keywords
    for (const auto& device : devices)
    {
        std::string deviceLower = device;
        for (char& c : deviceLower)
            c = (char)tolower(c);

        for (const char** keyword = vrKeywords; *keyword; ++keyword)
        {
            if (deviceLower.find(*keyword) != std::string::npos)
            {
                return device;
            }
        }
    }

    return "";
}

//======================================================================================================================
bool AudioManager::SwitchToDefaultAudio()
{
    TRACE_FILE_IF(ONCE_1) TRACE("AudioManager::SwitchToDefaultAudio()");

    if (mDefaultDeviceName.empty())
    {
        return SwitchAudioDevice(nullptr);
    }
    return SwitchAudioDevice(mDefaultDeviceName.c_str());
}
