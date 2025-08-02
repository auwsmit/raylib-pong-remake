#include "raylib.h"
#include "raymath.h" // needed for vector math
#include "config.h"
#include "pong.h"

// See header for data type and function descriptions

GameState InitGameState(void)
{
    GameState state = {
        0, 0, // scoreL, scoreR
        false, // playerWonFlag
        5.0f, // winTimer
        0.0f, // scoreTimer
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
                RENDER_WIDTH / 2 - BALL_SIZE / 2,
                RENDER_HEIGHT / 2 - BALL_SIZE / 2,
            },
            { // velocity x, y
                0, 0,
            },
            BALL_SPEED, // speed
            BALL_SIZE, // size
        },
    };
    return state;
}

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

void EdgeCollisionPaddle(Paddle *paddle)
{
    if (paddle->position.y <= 0)
        paddle->position.y = 0; // Don't get stuck
    if (paddle->position.y + paddle->length > RENDER_HEIGHT)
        paddle->position.y = RENDER_HEIGHT - paddle->length;
}

void BounceBallEdge(GameState *pong)
{
    int leftEdgeCollide = (pong->ball.position.x <= 0);
    int rightEdgeCollide = (pong->ball.position.x + pong->ball.size >= RENDER_WIDTH);
    int topEdgeCollide = (pong->ball.position.y <= 0);
    int bottomEdgeCollide = (pong->ball.position.y + pong->ball.size >= RENDER_HEIGHT);

    if (leftEdgeCollide && pong->ball.velocity.x < 0)
    {
        if (pong->playerWonFlag)
        {
            pong->ball.velocity.x *= -1; // Bounce
            pong->ball.position.x = 0; // Don't get stuck
        }
        else
        {
            pong->scoreR += 1;
            pong->scoreTimer = 1.0f;
            if (pong->scoreR != WIN_SCORE)
                ResetBall(&pong->ball);
        }
    }
    if (rightEdgeCollide && pong->ball.velocity.x > 0)
    {
        if (pong->playerWonFlag)
        {
        pong->ball.velocity.x *= -1;
        pong->ball.position.x = RENDER_WIDTH - pong->ball.size;
        }
        else
        {
            pong->scoreL += 1;
            pong->scoreTimer = 1.0f;
            if (pong->scoreL != WIN_SCORE)
                ResetBall(&pong->ball);
        }
    }
    if (topEdgeCollide && pong->ball.velocity.y < 0)
    {
        pong->ball.velocity.y *= -1;
        pong->ball.position.y = 0;
    }
    if (bottomEdgeCollide && pong->ball.velocity.y > 0)
    {
        pong->ball.velocity.y *= -1;
        pong->ball.position.y = RENDER_HEIGHT - pong->ball.size;
    }
}

void BounceBallPaddle(Ball *ball, Paddle *paddle)
{
    if (CheckCollisionBallPaddle(*ball, *paddle) == false)
        return;

    int paddleCenter = paddle->position.y + paddle->length / 2;
    int ballCenter = ball->position.y + ball->size / 2;
    float hitPosition =
        (float)(ballCenter - paddleCenter) / (paddle->length / 2);

    // Bounce off paddle
    if (paddle->position.x < ball->position.x)
    {
        ball->position.x = paddle->position.x + paddle->width + 1;
        ball->velocity.x = fabs(ball->velocity.x);
    }
    else
    {
        ball->position.x = paddle->position.x - ball->size - 1;
        ball->velocity.x = -fabs(ball->velocity.x);
    }

    // Increase ball speed
    ball->speed *= BOUNCE_MULTIPLIER;

    // Calculate the new angle based on hit position
    float maxAngle = 40.0f;
    float angleInDegrees = hitPosition * maxAngle;
    float angleInRadians = angleInDegrees * PI / 180.0f;

    // Set the new velocity components based on the angle
    float speed = ball->speed;
    ball->velocity.x = cosf(angleInRadians) * speed;
    ball->velocity.y = sinf(angleInRadians) * speed;

    // Ensure correct horizontal direction
    ball->velocity.x = (ball->velocity.x > 0 && paddle->position.x >= ball->position.x) ?
        -ball->velocity.x : ball->velocity.x;
    ball->velocity.x = (ball->velocity.x < 0 && paddle->position.x < ball->position.x) ?
        -ball->velocity.x : ball->velocity.x;

    // Change angle based on the velocity of the paddle
    // ball->velocity.y += (paddle->speed * GetFrameTime()) * PADDLE_SPEED_INFLUENCE;
    ball->velocity.y += (PADDLE_SPEED * GetFrameTime()) * PADDLE_SPEED_INFLUENCE;

    // Normalize speed
    ball->velocity = Vector2Scale(Vector2Normalize(ball->velocity),
                               Vector2Length(ball->velocity));

    // Debug
    // TraceLog(LOG_INFO, "cpu paddle speed: %f\n", paddle->speed);
}

