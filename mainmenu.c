#include "raylib.h"
#include "mainmenu.h"

// Functions
//------------------------------------------------------------------------------------
void DrawTitle(float scale)
{
    char *msg = "Title";
    int fontSize = 100 * scale;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (GetScreenWidth() - textWidth) / 2;
    int textPosY = (GetScreenHeight() / 2) - GetScreenHeight() / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
}

void DrawStartButton(float scale)
{
    char *msg = "Press enter or space to start";
    int fontSize = 40 * scale;
    int textWidth = MeasureText(msg, fontSize);
    int textPosX = (GetScreenWidth() - textWidth) / 2;
    int textPosY = (GetScreenHeight() / 2) + GetScreenHeight() / 4;
    DrawText(msg, textPosX, textPosY, fontSize, RAYWHITE);
    // TODO: draw box/outline
    // DrawRectangle(GetScreenWidth() / 2, GetScreenHeight() / 2, textWidth, int height, Color color);
}

void DrawStartMenu(float scale)
{
    DrawTitle(scale);
    DrawStartButton(scale);
}
