// EXPLANATION:
// Generates and manages pong beeps.

#ifndef PONG_BEEP_HEADER_GUARD
#define PONG_BEEP_HEADER_GUARD
#include "raylib.h"

// One beep
typedef struct PongBeep
{
    bool isPlaying;
    float frequency;
    float beepTimer;
    float sineIndex;

    // Fade control
    int fadeCounter;
    bool fadingOut;
    bool fadingIn;
} PongBeep;

// Global audio state for all beeps
typedef struct AudioBeepState
{
    PongBeep beeps[4]; // number of beeps that can play simultaneously
    AudioStream stream;

    // Frequency transition control
    int frequencyTransitionCounter;
    bool transitioningFrequency;
    float startFrequency;
} AudioBeepState;

extern AudioBeepState pongBeep;

void AudioBeepInputCallback(void *buffer, unsigned int frames);
PongBeep *FindBeepSlot(void);
void PlayBeepSound(float argFrequency, float beepLength);
void InitBeepSound(void);
void CloseBeepSound(void);
void UpdateBeepSound(void);

#endif // PONG_BEEP_HEADER_GUARD
