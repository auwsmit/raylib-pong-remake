#ifndef TEST_LOGO_HEADER_GUARD
#define TEST_LOGO_HEADER_GUARD

#include "config.h"

// Macros
#define RAYLIB_LOGO_WIDTH 256 // 128 is the minimum recommended size
#define RAYLIB_LOGO_OUTLINE (RAYLIB_LOGO_WIDTH / 16)
#define RAYLIB_LOGO_FONT_SIZE (RAYLIB_LOGO_WIDTH/8 + RAYLIB_LOGO_OUTLINE)

// Types and Structures
typedef enum LogoState { START, BOX1, BOX2, TEXT, END } LogoState;

typedef struct Logo {
    int logoPositionX;
    int logoPositionY;

    int framesCount;
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
void DrawRaylibLogo(Logo *l);
void UpdateRaylibLogo(Logo *l);

#endif // TEST_LOGO_HEADER_GUARD

