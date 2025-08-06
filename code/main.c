#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()
#include "config.h"
#include "logo.h"
#include "menu.h"
#include "pong.h"

// Types and Structures Definition
// ------------------------------------------------------------------------------------------
typedef enum ScreenState { LOGO, TITLE, DIFFICULTY, GAMEPLAY, ENDING } ScreenState;

// Functions Definition
// ------------------------------------------------------------------------------------------

int main(void)
{
    // Initialization
    // --------------------------------------------------------------------------------------
    const int gameScreenWidth = RENDER_WIDTH;
    const int gameScreenHeight = RENDER_HEIGHT;

    // Create a window
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE;
    if (VSYNC_ENABLED)
        windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(320, 240);

    // Initialize the render texture, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    // Game loop variables
    ScreenState currentScreen = LOGO;
    bool skipCurrentFrame = false;
    Logo raylibLogo = InitRaylibLogo();
    GameState pong = InitGameState();
    GameSession session = { ONEPLAYER, MEDIUM }; // tracks mode and difficulty selection

    if (MAX_FRAMERATE > 0)
        SetTargetFPS(MAX_FRAMERATE);
    // --------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        // ----------------------------------------------------------------------------------
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);

        // Debug: q for fast quitting
        SetExitKey(KEY_Q);

        // Fullscreen input via F11, Alt+Enter, and Shift+F
        if (IsKeyPressed(KEY_F11) ||
            ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) ||
            ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT)) && IsKeyPressed(KEY_F)))
        {
#ifdef PLATFORM_WEB
            ToggleFullscreen();
#else
            ToggleBorderlessWindowed();
#endif
            skipCurrentFrame = true;
        }

        if (skipCurrentFrame == true)
        {
            skipCurrentFrame = false;
            BeginDrawing();
            EndDrawing(); // This is required for raylib to properly update for the next frame
            continue;
        }

        switch(currentScreen)
        {
            case LOGO:
            {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
                }
                UpdateRaylibLogo(&raylibLogo);
                if (raylibLogo.state == END)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                // Press enter or space to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case DIFFICULTY:
            {
                // Press enter or space to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                UpdatePong(&pong, &session);

                // Press R to reset ball
                if (IsKeyPressed(KEY_R))
                {
                    ResetBall(&pong.ball);
                }

                // Escape back to beginning
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    currentScreen = LOGO;
                    raylibLogo = InitRaylibLogo();
                    pong = InitGameState();
                }
            } break;
            default: break;
        }
        // ----------------------------------------------------------------------------------

        // Draw
        // ----------------------------------------------------------------------------------
        BeginTextureMode(target); // Draw to the render texture

            DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, BLACK); // Default background color

            switch(currentScreen)
            {
                case LOGO:
                {
                    DrawRaylibLogo(&raylibLogo);

                } break;
                case TITLE:
                {
                    DrawStartMenu();

                } break;
                case GAMEPLAY:
                {
                    DrawPong(&pong);

                } break;
                default: break;
            }

        EndTextureMode();

        BeginDrawing();

            // Fill in any potential area outside of the render texture
            DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, BLACK);

            // Draw render texture to screen, properly scaled
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width,
                           (float)-target.texture.height }, (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                           (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

            // Debug
            DrawFPS(0,0);

        EndDrawing();
        // ----------------------------------------------------------------------------------
    }

    // De-Initialization
    // --------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    // --------------------------------------------------------------------------------------

    return 0;
}
