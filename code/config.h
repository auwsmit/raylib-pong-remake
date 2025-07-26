#ifndef TEST_CONFIG_HEADER_GUARD
#define TEST_CONFIG_HEADER_GUARD

// Global macros
#define ASPECT_RATIO 4.0f / 3.0f
#define SCREEN_RES 500    // height of the window (width will be based on aspect ratio)
                          // NOTE: might make this a variable to allow user to resize game window in the future
#define RENDER_RES 720.0f // elements are rendered as if the window were this big
#define WIN_SCALE (SCREEN_RES / RENDER_RES)

#endif // TEST_CONFIG_HEADER_GUARD
