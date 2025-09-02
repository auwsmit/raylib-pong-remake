// EXPLANATION:
// All the game logic, including how/when to draw to screen
// See pong.h for more documentation/descriptions

#include "pong.h"

#include <limits.h> // for SHRT_MAX for beep sound math
#include "raymath.h" // needed for vector math

#include "config.h"
#include "input.h"
#include "ui.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void InitGameState(void)
{
    // Start the ball in any random direction
    float ballStartDirectionX = (float)(GetRandomValue(0, 1) * 2 - 1) * 100; // either -100 or +100
    float ballStartDirectionY = (float)GetRandomValue(-100, 100);
    GameState defaultState =
    {
        // Game boots to raylib logo animation
        .currentScreen = SCREEN_LOGO,

        // Default ball settings
        .ball = {
            .position = {
                RENDER_WIDTH / 2 - BALL_SIZE / 2,
                RENDER_HEIGHT / 2 - BALL_SIZE / 2,
            },
            .direction = {
                ballStartDirectionX,
                ballStartDirectionY,
            },
            .speed = BALL_SPEED,
            .size = BALL_SIZE,
        },

        // Default paddle settings
        .paddleL = {
            .position = {
                PADDLE_WIDTH * 1.5,
                RENDER_HEIGHT / 2,
            },
            .speed = PADDLE_SPEED,
            .length = PADDLE_LENGTH,
            .width = PADDLE_WIDTH,
        },
        .paddleR = {
            .position = {
                RENDER_WIDTH - PADDLE_WIDTH * 2.5,
                RENDER_HEIGHT / 2,
            },
            .speed = PADDLE_SPEED,
            .length = PADDLE_LENGTH,
            .width = PADDLE_WIDTH,
        },

        // Default game settings
        .difficulty = DIFFICULTY_MEDIUM, // only relevant for demo mode (MODE_DEMO)
        .winTimer   = WIN_PAUSE_TIME,
        .scoreTimer = SCORE_PAUSE_TIME,
    };

    // Allocate memory for beep sine waves
    defaultState.beeps[BEEP_MENU] = GenBeep(200.0f, 0.03f);
    defaultState.beeps[BEEP_PADDLE] = GenBeep(450.0f, 0.1f);
    defaultState.beeps[BEEP_EDGE] = GenBeep(500.0f, 0.1f);
    defaultState.beeps[BEEP_SCORE] = GenBeep(600.0f, 0.4f);

    pongGame = defaultState;
}

Sound GenBeep(float freq, float lengthSec)
{
    int sampleRate = 44100;
    int samples = (int)(lengthSec * sampleRate);
    short *data = MemAlloc(samples * sizeof(short));

    // fade length in samples
    // (This prevents an unpleasant "pop" noise when the sound starts or stops)
    int fadeSamples = (int)(0.005f * sampleRate); // 5 ms

    // Generate wave data
    for (int i = 0; i < samples; i++)
    {
        float timeInSeconds = (float)i / sampleRate;
        float sample = sinf(2.0f * PI * freq * timeInSeconds);

        // Apply fade in/out
        float amplitude = 1.0f;
        if (i < fadeSamples) {
            amplitude = (float)i / fadeSamples; // fade in
        } else if (i > samples - fadeSamples) {
            amplitude = (float)(samples - i) / fadeSamples; // fade out
        }

        data[i] = (short)(sample * amplitude * SHRT_MAX * 0.25f);
    }

    Wave beepSoundWave = {
        .frameCount = samples,
        .sampleRate = sampleRate,
        .sampleSize = 16,
        .channels = 1,
        .data = data
    };

    Sound beep = LoadSoundFromWave(beepSoundWave);
    UnloadWave(beepSoundWave); // frees data
    return beep;
}

void FreeBeeps(void)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(pongGame.beeps); i++)
        UnloadSound(pongGame.beeps[i]);
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
    if (paddle->position.y <= FIELD_LINE_WIDTH)
        paddle->position.y = FIELD_LINE_WIDTH; // Don't get stuck
    if (paddle->position.y + paddle->length > RENDER_HEIGHT - FIELD_LINE_WIDTH)
        paddle->position.y = (float)(RENDER_HEIGHT - FIELD_LINE_WIDTH - paddle->length);
}

