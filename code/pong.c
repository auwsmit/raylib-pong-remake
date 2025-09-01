// EXPLANATION:
// All the game logic, including how/when to draw to screen
// See pong.h for more documentation/descriptions

#include "pong.h"

#include <limits.h> // for SHRT_MAX for beep sound
#include "raymath.h" // needed for vector math

#include "config.h"
#include "input.h"
#include "ui.h" // needed to reset the title menu

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

GameState InitGameState(void)
{
    // Set up default input key controls
    InputKeyMaps keyMaps = InitInputKeyMaps();

    // Start the ball in any random direction
    float ballStartDirectionX = (float)(GetRandomValue(0, 1) * 2 - 1) * 100; // either -100 or +100
    float ballStartDirectionY = (float)GetRandomValue(-100, 100);
    GameState pong =
    {
        .currentScreen = SCREEN_LOGO,
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

        .paddleL = {
            .position = {
                PADDLE_WIDTH * 1.5,
                RENDER_HEIGHT / 2,
            },
            .nextHitPos = 0.0f,
            .speed = PADDLE_SPEED,
            .length = PADDLE_LENGTH,
            .width = PADDLE_WIDTH,
        },

        .paddleR = {
            .position = {
                RENDER_WIDTH - PADDLE_WIDTH * 2.5,
                RENDER_HEIGHT / 2,
            },
            .nextHitPos = 0.0f,
            .speed = PADDLE_SPEED,
            .length = PADDLE_LENGTH,
            .width = PADDLE_WIDTH,
        },

        .input = keyMaps,
        .difficulty = DIFFICULTY_MEDIUM,
        .winTimer   = WIN_PAUSE_TIME,
        .scoreTimer = SCORE_PAUSE_TIME,
    };



    // Allocate memory for beep sine waves
    pong.beeps[BEEP_MENU] = GenBeep(200.0f, 0.03f);
    pong.beeps[BEEP_PADDLE] = GenBeep(450.0f, 0.1f);
    pong.beeps[BEEP_EDGE] = GenBeep(500.0f, 0.1f);
    pong.beeps[BEEP_SCORE] = GenBeep(600.0f, 0.4f);

    return pong;
}

Sound GenBeep(float freq, float lengthSec)
{
    int sampleRate = 44100;
    int samples = (int)(lengthSec * sampleRate);
    short *data = MemAlloc(samples * sizeof(short));

    // fade length in samples (5 ms)
    int fadeSamples = (int)(0.005f * sampleRate);

    for (int i = 0; i < samples; i++)
    {
        float timeInSeconds = (float)i / sampleRate;
        float sample = sinf(2.0f * PI * freq * timeInSeconds);

        // envelope factor
        float amp = 1.0f;
        if (i < fadeSamples) {
            amp = (float)i / fadeSamples; // fade in
        } else if (i > samples - fadeSamples) {
            amp = (float)(samples - i) / fadeSamples; // fade out
        }

        data[i] = (short)(sample * amp * SHRT_MAX * 0.25f);
    }

    Wave w = {
        .frameCount = samples,
        .sampleRate = sampleRate,
        .sampleSize = 16,
        .channels = 1,
        .data = data
    };

    Sound beep = LoadSoundFromWave(w);
    UnloadWave(w); // frees data
    return beep;
}

void FreeBeeps(GameState *pong)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(pong->beeps); i++)
        UnloadSound(pong->beeps[i]);
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

void BounceBallEdge(GameState *pong)
{
    bool leftEdgeCollide = (pong->ball.position.x <= 0);
    bool rightEdgeCollide = (pong->ball.position.x + pong->ball.size >= RENDER_WIDTH);
    bool topEdgeCollide = (pong->ball.position.y <= FIELD_LINE_WIDTH);
    bool bottomEdgeCollide = (pong->ball.position.y + pong->ball.size >= RENDER_HEIGHT - FIELD_LINE_WIDTH);

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
        pong->ball.position.y = FIELD_LINE_WIDTH;
    }
    if (bottomEdgeCollide && pong->ball.direction.y > 0)
    {
        pong->ball.direction.y *= -1;
        pong->ball.position.y = (float)RENDER_HEIGHT - pong->ball.size - FIELD_LINE_WIDTH;
    }

    if (leftEdgeCollide || rightEdgeCollide || topEdgeCollide || bottomEdgeCollide)
    {
        if (topEdgeCollide || bottomEdgeCollide || pong->playerWon)
            PlaySound(pong->beeps[BEEP_EDGE]);
        else if (leftEdgeCollide || rightEdgeCollide)
            PlaySound(pong->beeps[BEEP_SCORE]);
    }
}

