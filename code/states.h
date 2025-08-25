// EXPLANATION:
// Defines types and structs for state machines used across files

#ifndef PONG_STATES_HEADER_GUARD
#define PONG_STATES_HEADER_GUARD

#include "raylib.h"

// Pong Game
// --------------------------------------------------------------------------------

typedef enum ScreenState
{
    SCREEN_LOGO, SCREEN_TITLE, SCREEN_GAMEPLAY, SCREEN_ENDING
} ScreenState;

typedef enum GameMode
{
    MODE_1PLAYER, MODE_2PLAYER, MODE_DEMO
} GameMode;

typedef enum GameDifficulty // Multiplier for CPU paddle speed
{
    DIFFICULTY_EASY, DIFFICULTY_MEDIUM, DIFFICULTY_HARD
} GameDifficulty;

typedef enum PongBeep
{
    BEEP_MENU, BEEP_PADDLE, BEEP_EDGE, BEEP_SCORE
} PongBeep;

typedef struct Paddle
{
    Vector2 position;
    float nextHitPos; // Only used for Computer paddle
                      // Determines how the computer will angle its next bounce
    float speed;
    int length;
    int width;
} Paddle;

typedef struct Ball
{
    Vector2 position;
    Vector2 direction;
    float speed; // the ball is always set to this speed
    int size;
} Ball;

typedef struct GameState
{
    ScreenState currentScreen;
    Sound beeps[4];
    Ball ball;
    Paddle paddleL;
    Paddle paddleR;
    GameMode currentMode;
    bool leftSideServe; // keeps track of whose turn it currently is
    GameDifficulty difficulty; // unused for MODE_2PLAYER
    int scoreL;
    int scoreR;
    bool playerWon;
    bool isPaused;
    bool gameShouldExit;       // flag to tell the game window to close
    float textFade;            // tracks fade value over time
    float textFadeTimeElapsed; // tracks time for the fade animation
    float winTimer;            // countdown after player wins
    float scoreTimer;          // countdown after a score
} GameState;

// User Interface
// --------------------------------------------------------------------------------

typedef enum UiMenuId { MENU_TITLE, MENU_DIFFICULTY } UiMenuId;

typedef enum UiOptionId
{
    MENUID_1PLAYER, MENUID_2PLAYER, MENUID_DEMO, MENUID_EXIT
} UiOptionId;

typedef enum UiDifficultyId
{
    MENUID_EASY, MENUID_MEDIUM, MENUID_HARD, MENUID_BACK
} UiDifficultyId;

typedef struct UiButton // Holds data for a menu button
{
    const char *text;
    int fontSize;
    Vector2 position;
    Color color;
} UiButton;

typedef struct UiMenu // Holds data for a menu of buttons
{
    UiButton *buttons;
    unsigned int buttonCount;
} UiMenu;

typedef struct UiState // Holds data for the title screen menu
{
    UiMenuId currentMenu;
    UiMenu menus[2]; // title and difficulty menus
    UiButton text[2]; // Title and Paused text
    unsigned int selectedId;
    float cursorSize;
    float keyHeldTime;
    bool firstFrame; // used for mouse selection
    bool autoScroll;
} UiState;

#endif // PONG_STATES_HEADER_GUARD
