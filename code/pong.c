#include "raylib.h"
#include "raymath.h" // needed for vector math
#include "config.h"
#include "pong.h"

// See header for data type and function descriptions

GameState InitGameState(void)
{
    GameState state =
    {
        { // ball
            { // position x, y
                RENDER_WIDTH / 2 - BALL_SIZE / 2,
                RENDER_HEIGHT / 2 - BALL_SIZE / 2,
            },
            { // direction x, y
                (float)((GetRandomValue(0, 1) * 2 - 1) * 10), // randomly move towards left or right player
                (float)GetRandomValue(-10, 10),
            },
            BALL_SPEED, // speed
            BALL_SIZE, // size
        },

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
        0,                 // gameMode (selected by player)
        SCREEN_LOGO,       // currentScreen
        DIFFICULTY_MEDIUM, // difficulty (default used for cpu vs cpu demo mode)
        0, 0,              // scoreL, scoreR
        false,             // playerWon
        false,             // isPaused
        0.0f,              // pauseFade
        0.0f,              // pauseFadeTimeElapsed
        WIN_PAUSE_TIME,    // winTimer
        SCORE_PAUSE_TIME,  // scoreTimer
        false,             // gameShouldExit
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
        paddle->position.y = (float)RENDER_HEIGHT - paddle->length;
}

void BounceBallEdge(GameState *pong)
{
    int leftEdgeCollide = (pong->ball.position.x <= 0);
    int rightEdgeCollide = (pong->ball.position.x + pong->ball.size >= RENDER_WIDTH);
    int topEdgeCollide = (pong->ball.position.y <= 0);
    int bottomEdgeCollide = (pong->ball.position.y + pong->ball.size >= RENDER_HEIGHT);

    if (leftEdgeCollide && pong->ball.direction.x < 0)
    {
        if (pong->playerWon)
        {
            pong->ball.direction.x *= -1; // Bounce
            pong->ball.position.x = 0; // Don't get stuck
        }
        else
        {
            pong->scoreR += 1;
            pong->scoreTimer = SCORE_PAUSE_TIME;
            if (pong->scoreR != WIN_SCORE)
                ResetBall(&pong->ball);
        }
    }
    if (rightEdgeCollide && pong->ball.direction.x > 0)
    {
        if (pong->playerWon)
        {
        pong->ball.direction.x *= -1;
        pong->ball.position.x = (float)RENDER_WIDTH - pong->ball.size;
        }
        else
        {
            pong->scoreL += 1;
            pong->scoreTimer = SCORE_PAUSE_TIME;
            if (pong->scoreL != WIN_SCORE)
                ResetBall(&pong->ball);
        }
    }
    if (topEdgeCollide && pong->ball.direction.y < 0)
    {
        pong->ball.direction.y *= -1;
        pong->ball.position.y = 0;
    }
    if (bottomEdgeCollide && pong->ball.direction.y > 0)
    {
        pong->ball.direction.y *= -1;
        pong->ball.position.y = (float)RENDER_HEIGHT - pong->ball.size;
    }
}

void BounceBallPaddle(Ball *ball, Paddle *paddle)
{
    if (CheckCollisionBallPaddle(*ball, *paddle) == false)
        return;

    bool isLeftPaddle = paddle->position.x < RENDER_WIDTH / 2;
    // Position the ball outside the paddle
    if (isLeftPaddle)
    {
        ball->position.x = paddle->position.x + paddle->width + 1;
    }
    else
    {
        ball->position.x = paddle->position.x - ball->size - 1;
    }

    // Increase ball speed
    ball->speed *= BOUNCE_MULTIPLIER;

    // Modify the ball's angle based on where it hit the paddle
    float paddleCenter = paddle->position.y + paddle->length / 2.0f;
    float ballCenter = ball->position.y + ball->size / 2.0f;
    float hitPosition = (ballCenter - paddleCenter) / (paddle->length / 2.0f); // -1 to 1
    float newAngle = hitPosition * PADDLE_HIT_MAX_ANGLE * (PI / 180.0f);

    // Apply new direction
    ball->direction.y = sinf(newAngle);
    ball->direction.x = (isLeftPaddle) ? cosf(newAngle) : -cosf(newAngle);
}

void UpdatePaddlePlayer1(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default

    // W/S to move paddle
    if (IsKeyDown(KEY_W))
        newSpeed = -PADDLE_SPEED;
    if (IsKeyDown(KEY_S))
        newSpeed = PADDLE_SPEED;

    // Left Shift and A/D to speed up
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddlePlayer2(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default

    // I/K or Up/Down arrow keys to move paddle
    if (IsKeyDown(KEY_I) || IsKeyDown(KEY_UP))
        newSpeed = -PADDLE_SPEED;
    if (IsKeyDown(KEY_K) || IsKeyDown(KEY_DOWN))
        newSpeed = PADDLE_SPEED;

    // Left/Right arrow keys, or J/L to speed up
    if (IsKeyDown(KEY_J) || IsKeyDown(KEY_L) ||
        IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddleComputer(Paddle *paddle, Ball *ball, int difficulty)
{
    float newSpeed = 0.0f; // Not moving by default

    // Follow the ball
    if ((paddle->position.y + paddle->length / 10) > ball->position.y + ball->size)
        newSpeed = -PADDLE_SPEED;
    if ((paddle->position.y + paddle->length - paddle-> length / 10) < ball->position.y)
        newSpeed = PADDLE_SPEED;

    // Update Paddle
    float xdiff = paddle->position.x - ball->position.x;
    float distanceToBall = fabsf(xdiff);
    // bool isMovingTowards = (xdiff > 0 && ball->direction.x > 0) || (xdiff < 0 && ball->direction.x < 0);
    // (&& isMovingTowards) to stop paddle after it hits ball
    if (distanceToBall < RENDER_WIDTH / 2)
    {
        paddle->speed = newSpeed * difficulty;
        paddle->position.y += paddle->speed * GetFrameTime();
    }

    // // Perfect computer
    // paddle->position.y = ball->position.y;

    // TODO: make computer behavior more interesting/varied
}

void UpdateBall(Ball *ball)
{
    // Set minimum vertical angle for ball
    float speed = Vector2Length(ball->direction);
    float angleRad = MINIMUM_VERTICAL_ANGLE * (PI / 180.0f);
    float minX = speed * sinf(angleRad);

    if (fabsf(ball->direction.x) < minX)
    {
        ball->direction.x = (ball->direction.x >= 0) ? minX : -minX;
        // Recalculate y to preserve speed
        ball->direction.y = (ball->direction.y >= 0) ?
            sqrtf(speed*speed - ball->direction.x * ball->direction.x) :
            -sqrtf(speed*speed - ball->direction.x * ball->direction.x);
    }

    // Normalize direction's speed
    ball->direction = Vector2Scale(Vector2Normalize(ball->direction), ball->speed);

    // Update ball's position based on direction
    Vector2 deltaTimeSpeed = Vector2Scale(ball->direction, GetFrameTime());
    ball->position = Vector2Add(ball->position, deltaTimeSpeed);

}

void UpdatePongFrame(GameState *pong)
{
    if (!pong->isPaused)
    {
        // Get the difficulty multiplier
        // TODO: make better difficulty implementation
        int difficulty = (int)pong->difficulty + 1;

        // Update paddles
        if (pong->gameMode == MODE_ONEPLAYER)
        {
            UpdatePaddlePlayer1(&pong->paddleL);
            UpdatePaddleComputer(&pong->paddleR, &pong->ball, difficulty);
        }
        if (pong->gameMode == MODE_TWOPLAYER)
        {
            UpdatePaddlePlayer1(&pong->paddleL);
            UpdatePaddlePlayer2(&pong->paddleR);
        }
        if (pong->gameMode == MODE_DEMO)
        {
            UpdatePaddleComputer(&pong->paddleL, &pong->ball, difficulty);
            UpdatePaddleComputer(&pong->paddleR, &pong->ball, difficulty);
        }

        // Update ball
        if (pong->scoreTimer <= 0 ||
            pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
            UpdateBall(&pong->ball);

        // Collision logic
        BounceBallEdge(pong);
        if (pong->playerWon == false)
        {
            BounceBallPaddle(&pong->ball, &pong->paddleL);
            BounceBallPaddle(&pong->ball, &pong->paddleR);
        }
        EdgeCollisionPaddle(&pong->paddleL);
        EdgeCollisionPaddle(&pong->paddleR);

        // Check for winner
        if (pong->scoreL >= WIN_SCORE || pong->scoreR >= WIN_SCORE)
            pong->playerWon = true;

        // Press Enter to skip win screen
        if (pong->playerWon == true && (IsKeyPressed(KEY_ENTER)))
            pong->winTimer = 0;

        // Update timers for winning and scoring
        if (pong->scoreTimer > 0)
            pong->scoreTimer -= GetFrameTime();
        if (pong->playerWon && pong->winTimer > 0)
            pong->winTimer -= GetFrameTime();
    }
    else // isPaused
    {
        // Update pause fade animation
        pong->pauseFadeTimeElapsed += GetFrameTime();
        static float fadeIncrement = 0.1 / 10;
        const float animationSpeed = (1.0f/120.0f);
        if (pong->pauseFadeTimeElapsed > animationSpeed)
        {
            pong->pauseFadeTimeElapsed = 0;
            pong->pauseFade += fadeIncrement;
            if (pong->pauseFade >= 1)
                fadeIncrement = -fabsf(fadeIncrement);
            else if (pong->pauseFade <= 0)
                fadeIncrement = fabsf(fadeIncrement);
        }
    }

    // Press Space or P to pause
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_P))
    {
        pong->isPaused = !pong->isPaused;
        pong->pauseFade = 0;
    }

    // Reset game after a player wins
    if (pong->playerWon == true && pong->winTimer <= 0)
    {
        *pong = InitGameState();
        pong->currentScreen = SCREEN_GAMEPLAY;
    }

    // Debug: Press R to reset ball
    // if (IsKeyPressed(KEY_R))
    // {
    //     ResetBall(&pong.ball);
    // }
}

void DrawDottedLine(void)
{
    int dashHeight = 40;
    int spaceHeight = 40;
    int lineWidth = 15;

    // Calculate amount of dashes needed
    int totalSegmentHeight = dashHeight + spaceHeight;
    int totalSegments = (RENDER_HEIGHT - spaceHeight) / totalSegmentHeight;
    int usedHeight = totalSegments * totalSegmentHeight - spaceHeight;
    int offsetY = (RENDER_HEIGHT - usedHeight) / 2;

    for (int i = 0; i < totalSegments; i++)
    {
        int y = offsetY + i * totalSegmentHeight;
        DrawRectangle(RENDER_WIDTH / 2 - lineWidth / 2, y,
                      lineWidth, dashHeight, RAYWHITE);
    }
}

void DrawScores(GameState *pong)
{
    int fontSize = 180;

    const char *scoreLMsg = TextFormat("%i", pong->scoreL);
    int scoreLWidth = MeasureText(scoreLMsg, fontSize);
    int scoreLPosX = RENDER_WIDTH / 4 - scoreLWidth / 2;

    const char *scoreRMsg = TextFormat("%i", pong->scoreR);
    int scoreRWidth = MeasureText(scoreRMsg, fontSize);
    int scoreRPosX = RENDER_WIDTH / 4 * 3 - scoreRWidth / 2;

    int scorePosY = 50;
    DrawText(scoreLMsg, scoreLPosX, scorePosY, fontSize, RAYWHITE);
    DrawText(scoreRMsg, scoreRPosX, scorePosY, fontSize, RAYWHITE);
}

void DrawWinnerMessage(int scoreL, int scoreR)
{
    char *msg = "Winner";
    int fontSize = 100; // this is also the font height because we're using the default font
    int textWidth = MeasureText(msg, fontSize);
    int textPosY = (RENDER_HEIGHT - fontSize) / 4;
    if (scoreL == WIN_SCORE)
    {
        int textPosX = RENDER_WIDTH / 4 - textWidth / 2;
        DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
    }
    if (scoreR == WIN_SCORE)
    {
        int textPosX = RENDER_WIDTH / 4 * 3 - textWidth / 2;
        DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
    }
}

void DrawPongFrame(GameState *pong)
{
    // Draw dotted line down middle
    if (!pong->isPaused)
        DrawDottedLine();

    // Draw ball
    if (pong->scoreTimer <= 0 || pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
        DrawRectangle((int)pong->ball.position.x, (int)pong->ball.position.y,
                      (int)pong->ball.size,       (int)pong->ball.size, RAYWHITE);


    // Draw score
    DrawScores(pong);

    // Draw paddles
    if (pong->playerWon == false)
    {
        DrawRectangle((int)pong->paddleR.position.x, (int)pong->paddleR.position.y,
                      (int)pong->paddleR.width,      (int)pong->paddleR.length, RAYWHITE);
        DrawRectangle((int)pong->paddleL.position.x, (int)pong->paddleL.position.y,
                      (int)pong->paddleL.width,      (int)pong->paddleL.length, RAYWHITE);
    }

    // Draw win message
    if (pong->playerWon)
        DrawWinnerMessage(pong->scoreL, pong->scoreR);

    // Draw pause message
    if (pong->isPaused)
    {
        int textOffset = MeasureText("Paused", SCORE_FONT_SIZE) / 2;
        DrawText("Paused",
                 RENDER_WIDTH / 2 - textOffset,
                 RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2,
                 SCORE_FONT_SIZE,
                 Fade(RAYWHITE, pong->pauseFade));
    }
}

void ResetBall(Ball *ball)
{
    // Return to center, but keep previous vertical position
    ball->position.x = ((float)RENDER_WIDTH - ball->size) / 2.0f;

    // Change the ball's return position and angle a bit
    ball->position.y += GetRandomValue(-RETURN_POSITION_VARIATION, RETURN_POSITION_VARIATION);
    ball->direction.y += GetRandomValue(-RETURN_ANGLE_VARIATION, RETURN_ANGLE_VARIATION);
    ball->speed = BALL_SPEED;
}
