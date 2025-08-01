#ifndef TEST_CONFIG_HEADER_GUARD
#define TEST_CONFIG_HEADER_GUARD

// Global macros
// ------------------------------------------------------------------------------------------

#define WINDOW_TITLE "Raylib Testing"
#define ASPECT_RATIO (4.0f / 3.0f)
#define RENDER_HEIGHT 1080 // The native resolution of the game itself.
                           // This will scale to the game window.
#define RENDER_WIDTH (RENDER_HEIGHT * ASPECT_RATIO)

#define DEFAULT_HEIGHT 720 // Default size of the game window.
#define DEFAULT_WIDTH (DEFAULT_HEIGHT * ASPECT_RATIO)

// NOTE: Very high fps (~500+) is buggy because delta time between frames is too low to be tracked precisely.
// Unsure if/how to fix this so that the game could theoretically run fine at obsurdly high framerates.
#define MAX_FRAMERATE 120 // Set to 0 for uncapped framerate
#define VSYNC_ENABLED true

#define MAX(a, b) ((a)>(b)? (a) : (b)) // Used to calculate framebuffer scaling
#define MIN(a, b) ((a)<(b)? (a) : (b))

#endif // TEST_CONFIG_HEADER_GUARD
