// EXPLANATION:
// For the raylib logo animation at start of program
// See logo.h for more documentation/descriptions

#include "logo.h"
#include "config.h"
#include "pong.h"

// Global animation state
Logo raylibLogo = { 0 };

void InitRaylibLogo(void)
{
    Logo startOfAnimation =
    {
        .positionX = RENDER_WIDTH/2 - RAYLIB_LOGO_WIDTH/2,
        .positionY = RENDER_HEIGHT/2 - RAYLIB_LOGO_WIDTH/2,

        .elapsedTime = 0,
        .lettersCount = 0,

        .topSideRecWidth = RAYLIB_LOGO_OUTLINE,
        .leftSideRecHeight = RAYLIB_LOGO_OUTLINE,
        .bottomSideRecWidth = RAYLIB_LOGO_OUTLINE,
        .rightSideRecHeight = RAYLIB_LOGO_OUTLINE,

        .state = LOGO_START, // State machine

        .alpha = 0.0f, // Useful for fading
    };

    raylibLogo = startOfAnimation;
}

void UpdateRaylibLogo(void)
{
    float deltaTime = GetFrameTime();
    const float growSpeed = RAYLIB_LOGO_WIDTH * 0.9375f; // Speed that lines grow
    const float letterDelay = 0.2f; // Time between each letter appearing
    const float fadeSpeed = 1.0f; // Fade out in 1 second
    static bool skipped = false;

    // Press any key or click to skip intro
    if ((GetKeyPressed() != 0) || IsGestureDetected(GESTURE_TAP))
    {
        if (raylibLogo.state >= LOGO_TEXT)
            pongGame.currentScreen = SCREEN_TITLE;
        else
        {
            raylibLogo.topSideRecWidth = RAYLIB_LOGO_WIDTH;
            raylibLogo.leftSideRecHeight = RAYLIB_LOGO_WIDTH;
            raylibLogo.bottomSideRecWidth = RAYLIB_LOGO_WIDTH;
            raylibLogo.rightSideRecHeight = RAYLIB_LOGO_WIDTH;
            raylibLogo.lettersCount = 10;
            raylibLogo.elapsedTime = 0;
            raylibLogo.state = LOGO_TEXT;
            skipped = true;
        }
    }

    // Support raylib!
    // https://github.com/sponsors/raysan5 https://www.patreon.com/raylib :)
    if (skipped == true && raylibLogo.elapsedTime < 1.0f)
    {
        raylibLogo.elapsedTime += GetFrameTime();
        return;
    }

    switch (raylibLogo.state)
    {
        case LOGO_START: // Small box blinking
            raylibLogo.elapsedTime += deltaTime;
            if (raylibLogo.elapsedTime >= 2.0f) // 2 seconds delay
            {
                raylibLogo.state = LOGO_GROW1;
                raylibLogo.elapsedTime = 0.0f; // Reset counter... will be used later...
            }
            break;

        case LOGO_GROW1: // Top and left bars growing
            raylibLogo.topSideRecWidth += growSpeed * deltaTime;
            raylibLogo.leftSideRecHeight += growSpeed * deltaTime;

            if (raylibLogo.topSideRecWidth >= RAYLIB_LOGO_WIDTH)
            {
                raylibLogo.topSideRecWidth = RAYLIB_LOGO_WIDTH;
                raylibLogo.leftSideRecHeight = RAYLIB_LOGO_WIDTH;
                raylibLogo.state = LOGO_GROW2;
                raylibLogo.elapsedTime = 0.0f;
            }
            break;

        case LOGO_GROW2: // Bottom and right bars growing
            raylibLogo.bottomSideRecWidth += growSpeed * deltaTime;
            raylibLogo.rightSideRecHeight += growSpeed * deltaTime;

            if (raylibLogo.bottomSideRecWidth >= RAYLIB_LOGO_WIDTH)
            {
                raylibLogo.bottomSideRecWidth = RAYLIB_LOGO_WIDTH;
                raylibLogo.rightSideRecHeight = RAYLIB_LOGO_WIDTH;
                raylibLogo.state = LOGO_TEXT;
                raylibLogo.elapsedTime = 0.0f;
            }
            break;

        case LOGO_TEXT: // Letters appearing (one by one)
            raylibLogo.elapsedTime += deltaTime;

            if (raylibLogo.lettersCount < 10 && raylibLogo.elapsedTime >= letterDelay)
            {
                raylibLogo.lettersCount++;
                raylibLogo.elapsedTime = 0.0f;
            }

            // When all letters have appeared, just fade out everything
            if (raylibLogo.lettersCount >= 10)
            {
                raylibLogo.alpha += fadeSpeed * deltaTime;
                if (raylibLogo.alpha >= 1.0f)
                {
                    raylibLogo.alpha = 1.0f;
                    raylibLogo.state = LOGO_PAUSE;
                    raylibLogo.elapsedTime = 0.0f;
                }
            }
            break;

        case LOGO_PAUSE: // Pause at end of animation
            raylibLogo.elapsedTime += deltaTime;
            if (raylibLogo.elapsedTime >= 1.5f)
                raylibLogo.state = LOGO_END;
            break;

        case LOGO_END: // Animation is finished
            pongGame.currentScreen = SCREEN_TITLE;
            break;
    }
}

