// EXPLANATION:
// All the game's logic, including how/when to draw to screen
// See pong.h for more documentation/descriptions

#include "pong.h"

#include "raymath.h" // needed for vector math

#include "config.h"
#include "menu.h" // needed to reset the title menu

GameState InitGameState(void)
{
    // Random player starts the game
    int startingTurn = GetRandomValue(TURN_RIGHT_SIDE, TURN_LEFT_SIDE);

    // Start the ball in any random direction
    float ballStartDirectionX = (float)(startingTurn * 2 - 1) * 100; // either -100 or +100
    float ballStartDirectionY = (float)GetRandomValue(-100, 100);
    GameState state =
    {
        { // ball
            { // position x, y
                RENDER_WIDTH / 2 - BALL_SIZE / 2,
                RENDER_HEIGHT / 2 - BALL_SIZE / 2,
            },
            { // direction x, y
                ballStartDirectionX, // randomly move towards left or right player
                ballStartDirectionY,
            },
            BALL_SPEED, // speed
            BALL_SIZE, // size
        },

        { // paddleL
            { // position x, y
                PADDLE_WIDTH * 1.5,
                RENDER_HEIGHT / 2,
            },
            0.0f,          // nextHitPos
            PADDLE_SPEED,  // speed
            PADDLE_LENGTH, // length
            PADDLE_WIDTH,  // width
        },

        { // paddleR
            { // position x, y
                RENDER_WIDTH - PADDLE_WIDTH * 2.5,
                RENDER_HEIGHT / 2,
            },
            0.0f,          // nextHitPos
            PADDLE_SPEED,  // speed
            PADDLE_LENGTH, // length
            PADDLE_WIDTH,  // width
        },
        0,                 // currentMode (selected at title screen)
        (GameTurn)startingTurn,     // currentTurn
        SCREEN_LOGO,       // currentScreen
        DIFFICULTY_MEDIUM, // difficulty (default value used for cpu-vs-cpu demo mode)
        0, 0,              // scoreL, scoreR
        false,             // playerWon
        false,             // isPaused
        0.0f,              // textFade
        0.0f,              // textFadeTimeElapsed
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
    if (paddle->position.y <= FIELD_LINE_WIDTH)
        paddle->position.y = FIELD_LINE_WIDTH; // Don't get stuck
    if (paddle->position.y + paddle->length > RENDER_HEIGHT - FIELD_LINE_WIDTH)
        paddle->position.y = (float)(RENDER_HEIGHT - FIELD_LINE_WIDTH - paddle->length);
}

void BounceBallEdge(GameState *pong)
{
    int leftEdgeCollide = (pong->ball.position.x <= 0);
    int rightEdgeCollide = (pong->ball.position.x + pong->ball.size >= RENDER_WIDTH);
    int topEdgeCollide = (pong->ball.position.y <= FIELD_LINE_WIDTH);
    int bottomEdgeCollide = (pong->ball.position.y + pong->ball.size >= RENDER_HEIGHT - FIELD_LINE_WIDTH);

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
}

void BounceBallPaddle(Ball *ball, Paddle *paddle, GameTurn *currentTurn)
{
    if (CheckCollisionBallPaddle(*ball, *paddle) == false)
        return;

    // Track that it's a new player's turn
    *currentTurn = !*currentTurn;

    bool isLeftPaddle = paddle->position.x < RENDER_WIDTH / 2;
    // Position the ball outside the paddle
    if (*currentTurn == TURN_LEFT_SIDE)
    {
        ball->position.x = paddle->position.x + paddle->width + 1;
    }
    else // *currentTurn == TURN_RIGHT_SIDE
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
    ball->direction.x = (isLeftPaddle) ? cosf(newAngle) : -cosf(newAngle);
}

void UpdatePongFrame(GameState *pong, MenuState *titleMenu)
{
    if (!pong->isPaused)
    {
        // Get the difficulty multiplier

        // Update paddles
        if (pong->currentMode == MODE_1PLAYER)
        {
            UpdatePaddlePlayer1(&pong->paddleL);
            UpdatePaddleMouseInput(&pong->paddleL);
            UpdatePaddleComputer(&pong->paddleR, &pong->ball,
                                 pong->currentTurn, pong->difficulty);
        }
        if (pong->currentMode == MODE_2PLAYER)
        {
            UpdatePaddlePlayer1(&pong->paddleL);
            UpdatePaddlePlayer2(&pong->paddleR);
        }
        if (pong->currentMode == MODE_DEMO)
        {
            UpdatePaddleComputer(&pong->paddleL, &pong->ball,
                                 pong->currentTurn, pong->difficulty);
            UpdatePaddleComputer(&pong->paddleR, &pong->ball,
                                 pong->currentTurn, pong->difficulty);
        }

        // Update ball
        if (pong->scoreTimer <= 0 ||
            pong->scoreR == WIN_SCORE || pong->scoreL == WIN_SCORE)
            UpdateBall(&pong->ball);

        // Collision logic
        BounceBallEdge(pong);
        if (pong->playerWon == false)
        {
            BounceBallPaddle(&pong->ball, &pong->paddleL, &pong->currentTurn);
            BounceBallPaddle(&pong->ball, &pong->paddleR, &pong->currentTurn);
        }
        EdgeCollisionPaddle(&pong->paddleL);
        EdgeCollisionPaddle(&pong->paddleR);

        // Check for winner
        if (pong->scoreL >= WIN_SCORE || pong->scoreR >= WIN_SCORE)
            pong->playerWon = true;

        // Press Enter or Space or Click to skip win screen
        if (pong->playerWon == true &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsGestureDetected(GESTURE_TAP)))
            pong->winTimer = 0;

        // Update timers for winning and scoring
        if (pong->scoreTimer > 0)
            pong->scoreTimer -= GetFrameTime();
        if (pong->playerWon && pong->winTimer > 0)
            pong->winTimer -= GetFrameTime();
    }

    // Update pause fade animation
    static float fadeLength = 1.5f; // Fade in and out at this rate in seconds
    static bool fadingOut = false;
    float fadeIncrement = (1.0f / fadeLength) * GetFrameTime();

    if (pong->textFade >= 1.0f)
        fadingOut = true;
    else if (pong->textFade <= 0.0f)
        fadingOut = false;
    if (fadingOut)
        fadeIncrement *= -1;

    pong->textFade += fadeIncrement;

    // Press Space or P to pause
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_P))
    {
        pong->isPaused = !pong->isPaused;
    }

    // Reset game after a player wins
    if (pong->playerWon == true && pong->winTimer <= 0)
    {
        Difficulty prevDifficulty = pong->difficulty;
        *pong = InitGameState();
        pong->currentScreen = SCREEN_GAMEPLAY;
        pong->difficulty = prevDifficulty;
    }

    // Escape or Backspace to title
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
    {
        *titleMenu = InitMenuState();
        *pong = InitGameState();
        pong->currentScreen = SCREEN_TITLE;
#if !defined(PLATFORM_WEB)
        ShowCursor();
#endif
    }

    // Debug: Press R to reset ball
    // if (IsKeyPressed(KEY_R))
    // {
    //     ResetBall(&pong.ball);
    // }
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

