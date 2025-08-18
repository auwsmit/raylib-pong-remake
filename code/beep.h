// EXPLANATION:
// For the pong beep sound effect. Only plays one beep at a time.
// TODO: comments

#ifndef PONG_BEEP_HEADER_GUARD
#define PONG_BEEP_HEADER_GUARD
#include "raylib.h"

typedef struct AudioBeepState
{
    AudioStream stream;
    bool isPlaying;
    float beepTimer;

    float frequency;
    float targetFrequency;
    float sineIdx;

    // Fade control
    int fadeCounter;
    bool fadingOut;
    bool fadingIn;

    // Frequency transition control
    int frequencyTransitionCounter;
    bool transitioningFrequency;
    float startFrequency;
} AudioBeepState;

extern AudioBeepState pongBeep;

void AudioBeepInputCallback(void *buffer, unsigned int frames);
void PlayBeepSound(float argFrequency, float beepLength);
void InitBeepSound(void);
void CloseBeepSound(void);
void UpdateBeepSound(void);

#endif // PONG_BEEP_HEADER_GUARD
