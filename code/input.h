// EXPLANATION:
// Helps handle game input

#ifndef PONG_INPUT_HEADER_GUARD
#define PONG_INPUT_HEADER_GUARD

#include "raylib.h"

#define INPUT_ACTIONS_COUNT 10
#define INPUT_MAX_KEYS 9

typedef struct GameState GameState;

typedef enum InputAction
{
    INPUT_ACTION_CONFIRM,
    INPUT_ACTION_BACK,
    INPUT_ACTION_FULLSCREEN,
    INPUT_ACTION_PAUSE,
    INPUT_ACTION_P1_UP,
    INPUT_ACTION_P1_DOWN,
    INPUT_ACTION_P1_SPEED,
    INPUT_ACTION_P2_UP,
    INPUT_ACTION_P2_DOWN,
    INPUT_ACTION_P2_SPEED,
} InputAction;

typedef struct InputKeyMaps
{
    KeyboardKey keyMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_KEYS];
} InputKeyMaps;

// Prototypes
// --------------------------------------------------------------------------------
InputKeyMaps InitInputKeyMaps(void);
bool IsInputKeyModifier(KeyboardKey key);
bool IsInputActionPressed(InputAction action, GameState* pong);
bool IsInputActionDown(InputAction action, GameState* pong);

#endif // PONG_INPUT_HEADER_GUARD
