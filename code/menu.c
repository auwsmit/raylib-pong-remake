#include "raylib.h"
#include "config.h"
#include "menu.h"

void DrawTitle(void)
{
    char *msg = "Title";
    int fontSize = 120;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (RENDER_WIDTH - textWidth) / 2;
    int textPosY = (RENDER_HEIGHT / 2) - RENDER_HEIGHT / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartButton(void)
{
    char *msg = "Press enter to start";
    int fontSize = 60;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (RENDER_WIDTH - textWidth) / 2;
    int textPosY = (RENDER_HEIGHT / 2) + RENDER_HEIGHT / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
    // TODO: draw box/outline
}

void DrawStartMenu(void)
{
    DrawTitle();
    DrawStartButton();
}
