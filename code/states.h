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

typedef enum UiMenuId
{
    UI_MENU_TITLE, UI_MENU_DIFFICULTY, UI_MENU_NONE
} UiMenuId;

typedef enum UiTitleMenuId
{
    UI_ID_1PLAYER, UI_ID_2PLAYER, UI_ID_DEMO, UI_ID_EXIT
} UiTitleMenuId;

typedef enum UiDifficultyMenuId
{
    UI_ID_EASY, UI_ID_MEDIUM, UI_ID_HARD, UI_ID_BACK
} UiDifficultyMenuId;

typedef struct UiButton
{
    const char *text;
    int fontSize;
    bool mouseHovered;
    Vector2 position;
    Color color;
} UiButton;

typedef struct UiMenu
{
    UiButton *buttons; // allocate buttons for variable length menus
    unsigned int buttonCount;
} UiMenu;

typedef struct UiState // Holds data for the title screen menu
{
    UiMenu menus[2]; // title, difficulty
    UiButton title; // Title text
    UiButton pause;
    UiButton unpause;
    UiMenuId currentMenu;
    float cursorSize;
    float keyHeldTime;
    unsigned int selectedId;
    bool firstFrame; // used for mouse selection
    bool autoScroll;
} UiState;

#endif // PONG_STATES_HEADER_GUARD
