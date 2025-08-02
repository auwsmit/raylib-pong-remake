#ifndef TEST_GAME_HEADER_GUARD
#define TEST_GAME_HEADER_GUARD

#include "config.h"

// Macros
#define WIN_SCORE 5 // Score needed to win

#define PADDLE_LENGTH 150 // Initial settings for paddles
#define PADDLE_WIDTH 25
#define PADDLE_SPEED 600 // Paddle's default speed in pixels per second

#define BALL_SIZE 25 // Initial settings for ball
#define BALL_SPEED 500

// Physics modifiers
#define BOUNCE_MULTIPLIER 1.1f // How much faster the ball gets after hitting a paddle
#define MINIMUM_STEEPNESS 300.0f // The lower this number, the steeper the ball can go
#define PADDLE_SPEED_INFLUENCE 100 // How much the paddle speed affects the ball on hit

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
    Vector2 velocity;
    float speed;
    int size;
} Ball;

typedef struct GameState
{
    int scoreL;
    int scoreR;
    bool playerWonFlag; // set after a player wins
    float winTimer; // countdown after player wins
    float scoreTimer; // countdown after a score
    Paddle paddleL;
    Paddle paddleR;
    Ball ball;
} GameState;

// Prototypes
// ----------------------------------------------------------------------------------
// Core
GameState InitGameState(void); // Initialize game objects and data for the game loop.

// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle); // Check if ball and paddle are colliding.
void EdgeCollisionPaddle(Paddle *paddle); // Paddles collide with screen edges.

void BounceBallEdge(GameState *pong); // Ball bounces off screen edges and updates the score.
void BounceBallPaddle(Ball *ball, Paddle *paddle); // Ball bounces off paddle.

// Update game
void UpdatePaddlePlayer1(Paddle *paddle); // Paddle updates based on player input.
void UpdatePaddlePlayer2(Paddle *paddle); // Paddle updates based on player input.
void UpdatePaddleComputer(Paddle *paddle, Ball *ball); // Paddle updates based on Computer AI.
void UpdateBall(Ball *ball); // Moves the ball based on its velocity, and normalizes its speed.
void UpdatePong(GameState *pong); // Updates all the game's data and objects for the current frame.

// Draw game
void DrawDottedLine(void);
void DrawScores(GameState *pong);
void DrawGame(GameState *pong); // Draws all the game's objects for the current frame.

// Extra / Miscellaneous
void ResetBall(Ball *ball); // Reset only the ball and send it in a random direction.

#endif // TEST_GAME_HEADER_GUARD
