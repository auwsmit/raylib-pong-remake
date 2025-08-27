// EXPLANATION:
// For managing the user interface (text, buttons, menus)

#ifndef PONG_MENU_HEADER_GUARD
#define PONG_MENU_HEADER_GUARD

#include "states.h"

// Macros
// --------------------------------------------------------------------------------

// Size of menu elements
#define UI_TITLE_SIZE  180   // title font size
#define UI_BUTTON_SIZE 80    // button text font size
#define UI_CURSOR_SIZE 20.0f // cursor triangle size

// Ui spacing in pixels
#define UI_TITLE_SPACE_FROM_TOP 100 // space from the top of the screen
#define UI_SPACE_FROM_TITLE     200 // space between the first option and title text
#define UI_BUTTON_SPACING       50  // spacing between each button

// Prototypes
// --------------------------------------------------------------------------------

// Initialize
UiState InitUiState(void); // Initializes the title screen and allocates memory for buttons
UiButton InitUiTitle(char *text);
UiButton InitUiButton(char *text, int fontSize, float textPosX, float textPosY);
UiButton *InitUiMenuButton(char *text, int fontSize, float textPosX, float textPosY, UiMenu *menu);
UiButton *InitUiButtonRelative(char* text, UiButton *originButton, float offsetY, UiMenu *menu);
void FreeUiElements(UiState *menu); // Releases memory for menu buttons

// Update / Input
void UpdateUiFrame(UiState *ui, GameState *pong); // Updates the menu for the current frame
void UpdateUiMenuMove(UiState *menu, GameState *pong); // Updates the cursor for movement by user input
void UpdateUiButtonMove(UiButton *button, GameState *pong);
void UpdateUiButtonSelect(UiButton *button, UiState *ui, GameState *pong);
bool IsMouseWithinButton(Vector2 mousePos, UiButton *button);

// Draw
void DrawUiFrame(UiState *state, GameState *pong); // Draws the menu for the current frame
void DrawUiElement(UiButton *button);
void DrawUiCursor(UiState *menu, UiButton *selected);

#endif // PONG_MENU_HEADER_GUARD
