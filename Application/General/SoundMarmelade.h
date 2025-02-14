#ifndef SOUNDMARMELADE_H
#define SOUNDMARMELADE_H

// #include "Trace.h"

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
extern float mPlaybackFrequency; // Fréquence de lecture par défaut
extern int mNumAvailableChannels; // Nombre de canaux (stéréo)
extern float mVolScale; // Volume global (échelle [0, 1])
extern uint32_t mSampleRate; // Taux d'échantillonnage par défaut
extern uint32_t mBufferSize; // Taille du tampon par défaut
extern bool mMixerMode; // Mode du mixeur audio (true ou false)
extern bool mSoundAvailable; // Disponibilité du son (true ou false)

extern Channel mChannels[16]; // Tableaux des canaux disponibles, ajustez le nombre selon le besoin

int s3eSoundGetInt(int param);

void s3eSoundSetInt(int param, int value);

void s3eSoundChannelSetInt(int channelId, int param, int value);

void s3eSoundChannelPlay(int channelId, void* soundData, uint32_t numSamples, bool loop, int delay);

void s3eSoundChannelRegister(int channelId, S3EChannelGen channelType, AudioCallback callback, void* userData);

void s3eSoundChannelStop(int channelId);








bool s3eIOSBackgroundAudioAvailable();

void s3eIOSBackgroundAudioSetMix(bool enable);



#endif // SOUNDMARMELADE_H
