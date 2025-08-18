// EXPLANATION:
// For the raylib logo animation at start of program
// See logo.h for more documentation/descriptions

#include "logo.h"
#include "config.h"

Logo InitRaylibLogo(void)
{
    Logo raylibLogo =
    {
        RENDER_WIDTH/2 - RAYLIB_LOGO_WIDTH/2,  // positionX
        RENDER_HEIGHT/2 - RAYLIB_LOGO_WIDTH/2, // positionY

        0, 0, // framesCount, lettersCount

        RAYLIB_LOGO_OUTLINE, // topSideRecWidth
        RAYLIB_LOGO_OUTLINE, // leftSideRecHeight

        RAYLIB_LOGO_OUTLINE, // bottomSideRecWidth
        RAYLIB_LOGO_OUTLINE, // rightSideRecHeight

        START, // state
               // Tracking animation states (State Machine)

        0.0f,  // alpha
               // Useful for fading
    };
    return raylibLogo;
}

void UpdateRaylibLogo(Logo *logo, GameState *pong)
{
    float deltaTime = GetFrameTime();
    const float growSpeed = RAYLIB_LOGO_WIDTH * 0.9375f; // Speed that lines grow
    const float letterDelay = 0.2f; // Time between each letter appearing
    const float fadeSpeed = 1.0f; // Fade out in 1 second
    static bool skipped = false;

    // Enter or space or click to skip logo animation
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsGestureDetected(GESTURE_TAP))
    {
        if (logo->state >= TEXT)
            pong->currentScreen = SCREEN_TITLE;
        else
        {
            logo->topSideRecWidth = RAYLIB_LOGO_WIDTH;
            logo->leftSideRecHeight = RAYLIB_LOGO_WIDTH;
            logo->bottomSideRecWidth = RAYLIB_LOGO_WIDTH;
            logo->rightSideRecHeight = RAYLIB_LOGO_WIDTH;
            logo->lettersCount = 10;
            logo->elapsedTime = 0;
            logo->state = TEXT;
            skipped = true;
        }
    }

    // https://github.com/sponsors/raysan5 https://www.patreon.com/raylib :)
    if (skipped == true && logo->elapsedTime < 1.5)
    {
        logo->elapsedTime += GetFrameTime();
        return;
    }

    switch (logo->state)
    {
        case START: // Small box blinking
            logo->elapsedTime += deltaTime;
            if (logo->elapsedTime >= 2.0f) // 2 seconds delay
            {
                logo->state = GROW1;
                logo->elapsedTime = 0.0f; // Reset counter... will be used later...
            }
            break;

        case GROW1: // Top and left bars growing
            logo->topSideRecWidth += growSpeed * deltaTime;
            logo->leftSideRecHeight += growSpeed * deltaTime;

            if (logo->topSideRecWidth >= RAYLIB_LOGO_WIDTH)
            {
                logo->topSideRecWidth = RAYLIB_LOGO_WIDTH;
                logo->leftSideRecHeight = RAYLIB_LOGO_WIDTH;
                logo->state = GROW2;
                logo->elapsedTime = 0.0f;
            }
            break;

        case GROW2: // Bottom and right bars growing
            logo->bottomSideRecWidth += growSpeed * deltaTime;
            logo->rightSideRecHeight += growSpeed * deltaTime;

            if (logo->bottomSideRecWidth >= RAYLIB_LOGO_WIDTH)
            {
                logo->bottomSideRecWidth = RAYLIB_LOGO_WIDTH;
                logo->rightSideRecHeight = RAYLIB_LOGO_WIDTH;
                logo->state = TEXT;
                logo->elapsedTime = 0.0f;
            }
            break;

        case TEXT: // Letters appearing (one by one)
            logo->elapsedTime += deltaTime;

            if (logo->lettersCount < 10 && logo->elapsedTime >= letterDelay)
            {
                logo->lettersCount++;
                logo->elapsedTime = 0.0f;
            }

            // When all letters have appeared, just fade out everything
            if (logo->lettersCount >= 10)
            {
                logo->alpha += fadeSpeed * deltaTime;
                if (logo->alpha >= 1.0f)
                {
                    logo->alpha = 1.0f;
                    logo->state = PAUSE;
                    logo->elapsedTime = 0.0f;
                }
            }
            break;

        case PAUSE: // Pause at end of animation
            logo->elapsedTime += deltaTime;
            if (logo->elapsedTime >= 1.5f)
                logo->state = END;
            break;

        case END: // Animation is finished
            pong->currentScreen = SCREEN_TITLE;
            break;
    }
}

void DrawRaylibLogo(Logo *logo)
{
    int lineWidth = (int)(RAYLIB_LOGO_OUTLINE); // DrawRectangle() takes ints, so all this casting is just to remove warnings
    int offsetA   = (int)(RAYLIB_LOGO_WIDTH*0.9375f);
    int offsetB   = (int)(lineWidth * 2);
    int fontSize  = (int)(RAYLIB_LOGO_FONT_SIZE);
    int offsetC   = (int)(RAYLIB_LOGO_WIDTH*0.171875);
    int offsetD   = (int)(RAYLIB_LOGO_WIDTH*0.1875);

    int posX        = (int)logo->positionX;
    int posY        = (int)logo->positionY;
    int topWidth    = (int)logo->topSideRecWidth;
    int leftHeight  = (int)logo->leftSideRecHeight;
    int rightHeight = (int)logo->rightSideRecHeight;
    int bottomWidth = (int)logo->bottomSideRecWidth;

    if (logo->state != PAUSE)
        DrawText("powered by",
                 (int)((RENDER_WIDTH / 2) - (RAYLIB_LOGO_WIDTH / 2)),
                 (int)((RENDER_HEIGHT / 2) - (RAYLIB_LOGO_WIDTH / 2) - offsetB - lineWidth / 4),
                 (int)(fontSize / 2), RAYWHITE);

    switch (logo->state)
    {
        case START:
            if (((int)(logo->elapsedTime * 4)) % 2)
                DrawRectangle(posX, posY, lineWidth, lineWidth, RAYWHITE);
            else
                DrawRectangle(posX, posY, lineWidth, lineWidth, BLACK);
            break;
        case GROW1:
            DrawRectangle(posX, posY, topWidth, lineWidth, RAYWHITE);
            DrawRectangle(posX, posY, lineWidth, leftHeight, RAYWHITE);
            break;
        case GROW2:
            DrawRectangle(posX, posY, topWidth, lineWidth, RAYWHITE);
            DrawRectangle(posX, posY, lineWidth, leftHeight, RAYWHITE);

            DrawRectangle(posX + offsetA, posY, lineWidth, rightHeight, RAYWHITE);
            DrawRectangle(posX, posY + offsetA, bottomWidth, lineWidth, RAYWHITE);
            break;
        case TEXT:
            DrawRectangle(posX, posY, topWidth, lineWidth, RAYWHITE);
            DrawRectangle(posX, posY + lineWidth, lineWidth, leftHeight - offsetB, RAYWHITE);

            DrawRectangle(posX + offsetA, posY + lineWidth, lineWidth, rightHeight - offsetB, RAYWHITE);
            DrawRectangle(posX, posY + offsetA, bottomWidth, lineWidth, RAYWHITE);

            DrawText(TextSubtext("raylib", 0, logo->lettersCount),
                     RENDER_WIDTH/2 - offsetC, RENDER_HEIGHT/2 + offsetD,
                     fontSize, RAYWHITE);

            DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, Fade(BLACK, logo->alpha));
            break;
        case PAUSE:
            // DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, BLACK);
            break;
        case END:
            break;
    }
}

