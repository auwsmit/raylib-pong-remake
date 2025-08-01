#include "raylib.h"
#include "config.h"
#include "menu.h"

void DrawTitle(void)
{
    const char *msg = WINDOW_TITLE;
    const int fontSize = 180;
    const int textWidth = MeasureText(msg, fontSize);
    const int textPosX = (RENDER_WIDTH - textWidth) / 2;
    const int textPosY = (RENDER_HEIGHT / 2) - RENDER_HEIGHT / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartButton(void)
{
    const char *msg = "Press enter to start";
    const int fontSize = 80;
    const int textWidth = MeasureText(msg, fontSize);
    const int textPosX = (RENDER_WIDTH - textWidth) / 2;
    const int textPosY = (RENDER_HEIGHT / 2) + RENDER_HEIGHT / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartMenu(void)
{
    DrawTitle();
    DrawStartButton();
}
