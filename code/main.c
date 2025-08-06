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
typedef struct AppData // Local variables for the game loop in main()
{
    RenderTexture2D renderTarget; // used to hold the rendering result to rescale window
    bool skipCurrentFrame;
    Logo raylibLogo; // data for logo animation
    GameState pong;
    MenuState menu; // data for main menu
} AppData;

// Local Functions Declaration
// --------------------------------------------------------------------------------
void UpdateDrawFrame(AppData *app);

// Main entry point
// --------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    // --------------------------------------------------------------------------------
    AppData app = { 0 };

    // Create a window
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE;
    if (VSYNC_ENABLED)
        windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(320, 240);

    // Initialize the render texture, used to hold the rendering result so we can easily resize it
    app.renderTarget = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    SetTextureFilter(app.renderTarget.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    app.skipCurrentFrame = false;
    app.raylibLogo = InitRaylibLogo();
    app.pong = InitGameState();
    app.menu = InitMenuState();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg((em_arg_callback_func)UpdateDrawFrame, &app, 0, 1);
#else
    if (MAX_FRAMERATE > 0)
        SetTargetFPS(MAX_FRAMERATE);
    // --------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !app.pong.gameShouldExit) // Detect window close button
    {
        UpdateDrawFrame(&app);
    }
#endif // PLATFORM_WEB

    // De-Initialization
    // --------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    // --------------------------------------------------------------------------------

    return 0;
}

void UpdateDrawFrame(AppData *app)
{
    // Update
    // --------------------------------------------------------------------------------
    // Compute required framebuffer scaling
    float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);

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
        app->skipCurrentFrame = true;
    }

    if (app->skipCurrentFrame == true)
    {
        app->skipCurrentFrame = false;
        BeginDrawing();
        EndDrawing(); // This is required for raylib to properly update for the next frame
        return;
    }

    switch(app->pong.currentScreen)
    {
        case SCREEN_LOGO:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsGestureDetected(GESTURE_TAP))
            {
                app->pong.currentScreen = SCREEN_TITLE;
            }
            UpdateRaylibLogo(&app->raylibLogo);
            if (app->raylibLogo.state == END)
            {
                app->pong.currentScreen = SCREEN_TITLE;
            }
            break;

        case SCREEN_TITLE:
            UpdateStartMenu(&app->menu, &app->pong);
            break;

        case SCREEN_GAMEPLAY:
            UpdatePongFrame(&app->pong);

            // Escape or Backspace to title
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
            {
                app->raylibLogo = InitRaylibLogo();
                app->pong = InitGameState();
                app->menu = InitMenuState();
                app->pong.currentScreen = SCREEN_TITLE;
            }
            break;

        default: break;
    }
    // --------------------------------------------------------------------------------

    // Draw
    // --------------------------------------------------------------------------------
    BeginTextureMode(app->renderTarget); // Draw to the render texture
    {
        ClearBackground(BLACK); // Default background color

        switch(app->pong.currentScreen)
        {
            case SCREEN_LOGO:     DrawRaylibLogo(&app->raylibLogo);
                                  break;
            case SCREEN_TITLE:    DrawStartMenu(&app->menu);
                                  break;
            case SCREEN_GAMEPLAY: DrawPongFrame(&app->pong);
                                  break;
            default: break;
        }
    } EndTextureMode();

    BeginDrawing();
    {
        // Fill in any potential area outside of the render texture
        DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, BLACK);

        // Draw render texture to screen, properly scaled
        DrawTexturePro(app->renderTarget.texture, (Rectangle){ 0.0f, 0.0f, (float)app->renderTarget.texture.width,
                       (float)-app->renderTarget.texture.height }, (Rectangle){ (GetScreenWidth() - ((float)RENDER_WIDTH*scale))*0.5f, (GetScreenHeight() - ((float)RENDER_HEIGHT*scale))*0.5f,
                       (float)RENDER_WIDTH*scale, (float)RENDER_HEIGHT*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Debug
        DrawFPS(0,0);
    } EndDrawing();
    // --------------------------------------------------------------------------------
}