void BounceBallPaddle(Ball *ball, Paddle *paddle, Sound *beep)
{
    if (CheckCollisionBallPaddle(*ball, *paddle) == false)
        return;

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

    PlaySound(*beep);
}

void UpdatePongFrame(GameState *pong, UiState *ui)
{
    // Input to go back to title screen
    if (IsInputActionPressed(INPUT_ACTION_BACK, pong) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        ChangeUiMenu(UI_MENU_TITLE, ui, pong);
        return; // back to main game loop
    }

        // Input to pause
        if (IsInputActionPressed(INPUT_ACTION_PAUSE, pong))
        {
            pong->isPaused = !pong->isPaused;
            if (pong->isPaused)
                ChangeUiMenu(UI_MENU_PAUSE, ui, pong);
            else
                ui->currentMenu = UI_MENU_GAMEPLAY;
            PlaySound(pong->beeps[BEEP_MENU]);
        }

    if (!pong->isPaused)
    {
        // Update paddles (checks player Input)
        if (pong->currentMode == MODE_1PLAYER)
        {
            UpdatePaddlePlayer1(&pong->paddleL, pong);
            UpdatePaddleMouseInput(&pong->paddleL);
            UpdatePaddleComputer(&pong->paddleR, pong);
        }
        if (pong->currentMode == MODE_2PLAYER)
        {
            UpdatePaddlePlayer1(&pong->paddleL, pong);
            UpdatePaddlePlayer2(&pong->paddleR, pong);
        }
        if (pong->currentMode == MODE_DEMO)
        {
            UpdatePaddleComputer(&pong->paddleL, pong);
            UpdatePaddleComputer(&pong->paddleR, pong);
        }

        // Update ball
        if (pong->playerWon && (pong->ball.speed < BALL_SPEED * 4))
            pong->ball.speed = BALL_SPEED * 4;

        if (pong->scoreTimer <= 0 ||
            pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
            UpdateBall(&pong->ball);

        // Collision logic
        BounceBallEdge(pong);
        if (pong->playerWon == false)
        {
            BounceBallPaddle(&pong->ball, &pong->paddleL, &pong->beeps[BEEP_PADDLE]);
            BounceBallPaddle(&pong->ball, &pong->paddleR, &pong->beeps[BEEP_PADDLE]);
        }
        EdgeCollisionPaddle(&pong->paddleL);
        EdgeCollisionPaddle(&pong->paddleR);

        // Check for winner
        if (pong->scoreL >= WIN_SCORE || pong->scoreR >= WIN_SCORE)
            pong->playerWon = true;

        // Input to skip win screen
        if (pong->playerWon == true &&
            (IsInputActionPressed(INPUT_ACTION_CONFIRM, pong) || IsGestureDetected(GESTURE_TAP)))
            pong->winTimer = 0;

        // Update timers for winning and scoring
        if (pong->scoreTimer > 0)
            pong->scoreTimer -= GetFrameTime();
        if (pong->playerWon && pong->winTimer > 0)
            pong->winTimer -= GetFrameTime();
    }

    // Reset game after a player wins
    if (pong->playerWon == true && pong->winTimer <= 0)
    {
        GameDifficulty prevDifficulty = pong->difficulty;
        FreeBeeps(pong);
        *pong = InitGameState();
        pong->currentScreen = SCREEN_GAMEPLAY;
        pong->difficulty = prevDifficulty;
    }

    UpdateUiFrame(ui, pong);
}

void UpdatePaddlePlayer1(Paddle *paddle, GameState *pong)
{
    float newSpeed = 0.0f; // Not moving by default

    // Input to move paddle
    if (IsInputActionDown(INPUT_ACTION_P1_UP, pong))
        newSpeed = -PADDLE_SPEED;
    if (IsInputActionDown(INPUT_ACTION_P1_DOWN, pong))
        newSpeed = PADDLE_SPEED;

    // Left Shift and A/D to speed up
    if (IsInputActionDown(INPUT_ACTION_P1_SPEED, pong))
        newSpeed *= 2;

    // Update paddle
    paddle->speed = newSpeed;
    paddle->position.y += paddle->speed * GetFrameTime();
}

void UpdatePaddlePlayer2(Paddle *paddle, GameState *pong)
{
    float newSpeed = 0.0f; // Not moving by default

    // Input to move paddle
    if (IsInputActionDown(INPUT_ACTION_P2_UP, pong))
        newSpeed = -PADDLE_SPEED;
    if (IsInputActionDown(INPUT_ACTION_P2_DOWN, pong))
        newSpeed = PADDLE_SPEED;

    // Left Shift and A/D to speed up
    if (IsInputActionDown(INPUT_ACTION_P2_SPEED, pong))
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
    {
        paddle->position.y = mousePos.y - paddle->length / 2;

        // float distBetweenMousePaddle = fabsf(scaledMousePos.x - paddle->position.x);
        // if (distBetweenMousePaddle < RENDER_WIDTH / 2)
        //     HideCursor();
        // else
        //     ShowCursor();
    }
}

void UpdatePaddleComputer(Paddle *paddle, GameState *pong)
{
    float newSpeed = 0.0f; // Not moving by default
    bool paddleIsLeft = paddle->position.x < RENDER_WIDTH / 2;
    // Follow the ball
    if ((paddle->position.y + paddle->nextHitPos) > pong->ball.position.y + pong->ball.size)
        newSpeed = -PADDLE_SPEED;
    if ((paddle->position.y + paddle->length - paddle->nextHitPos) < pong->ball.position.y)
        newSpeed = PADDLE_SPEED;

    // Update Paddle
    bool ballMovingLeft = pong->ball.direction.x < 0;
    bool movingTowardsPaddle = ((paddleIsLeft && ballMovingLeft) ||
                                (!paddleIsLeft && !ballMovingLeft));
    float distanceToBall = fabsf(paddle->position.x - pong->ball.position.x);
    float ballIsHalfway = (float)(distanceToBall < RENDER_WIDTH/2 - pong->ball.size*2);

    if (ballIsHalfway)
    {
        GameDifficulty diff = pong->difficulty;
        paddle->speed = newSpeed * (diff + 1);

        // Tweak/adjust diff speeds
        if (diff == DIFFICULTY_EASY)
            paddle->speed += paddle->speed * 0.30f;
        // else if (diff == DIFFICULTY_MEDIUM)
        //     paddle->speed = paddle->speed * 0.05f;
        else if (diff == DIFFICULTY_HARD)
            paddle->speed -= paddle->speed * 0.10f;

        // Move slower after hitting ball
        if (!movingTowardsPaddle && (distanceToBall < RENDER_WIDTH / 8))
            paddle->speed /= 3;

        // if (pong->scoreTimer <= 0)
        paddle->position.y += paddle->speed * GetFrameTime();
    }

    // // Perfect computer
    // paddle->position.y = pong->ball.position.y;

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

void DrawPongFrame(GameState *pong, UiState *ui)
{
    DrawUiFrame(ui, pong);

    // Draw ball
    if (pong->scoreTimer <= 0 || pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
        DrawRectangle((int)pong->ball.position.x, (int)pong->ball.position.y,
                      (int)pong->ball.size,       (int)pong->ball.size, RAYWHITE);


    // Draw paddles
    if (pong->playerWon == false)
    {
        DrawRectangle((int)pong->paddleR.position.x, (int)pong->paddleR.position.y,
                      (int)pong->paddleR.width,      (int)pong->paddleR.length, RAYWHITE);
        DrawRectangle((int)pong->paddleL.position.x, (int)pong->paddleL.position.y,
                      (int)pong->paddleL.width,      (int)pong->paddleL.length, RAYWHITE);
    }

    // Draw difficulty mode text in lower right
    if (pong->currentMode == MODE_1PLAYER)
    {
        const char *difficultyText;
        switch (pong->difficulty)
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

}

void ResetBall(Ball *ball)
{
    // Return to center, but keep previous vertical position
    ball->position.x = ((float)RENDER_WIDTH - ball->size) / 2.0f;

    // Change the ball's return position and angle a bit
    ball->position.y += GetRandomValue(-RETURN_POSITION_VARIATION, RETURN_POSITION_VARIATION);
    ball->direction.y += GetRandomValue(-RETURN_ANGLE_VARIATION, RETURN_ANGLE_VARIATION);
    if (ball->position.y <= FIELD_LINE_WIDTH)
        ball->position.y = (float)(FIELD_LINE_WIDTH + ball->size);
    else if (ball->position.y >= RENDER_HEIGHT - FIELD_LINE_WIDTH)
        ball->position.y = (float)(RENDER_HEIGHT - FIELD_LINE_WIDTH - ball->size*2);
    ball->speed = BALL_SPEED;
}
