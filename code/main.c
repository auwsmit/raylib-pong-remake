// EXPLANATION:
// The main entry point for the game/program
// The meat of the game loop can be found in the "UpdateDrawFrame" function

#include "raylib.h"
#include "raymath.h" // Required for: Vector2Clamp()

#include "config.h"  // Program config, e.g. window title/size, fps, vsync
#include "states.h"  // State machines shared across files
#include "logo.h"    // Raylib logo animation
#include "menu.h"    // Title menu
#include "beep.h"    // Pong beep sound
#include "pong.h"    // Game logic

#if defined(PLATFORM_WEB) // for compiling to wasm (web assembly)
    #include <emscripten/emscripten.h>
#endif

// Types and Structures Definition
// --------------------------------------------------------------------------------
typedef struct AppData // Local variables for the game loop in main()
{
    RenderTexture2D renderTarget; // used to hold the rendering result to rescale window
    Logo raylibLogo; // data for logo animation
    bool skipCurrentFrame;
    GameState pong;
    MenuState titleMenu; // data for main menu
} AppData;

// Local Functions Declaration
// --------------------------------------------------------------------------------
void CreateNewWindow(void); // Creates a new window with the proper initial settings
AppData InitGameLoop(void); // Initializes data for the game loop
void RunGameLoop(AppData *app); // Runs the game loop
void UpdateDrawFrame(AppData *app); // Update and Draw the current frame
                                    // Most of the game loop's code is found in here
void HandleToggleFullscreen(AppData *app);

// Main entry point
// --------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    // --------------------------------------------------------------------------------
    CreateNewWindow();
    InitAudioDevice();
    InitBeepSound();

    AppData app = InitGameLoop();
    RunGameLoop(&app);

    // De-Initialization
    // --------------------------------------------------------------------------------
    CloseBeepSound();
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void CreateNewWindow(void)
{
#if defined(PLATFORM_WEB)
    SetConfigFlags(0); // no vsync or window resize for web
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    // SetWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
#else
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE;
    if (VSYNC_ENABLED) windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
#endif
    SetWindowMinSize(320, 240);
}

AppData InitGameLoop(void)
{
    AppData app = { 0 };

    // Initialize the render texture, used to hold the rendering result so we can easily resize it
    app.renderTarget = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    SetTextureFilter(app.renderTarget.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    app.skipCurrentFrame = false;
    app.raylibLogo = InitRaylibLogo();
    app.titleMenu = InitMenuState();
    app.pong = InitGameState();

    return app;
}

void RunGameLoop(AppData *app)
{
#if defined(PLATFORM_WEB)
    int emscriptenFPS = 0; // Let emscripten handle the framerate because setting a specific one is kinda janky
                           // Generally, it will use whatever the monitor's refresh rate is
    emscripten_set_main_loop_arg((em_arg_callback_func)UpdateDrawFrame, app, emscriptenFPS, 1);
#else
    if (MAX_FRAMERATE > 0)
        SetTargetFPS(MAX_FRAMERATE);
    // --------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !app->pong.gameShouldExit) // Detect window close button
    {
        UpdateDrawFrame(app);
    }
#endif
}

// Update data and draw elements to the screen for the current frame
void UpdateDrawFrame(AppData *app)
{
    // Update
    // --------------------------------------------------------------------------------
    // Compute required framebuffer scaling
    float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);

    SetExitKey(KEY_NULL); // No exit key (use alt+F4 or in-game exit)

    // Debug: q for fast quitting
    // SetExitKey(KEY_Q);

    HandleToggleFullscreen(app);

    if (app->skipCurrentFrame == true)
    {
        app->skipCurrentFrame = false;
        PollInputEvents(); // Skip the current frame's input
        return;
    }

    UpdateBeepSound();

    switch(app->pong.currentScreen)
    {
        case SCREEN_LOGO:     UpdateRaylibLogo(&app->raylibLogo, &app->pong);
                              break;
        case SCREEN_TITLE:    UpdateTitleMenuFrame(&app->titleMenu, &app->pong);
                              break;
        case SCREEN_GAMEPLAY: UpdatePongFrame(&app->pong, &app->titleMenu);
                              break;

        default: break;
    }
    // --------------------------------------------------------------------------------

    // Draw
    // --------------------------------------------------------------------------------
    BeginTextureMode(app->renderTarget); // Draw to the render texture for screen scaling
    {
        ClearBackground(BLACK); // Default background color

        switch(app->pong.currentScreen)
        {
            case SCREEN_LOGO:     DrawRaylibLogo(&app->raylibLogo);
                                  break;
            case SCREEN_TITLE:    DrawTitleMenuFrame(&app->titleMenu);
                                  break;
            case SCREEN_GAMEPLAY: DrawPongFrame(&app->pong);
                                  break;
            default: break;
        }
    } EndTextureMode();

    BeginDrawing(); // Draw to screen
    {
        // Fill in any potential area outside of the render texture
        ClearBackground(BLACK); // Default background color
        // DrawRectangle(0, 0, RENDER_WIDTH, RENDER_HEIGHT, BLACK);

        // Draw render texture to screen, properly scaled
        float destPosX = (GetScreenWidth() - ((float)RENDER_WIDTH*scale))*0.5f;
        float destPosY = (GetScreenHeight() - ((float)RENDER_HEIGHT*scale))*0.5f;
        DrawTexturePro(app->renderTarget.texture,
                       (Rectangle){ 0.0f, 0.0f, (float)app->renderTarget.texture.width, (float)-app->renderTarget.texture.height },
                       (Rectangle){ destPosX, destPosY, (float)RENDER_WIDTH*scale, (float)RENDER_HEIGHT*scale },
                       (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Debug:
        // DrawFPS(0,0);
    } EndDrawing();
    // --------------------------------------------------------------------------------
}

void HandleToggleFullscreen(AppData *app)
{
#if !defined(PLATFORM_WEB) // No fullscreen input for web because it's buggy
                           // For now just use emscripten's fullscreen button

    // Fullscreen inputs: F11, Alt+Enter, and Shift+F
    bool isPressedAltEnter = ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER));
    bool isPressedShiftF = ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_F));
    if (IsKeyPressed(KEY_F11) || isPressedAltEnter || isPressedShiftF)
    {
        // Borderless Windowed is generally nicer to use on desktop
        ToggleBorderlessWindowed();
        // ToggleFullscreen();
        app->skipCurrentFrame = true;
    }

#endif
}

