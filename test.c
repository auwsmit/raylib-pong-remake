#include "raylib.h"
#include "mainmenu.h"

// Macros
//------------------------------------------------------------------------------------
#define SCREEN_HEIGHT 600 // width will be calculated to a 16:9 aspect ratio
#define NATIVE_SCALE 720.0f

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = SCREEN_HEIGHT * (16.0f / 9.0f);
    int inputToQuitMenu = 0;

    // Scale elements based on 720p
    float winScale = SCREEN_HEIGHT / NATIVE_SCALE;

    InitWindow(screenWidth, SCREEN_HEIGHT, "Raylib Test");

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE))
            inputToQuitMenu = 1;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        DrawStartMenu(winScale);

        EndDrawing();
        //----------------------------------------------------------------------------------

        if (inputToQuitMenu == 1) break;
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
