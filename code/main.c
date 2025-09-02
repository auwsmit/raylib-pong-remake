// EXPLANATION:
// The main entry point for the game/program
// - See header files for more explanations/documentation

#include "raylib.h"

#include "config.h"  // Program config, e.g. window title/size, fps, vsync
#include "input.h"   // Input controls / key mappings
#include "logo.h"    // Raylib logo animation
#include "ui.h"      // User interface (menus and buttons)
#include "pong.h"    // Game logic

#if defined(PLATFORM_WEB) // for compiling to wasm (web assembly)
    #include <emscripten/emscripten.h>
#endif

// Globals
// ----------------------------------------------------------------------------
RenderTexture2D renderTarget; // used to hold the rendering result to rescale window
GameState pongGame; // for game logic data
UiState   pongUi;   // for user interface data

// Local Functions Declaration
// ----------------------------------------------------------------------------
void CreateNewWindow(void); // Creates a new window with the proper initial settings
void RunGameLoopForPlatform(void); // Runs the game loop depending on platform,
                                   // this is just for emscripten's main function

void UpdateDrawFrame(void); // Update and Draw the current frame
                            // Most of the game loop's code is found in here

// Main entry point
// ----------------------------------------------------------------------------
int main(void)
{
    // Initialization
    // ----------------------------------------------------------------------------
    CreateNewWindow();

    // Init the render texture, used to hold the rendering result so we can easily resize it
    renderTarget = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    SetTextureFilter(renderTarget.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter

    // Init everything else
    InitAudioDevice();
    InitDefaultInputControls();
    InitRaylibLogo();
    InitUiState();   // also allocates memory for menu buttons
    InitGameState(); // also allocates memory for beep sound effects

    // No exit key (use alt+F4 or in-game exit option)
    SetExitKey(KEY_NULL);

    // Start the game loop
    // (See UpdateDrawFrame() for the full game loop)
    RunGameLoopForPlatform();

    // De-Initialization
    // ----------------------------------------------------------------------------
    FreeBeeps();
    FreeUiMenuButtons();
    CloseAudioDevice();
    UnloadRenderTexture(renderTarget);
    CloseWindow(); // Close window and OpenGL context

    return 0;
}

void CreateNewWindow(void)
{
#if defined(PLATFORM_WEB)
    SetConfigFlags(0); // no vsync or window resize for web
#else
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE;
    if (VSYNC_ENABLED) windowFlags |= FLAG_VSYNC_HINT;
    SetConfigFlags(windowFlags);
#endif
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(320, 240);
}

void RunGameLoopForPlatform(void)
{
#if defined(PLATFORM_WEB)
    const int emscriptenFPS = 0; // Let emscripten handle the framerate because setting a specific one is kinda janky
                                 // Generally, it will use whatever the monitor's refresh rate is
    emscripten_set_main_loop(UpdateDrawFrame, emscriptenFPS, 1);
#else
    if (MAX_FRAMERATE > 0)
        SetTargetFPS(MAX_FRAMERATE);
    // ----------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !pongGame.gameShouldExit) // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif
}

// Update data and draw elements to the screen for the current frame
void UpdateDrawFrame(void)
{
    // Update
    // ----------------------------------------------------------------------------
    HandleToggleFullscreen();

    // Compute required framebuffer scaling
    float renderScale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);
    float renderPosX = (GetScreenWidth() - ((float)RENDER_WIDTH*renderScale))*0.5f;
    float renderPosY = (GetScreenHeight() - ((float)RENDER_HEIGHT*renderScale))*0.5f;

    switch(pongGame.currentScreen)
    {
        case SCREEN_LOGO:     UpdateRaylibLogo();
                              break;
        case SCREEN_TITLE:    UpdateUiFrame();
                              break;
        case SCREEN_GAMEPLAY: UpdatePongFrame();
                              break;
        default: break;
    }

    // Draw
    // ----------------------------------------------------------------------------

    BeginTextureMode(renderTarget); // Draw to the render texture for screen scaling
    {
        ClearBackground(BLACK); // Default background color

        switch(pongGame.currentScreen)
        {
            case SCREEN_LOGO:     DrawRaylibLogo();
                                  break;
            case SCREEN_TITLE:    DrawUiFrame();
                                  break;
            case SCREEN_GAMEPLAY: DrawPongFrame();
                                  break;
            default: break;
        }
    } EndTextureMode();

    BeginDrawing(); // Draw to screen
    {
        // Fill in any potential area outside of the render texture
        ClearBackground(BLACK); // Default background color

        // Draw render texture to screen, properly scaled
        DrawTexturePro(renderTarget.texture,
                       (Rectangle){ 0.0f, 0.0f, (float)renderTarget.texture.width, (float)-renderTarget.texture.height },
                       (Rectangle){ renderPosX, renderPosY, (float)RENDER_WIDTH*renderScale, (float)RENDER_HEIGHT*renderScale },
                       (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Debug:
        // DrawFPS(0,0);
    } EndDrawing();
}

