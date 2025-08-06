#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()
#include "config.h"
#include "logo.h"
#include "menu.h"
#include "pong.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Types and Structures Definition
// --------------------------------------------------------------------------------

// Local Variables Definition
// --------------------------------------------------------------------------------
const int gameScreenWidth = RENDER_WIDTH;
const int gameScreenHeight = RENDER_HEIGHT;
RenderTexture2D gameTarget; // used to hold the rendering result to rescale window

// Game loop variables
bool skipCurrentFrame = false;
Logo raylibLogo;
GameState pong;
MenuState menu;

// Local Functions Declaration
// --------------------------------------------------------------------------------
void UpdateDrawFrame(void);

// Main entry point
// --------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    // --------------------------------------------------------------------------------

    // Create a window
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE;
    if (VSYNC_ENABLED)
        windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(320, 240);

    // Initialize the render texture, used to hold the rendering result so we can easily resize it
    gameTarget = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(gameTarget.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    raylibLogo = InitRaylibLogo();
    pong = InitGameState();
    menu = InitMenuState();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    if (MAX_FRAMERATE > 0)
        SetTargetFPS(MAX_FRAMERATE);
    // --------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !pong.gameShouldExit) // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif // PLATFORM_WEB

    // De-Initialization
    // --------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    // --------------------------------------------------------------------------------

    return 0;
}

void UpdateDrawFrame()
{
    // Update
    // --------------------------------------------------------------------------------
    // Compute required framebuffer scaling
    float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);

    // Debug: q for fast quitting
    SetExitKey(KEY_Q);
    // SetExitKey(KEY_NULL); // No exit key

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
        return;
    }

    switch(pong.currentScreen)
    {
        case SCREEN_LOGO:
            {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsGestureDetected(GESTURE_TAP))
                {
                    pong.currentScreen = SCREEN_TITLE;
                }
                UpdateRaylibLogo(&raylibLogo);
                if (raylibLogo.state == END)
                {
                    pong.currentScreen = SCREEN_TITLE;
                }
            } break;
        case SCREEN_TITLE: UpdateStartMenu(&menu, &pong);
                           break;
        case SCREEN_GAMEPLAY:
            {
                UpdatePong(&pong);

                // Escape back to title
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
                {
                    raylibLogo = InitRaylibLogo();
                    pong = InitGameState();
                    menu = InitMenuState();
                    pong.currentScreen = SCREEN_TITLE;
                }
            } break;
        default: break;
    }
    // --------------------------------------------------------------------------------

    // Draw
    // --------------------------------------------------------------------------------
    BeginTextureMode(gameTarget); // Draw to the render texture
    {
        ClearBackground(BLACK); // Default background color

        switch(pong.currentScreen)
        {
            case SCREEN_LOGO:     DrawRaylibLogo(&raylibLogo);
                                  break;
            case SCREEN_TITLE:    DrawStartMenu(&menu);
                                  break;
            case SCREEN_GAMEPLAY: DrawPong(&pong);
                                  break;
            default: break;
        }
    } EndTextureMode();

    BeginDrawing();
    {
        // Fill in any potential area outside of the render texture
        DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, BLACK);

        // Draw render texture to screen, properly scaled
        DrawTexturePro(gameTarget.texture, (Rectangle){ 0.0f, 0.0f, (float)gameTarget.texture.width,
                       (float)-gameTarget.texture.height }, (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                       (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Debug
        DrawFPS(0,0);
    } EndDrawing();
    // --------------------------------------------------------------------------------
}

