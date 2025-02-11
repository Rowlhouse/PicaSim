#ifndef SOUNDMARMELADE_H
#define SOUNDMARMELADE_H

#include "Trace.h"
#include "AudioManager.h"

#include <cstdint>

// Définition d'un callback audio pour la gestion des événements
typedef int32_t (*AudioCallback)(void* userData, void* soundData);



// Enumération pour les différents paramètres audio dans l'API s3eSound
enum S3ESoundParam
{
    S3E_SOUND_OUTPUT_FREQ = 0,     // Fréquence de sortie audio
    S3E_SOUND_NUM_CHANNELS,        // Nombre de canaux audio disponibles
    S3E_SOUND_VOLUME,              // Volume global
    S3E_SOUND_SAMPLE_RATE,         // Taux d'échantillonnage (si nécessaire)
    S3E_SOUND_BUFFER_SIZE,         // Taille du tampon audio (si nécessaire)
    S3E_SOUND_MIXER_MODE,          // Mode du mixeur audio (si nécessaire)
    S3E_SOUND_AVAILABLE,
    S3E_CHANNEL_VOLUME,
    S3E_CHANNEL_FREQUENCY,
    
    // Ajouter d'autres paramètres selon les besoins de ton API
};

enum S3EChannelGen {
    S3E_CHANNEL_GEN_AUDIO,
    S3E_CHANNEL_GEN_AUDIO_STEREO
};


// Structure représentant un canal audio
struct Channel
{
    bool mInUse;                     // Indicateur si le canal est en cours d'utilisation
    void* mSoundData;                // Pointeur vers les données sonores
    uint32_t mSoundSamples;          // Nombre d'échantillons du son
    float mPlaybackPosF;             // Position de lecture du son
    bool mLoop;                      // Indicateur de boucle
    int mDelay;                      // Délai avant de commencer la lecture
    float mVolumeScale;              // Volume du canal (0.0 à 1.0)
    float mLeftVolF;                 // Volume du canal pour le canal gauche
    float mRightVolF;                // Volume du canal pour le canal droit
    float mPlaybackFrequencyMultiplierF; // Multiplicateur de fréquence pour la lecture
    AudioCallback mCallback;         // Callback pour le canal
    void* mCallbackUserData;         // Données utilisateur pour le callback
    S3EChannelGen mChannelType;                // Type du canal (mono, stéréo, etc.)
};

// Structure pour représenter des informations audio à lire
struct s3eSoundGenAudioInfo
{
    int16_t* m_Target; // Pointeur vers le buffer audio cible
    uint32_t m_NumSamples; // Nombre d'échantillons à lire
    bool m_Stereo; // Indicateur si c'est un son stéréo ou mono
    bool m_Mix; // Indicateur si le mixage est activé
    bool m_EndSample; // Indicateur pour marquer la fin de l'échantillon
};


// Déclarations globales simulées pour illustration
float mPlaybackFrequency = 44100.0f; // Fréquence de lecture par défaut
int mNumAvailableChannels = 2; // Nombre de canaux (stéréo)
float mVolScale = 1.0f; // Volume global (échelle [0, 1])
uint32_t mSampleRate = 44100; // Taux d'échantillonnage par défaut
uint32_t mBufferSize = 1024; // Taille du tampon par défaut
bool mMixerMode = true; // Mode du mixeur audio (true ou false)
bool mSoundAvailable = true; // Disponibilité du son (true ou false)

Channel mChannels[16]; // Tableaux des canaux disponibles, ajustez le nombre selon le besoin

int s3eSoundGetInt(int param)
{
    switch (param)
    {
        case S3E_SOUND_OUTPUT_FREQ:
            return static_cast<int>(mPlaybackFrequency); // Retourne la fréquence en tant qu'entier
        case S3E_SOUND_NUM_CHANNELS:
            return mNumAvailableChannels; // Retourne le nombre de canaux audio disponibles
        case S3E_SOUND_VOLUME:
            return static_cast<int>(mVolScale * 100); // Retourne le volume global en pourcentage
        case S3E_SOUND_SAMPLE_RATE:
            return static_cast<int>(mSampleRate); // Retourne le taux d'échantillonnage
        case S3E_SOUND_BUFFER_SIZE:
            return static_cast<int>(mBufferSize); // Retourne la taille du tampon
        case S3E_SOUND_MIXER_MODE:
            return static_cast<int>(mMixerMode); // Retourne le mode du mixeur audio (1 ou 0)
        case S3E_SOUND_AVAILABLE:
            return mSoundAvailable ? 1 : 0; // Indicateur de disponibilité du son
        default:
            TRACE_FILE_IF(1) TRACE("Paramètre inconnu dans s3eSoundGetInt()\n");
            return -1; // Retourne -1 si le paramètre est inconnu
    }
}

