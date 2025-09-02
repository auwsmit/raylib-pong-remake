// EXPLANATION:
// All the game logic, including how/when to draw to screen

#ifndef PONG_GAME_HEADER_GUARD
#define PONG_GAME_HEADER_GUARD

#include "raylib.h"

// Macros
// ----------------------------------------------------------------------------

#define WIN_SCORE 5 // Score needed to win

// Paddle and ball properties
#define PADDLE_LENGTH 120 // Initial settings for paddles
#define PADDLE_WIDTH 20
#define PADDLE_SPEED 500  // Paddle's default speed in pixels per second
#define BALL_SIZE 20      // Initial settings for ball
#define BALL_SPEED 500

// Ball physics modifiers for game feel
#define BOUNCE_MULTIPLIER 1.1f       // How much faster the ball gets after hitting a paddle
#define PADDLE_HIT_MAX_ANGLE 40.0f   // How much the ball's angle is affected by where it hits the paddle (0 to 90 degrees)
                                     // This is the angle the ball will deflect at if it hits the top or bottom of the paddle
                                     // Hitting the center of the paddle will deflect the ball straight sideways, 0 degrees
#define MINIMUM_VERTICAL_ANGLE 25.0f // The minimum vertical angle the ball can move (1 degree minimum)
#define RETURN_POSITION_VARIATION 50 // How much the ball's vertical position can change after scoring
#define RETURN_ANGLE_VARIATION 200   // How much the ball's y-directon can change after scoring, in pixels
                                     // e.g. ball.direction.y += rand(-angle_variation,+angle_variation)

#define SCORE_PAUSE_TIME 1.0f  // Time to pause after a score
#define WIN_PAUSE_TIME 10.0f   // Time to pause after a win

// Types and Structures
// ----------------------------------------------------------------------------

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
    float winTimer;   // countdown after player wins
    float scoreTimer; // countdown after a score
    GameMode currentMode;
    GameDifficulty difficulty;
    ScreenState currentScreen;
    int scoreL;
    int scoreR;
    bool playerWon;
    bool isPaused;
    bool gameShouldExit;
} GameState;

extern GameState pongGame; // global declaration

// Prototypes
// ----------------------------------------------------------------------------

// Initialization
void InitGameState(void); // Initialize game data and allocate memory for beeps
Sound GenBeep(float freq, float lengthSec); // Generate and allocate memory a sine wave buffer for a beep
void FreeBeeps(void);

// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle); // Check if ball and paddle are colliding
void EdgeCollisionPaddle(Paddle *paddle);          // Paddles collide with screen edges
void BounceBallEdge(Ball *ball);                   // Ball bounces off screen edges, and updates the score
void BounceBallPaddle(Ball *ball, Paddle *paddle); // Ball bounces off paddle

// Update / User Input
void UpdatePongFrame(void); // Updates all the game's data and objects for the current frame
void UpdatePaddleMouseInput(Paddle *paddle); // Updates paddle's position based on mouse position
void UpdatePaddlePlayer1(Paddle *paddle);    // Paddle speed updates based on player input (W/S with Left Shift)
void UpdatePaddlePlayer2(Paddle *paddle);    // Paddle speed updates based on player input (O/L and Up/Down with Right Shift)
void UpdatePaddleComputer(Paddle *paddle);   // Paddle speed updates based on Computer AI
void UpdateBall(Ball *ball);                 // Moves the ball based on its direction, and normalizes its speed

// Draw
void DrawPongFrame(void); // Draws all the game's objects for the current frame

// Game functions
void ResetBall(Ball *ball); // Reset the ball's horizontal position and modify its vertical position and angle

#endif // PONG_GAME_HEADER_GUARD
