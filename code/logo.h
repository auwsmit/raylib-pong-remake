#ifndef TEST_LOGO_HEADER_GUARD
#define TEST_LOGO_HEADER_GUARD

#include "config.h"

#define RAYLIB_LOGO_WIDTH 256
#define RAYLIB_LOGO_OUTLINE 16

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