void DrawRaylibLogo(void)
{
    int lineWidth = (int)(RAYLIB_LOGO_OUTLINE); // DrawRectangle() takes ints, so all this casting is just to remove warnings
    int offsetA   = (int)(RAYLIB_LOGO_WIDTH*0.9375f);
    int offsetB   = (int)(lineWidth * 2);
    int offsetC   = (int)(RAYLIB_LOGO_WIDTH*0.171875f);
    int offsetD   = (int)(RAYLIB_LOGO_WIDTH*0.1875f);
    int fontSize  = (int)(RAYLIB_LOGO_FONT_SIZE);

    int rectPosX    = (int)raylibLogo.positionX;
    int rectPosY    = (int)raylibLogo.positionY;
    int topWidth    = (int)raylibLogo.topSideRecWidth;
    int leftHeight  = (int)raylibLogo.leftSideRecHeight;
    int rightHeight = (int)raylibLogo.rightSideRecHeight;
    int bottomWidth = (int)raylibLogo.bottomSideRecWidth;

    if (raylibLogo.state != LOGO_PAUSE)
        DrawText("powered by",
                 (int)((RENDER_WIDTH / 2) - (RAYLIB_LOGO_WIDTH / 2)),
                 (int)((RENDER_HEIGHT / 2) - (RAYLIB_LOGO_WIDTH / 2) - offsetB - lineWidth / 4),
                 (int)(fontSize / 2), RAYWHITE);

    switch (raylibLogo.state)
    {
        case LOGO_START:
            if (((int)(raylibLogo.elapsedTime * 4)) % 2)
                DrawRectangle(rectPosX, rectPosY, lineWidth, lineWidth, RAYWHITE);
            else
                DrawRectangle(rectPosX, rectPosY, lineWidth, lineWidth, BLACK);
            break;
        case LOGO_GROW1:
            DrawRectangle(rectPosX, rectPosY, topWidth, lineWidth, RAYWHITE);
            DrawRectangle(rectPosX, rectPosY, lineWidth, leftHeight, RAYWHITE);
            break;
        case LOGO_GROW2:
            DrawRectangle(rectPosX, rectPosY, topWidth, lineWidth, RAYWHITE);
            DrawRectangle(rectPosX, rectPosY, lineWidth, leftHeight, RAYWHITE);

            DrawRectangle(rectPosX + offsetA, rectPosY, lineWidth, rightHeight, RAYWHITE);
            DrawRectangle(rectPosX, rectPosY + offsetA, bottomWidth, lineWidth, RAYWHITE);
            break;
        case LOGO_TEXT:
            DrawRectangle(rectPosX, rectPosY, topWidth, lineWidth, RAYWHITE);
            DrawRectangle(rectPosX, rectPosY + lineWidth, lineWidth, leftHeight - offsetB, RAYWHITE);

            DrawRectangle(rectPosX + offsetA, rectPosY + lineWidth, lineWidth, rightHeight - offsetB, RAYWHITE);
            DrawRectangle(rectPosX, rectPosY + offsetA, bottomWidth, lineWidth, RAYWHITE);

            DrawText(TextSubtext("raylib", 0, raylibLogo.lettersCount),
                     RENDER_WIDTH/2 - offsetC, RENDER_HEIGHT/2 + offsetD,
                     fontSize, RAYWHITE);

            DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, Fade(BLACK, raylibLogo.alpha));
            break;
        case LOGO_PAUSE:
            break;
        case LOGO_END:
            break;
    }
}

