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
    Sound beeps[4];
    Ball ball;
    Paddle paddleL;
    Paddle paddleR;
    GameMode currentMode;
    GameDifficulty difficulty; // unused for MODE_2PLAYER
    ScreenState currentScreen;
    int scoreL;
    int scoreR;
    bool playerWon;
    bool isPaused;
    bool gameShouldExit;       // flag to tell the game window to close
    float winTimer;            // countdown after player wins
    float scoreTimer;          // countdown after a score
} GameState;

// User Interface
// --------------------------------------------------------------------------------

typedef enum UiMenuState
{
    UI_MENU_TITLE, UI_MENU_DIFFICULTY, UI_MENU_PAUSE, UI_MENU_GAMEPLAY
} UiMenuState;

typedef enum UiButtonIdTitle
{
    UI_BID_1PLAYER, UI_BID_2PLAYER, UI_BID_DEMO, UI_BID_EXIT
} UiButtonIdTitle;

typedef enum UiDifficultyMenuId
{
    UI_BID_EASY, UI_BID_MEDIUM, UI_BID_HARD, UI_BID_BACK
} UiDifficultyMenuId;

typedef enum UiPauseMenuId
{
    UI_BID_RESUME, UI_BID_BACKTOTITLE
} UiPauseMenuId;

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
    UiButton title; // Title text
    UiButton pause;
    UiMenu menus[3]; // title, difficulty, pause
    UiMenuState currentMenu;
    float cursorSize;
    float keyHeldTime;
    float textFade;            // tracks fade value over time
    float textFadeTimeElapsed; // tracks time for the fade animation
    unsigned int selectedId;
    bool firstFrame;
    bool lastSelectWithMouse;
    bool autoScroll;
} UiState;

#endif // PONG_STATES_HEADER_GUARD
