#ifndef TEST_CONFIG_HEADER_GUARD
#define TEST_CONFIG_HEADER_GUARD

// Global macros
#define ASPECT_RATIO (4.0f / 3.0f)
#define RENDER_HEIGHT 1080 // The native resolution of the game itself.
                           // This will scale to the game window.
#define RENDER_WIDTH (RENDER_HEIGHT * ASPECT_RATIO)

#define DEFAULT_HEIGHT 640 // Default size of the game window.
#define DEFAULT_WIDTH (DEFAULT_HEIGHT * ASPECT_RATIO)

#endif // TEST_CONFIG_HEADER_GUARD
