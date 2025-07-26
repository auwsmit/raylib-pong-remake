#include "raylib.h"
#include "config.h"
#include "logo.h"

Logo InitRaylibLogo(void)
{
    Logo raylibLogo = {
        GetScreenWidth()/2 - RAYLIB_LOGO_WIDTH/2,  // logoPositionX
        GetScreenHeight()/2 - RAYLIB_LOGO_WIDTH/2, // logoPositionY

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

void UpdateRaylibLogo(Logo *l)
{
    int lineIncrement = RAYLIB_LOGO_WIDTH / 64;
    if (l->state == START)            // State 0: Small box blinking
    {
        l->framesCount++;

        if (l->framesCount == 120)
        {
            l->state = BOX1;
            l->framesCount = 0;      // Reset counter... will be used later...
        }
    }
    else if (l->state == BOX1)        // State 1: Top and left bars growing
    {
        l->topSideRecWidth += lineIncrement;
        l->leftSideRecHeight += lineIncrement;

        if (l->topSideRecWidth >= RAYLIB_LOGO_WIDTH) l->state = BOX2;
    }
    else if (l->state == BOX2)        // State 2: Bottom and right bars growing
    {
        l->bottomSideRecWidth += lineIncrement;
        l->rightSideRecHeight += lineIncrement;

        if (l->bottomSideRecWidth >= RAYLIB_LOGO_WIDTH) l->state = TEXT;
    }
    else if (l->state == TEXT)        // State 3: Letters appearing (one by one)
    {
        l->framesCount++;

        if (l->framesCount/12)       // Every 12 frame, one more letter!
        {
            l->lettersCount++;
            l->framesCount = 0;
        }

        if (l->lettersCount >= 10)    // When all letters have appeared, just fade out everything
        {
            l->alpha += 0.02f;

            if (l->alpha >= 1.0f)
            {
                l->alpha = 1.0f;
                l->state = END;
            }
        }
    }
}

void DrawRaylibLogo(Logo *l)
{
    int lineWidth = RAYLIB_LOGO_OUTLINE;
    int offsetA = RAYLIB_LOGO_WIDTH*0.9375f;
    int offsetB = lineWidth * 2;
    int fontSize = 50;

    if (l->state == START)
    {
        if ((l->framesCount/15)%2)
            DrawRectangle(l->logoPositionX, l->logoPositionY, lineWidth, lineWidth, RAYWHITE);
        else
            DrawRectangle(l->logoPositionX, l->logoPositionY, lineWidth, lineWidth, BLACK);
    }
    else if (l->state == BOX1)
    {
        DrawRectangle(l->logoPositionX, l->logoPositionY, l->topSideRecWidth, lineWidth, RAYWHITE);
        DrawRectangle(l->logoPositionX, l->logoPositionY, lineWidth, l->leftSideRecHeight, RAYWHITE);
    }
    else if (l->state == BOX2)
    {
        DrawRectangle(l->logoPositionX, l->logoPositionY, l->topSideRecWidth, lineWidth, RAYWHITE);
        DrawRectangle(l->logoPositionX, l->logoPositionY, lineWidth, l->leftSideRecHeight, RAYWHITE);

        DrawRectangle(l->logoPositionX + offsetA, l->logoPositionY, lineWidth, l->rightSideRecHeight, RAYWHITE);
        DrawRectangle(l->logoPositionX, l->logoPositionY + offsetA, l->bottomSideRecWidth, lineWidth, RAYWHITE);
    }
    else if (l->state == TEXT)
    {
        // DrawText("powered by",
        //          (GetScreenWidth() - MeasureText("powered by", 25)) / 2,
        //          (GetScreenHeight()/2),
        //          25, Fade(WHITE, 0.1f));
        DrawRectangle(l->logoPositionX, l->logoPositionY, l->topSideRecWidth, lineWidth, RAYWHITE);
        DrawRectangle(l->logoPositionX, l->logoPositionY + lineWidth, lineWidth, l->leftSideRecHeight - offsetB, RAYWHITE);

        DrawRectangle(l->logoPositionX + offsetA, l->logoPositionY + lineWidth, lineWidth, l->rightSideRecHeight - offsetB, RAYWHITE);
        DrawRectangle(l->logoPositionX, l->logoPositionY + offsetA, l->bottomSideRecWidth, lineWidth, RAYWHITE);

        DrawText(TextSubtext("raylib", 0, l->lettersCount), RENDER_WIDTH/2 - 44, RENDER_HEIGHT/2 + 48, fontSize, RAYWHITE);

        DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, Fade(BLACK, l->alpha));
    }
    else if (l->state == END)
        DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, BLACK);
}

