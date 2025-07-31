#include "raylib.h"
#include "config.h"
#include "menu.h"

void DrawTitle(void)
{
    char *msg = WINDOW_TITLE;
    int fontSize = 200;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (RENDER_WIDTH - textWidth) / 2;
    int textPosY = (RENDER_HEIGHT / 2) - RENDER_HEIGHT / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartButton(void)
{
    char *msg = "Press enter to start";
    int fontSize = 100;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (RENDER_WIDTH - textWidth) / 2;
    int textPosY = (RENDER_HEIGHT / 2) + RENDER_HEIGHT / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartMenu(void)
{
    DrawTitle();
    DrawStartButton();
}