void UpdatePaddleMouseInput(Paddle *paddle)
{
    Vector2 scaledMousePos = GetMousePosition();
    scaledMousePos.x *= (float)RENDER_WIDTH / GetScreenWidth();
    scaledMousePos.y *= (float)RENDER_HEIGHT / GetScreenHeight();

    // Only move if the mouse moved and if no keyboard input was detected
    if (Vector2Length(GetMouseDelta()) > 0 && paddle->speed == 0)
    {
        paddle->position.y = scaledMousePos.y - paddle->length / 2;

        // float distBetweenMousePaddle = fabsf(scaledMousePos.x - paddle->position.x);
        // if (distBetweenMousePaddle < RENDER_WIDTH / 2)
        //     HideCursor();
        // else
        //     ShowCursor();
    }
}

void UpdatePaddleComputer(Paddle *paddle, Ball *ball, GameTurn currentTurn, Difficulty difficulty)
{
    float newSpeed = 0.0f; // Not moving by default
    bool paddleIsLeft = paddle->position.x < RENDER_WIDTH / 2;
    // Follow the ball
    if ((paddle->position.y + paddle->nextHitPos) > ball->position.y + ball->size)
        newSpeed = -PADDLE_SPEED;
    if ((paddle->position.y + paddle->length - paddle->nextHitPos) < ball->position.y)
        newSpeed = PADDLE_SPEED;

    // Update Paddle
    float distanceToBall = fabsf(paddle->position.x - ball->position.x);
    float ballIsHalfway = (float)(distanceToBall < RENDER_WIDTH / 2);
    bool movingTowardsPaddle =
        ((paddleIsLeft && currentTurn == TURN_RIGHT_SIDE) ||
        (!paddleIsLeft && currentTurn == TURN_LEFT_SIDE));

    if (ballIsHalfway)
    {
        paddle->speed = newSpeed * (difficulty + 1);

        // Tweak/adjust difficulty speeds
        if (difficulty == DIFFICULTY_EASY)
            paddle->speed += paddle->speed * 0.30f;
        // else if (difficulty == DIFFICULTY_MEDIUM)
        //     paddle->speed = paddle->speed * 0.05f;
        else if (difficulty == DIFFICULTY_HARD)
            paddle->speed -= paddle->speed * 0.10f;

        // Move slower after hitting ball
        if (!movingTowardsPaddle && (distanceToBall < RENDER_WIDTH / 8))
            paddle->speed /= 2;

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

void DrawPongFrame(GameState *pong)
{
    // Draw dotted line down middle
    DrawFieldLines(pong->isPaused, pong->currentMode == MODE_DEMO);

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

    // Draw difficulty mode text in lower right
    if (pong->currentMode == MODE_1PLAYER)
    {
        const char *difficultyText;
        int diffTextLength;
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
        diffTextLength = MeasureText(difficultyText, DIFFICULTY_FONT_SIZE);
        DrawText(difficultyText,
                 RENDER_WIDTH / 4 * 3 - diffTextLength / 2,
                 RENDER_HEIGHT - (DIFFICULTY_FONT_SIZE * 2),
                 DIFFICULTY_FONT_SIZE, RAYWHITE);
    }

    // Draw fancy conditional text with a fade animation
    Color fadeColor = Fade(RAYWHITE, pong->textFade);

    // Draw win message
    if (pong->playerWon)
        DrawWinnerMessage(pong->scoreL, pong->scoreR, fadeColor);

    // Draw pause message
    char *text;
    if (pong->isPaused)
    {
        text = "PAUSED";
        int textOffset = MeasureText(text, SCORE_FONT_SIZE) / 2;
        DrawText(text, RENDER_WIDTH / 2 - textOffset,
                 RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2,
                 SCORE_FONT_SIZE, fadeColor);
    }
    else if (pong->currentMode == MODE_DEMO) // Draw demo mode message
    {
        text = "DEMO MODE";
        int textOffset = MeasureText(text, SCORE_FONT_SIZE) / 2;
        DrawText(text, RENDER_WIDTH / 2 - textOffset,
                 RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2,
                 SCORE_FONT_SIZE, fadeColor);
    }
}

void DrawFieldLines(bool isPaused, bool isDemoMode)
{
    int dashHeight = 40;
    int spaceHeight = 40;

    // Draw top and bottom lines
    DrawRectangle(0, 0,
                  RENDER_WIDTH, FIELD_LINE_WIDTH, RAYWHITE);
    DrawRectangle(0, RENDER_HEIGHT - FIELD_LINE_WIDTH,
                  RENDER_WIDTH, FIELD_LINE_WIDTH, RAYWHITE);

    // Calculate amount of dashes needed for dotted line
    int totalSegmentHeight = dashHeight + spaceHeight;
    int totalSegments = (RENDER_HEIGHT - spaceHeight) / totalSegmentHeight;
    int usedHeight = totalSegments * totalSegmentHeight - spaceHeight;
    int offsetY = (RENDER_HEIGHT - usedHeight) / 2;

    // Dotted line down the middle
    for (int i = 0; i < totalSegments; i++)
    {
        int y = offsetY + i * totalSegmentHeight;

        // do not draw dashes that overlap with text
        // TODO: modify generalize for any UI elements on screen
        //       will probably have to redo menu.h into ui.h or something
        int pauseMessageYPos = RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2;
        if (((isPaused || isDemoMode) &&
             (y+dashHeight > pauseMessageYPos) &&
             (y < pauseMessageYPos + SCORE_FONT_SIZE)))
            continue;

        DrawRectangle(RENDER_WIDTH / 2 - FIELD_LINE_WIDTH / 2, y,
                      FIELD_LINE_WIDTH, dashHeight, RAYWHITE);

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

void DrawWinnerMessage(int scoreL, int scoreR, Color fadeColor)
{
    char *msg = "Winner";
    int fontSize = 100; // this is also the font height because we're using the default font
    int textWidth = MeasureText(msg, fontSize);
    int textPosY = (RENDER_HEIGHT - fontSize) / 4;
    if (scoreL == WIN_SCORE)
    {
        int textPosX = RENDER_WIDTH / 4 - textWidth / 2;
        DrawText(msg, textPosX, textPosY, fontSize, fadeColor);
    }
    if (scoreR == WIN_SCORE)
    {
        int textPosX = RENDER_WIDTH / 4 * 3 - textWidth / 2;
        DrawText(msg, textPosX, textPosY, fontSize, fadeColor);
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
