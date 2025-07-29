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
    int const lineIncrement = RAYLIB_LOGO_WIDTH / 64;
    if (logo->state == START)            // State 0: Small box blinking
    {
        logo->framesCount++;

        if (logo->framesCount == 120)
        {
            logo->state = BOX1;
            logo->framesCount = 0;      // Reset counter... will be used later...
        }
    }
    else if (logo->state == BOX1)        // State 1: Top and left bars growing
    {
        logo->topSideRecWidth += lineIncrement;
        logo->leftSideRecHeight += lineIncrement;

        if (logo->topSideRecWidth >= RAYLIB_LOGO_WIDTH) logo->state = BOX2;
    }
    else if (logo->state == BOX2)        // State 2: Bottom and right bars growing
    {
        logo->bottomSideRecWidth += lineIncrement;
        logo->rightSideRecHeight += lineIncrement;

        if (logo->bottomSideRecWidth >= RAYLIB_LOGO_WIDTH) logo->state = TEXT;
    }
    else if (logo->state == TEXT)        // State 3: Letters appearing (one by one)
    {
        logo->framesCount++;

        if (logo->framesCount/12)       // Every 12 frame, one more letter!
        {
            logo->lettersCount++;
            logo->framesCount = 0;
        }

        if (logo->lettersCount >= 10)    // When all letters have appeared, just fade out everything
        {
            logo->alpha += 0.02f;

            if (logo->alpha >= 1.0f)
            {
                logo->alpha = 1.0f;
                logo->state = END;
            }
        }
    }
}

void DrawRaylibLogo(Logo *l)
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
    if (l->state == START)
    {
        if ((l->framesCount/15)%2)
            DrawRectangle(l->positionX, l->positionY, lineWidth, lineWidth, RAYWHITE);
        else
            DrawRectangle(l->positionX, l->positionY, lineWidth, lineWidth, BLACK);
    }
    else if (l->state == BOX1)
    {
        DrawRectangle(l->positionX, l->positionY, l->topSideRecWidth, lineWidth, RAYWHITE);
        DrawRectangle(l->positionX, l->positionY, lineWidth, l->leftSideRecHeight, RAYWHITE);
    }
    else if (l->state == BOX2)
    {
        DrawRectangle(l->positionX, l->positionY, l->topSideRecWidth, lineWidth, RAYWHITE);
        DrawRectangle(l->positionX, l->positionY, lineWidth, l->leftSideRecHeight, RAYWHITE);

        DrawRectangle(l->positionX + offsetA, l->positionY, lineWidth, l->rightSideRecHeight, RAYWHITE);
        DrawRectangle(l->positionX, l->positionY + offsetA, l->bottomSideRecWidth, lineWidth, RAYWHITE);
    }
    else if (l->state == TEXT)
    {
        DrawRectangle(l->positionX, l->positionY, l->topSideRecWidth, lineWidth, RAYWHITE);
        DrawRectangle(l->positionX, l->positionY + lineWidth, lineWidth, l->leftSideRecHeight - offsetB, RAYWHITE);

        DrawRectangle(l->positionX + offsetA, l->positionY + lineWidth, lineWidth, l->rightSideRecHeight - offsetB, RAYWHITE);
        DrawRectangle(l->positionX, l->positionY + offsetA, l->bottomSideRecWidth, lineWidth, RAYWHITE);

        DrawText(TextSubtext("raylib", 0, l->lettersCount), RENDER_WIDTH/2 - offsetC, RENDER_HEIGHT/2 + offsetD, fontSize, RAYWHITE);

        DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, Fade(BLACK, l->alpha));
    }
    else if (l->state == END)
        DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, BLACK);
}