void BounceBallEdge(Ball *ball)
{
    bool leftEdgeCollide = (ball->position.x <= 0);
    bool rightEdgeCollide = (ball->position.x + ball->size >= RENDER_WIDTH);
    bool topEdgeCollide = (ball->position.y <= FIELD_LINE_WIDTH);
    bool bottomEdgeCollide = (ball->position.y + ball->size >= RENDER_HEIGHT - FIELD_LINE_WIDTH);

    if (leftEdgeCollide && ball->direction.x < 0)
    {
        if (pongGame.playerWon)
        {
            ball->direction.x *= -1; // Bounce
            ball->position.x = 0; // Don't get stuck
        }
        else
        {
            pongGame.scoreR += 1;
            pongGame.scoreTimer = SCORE_PAUSE_TIME;
            if (pongGame.scoreR != WIN_SCORE)
                ResetBall(&pongGame.ball);
        }
    }
    if (rightEdgeCollide && ball->direction.x > 0)
    {
        if (pongGame.playerWon)
        {
        ball->direction.x *= -1;
        ball->position.x = (float)RENDER_WIDTH - ball->size;
        }
        else
        {
            pongGame.scoreL += 1;
            pongGame.scoreTimer = SCORE_PAUSE_TIME;
            if (pongGame.scoreL != WIN_SCORE)
                ResetBall(&pongGame.ball);
        }
    }
    if (topEdgeCollide && ball->direction.y < 0)
    {
        ball->direction.y *= -1;
        ball->position.y = FIELD_LINE_WIDTH;
    }
    if (bottomEdgeCollide && ball->direction.y > 0)
    {
        ball->direction.y *= -1;
        ball->position.y = (float)RENDER_HEIGHT - ball->size - FIELD_LINE_WIDTH;
    }

    if (leftEdgeCollide || rightEdgeCollide || topEdgeCollide || bottomEdgeCollide)
    {
        if (topEdgeCollide || bottomEdgeCollide || pongGame.playerWon)
            PlaySound(pongGame.beeps[BEEP_EDGE]);
        else if (leftEdgeCollide || rightEdgeCollide)
            PlaySound(pongGame.beeps[BEEP_SCORE]);
    }
}

void BounceBallPaddle(Ball *ball, Paddle *paddle)
{
    if (CheckCollisionBallPaddle(*ball, *paddle) == false)
        return; // Do nothing if ball isn't colliding with paddle

    bool ballMovingLeft = ball->direction.x < 0;

    // Position the ball outside the paddle
    if (ballMovingLeft)
    {
        ball->position.x = paddle->position.x + paddle->width + 1;
    }
    else
    {
        ball->position.x = paddle->position.x - ball->size - 1;
    }

    // Set a new hit position for the potential computer paddle
    paddle->nextHitPos = (float)GetRandomValue(0, paddle->length/2);

    // Increase ball speed
    ball->speed *= BOUNCE_MULTIPLIER;

    // Modify the ball's angle based on where it hit the paddle
    float paddleCenter = paddle->position.y + paddle->length / 2.0f;
    float ballCenter = ball->position.y + ball->size / 2.0f;
    float hitPosition = (ballCenter - paddleCenter) / (paddle->length / 2.0f); // -1 to 1
    float newAngle = hitPosition * PADDLE_HIT_MAX_ANGLE * (PI / 180.0f);

    // Apply new direction
    ball->direction.y = sinf(newAngle);
    ball->direction.x = (ballMovingLeft) ? cosf(newAngle) : -cosf(newAngle);

    PlaySound(pongGame.beeps[BEEP_PADDLE]);
}

