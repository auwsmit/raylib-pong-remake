// EXPLANATION:
// Helps handle game input

#include "input.h"
#include "pong.h"

InputKeyMaps InitInputKeyMaps(void)
{
    InputKeyMaps input =
    {
        // Input action key mappings
        .keyMaps[INPUT_ACTION_CONFIRM][0] = KEY_ENTER,
        .keyMaps[INPUT_ACTION_CONFIRM][1] = KEY_SPACE,

        .keyMaps[INPUT_ACTION_BACK][0] = KEY_ESCAPE,
        .keyMaps[INPUT_ACTION_BACK][1] = KEY_BACKSPACE,

        .keyMaps[INPUT_ACTION_PAUSE][0] = KEY_P,

        .keyMaps[INPUT_ACTION_FULLSCREEN][0] = KEY_LEFT_ALT,
        .keyMaps[INPUT_ACTION_FULLSCREEN][1] = KEY_ENTER,
        .keyMaps[INPUT_ACTION_FULLSCREEN][2] = KEY_RIGHT_ALT,
        .keyMaps[INPUT_ACTION_FULLSCREEN][3] = KEY_ENTER,
        .keyMaps[INPUT_ACTION_FULLSCREEN][4] = KEY_LEFT_SHIFT,
        .keyMaps[INPUT_ACTION_FULLSCREEN][5] = KEY_F,
        .keyMaps[INPUT_ACTION_FULLSCREEN][6] = KEY_RIGHT_SHIFT,
        .keyMaps[INPUT_ACTION_FULLSCREEN][7] = KEY_F,
        .keyMaps[INPUT_ACTION_FULLSCREEN][8] = KEY_F11,

        .keyMaps[INPUT_ACTION_P1_UP][0] = KEY_W,
        .keyMaps[INPUT_ACTION_P1_DOWN][0] = KEY_S,
        .keyMaps[INPUT_ACTION_P1_SPEED][0] = KEY_D,
        .keyMaps[INPUT_ACTION_P1_SPEED][1] = KEY_A,
        .keyMaps[INPUT_ACTION_P1_SPEED][2] = KEY_LEFT_SHIFT,

        .keyMaps[INPUT_ACTION_P2_UP][0] = KEY_I,
        .keyMaps[INPUT_ACTION_P2_UP][1] = KEY_UP,
        .keyMaps[INPUT_ACTION_P2_DOWN][0] = KEY_K,
        .keyMaps[INPUT_ACTION_P2_DOWN][1] = KEY_DOWN,
        .keyMaps[INPUT_ACTION_P2_SPEED][0] = KEY_J,
        .keyMaps[INPUT_ACTION_P2_SPEED][1] = KEY_K,
        .keyMaps[INPUT_ACTION_P2_SPEED][2] = KEY_LEFT,
        .keyMaps[INPUT_ACTION_P2_SPEED][3] = KEY_RIGHT,
        .keyMaps[INPUT_ACTION_P2_SPEED][4] = KEY_LEFT_SHIFT,
    };

    return input;
}

bool IsInputKeyModifier(KeyboardKey key)
{
    if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT ||
        key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT ||
        key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
        return true;
    return false;
}

bool IsInputActionPressed(InputAction action, GameState* pong)
{
    KeyboardKey* keys = pong->input.keyMaps[action];

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

bool IsInputActionDown(InputAction action, GameState* pong)
{
    KeyboardKey* keys = pong->input.keyMaps[action];

    for (int i = 0; i < INPUT_MAX_KEYS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT ||
            key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT ||
            key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
        {
            if (i + 1 < INPUT_MAX_KEYS && keys[i + 1] != 0)
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
