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
    int positionX;
    int positionY;

    float elapsedTime;
    int lettersCount;

    int topSideRecWidth;
    int leftSideRecHeight;

    int bottomSideRecWidth;
    int rightSideRecHeight;

    LogoState state;   // Tracking animation states (State Machine)
    float alpha; // Useful for fading
} Logo;

// Prototypes
Logo InitRaylibLogo(void);
void DrawRaylibLogo(Logo *logo);
void UpdateRaylibLogo(Logo *logo);

#endif // TEST_LOGO_HEADER_GUARD
