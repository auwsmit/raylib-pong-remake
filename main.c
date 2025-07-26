#include "raylib.h"
#include "config.h"
#include "menu.h"

// Macros
//------------------------------------------------------------------------------------

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenHeight = SCREEN_RES;
    const int screenWidth = screenHeight * (ASPECT_RATIO);

    int inputToQuitMenu = 0;

    InitWindow(screenWidth, screenHeight, "Raylib Test");

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

        DrawStartMenu();

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
