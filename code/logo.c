#include "raylib.h"
#include "config.h"
#include "logo.h"

Logo InitRaylibLogo(void)
{
    Logo raylibLogo =
    {
        RENDER_WIDTH/2 - RAYLIB_LOGO_WIDTH/2,  // positionX
        RENDER_HEIGHT/2 - RAYLIB_LOGO_WIDTH/2, // positionY

        0,  // framesCount
        0,  // lettersCount

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

void UpdateRaylibLogo(Logo *logo)
{
    float deltaTime = GetFrameTime();
    const float growSpeed = RAYLIB_LOGO_WIDTH * 0.9375f; // Speed that lines grow
    const float letterDelay = 0.2f; // Time between each letter appearing
    const float fadeSpeed = 1.0f; // Fade out in 1 second

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
            break;
    }
}

void DrawRaylibLogo(Logo *logo)
{
    const int lineWidth = RAYLIB_LOGO_OUTLINE;
    const int offsetA   = RAYLIB_LOGO_WIDTH*0.9375f;
    const int offsetB   = lineWidth * 2;
    const int fontSize  = RAYLIB_LOGO_FONT_SIZE;
    const int offsetC   = RAYLIB_LOGO_WIDTH*0.171875;
    const int offsetD   = RAYLIB_LOGO_WIDTH*0.1875;

    DrawText("powered by",
             (RENDER_WIDTH/2) - (RAYLIB_LOGO_WIDTH/2),
             (RENDER_HEIGHT/2) - (RAYLIB_LOGO_WIDTH/2) - offsetB - lineWidth/4,
             fontSize / 2, RAYWHITE);

    switch (logo->state)
    {
        case START:
            if (((int)(logo->elapsedTime * 4)) % 2)
                DrawRectangle(logo->positionX, logo->positionY,
                              lineWidth, lineWidth, RAYWHITE);
            else
                DrawRectangle(logo->positionX, logo->positionY,
                              lineWidth, lineWidth, BLACK);
            break;
        case GROW1:
            DrawRectangle(logo->positionX, logo->positionY,
                          logo->topSideRecWidth, lineWidth, RAYWHITE);
            DrawRectangle(logo->positionX, logo->positionY,
                          lineWidth, logo->leftSideRecHeight, RAYWHITE);
            break;
        case GROW2:
            DrawRectangle(logo->positionX, logo->positionY,
                          logo->topSideRecWidth, lineWidth, RAYWHITE);
            DrawRectangle(logo->positionX, logo->positionY,
                          lineWidth, logo->leftSideRecHeight, RAYWHITE);

            DrawRectangle(logo->positionX + offsetA, logo->positionY,
                          lineWidth, logo->rightSideRecHeight, RAYWHITE);
            DrawRectangle(logo->positionX, logo->positionY + offsetA,
                          logo->bottomSideRecWidth, lineWidth, RAYWHITE);
            break;
        case TEXT:
            DrawRectangle(logo->positionX, logo->positionY,
                          logo->topSideRecWidth, lineWidth, RAYWHITE);
            DrawRectangle(logo->positionX, logo->positionY + lineWidth,
                          lineWidth, logo->leftSideRecHeight - offsetB, RAYWHITE);

            DrawRectangle(logo->positionX + offsetA, logo->positionY + lineWidth,
                          lineWidth, logo->rightSideRecHeight - offsetB, RAYWHITE);
            DrawRectangle(logo->positionX, logo->positionY + offsetA,
                          logo->bottomSideRecWidth, lineWidth, RAYWHITE);

            DrawText(TextSubtext("raylib", 0, logo->lettersCount),
                     RENDER_WIDTH/2 - offsetC, RENDER_HEIGHT/2 + offsetD,
                     fontSize, RAYWHITE);

            DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, Fade(BLACK, logo->alpha));
            break;
        case PAUSE:
            DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, BLACK);
            break;
        case END:
            break;
    }
}

