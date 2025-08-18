// EXPLANATION:
// For the pong beep sound effect. Only plays one beep at a time.

#include "beep.h"

#include "raymath.h"

// Global beep state, used by AudioBeepInputCallback which can't take extra arguments
AudioBeepState pongBeep = { 0 };

// Audio input processing callback with smooth fade in/out and frequency transitions
void AudioBeepInputCallback(void *buffer, unsigned int frames)
{
    short *samples = (short *)buffer;
    static int fadeSamples = 256; // This controls how long the fade in/out lasts
    static int frequencyTransitionSamples = 256;

    for (unsigned int i = 0; i < frames; i++)
    {
        // Handle frequency transitions
        if (pongBeep.transitioningFrequency)
        {
            float transitionProgress = (float)pongBeep.frequencyTransitionCounter / frequencyTransitionSamples;
            pongBeep.frequency = Lerp(pongBeep.startFrequency, pongBeep.targetFrequency, transitionProgress);

            if (++pongBeep.frequencyTransitionCounter >= frequencyTransitionSamples)
            {
                pongBeep.frequency = pongBeep.targetFrequency;
                pongBeep.transitioningFrequency = false;
                pongBeep.frequencyTransitionCounter = 0;
            }
        }

        float incr = (pongBeep.frequency * 0.95f) / 44100.0f;
        float amplitude = 32000.0f;
        float sample = sinf(2 * PI * pongBeep.sineIdx);

        // Handle fade in/out
        if (pongBeep.fadingIn || pongBeep.fadingOut)
        {
            float fadeProgress = (float)pongBeep.fadeCounter / fadeSamples;
            if (pongBeep.fadingIn)
            {
                amplitude *= fadeProgress;
                if (++pongBeep.fadeCounter >= fadeSamples)
                {
                    pongBeep.fadingIn = false;
                    pongBeep.fadeCounter = 0;
                }
            }
            else // pongBeep.fadingOut
            {
                amplitude *= (1.0f - fadeProgress);
                if (++pongBeep.fadeCounter >= fadeSamples)
                {
                    amplitude = 0.0f;
                    pongBeep.isPlaying = false;
                }
            }
        }

        samples[i] = (short)(amplitude * sample);
        pongBeep.sineIdx += incr;
        if (pongBeep.sineIdx > 1.0f) pongBeep.sineIdx -= 1.0f;
    }
}

void PlayBeepSound(float argFrequency, float beepLength)
{
    pongBeep.beepTimer = beepLength;

    if (pongBeep.isPlaying)
    {
        // If already playing, smoothly transition to new frequency
        pongBeep.startFrequency = pongBeep.frequency;
        pongBeep.targetFrequency = argFrequency;
        pongBeep.transitioningFrequency = true;
        pongBeep.frequencyTransitionCounter = 0;
    }
    else
    {
        // If not playing, start fresh
        static int maxSamplesPerUpdate = 4096;
        pongBeep.frequency = argFrequency;
        pongBeep.targetFrequency = argFrequency;
        pongBeep.fadeCounter = 0;
        pongBeep.fadingOut = false;
        pongBeep.fadingIn = true;

        SetAudioStreamBufferSizeDefault(maxSamplesPerUpdate);
        SetAudioStreamCallback(pongBeep.stream, AudioBeepInputCallback);
        PlayAudioStream(pongBeep.stream);
        pongBeep.isPlaying = true;
    }
}

void InitBeepSound(void)
{
    AudioBeepState new = { 0 };
    pongBeep = new;
    pongBeep.stream = LoadAudioStream(44100, 16, 1);
}

void CloseBeepSound(void)
{
    UnloadAudioStream(pongBeep.stream);
}

void UpdateBeepSound(void)
{
    // Update beep timer
    if (pongBeep.beepTimer < 0)
        pongBeep.beepTimer = 0;
    else if (pongBeep.beepTimer > 0)
        pongBeep.beepTimer -= GetFrameTime();

    // Fade out and stop after timer ends
    if (pongBeep.beepTimer == 0)
    {
        pongBeep.fadingOut = true;
        pongBeep.fadingIn = false;
    }
    else if (!pongBeep.isPlaying && IsAudioStreamPlaying(pongBeep.stream))
    {
        StopAudioStream(pongBeep.stream);
    }
}

