// EXPLANATION:
// Defines types and structs for state machines used across files

#ifndef PONG_STATES_HEADER_GUARD
#define PONG_STATES_HEADER_GUARD

#include "raylib.h"

// Title menu
// --------------------------------------------------------------------------------

typedef enum MenuScreenState { MENU_SS_DEFAULT, MENU_SS_DIFFICULTY } MenuScreenState;

typedef enum MenuOption // these correspond to enum GameMode (e.g. MODE_1PLAYER)
{
    MENU_1PLAYER, MENU_2PLAYER, MENU_DEMO, MENU_EXIT
} MenuOption;

typedef struct MenuButton // Holds data for a menu button
{
    const char *text;
    int fontSize;
    Vector2 position;
    Color color;
} MenuButton;

typedef struct MenuState // Holds data for the title screen menu
{
    MenuScreenState currentScreen;
    MenuButton title;
    MenuButton *buttons;      // allocate memory for buttons when created
    MenuButton *difficulties;
    MenuOption selectedIndex;
    float cursorSize;
    float keyHeldTime;
    int buttonCount;
    int diffCount;
    bool firstFrame; // used for mouse selection
    bool autoScroll;
} MenuState;

// Pong game
// --------------------------------------------------------------------------------

typedef enum GameTurn { TURN_RIGHT_SIDE, TURN_LEFT_SIDE } GameTurn;

typedef enum ScreenState
{
    SCREEN_LOGO, SCREEN_TITLE, SCREEN_GAMEPLAY, SCREEN_ENDING
} ScreenState;

typedef enum GameMode
{
    MODE_1PLAYER, MODE_2PLAYER, MODE_DEMO
} GameMode;

typedef enum Difficulty // Multiplier for CPU paddle speed
{
    DIFFICULTY_EASY, DIFFICULTY_MEDIUM, DIFFICULTY_HARD
} Difficulty;

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
    Ball ball;
    Paddle paddleL;
    Paddle paddleR;
    GameMode currentMode;
    GameTurn currentTurn; // keeps track of whose turn it currently is
    ScreenState currentScreen;
    Difficulty difficulty; // unused for MODE_2PLAYER
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

#endif // PONG_STATES_HEADER_GUARD
