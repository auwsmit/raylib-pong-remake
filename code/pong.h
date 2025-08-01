#ifndef TEST_GAME_HEADER_GUARD
#define TEST_GAME_HEADER_GUARD

#include "config.h"

// Macros
#define PADDLE_LENGTH 150 // Initial settings for paddles
#define PADDLE_WIDTH 25
#define PADDLE_SPEED 500 // Paddle's default speed in pixels per second

#define BALL_SIZE 25 // Initial settings for ball
#define BALL_SPEED 800

// Types and Structures
typedef struct Paddle
{
    Vector2 position;
    float speed;
    int length;
    int width;
} Paddle;

typedef struct Ball
{
    Vector2 position;
    Vector2 speed;
    int size;
} Ball;

typedef struct GameState
{
    Paddle paddleL;
    Paddle paddleR;
    Ball ball;
} GameState;

// Prototypes
//----------------------------------------------------------------------------------
// Core
GameState InitGameState(void); // Initialize game objects and data for the game loop.

// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle); // Check if ball and paddle are colliding.
void EdgeCollisionPaddle(Paddle *paddle); // Paddles collide with screen edges.
void BounceBallEdge(Ball *ball); // Ball bounces off screen edges.
void BounceBallPaddle(Ball *ball, Paddle *paddle); // Ball bounces off paddle.

// Update game objects
void UpdatePaddlePlayerInput(Paddle *paddle); // Paddle updates based on player input.
void UpdatePaddleComputerPlayer(Paddle *paddle, Ball *ball); // Paddle updates based on CPU AI.
void UpdatePaddle(Paddle *paddle); // Moves the paddle based on its speed.
void UpdateBall(Ball *ball); // Moves the ball based on its speed, and normalizes its speed.
void UpdatePong(GameState *pong); // Updates all the game's objects for the current frame.

// Draw game objects
void DrawGame(GameState *pong); // Draws all the game's objects for the current frame.

// Extra / Miscellaneous
void ResetBall(Ball *ball);

#endif // TEST_GAME_HEADER_GUARD
