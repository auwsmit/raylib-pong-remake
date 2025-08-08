// EXPLANATION:
// For managing the title screen menu.

#ifndef TEST_MENU_HEADER_GUARD
#define TEST_MENU_HEADER_GUARD

#include "pong.h" // needed for GameState struct

// Macros
// --------------------------------------------------------------------------------

// Size of menu elements
#define MENU_TITLE_SIZE  180   // title font size
#define MENU_BUTTON_SIZE 80    // button text font size
#define MENU_CURSOR_SIZE 20.0f // cursor triangle size

// Menu spacing in pixels
#define MENU_TITLE_SPACE_FROM_TOP 100 // space from the top of the screen
#define MENU_SPACE_FROM_TITLE     200 // space between the first option and title text
#define MENU_OPTION_SPACING       50  // spacing between each option

#define MENU_TOTAL_OPTIONS 4
#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

// Types and Structures
// --------------------------------------------------------------------------------
typedef enum MenuScreenState { MENU_SS_DEFAULT, MENU_SS_DIFFICULTY } MenuScreenState;

typedef enum MenuOption // these correspond enum GameMode in pong.h
{
    MENU_1PLAYER, MENU_2PLAYER, MENU_DEMO, MENU_EXIT
} MenuOption;

typedef struct MenuButton // Holds data for a menu button
{
    const char *text;
    int fontSize;
    Vector2 position;
    Vector2 offset;
    Color color;
} MenuButton;

typedef struct MenuState // Holds data for the title screen menu
{
    MenuScreenState currentScreen;
    MenuButton title;
    MenuButton options[MENU_TOTAL_OPTIONS];
    MenuButton difficulties[3];
    MenuOption selectedIndex;
    float cursorSize;
    float keyHeldTime;
    int lastKeyHeld;
    bool autoScroll;
} MenuState;

// Prototypes
// --------------------------------------------------------------------------------
MenuState InitMenuState(void); // Initializes the entire title screen
MenuButton InitMenuButtonTitle(char* text);
MenuButton InitMenuButtonOption(char* text, MenuButton *originButton, float offsetY);
void UpdateMenuCursorMove(MenuState *menu);
void UpdateMenuCursorSelect(MenuState *menu, GameState *pong);
void UpdateTitleMenuFrame(MenuState *menu, GameState *pong);
void DrawMenuElement(MenuButton *button);
void DrawCursor(MenuState *menu);
void DrawTitleMenuFrame(MenuState *state);

#endif // TEST_MENU_HEADER_GUARD