void s3eSoundSetInt(int param, int value)
{
    switch (param)
    {
        case S3E_SOUND_OUTPUT_FREQ:
            mPlaybackFrequency = static_cast<float>(value); // Modifie la fréquence de lecture
            break;
        case S3E_SOUND_NUM_CHANNELS:
            TRACE_FILE_IF(1) TRACE("Modification du nombre de canaux non supportée\n");
            break;
        case S3E_SOUND_VOLUME:
            mVolScale = value / 100.0f; // Modifie l'échelle du volume
            break;
        case S3E_SOUND_SAMPLE_RATE:
            mSampleRate = static_cast<uint32_t>(value); // Modifie le taux d'échantillonnage
            break;
        case S3E_SOUND_BUFFER_SIZE:
            mBufferSize = static_cast<uint32_t>(value); // Modifie la taille du tampon
            break;
        case S3E_SOUND_MIXER_MODE:
            mMixerMode = value != 0; // Modifie le mode du mixeur (1 pour actif, 0 pour inactif)
            break;
        case S3E_SOUND_AVAILABLE:
            mSoundAvailable = (value != 0); // Active/désactive le son en fonction de la valeur
            break;
        default:
            TRACE_FILE_IF(1) TRACE("Paramètre inconnu dans s3eSoundSetInt()\n");
            break;
    }
}

void s3eSoundChannelSetInt(int channelId, int param, int value)
{
    if (channelId < 0 || channelId >= mNumAvailableChannels) 
    {
        TRACE_FILE_IF(1) TRACE("Canal invalide : %d\n", channelId);
        return;
    }

    Channel& channel = mChannels[channelId];

    switch (param)
    {
        case S3E_CHANNEL_VOLUME:
            channel.mVolumeScale = value / 100.0f;  // Exemple : valeur entre 0 et 100 pour ajuster le volume
            break;

        case S3E_CHANNEL_FREQUENCY:
            channel.mPlaybackFrequencyMultiplierF = static_cast<float>(value);
            break;

        default:
            TRACE_FILE_IF(1) TRACE("Paramètre inconnu dans s3eSoundChannelSetInt()\n");
            break;
    }
}

void s3eSoundChannelPlay(int channelId, void* soundData, uint32_t numSamples, bool loop, int delay)
{
    if (channelId < 0 || channelId >= mNumAvailableChannels) 
    {
        TRACE_FILE_IF(1) TRACE("Canal invalide : %d\n", channelId);
        return;
    }

    Channel& channel = mChannels[channelId];

    if (!channel.mInUse)
    {
        TRACE_FILE_IF(1) TRACE("Le canal %d n'est pas en cours d'utilisation\n", channelId);
        return;
    }

    channel.mSoundData = soundData;
    channel.mSoundSamples = numSamples;
    channel.mPlaybackPosF = 0.0f;
    channel.mLoop = loop;
    channel.mDelay = delay;

    channel.mLeftVolF = channel.mVolumeScale;
    channel.mRightVolF = channel.mVolumeScale;
    channel.mPlaybackFrequencyMultiplierF = 1.0f;

    TRACE_FILE_IF(1) TRACE("Lecture du son sur le canal %d\n", channelId);
}

void s3eSoundChannelRegister(int channelId, S3EChannelGen channelType, AudioCallback callback, void* userData)
{
    if (channelId < 0 || channelId >= mNumAvailableChannels) 
    {
        TRACE_FILE_IF(1) TRACE("Canal invalide : %d\n", channelId);
        return;
    }

    Channel& channel = mChannels[channelId];

    channel.mCallback = callback;
    channel.mCallbackUserData = userData;
    channel.mChannelType = channelType;

    TRACE_FILE_IF(1) TRACE("Callback enregistré pour le canal %d\n", channelId);
}

void s3eSoundChannelStop(int channelId)
{
    if (channelId < 0 || channelId >= mNumAvailableChannels) 
    {
        TRACE_FILE_IF(1) TRACE("Canal invalide : %d\n", channelId);
        return;
    }

    Channel& channel = mChannels[channelId];

    if (!channel.mInUse)
    {
        TRACE_FILE_IF(1) TRACE("Le canal %d n'est pas en cours d'utilisation\n", channelId);
        return;
    }

    channel.mPlaybackPosF = 0.0f;
    channel.mSoundData = nullptr;
    channel.mSoundSamples = 0;
    channel.mInUse = false;

    TRACE_FILE_IF(1) TRACE("Lecture arrêtée sur le canal %d\n", channelId);
}

#endif // SOUNDMARMELADE_H