void UpdatePongFrame(void)
{
    // Input to go back to title screen
    if (IsInputActionPressed(INPUT_ACTION_BACK) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        ChangeUiMenu(UI_MENU_TITLE);
        return; // back to main game loop: UpdateDrawFrame()
    }

    // Input to pause
    if (IsInputActionPressed(INPUT_ACTION_PAUSE))
    {
        pongGame.isPaused = !pongGame.isPaused;
        if (pongGame.isPaused)
            ChangeUiMenu(UI_MENU_PAUSE);
        else
            pongUi.currentMenu = UI_MENU_GAMEPLAY;
        PlaySound(pongGame.beeps[BEEP_MENU]);
    }

    if (!pongGame.isPaused)
    {
        // Update paddles (checks player Input)
        if (pongGame.currentMode == MODE_1PLAYER)
        {
            UpdatePaddlePlayer1(&pongGame.paddleL);
            UpdatePaddleMouseInput(&pongGame.paddleL);
            UpdatePaddleComputer(&pongGame.paddleR);
        }
        if (pongGame.currentMode == MODE_2PLAYER)
        {
            UpdatePaddlePlayer1(&pongGame.paddleL);
            UpdatePaddlePlayer2(&pongGame.paddleR);
        }
        if (pongGame.currentMode == MODE_DEMO)
        {
            UpdatePaddleComputer(&pongGame.paddleL);
            UpdatePaddleComputer(&pongGame.paddleR);
        }

        // Update ball
        if (pongGame.playerWon && (pongGame.ball.speed < BALL_SPEED * 4))
            pongGame.ball.speed = BALL_SPEED * 4;

        if (pongGame.scoreTimer <= 0 ||
            pongGame.scoreR == WIN_SCORE || pongGame.scoreL == WIN_SCORE)
            UpdateBall(&pongGame.ball);

        // Collision logic
        BounceBallEdge(&pongGame.ball);
        if (pongGame.playerWon == false)
        {
            BounceBallPaddle(&pongGame.ball, &pongGame.paddleL);
            BounceBallPaddle(&pongGame.ball, &pongGame.paddleR);
        }
        EdgeCollisionPaddle(&pongGame.paddleL);
        EdgeCollisionPaddle(&pongGame.paddleR);

        // Check for winner
        if (pongGame.scoreL >= WIN_SCORE || pongGame.scoreR >= WIN_SCORE)
            pongGame.playerWon = true;

        // Input to skip win screen
        if (pongGame.playerWon == true &&
            (IsInputActionPressed(INPUT_ACTION_CONFIRM) || IsGestureDetected(GESTURE_TAP)))
            pongGame.winTimer = 0;

        // Update timers for winning and scoring
        if (pongGame.scoreTimer > 0)
            pongGame.scoreTimer -= GetFrameTime();
        if (pongGame.playerWon && pongGame.winTimer > 0)
            pongGame.winTimer -= GetFrameTime();
    }

    // Reset game after a player wins
    if (pongGame.playerWon == true && pongGame.winTimer <= 0)
    {
        GameMode prevMode = pongGame.currentMode;
        GameDifficulty prevDifficulty = pongGame.difficulty;
        FreeBeeps();
        InitGameState();
        pongGame.currentScreen = SCREEN_GAMEPLAY;
        pongGame.currentMode = prevMode;
        pongGame.difficulty = prevDifficulty;
    }

    // Update user interface elements and logic
    UpdateUiFrame();
}

