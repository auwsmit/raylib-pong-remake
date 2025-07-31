#include "raylib.h"
#include "raymath.h" // needed for vector math
#include "config.h"
#include "pong.h"

GameState InitGameState(void)
{
    GameState state = {
        { // paddleL
            { // position x, y
                PADDLE_WIDTH * 1.5,
                RENDER_HEIGHT / 2,
            },
            PADDLE_SPEED,  // speed
            PADDLE_LENGTH, // length
            PADDLE_WIDTH,  // width
        },
        { // paddleR
            { // position x, y
                RENDER_WIDTH - PADDLE_WIDTH * 2.5,
                RENDER_HEIGHT / 2,
            },
            PADDLE_SPEED,  // speed
            PADDLE_LENGTH, // length
            PADDLE_WIDTH,  // width
        },

        { // ball
            { // position x, y
                RENDER_WIDTH / 2 + BALL_SIZE / 2,
                RENDER_HEIGHT / 2 + BALL_SIZE / 2,
            },
            { // speed x, y
                BALL_SPEED, BALL_SPEED,
            },
            BALL_SIZE, // size
        },
    };
    return state;
}


// Check collision between two rectangles
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle)
{
    bool collision = false;

    if ((ball.position.x < (paddle.position.x + paddle.width) &&
         (ball.position.x + ball.size) > paddle.position.x) &&
        (ball.position.y < (paddle.position.y + paddle.length) &&
         (ball.position.y + ball.size) > paddle.position.y))
        collision = true;

    return collision;
}

void PaddleEdgeCollision(Paddle *paddle)
{
    if (paddle->position.y <= 0)
        paddle->position.y = 0;
    if (paddle->position.y + paddle->length > RENDER_HEIGHT)
        paddle->position.y = RENDER_HEIGHT - paddle->length;
}

void BallEdgeBounce(Ball *ball)
{
    if (ball->position.x <= 0 || ball->position.x + ball->size / 2 >= RENDER_WIDTH)
    {
        ball->speed.x *= -1; // Reverse horizontal direction
    }

    if (ball->position.y <= 0 || ball->position.y + ball->size / 2 >= RENDER_HEIGHT)
    {
        ball->speed.y *= -1; // Reverse vertical direction
    }
}

void PlayerUpdatePaddle(Paddle *paddle)
{
    float deltaTime = GetFrameTime();

    // Shift to speed up
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        paddle->speed = (PADDLE_SPEED * 2) * deltaTime;
    else
        paddle->speed = (PADDLE_SPEED) * deltaTime;

    // W/S or Up/Down to move paddle
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        paddle->position.y -= (paddle->speed);
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        paddle->position.y += (paddle->speed);

    // TODO: mouse and gamepad

}

void ComputerUpdatePaddle(Paddle *paddle, Ball *ball)
{
    float deltaTime = GetFrameTime();
    int paddleCenter = paddle->position.y + paddle->length / 2;
    int ballCenter = ball->position.y + ball->size / 2;

    if (paddleCenter > ballCenter)
        paddle->position.y -= (paddle->speed) * deltaTime;
    else
        paddle->position.y += (paddle->speed) * deltaTime;

    // TODO more AI behavior
}

void UpdateBall(Ball *ball)
{
    Vector2 speed = Vector2Scale(ball->speed, GetFrameTime());
    ball->position = Vector2Add(ball->position, speed);
}

void ResetBall(Ball *ball)
{
    Vector2 center = { RENDER_HEIGHT / 2, RENDER_HEIGHT / 2 };
    ball->position = center;
}

void UpdatePong(GameState *pong)
{
    // Update position
    PlayerUpdatePaddle(&pong->paddleR);
    ComputerUpdatePaddle(&pong->paddleL, &pong->ball);
    UpdateBall(&pong->ball);

    // Collision logic
    BallEdgeBounce(&pong->ball);
    if (CheckCollisionBallPaddle(pong->ball, pong->paddleR) ||
        CheckCollisionBallPaddle(pong->ball, pong->paddleL))
        pong->ball.speed.x *= -1; // Bounce off paddles

    PaddleEdgeCollision(&pong->paddleR);
    PaddleEdgeCollision(&pong->paddleL);
}

void DrawGame(GameState *pong)
{
    // Draw paddles
    DrawRectangle(pong->paddleR.position.x, pong->paddleR.position.y, pong->paddleR.width, pong->paddleR.length, WHITE);
    DrawRectangle(pong->paddleL.position.x, pong->paddleL.position.y, pong->paddleL.width, pong->paddleL.length, WHITE);

    // Draw ball
    DrawRectangle(pong->ball.position.x, pong->ball.position.y, pong->ball.size, pong->ball.size, WHITE);
}