void UpdatePaddlePlayer1(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default

    // W/S to move paddle
    if (IsKeyDown(KEY_W))
        newSpeed = -PADDLE_SPEED;
    if (IsKeyDown(KEY_S))
        newSpeed = PADDLE_SPEED;

    // Left Shift to speed up
    if (IsKeyDown(KEY_LEFT_SHIFT))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddlePlayer2(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default

    // O/L or Up/Down to move paddle
    if (IsKeyDown(KEY_O) || IsKeyDown(KEY_UP))
        newSpeed = -PADDLE_SPEED;
    if (IsKeyDown(KEY_L) || IsKeyDown(KEY_DOWN))
        newSpeed = PADDLE_SPEED;

    // Right Shift to speed up
    if (IsKeyDown(KEY_RIGHT_SHIFT))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddleComputer(Paddle *paddle, Ball *ball)
{
    float newSpeed = 0.0f; // Not moving by default

    // Follow the ball
    if ((paddle->position.y + paddle->length / 10) > ball->position.y + ball->size)
        newSpeed = -PADDLE_SPEED * 2;
    if ((paddle->position.y + paddle->length - paddle-> length / 10) < ball->position.y)
        newSpeed = PADDLE_SPEED * 2;

    // Update Paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();

    // perfect AI
    // paddle->position.y = ball->position.y;

    // TODO: a seek position which periodically updates to look more natural, and has difficulty factors
}

void UpdateBall(Ball *ball)
{
    Vector2 deltaTimeSpeed = Vector2Scale(ball->velocity, GetFrameTime());
    ball->position = Vector2Add(ball->position, deltaTimeSpeed);

    // Make sure diagonal movement isn't too steep
    float minimumXspeed = MINIMUM_STEEPNESS;
    if (fabsf(ball->velocity.x) < minimumXspeed) {
        ball->velocity.x = (ball->velocity.x >= 0) ?
            minimumXspeed : -minimumXspeed;
    }

    // Normalize velocity
    ball->velocity = Vector2Scale(Vector2Normalize(ball->velocity), ball->speed);
}

void UpdatePong(GameState *pong)
{
    // Update position
    UpdatePaddlePlayer1(&pong->paddleR);
    // UpdatePaddlePlayer2(&pong->paddleR);
    UpdatePaddleComputer(&pong->paddleL, &pong->ball);

    if (pong->scoreTimer <= 0 ||
        pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
        UpdateBall(&pong->ball);

    // Collision logic
    BounceBallEdge(pong);
    if (pong->playerWonFlag == false)
    {
        BounceBallPaddle(&pong->ball, &pong->paddleL);
        BounceBallPaddle(&pong->ball, &pong->paddleR);
    }
    EdgeCollisionPaddle(&pong->paddleL);
    EdgeCollisionPaddle(&pong->paddleR);

    // Check for winner
    if (pong->scoreL >= WIN_SCORE || pong->scoreR >= WIN_SCORE)
        pong->playerWonFlag = true;

    // Update timers for winning and scoring
    if (pong->scoreTimer > 0)
        pong->scoreTimer -= GetFrameTime();
    if (pong->playerWonFlag && pong->winTimer > 0)
        pong->winTimer -= GetFrameTime();

    // Reset game after a player wins
    if (pong->playerWonFlag == true && pong->winTimer <= 0)
        *pong = InitGameState();
}

void DrawDottedLine(void)
{
    int dashHeight = 40;
    int spaceHeight = 40;
    int lineWidth = 15;

    for (int y = 0; y < RENDER_HEIGHT; y += dashHeight + spaceHeight) {
        DrawRectangle(RENDER_WIDTH / 2 - lineWidth / 2, y, lineWidth, dashHeight, WHITE);
    }
}

void DrawScores(GameState *pong)
{
    int fontSize = 180;

    const char *scoreLmsg = TextFormat("%i", pong->scoreL);
    int scoreLwidth = MeasureText(scoreLmsg, fontSize);
    int scoreLposX = RENDER_WIDTH / 4 - scoreLwidth / 2;

    const char *scoreRmsg = TextFormat("%i", pong->scoreR);
    int scoreRwidth = MeasureText(scoreRmsg, fontSize);
    int scoreRposX = 3 * RENDER_WIDTH / 4 - scoreRwidth / 2;

    int scorePosY = 50;
    DrawText(scoreLmsg, scoreLposX, scorePosY, fontSize, RAYWHITE);
    DrawText(scoreRmsg, scoreRposX, scorePosY, fontSize, RAYWHITE);
}

void DrawGame(GameState *pong)
{
    // Draw dotted line down middle
    DrawDottedLine();

    // Draw score
    DrawScores(pong);

    // Draw paddles
    if (pong->playerWonFlag == false)
    {
        DrawRectangle(pong->paddleR.position.x, pong->paddleR.position.y,
                      pong->paddleR.width, pong->paddleR.length, WHITE);
        DrawRectangle(pong->paddleL.position.x, pong->paddleL.position.y,
                      pong->paddleL.width, pong->paddleL.length, WHITE);
    }

    // Draw ball
    if (pong->scoreTimer <= 0 || pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
        DrawRectangle(pong->ball.position.x, pong->ball.position.y,
                      pong->ball.size, pong->ball.size, WHITE);
}

void ResetBall(Ball *ball)
{
    float horizontalCenter = RENDER_WIDTH / 2 - ball->size / 2;
    ball->position.x = horizontalCenter;
    ball->speed = BALL_SPEED;
}
