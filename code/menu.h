// EXPLANATION:
// For managing the title screen menu

#ifndef PONG_MENU_HEADER_GUARD
#define PONG_MENU_HEADER_GUARD

#include "states.h"

// Macros
// --------------------------------------------------------------------------------

// Size of menu elements
#define MENU_TITLE_SIZE  180   // title font size
#define MENU_BUTTON_SIZE 80    // button text font size
#define MENU_CURSOR_SIZE 20.0f // cursor triangle size

// Menu spacing in pixels
#define MENU_TITLE_SPACE_FROM_TOP 100 // space from the top of the screen
#define MENU_SPACE_FROM_TITLE     200 // space between the first option and title text
#define MENU_BUTTON_SPACING       50  // spacing between each button

// Prototypes
// --------------------------------------------------------------------------------

// Initialize
MenuState InitMenuState(void); // Initializes the entire title screen
MenuButton InitMenuTitleButton(char* text);
MenuButton InitMenuButton(char* text, MenuButton *originButton, float offsetY);

// Update / Input
void UpdateTitleMenuFrame(MenuState *menu, GameState *pong); // Updates the menu for the current frame
void UpdateMenuCursorMove(MenuState *menu); // Updates the cursor for movement by user input
void UpdateMenuCursorSelect(MenuState *menu, GameState *pong); // Updates the cursor for button selection
bool isMouseWithinButton(Vector2 mousePos, MenuButton *button);

// Draw
void DrawTitleMenuFrame(MenuState *state); // Draws the menu for the current frame
void DrawMenuElement(MenuButton *button);
void DrawCursor(MenuState *menu);

#endif // PONG_MENU_HEADER_GUARD
