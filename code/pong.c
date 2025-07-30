#include "raylib.h"
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
            PADDLE_LENGTH, // length
            PADDLE_WIDTH, // width
            PADDLE_SPEED, // speed
        },
        { // paddleR
            { // position x, y
                RENDER_WIDTH - PADDLE_WIDTH * 2.5,
                RENDER_HEIGHT / 2,
            },
            PADDLE_LENGTH, // length
            PADDLE_WIDTH, // width
            PADDLE_SPEED, // speed
        },
    };
    return state;
}

bool CheckScreenEdgeCollision(Paddle paddle)
{
    if (paddle.position.y <= 0)
        return true;
    if (paddle.position.y + paddle.length > RENDER_HEIGHT)
        return true;

    return false;
}

void UpdatePaddle(Paddle *paddle)
{
    // Shift to speed up
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        paddle->speed = deltaTime(PADDLE_SPEED * 2);
    else
        paddle->speed = deltaTime(PADDLE_SPEED);

    // W/S or Up/Down to move paddle
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        paddle->position.y -= (paddle->speed);
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        paddle->position.y += (paddle->speed);

    // TODO: mouse and gamepad

    if (CheckScreenEdgeCollision(*paddle))
    {
        if ((paddle->position.y + paddle->length / 2) < RENDER_HEIGHT / 2)
            paddle->position.y = 0;
        else
            paddle->position.y = RENDER_HEIGHT - paddle->length;
    }
}

void DrawGame(GameState *pong)
{
    // Draw paddles
    DrawRectangle(pong->paddleR.position.x, pong->paddleR.position.y, pong->paddleR.width, pong->paddleR.length, WHITE);
    DrawRectangle(pong->paddleL.position.x, pong->paddleL.position.y, pong->paddleL.width, pong->paddleL.length, WHITE);
}
