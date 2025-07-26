#include "raylib.h"
#include "config.h"
#include "menu.h"

// Functions
//------------------------------------------------------------------------------------
void DrawTitle()
{
    char *msg = "Title";
    int fontSize = 100 * WIN_SCALE;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (GetScreenWidth() - textWidth) / 2;
    int textPosY = (GetScreenHeight() / 2) - GetScreenHeight() / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartButton()
{
    char *msg = "Press enter or space to start";
    int fontSize = 40 * WIN_SCALE;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (GetScreenWidth() - textWidth) / 2;
    int textPosY = (GetScreenHeight() / 2) + GetScreenHeight() / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
    // TODO: draw box/outline
    // DrawRectangle(GetScreenWidth() / 2, GetScreenHeight() / 2, textWidth, int height, Color color);
}

void DrawStartMenu()
{
    DrawTitle();
    DrawStartButton();
}
