// EXPLANATION:
// For the raylib logo animation at start of program

#ifndef PONG_LOGO_HEADER_GUARD
#define PONG_LOGO_HEADER_GUARD

#include "states.h"

// Types and Structures
// --------------------------------------------------------------------------------
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

// Macros
// --------------------------------------------------------------------------------
#define RAYLIB_LOGO_WIDTH 512 // takes multiples of 64
                              // 192 is the minimum size
#define RAYLIB_LOGO_OUTLINE (RAYLIB_LOGO_WIDTH / 16)
#define RAYLIB_LOGO_FONT_SIZE (RAYLIB_LOGO_WIDTH/8 + RAYLIB_LOGO_OUTLINE)

// Prototypes
// --------------------------------------------------------------------------------
Logo InitRaylibLogo(void); // Initialize the logo animation
void UpdateRaylibLogo(Logo *logo, GameState *pong); // Update logo animation for the current frame
                                                    // Also transitions to title screen when finished
void DrawRaylibLogo(Logo *logo);

#endif // PONG_LOGO_HEADER_GUARD
