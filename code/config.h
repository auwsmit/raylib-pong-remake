// EXPLANATION:
// For configuring aspects of the program outside of game logic

#ifndef PONG_CONFIG_HEADER_GUARD
#define PONG_CONFIG_HEADER_GUARD

// Global macros
// --------------------------------------------------------------------------------

#define WINDOW_TITLE "Pong Remake with raylib"
#define ASPECT_RATIO (4.0f / 3.0f)
#define RENDER_HEIGHT 1080 // The native resolution of the game itself
                           // This will scale to the game window
#define RENDER_WIDTH (int)(RENDER_HEIGHT * ASPECT_RATIO)

#define DEFAULT_HEIGHT 720 // Default size of the game window
#define DEFAULT_WIDTH (int)(DEFAULT_HEIGHT * ASPECT_RATIO)

// there may be small bugs with very high FPS (uncapped + no vsync), but should work fine overall
#define MAX_FRAMERATE 120 // Set to 0 for uncapped framerate
#define VSYNC_ENABLED true

#define MAX(a, b) ((a)>(b)? (a) : (b)) // Used to calculate framebuffer scaling
#define MIN(a, b) ((a)<(b)? (a) : (b))

#endif // PONG_CONFIG_HEADER_GUARD