void UpdatePaddlePlayer1(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default

    // Input to move paddle
    if (IsInputActionDown(INPUT_ACTION_P1_UP))
        newSpeed = -PADDLE_SPEED;
    if (IsInputActionDown(INPUT_ACTION_P1_DOWN))
        newSpeed = PADDLE_SPEED;

    // Left Shift and A/D to speed up
    if (IsInputActionDown(INPUT_ACTION_P1_SPEED))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddlePlayer2(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default

    // Input to move paddle
    if (IsInputActionDown(INPUT_ACTION_P2_UP))
        newSpeed = -PADDLE_SPEED;
    if (IsInputActionDown(INPUT_ACTION_P2_DOWN))
        newSpeed = PADDLE_SPEED;

    // Left Shift and A/D to speed up
    if (IsInputActionDown(INPUT_ACTION_P2_SPEED))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddleMouseInput(Paddle *paddle)
{
    Vector2 scaleFactor = { (float)RENDER_WIDTH / GetScreenWidth(),
                            (float)RENDER_HEIGHT / GetScreenHeight() };
    Vector2 mousePos = Vector2Multiply(GetMousePosition(), scaleFactor);

    // Only move if the mouse moved and if no keyboard input was detected
    if (Vector2Length(GetMouseDelta()) > 0 && paddle->speed == 0)
        paddle->position.y = mousePos.y - paddle->length / 2;
}

void UpdatePaddleComputer(Paddle *paddle)
{
    float newSpeed = 0.0f; // Not moving by default
    Ball *ball = &pongGame.ball;

    // Follow the ball
    if ((paddle->position.y + paddle->nextHitPos) > ball->position.y + ball->size)
        newSpeed = -PADDLE_SPEED;
    if ((paddle->position.y + paddle->length - paddle->nextHitPos) < ball->position.y)
        newSpeed = PADDLE_SPEED;


    // Update Paddle
    bool ballMovingLeft = ball->direction.x < 0;
    bool paddleIsLeft = paddle->position.x < RENDER_WIDTH / 2;
    bool movingTowardsPaddle = ((paddleIsLeft && ballMovingLeft) ||
                                (!paddleIsLeft && !ballMovingLeft));
    float distanceToBall = fabsf(paddle->position.x - ball->position.x);
    float ballIsHalfway = (float)(distanceToBall < RENDER_WIDTH/2 - ball->size*2);

    if (ballIsHalfway)
    {
        GameDifficulty diff = pongGame.difficulty;
        paddle->speed = newSpeed * (diff + 1);

        // Tweak/adjust difficulty speeds
        if (diff == DIFFICULTY_EASY)
            paddle->speed += paddle->speed * 0.30f;
        // else if (diff == DIFFICULTY_MEDIUM)
        //     paddle->speed = paddle->speed * 0.05f;
        else if (diff == DIFFICULTY_HARD)
            paddle->speed -= paddle->speed * 0.10f;

        // Move slower after hitting ball
        if (!movingTowardsPaddle && (distanceToBall < RENDER_WIDTH / 8))
            paddle->speed /= 4;
        else if (!movingTowardsPaddle && (distanceToBall < RENDER_WIDTH / 4))
            paddle->speed /= 2;

        paddle->position.y += paddle->speed * GetFrameTime();
    }

    // // Perfect computer
    // paddle->position.y = pongGame.ball.position.y;

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

void DrawPongFrame(void)
{
    // Draw ball
    if (pongGame.scoreTimer <= 0 || pongGame.scoreR == WIN_SCORE || pongGame.scoreL == WIN_SCORE)
        DrawRectangle((int)pongGame.ball.position.x, (int)pongGame.ball.position.y,
                      (int)pongGame.ball.size,       (int)pongGame.ball.size, RAYWHITE);


    // Draw paddles
    if (pongGame.playerWon == false)
    {
        DrawRectangle((int)pongGame.paddleR.position.x, (int)pongGame.paddleR.position.y,
                      (int)pongGame.paddleR.width,      (int)pongGame.paddleR.length, RAYWHITE);
        DrawRectangle((int)pongGame.paddleL.position.x, (int)pongGame.paddleL.position.y,
                      (int)pongGame.paddleL.width,      (int)pongGame.paddleL.length, RAYWHITE);
    }

    // Draw difficulty mode text in lower right
    if (pongGame.currentMode == MODE_1PLAYER)
    {
        const char *difficultyText;
        switch (pongGame.difficulty)
        {
            case DIFFICULTY_EASY:
                difficultyText = "Difficulty Easy";
                break;
            case DIFFICULTY_MEDIUM:
                difficultyText = "Difficulty Medium";
                break;
            case DIFFICULTY_HARD:
                difficultyText = "Difficulty Hard";
                break;
        }

        int diffTextLength = MeasureText(difficultyText, DIFFICULTY_FONT_SIZE);
        DrawText(difficultyText,
                 RENDER_WIDTH / 4 * 3 - diffTextLength / 2,
                 RENDER_HEIGHT - (DIFFICULTY_FONT_SIZE * 2),
                 DIFFICULTY_FONT_SIZE, RAYWHITE);
    }

    // Draw user interface elements
    DrawUiFrame();
}

void ResetBall(Ball *ball)
{
    // Return to center, but keep previous vertical position
    ball->position.x = ((float)RENDER_WIDTH - ball->size) / 2.0f;

    // Change the ball's return position and angle a bit
    ball->position.y += GetRandomValue(-RETURN_POSITION_VARIATION, RETURN_POSITION_VARIATION);
    ball->direction.y += GetRandomValue(-RETURN_ANGLE_VARIATION, RETURN_ANGLE_VARIATION);

    // Don't get stuck in the wall
    if (ball->position.y <= FIELD_LINE_WIDTH)
        ball->position.y = (float)(FIELD_LINE_WIDTH + ball->size);
    else if (ball->position.y >= RENDER_HEIGHT - FIELD_LINE_WIDTH)
        ball->position.y = (float)(RENDER_HEIGHT - FIELD_LINE_WIDTH - ball->size*2);
    ball->speed = BALL_SPEED;
}
