// EXPLANATION:
// Helps handle game input

#include "input.h"

// Global struct to track input key mappings
InputKeyMaps gameInput = { 0 };

void InitDefaultInputControls(void)
{
    InputKeyMaps defaultControls =
    {
        // Global across program
        .keyMaps[INPUT_ACTION_FULLSCREEN] =
        {
            KEY_LEFT_ALT, KEY_ENTER,
            KEY_RIGHT_ALT, KEY_ENTER,
            KEY_LEFT_SHIFT, KEY_F,
            KEY_RIGHT_SHIFT, KEY_F,
            KEY_F11,
        },

        // Menu and Game
        .keyMaps[INPUT_ACTION_CONFIRM] = { KEY_ENTER, KEY_SPACE },
        .keyMaps[INPUT_ACTION_BACK] =    { KEY_ESCAPE, KEY_BACKSPACE, },
        .keyMaps[INPUT_ACTION_PAUSE] =   { KEY_P },

        // Player 1 controls
        .keyMaps[INPUT_ACTION_P1_UP] =   { KEY_W, },
        .keyMaps[INPUT_ACTION_P1_DOWN] = { KEY_S, },
        .keyMaps[INPUT_ACTION_P1_SPEED] = { KEY_D, KEY_A, KEY_LEFT_SHIFT, },

        // Player 2 controls
        .keyMaps[INPUT_ACTION_P2_UP] =   { KEY_I, KEY_UP },
        .keyMaps[INPUT_ACTION_P2_DOWN] = { KEY_K, KEY_DOWN },
        .keyMaps[INPUT_ACTION_P2_SPEED] = { KEY_J, KEY_K, KEY_LEFT, KEY_RIGHT },
    };

    gameInput = defaultControls;
}

bool IsInputKeyModifier(KeyboardKey key)
{
    if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT ||
        key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT ||
        key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
        return true;
    return false;
}

bool IsInputActionPressed(InputAction action)
{
    KeyboardKey* keys = gameInput.keyMaps[action];

    // Check potential key combinations
    for (int i = 0; i < INPUT_MAX_KEYS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        // Check modifier plus next key (only 1 modifier for now)
        if (IsInputKeyModifier(key))
        {
            if ((i + 1 < INPUT_MAX_KEYS) && (keys[i + 1] != 0) &&
                (!IsInputKeyModifier(keys[i + 1])))
            {
                if (IsKeyDown(key) && IsKeyPressed(keys[i + 1]))
                    return true;
                i++; // Skip the next key
            }
            // Check just the modifier by itself
            else if (IsKeyPressed(key))
                return true;
        }

        // Check a single key
        else if (IsKeyPressed(key))
            return true;
    }

    return false;
}

bool IsInputActionDown(InputAction action)
{
    KeyboardKey* keys = gameInput.keyMaps[action];

    for (int i = 0; i < INPUT_MAX_KEYS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        // Check modifier plus next key (only 1 modifier for now)
        if (IsInputKeyModifier(key))
        {
            if ((i + 1 < INPUT_MAX_KEYS) && (keys[i + 1] != 0) &&
                (!IsInputKeyModifier(keys[i + 1])))
            {
                if (IsKeyDown(key) && IsKeyDown(keys[i + 1]))
                    return true;
                i++;
            }
            // Check just the modifier by itself
            else if (IsKeyDown(key))
                return true;
        }

        else if (IsKeyDown(key))
            return true;
    }
    return false;
}

void HandleToggleFullscreen(void)
{
    // No fullscreen input for web because it's buggy
    // For now just use emscripten's fullscreen button
#if !defined(PLATFORM_WEB)
    // Input for fullscreen
    if (IsInputActionPressed(INPUT_ACTION_FULLSCREEN))
    {
        // Borderless Windowed is generally nicer to use on desktop
        ToggleBorderlessWindowed();
        PollInputEvents(); // Skip to the next frame's input
    }
#endif
}
