#ifndef TEST_MENU_HEADER_GUARD
#define TEST_MENU_HEADER_GUARD

#include "pong.h"

// Macros
// ----------------------------------------------------------------------------------
#define MENU_TOTAL_OPTIONS 4 // total options to pick on the main menu
#define MENU_TOTAL_DIFFS 3 // total difficulty options

// Types and Structures
// ----------------------------------------------------------------------------------
typedef enum MenuScreenState { MENU_SS_DEFAULT, MENU_SS_DIFFICULTY } MenuScreenState;
typedef enum MenuOption // these correspond enum GameMode in pong.h
{
    MENU_1PLAYER, MENU_2PLAYER, MENU_DEMO, MENU_EXIT
} MenuOption;

typedef struct MenuButton
{
    char *text;
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
    MenuButton difficulties[MENU_TOTAL_DIFFS];
    int cursorSize;
    MenuOption selectedIndex;
} MenuState;

// Prototypes
// ----------------------------------------------------------------------------------
MenuState InitMenuState(void);
MenuButton InitMenuButtonTitle(char* text);
MenuButton InitMenuButtonOption(char* text, MenuButton *originButton, int offsetY);
void UpdateMenuCursorMove(MenuState *menu);
void UpdateMenuCursorSelect(MenuState *menu, GameState *pong);
void UpdateStartMenu(MenuState *menu, GameState *pong);
void DrawMenuElement(MenuButton *button);
void DrawCursor(MenuState *menu);
void DrawStartMenu(MenuState *state);

#endif // TEST_MENU_HEADER_GUARD
