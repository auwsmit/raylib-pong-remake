// EXPLANATION:
// For managing the user interface

#ifndef PONG_MENU_HEADER_GUARD
#define PONG_MENU_HEADER_GUARD

#include "raylib.h"

// Macros
// ----------------------------------------------------------------------------

// Size of menu elements
#define UI_TITLE_SIZE  180      // title font size
#define UI_TITLE_BUTTON_SIZE 80 // title menu button font size
#define UI_PAUSE_SIZE 50        // pause menu font size
#define UI_CURSOR_SIZE 20.0f    // cursor triangle size

// UI spacing
#define UI_TITLE_SPACE_FROM_TOP 100 // space from the top of the screen
#define UI_SPACE_FROM_TITLE     200 // space between the first option and title text
#define UI_BUTTON_SPACING       50  // spacing between each button

// UI element size
#define FIELD_LINE_WIDTH  15    // Width of the field lines (top, bottom, dotted center-line)
#define SCORE_FONT_SIZE 180     // Also used for pause font size
#define DIFFICULTY_FONT_SIZE 50 // For text that shows difficulty at bottom of screen
#define WIN_FONT_SIZE 100

// Forward Declarations
// ----------------------------------------------------------------------------
typedef struct GameState GameState;

// Types and Structures
// ----------------------------------------------------------------------------

typedef enum UiMenuState
{
    UI_MENU_TITLE, UI_MENU_DIFFICULTY, UI_MENU_PAUSE, UI_MENU_GAMEPLAY
} UiMenuState;

typedef enum UiButtonIdTitle
{
    UI_BID_1PLAYER, UI_BID_2PLAYER, UI_BID_DEMO, UI_BID_EXIT
} UiButtonIdTitle;

typedef enum UiDifficultyMenuId
{
    UI_BID_EASY, UI_BID_MEDIUM, UI_BID_HARD, UI_BID_BACK
} UiDifficultyMenuId;

typedef enum UiPauseMenuId
{
    UI_BID_RESUME, UI_BID_BACKTOTITLE
} UiPauseMenuId;

typedef struct UiButton
{
    const char *text;
    int fontSize;
    bool mouseHovered;
    Vector2 position;
    Color color;
} UiButton;

typedef struct UiMenu
{
    UiButton *buttons; // allocate buttons for variable length menus
    unsigned int buttonCount;
} UiMenu;

typedef struct UiState // Holds data for the title screen menu
{
    UiButton title; // Title text
    UiButton pause;
    UiMenu menus[3]; // title, difficulty, pause
    float cursorSize;
    float keyHeldTime;
    float textFade;            // tracks fade value over time
    float textFadeTimeElapsed; // tracks time for the fade animation
    UiMenuState currentMenu;
    unsigned int selectedId;
    bool firstFrame;
    bool lastSelectWithMouse;
    bool autoScroll;
} UiState;

// Prototypes
// ----------------------------------------------------------------------------

// Initialize
UiState InitUiState(void); // Initializes the title screen and allocates memory for buttons
UiButton InitUiTitle(char *text);
UiButton InitUiButton(char *text, int fontSize, float textPosX, float textPosY);
UiButton *InitUiMenuButton(char *text, int fontSize, float textPosX, float textPosY, UiMenu *menu);
UiButton *InitUiMenuButtonRelative(char* text, int fontSize, UiButton *originButton, float offsetY, UiMenu *menu);
void FreeUiElements(UiState *menu);

// Update / Input
void UpdateUiFrame(UiState *ui, GameState *pong); // Updates the menu for the current frame
void UpdateUiMenuTraverse(UiState *menu, GameState *pong); // Updates the cursor for movement by user input
void UpdateUiButtonMouseHover(UiButton *button, GameState *pong); // Draw cursor when mouse is over button
void UpdateUiButtonSelect(UiButton *button, UiState *ui, GameState *pong); // Selects a button by user input
bool IsMouseWithinUiButton(Vector2 mousePos, UiButton *button);
void ChangeUiMenu(UiMenuState newMenu, UiState *ui, GameState *pong); // Change from one menu state to another

// Draw
void DrawUiFrame(UiState *state, GameState *pong); // Draws the menu for the current frame
void DrawUiElement(UiButton *button);
void DrawUiCursor(UiState *menu, UiButton *selected);
void DrawUiFieldLines(bool isPaused, bool isDemoMode);
void DrawUiScores(GameState *pong);
void DrawUiWinnerMessage(int scoreL, int scoreR, Color fadeColor);

#endif // PONG_MENU_HEADER_GUARD
