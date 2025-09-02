// EXPLANATION:
// For the raylib logo animation at start of program

#ifndef PONG_LOGO_HEADER_GUARD
#define PONG_LOGO_HEADER_GUARD

#include "raylib.h"

// Macros
// ----------------------------------------------------------------------------
#define RAYLIB_LOGO_WIDTH 512 // takes multiples of 64
                              // 192 is the minimum size
#define RAYLIB_LOGO_OUTLINE (RAYLIB_LOGO_WIDTH / 16)
#define RAYLIB_LOGO_FONT_SIZE (RAYLIB_LOGO_WIDTH/8 + RAYLIB_LOGO_OUTLINE)

// Types and Structures
// ----------------------------------------------------------------------------
typedef enum LogoState { LOGO_START, LOGO_GROW1, LOGO_GROW2, LOGO_TEXT, LOGO_PAUSE, LOGO_END } LogoState;

typedef struct Logo {
    float positionX;
    float positionY;

    float elapsedTime;
    int lettersCount;

    float topSideRecWidth;
    float leftSideRecHeight;
    float bottomSideRecWidth;
    float rightSideRecHeight;

    LogoState state; // Tracking animation states (State Machine)
    float alpha; // Useful for fading
} Logo;

extern Logo raylibLogo; // global declaration

// Prototypes
// ----------------------------------------------------------------------------
void InitRaylibLogo(void);   // Initialize the logo animation
void UpdateRaylibLogo(void); // Update logo animation for the current frame
                             // Also transitions to title screen when finished
void DrawRaylibLogo(void);

#endif // PONG_LOGO_HEADER_GUARD
