// EXPLANATION:
// Generates and manages pong beeps.

#include "beep.h"

#include <limits.h>
#include "raymath.h"

#define ARRAY_SIZE(arr) (unsigned int)(sizeof(arr) / sizeof((arr)[0]))

// Global beep state
AudioBeepState beepState = { 0 };

void AudioBeepInputCallback(void *buffer, unsigned int frames)
{
    short *samples = (short *)buffer;
    static int fadeSamples = 256; // fade duration

    // clear output buffer
    for (unsigned int i = 0; i < frames; i++)
        samples[i] = 0;

    for (unsigned int i = 0; i < frames; i++)
    {
        float mix = 0.0f;

        for (unsigned int j = 0; j < ARRAY_SIZE(beepState.beeps); j++)
        {
            PongBeep *beep = &beepState.beeps[j];
            if (!beep->isPlaying) continue;

            float incr = (beep->frequency * 0.95f) / 44100.0f;
            float amplitude = SHRT_MAX / 4; // individual beep volume
            float sample = sinf(2 * PI * beep->sineIndex);

            // fade in/out
            if (beep->fadingIn || beep->fadingOut)
            {
                float fadeProgress = (float)beep->fadeCounter / fadeSamples;
                if (beep->fadingIn)
                {
                    amplitude *= fadeProgress;
                    if (++beep->fadeCounter >= fadeSamples) {
                        beep->fadingIn = false;
                        beep->fadeCounter = 0;
                    }
                }
                else // fadingOut
                {
                    amplitude *= (1.0f - fadeProgress);
                    if (++beep->fadeCounter >= fadeSamples) {
                        amplitude = 0.0f;
                        beep->isPlaying = false;
                    }
                }
            }

            mix += amplitude * sample;

            beep->sineIndex += incr;
            if (beep->sineIndex > 1.0f) beep->sineIndex -= 1.0f;
        }

        // limiter to avoid clipping
        if (mix > SHRT_MAX) mix = SHRT_MAX;
        if (mix < SHRT_MIN) mix = SHRT_MIN;

        samples[i] = (short)mix;
    }
}

PongBeep *FindBeepSlot(void)
{
    PongBeep *oldest = &beepState.beeps[0];
    for (unsigned int i = 0; i < ARRAY_SIZE(beepState.beeps); i++)
    {
        PongBeep *beep = &beepState.beeps[i];
        if (beep->beepTimer < oldest->beepTimer)
            oldest = &beepState.beeps[i];
        if (!beep->isPlaying)
        {
            return &beepState.beeps[i];
        }
    }

    // if all beeps are playing, overwrite the oldest one
    return oldest;
}

void PlayBeepSound(float argFrequency, float beepLength)
{
    PongBeep *beep = FindBeepSlot();

    beep->beepTimer = beepLength;
    beep->frequency = argFrequency;
    beep->fadeCounter = 0;
    beep->fadingOut = false;
    beep->fadingIn = true;
    beep->isPlaying = true;
}

void InitBeepSound(void)
{
    AudioBeepState new = { 0 };
    beepState = new;
    beepState.stream = LoadAudioStream(44100, 16, 1);
    static int maxSamplesPerUpdate = 4096;
    SetAudioStreamBufferSizeDefault(maxSamplesPerUpdate);
    SetAudioStreamCallback(beepState.stream, AudioBeepInputCallback);
}

void CloseBeepSound(void)
{
    UnloadAudioStream(beepState.stream);
}

void UpdateBeepSound(void)
{
    bool anyBeepActive = false;

    for (unsigned int i = 0; i < ARRAY_SIZE(beepState.beeps); i++)
    {
        PongBeep *beep = &beepState.beeps[i];
        if (!beep->isPlaying)
            continue;
        else
            anyBeepActive = true;

        // Update beep timer
        if (beep->beepTimer < 0)
            beep->beepTimer = 0;
        else if (beep->beepTimer > 0)
            beep->beepTimer -= GetFrameTime();

        // Fade out and stop after timer ends
        if (beep->beepTimer == 0)
        {
            beep->fadingOut = true;
            beep->fadingIn = false;
        }
    }

    if (!anyBeepActive && IsAudioStreamPlaying(beepState.stream))
    {
        StopAudioStream(beepState.stream);
    }
    else if (anyBeepActive && !IsAudioStreamPlaying(beepState.stream))
    {
        PlayAudioStream(beepState.stream);
    }
}

