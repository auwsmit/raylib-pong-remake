#ifndef TEST_MENU_HEADER_GUARD
#define TEST_MENU_HEADER_GUARD

#include "pong.h"

// Macros
// --------------------------------------------------------------------------------

#define MENU_TITLE_SPACE_FROM_TOP 100
#define MENU_SPACE_FROM_TITLE 200
#define MENU_OPTION_SPACING 50

#if defined(PLATFORM_WEB)
    #define MENU_TOTAL_OPTIONS 3 // no Exit option for web
#else
    #define MENU_TOTAL_OPTIONS 4
#endif

#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

// Types and Structures
// --------------------------------------------------------------------------------
typedef enum MenuScreenState { MENU_SS_DEFAULT, MENU_SS_DIFFICULTY } MenuScreenState;
typedef enum MenuOption // these correspond enum GameMode in pong.h
{
    MENU_1PLAYER, MENU_2PLAYER, MENU_DEMO, MENU_EXIT
} MenuOption;

typedef struct MenuButton
{
    const char *text;
    int fontSize;
    Vector2 position;
    Vector2 offset;
    Color color;
} MenuButton;

typedef struct MenuState
{
    MenuScreenState currentScreen;
    MenuButton title;
    MenuButton options[MENU_TOTAL_OPTIONS];
    MenuButton difficulties[3];
    float cursorSize;
    MenuOption selectedIndex;
} MenuState;

// Prototypes
// --------------------------------------------------------------------------------
MenuState InitMenuState(void);
MenuButton InitMenuButtonTitle(char* text);
MenuButton InitMenuButtonOption(char* text, MenuButton *originButton, float offsetY);
void UpdateMenuCursorMove(MenuState *menu);
void UpdateMenuCursorSelect(MenuState *menu, GameState *pong);
void UpdateStartMenu(MenuState *menu, GameState *pong);
void DrawMenuElement(MenuButton *button);
void DrawCursor(MenuState *menu);
void DrawStartMenu(MenuState *state);

#endif // TEST_MENU_HEADER_GUARD
