#ifndef TEST_GAME_HEADER_GUARD
#define TEST_GAME_HEADER_GUARD

#include "config.h"

// Macros
#define PADDLE_LENGTH 150 // Initial settings for paddles
#define PADDLE_WIDTH 25
#define PADDLE_SPEED 400 // Speed in pixels per second

#define BALL_SIZE 25 // Initial settings for ball
#define BALL_SPEED 500

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
GameState InitGameState(void);

// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle);
void PaddleEdgeCollision(Paddle *paddle);
void BallEdgeBounce(Ball *ball);

// Updating state
void PlayerUpdatePaddle(Paddle *paddle);
void ComputerUpdatePaddle(Paddle *paddle, Ball *ball);
void UpdatePaddle(Paddle *paddle);
void UpdateBall(Ball *ball);
void UpdatePong(GameState *pong);

void DrawGame(GameState *pong);

// Extra
void ResetBall(Ball *ball);

#endif // TEST_GAME_HEADER_GUARD
