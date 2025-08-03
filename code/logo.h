#ifndef TEST_LOGO_HEADER_GUARD
#define TEST_LOGO_HEADER_GUARD

#include "config.h"

// Macros
#define RAYLIB_LOGO_WIDTH 512 // takes multiples of 64
                                 // 192 is the minimum size
#define RAYLIB_LOGO_OUTLINE (RAYLIB_LOGO_WIDTH / 16)
#define RAYLIB_LOGO_FONT_SIZE (RAYLIB_LOGO_WIDTH/8 + RAYLIB_LOGO_OUTLINE)

// Types and Structures
typedef enum LogoState { START, GROW1, GROW2, TEXT, PAUSE, END } LogoState;

typedef struct Logo {
    float positionX;
    float positionY;

    float elapsedTime;
    int lettersCount;

    float topSideRecWidth;
    float leftSideRecHeight;

    float bottomSideRecWidth;
    float rightSideRecHeight;

    LogoState state;   // Tracking animation states (State Machine)
    float alpha; // Useful for fading
} Logo;

// Prototypes
Logo InitRaylibLogo(void); // Initialize the logo animation
void UpdateRaylibLogo(Logo *logo); // Update logo animation for the current frame
void DrawRaylibLogo(Logo *logo);

#endif // TEST_LOGO_HEADER_GUARD
