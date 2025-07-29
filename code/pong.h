#ifndef TEST_GAME_HEADER_GUARD
#define TEST_GAME_HEADER_GUARD

#include "config.h"

// Macros
#define PADDLE_LENGTH 150 // Initial settings for paddles
#define PADDLE_WIDTH 25
#define PADDLE_SPEED 6

// Types and Structures
typedef struct Paddle
{
    Vector2 position;
    int length;
    int width;
    float speed;
} Paddle;

typedef struct GameState
{
    Paddle paddleL;
    Paddle paddleR;
} GameState;

// Prototypes
GameState InitGameState(void);
bool CheckScreenEdgeCollision(Paddle pong);
void UpdatePaddle(Paddle *paddle);
void DrawGame(GameState *gs);

#endif // TEST_GAME_HEADER_GUARD
