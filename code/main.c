#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()
#include "config.h"
#include "logo.h"
#include "menu.h"

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

// Types and Structures Definition
// ------------------------------------------------------------------------------------------
typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, ENDING } GameScreen;

int main(void)
{
    // Initialization
    // --------------------------------------------------------------------------------------
    const int screenWidth = RENDER_WIDTH;
    const int screenHeight = RENDER_HEIGHT;

    const int gameScreenWidth = RENDER_WIDTH;
    const int gameScreenHeight = RENDER_HEIGHT;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Raylib Test");
    SetWindowMinSize(320, 240);

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    GameScreen currentScreen = LOGO;
    Logo raylibLogo = InitRaylibLogo();
    int framesCounter = 0;
    int inputToQuitMenu = 0;

    SetTargetFPS(60);
    // --------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        // ----------------------------------------------------------------------------------
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);

        // TEMP: Easily quit for testing
        if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_SPACE))
            inputToQuitMenu = 1;

        switch(currentScreen)
        {
            case LOGO:
            {
                UpdateRaylibLogo(&raylibLogo);
                if (raylibLogo.state == END)
                {
                    framesCounter++;
                    if (framesCounter >= 60) // a small pause after logo is finished
                        currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                // Press enter to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                // TODO: Update GAMEPLAY screen variables here!

                // Press enter to change to ENDING screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = ENDING;
                }
            } break;
            case ENDING:
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;
            default: break;
        }
        // ----------------------------------------------------------------------------------

        // Draw
        // ----------------------------------------------------------------------------------
        BeginTextureMode(target);

            switch(currentScreen)
            {
                case LOGO:
                {
                    DrawRaylibLogo(&raylibLogo);
                } break;
                case TITLE:
                {
                    DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, BLACK);
                    DrawStartMenu();
                } break;
                case GAMEPLAY:
                {
                    // TODO: Draw GAMEPLAY screen here!
                    DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, PURPLE);
                    DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
                    DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

                } break;
                case ENDING:
                {
                    // TODO: Draw ENDING screen here!
                    DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, BLUE);
                    DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);

                } break;
                default: break;
            }

        EndTextureMode();

        BeginDrawing();

            // Draw render texture to screen, properly scaled
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width,
                           (float)-target.texture.height }, (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                           (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        EndDrawing();
        // ----------------------------------------------------------------------------------

        if (inputToQuitMenu == 1) break;
    }

    // De-Initialization
    // --------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    // --------------------------------------------------------------------------------------

    return 0;
}
