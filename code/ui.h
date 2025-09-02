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

// Types and Structures
// ----------------------------------------------------------------------------

typedef enum UiMenuState
{
    UI_MENU_TITLE, UI_MENU_DIFFICULTY, UI_MENU_PAUSE, UI_MENU_GAMEPLAY
} UiMenuState;

typedef enum UiTitleMenuId
{
    UI_BID_1PLAYER, UI_BID_2PLAYER, UI_BID_DEMO, UI_BID_EXIT
} UiTitleMenuId;

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
    UiMenu menus[3]; // title, difficulty, and pause menus
    float keyHeldTime;
    float textFade;            // tracks fade value over time
    float textFadeTimeElapsed; // tracks time for the fade animation
    UiMenuState currentMenu;
    unsigned int selectedId;
    bool firstFrame;
    bool lastSelectWithMouse;
    bool autoScroll;
} UiState;

extern UiState pongUi; // global declaration

// Prototypes
// ----------------------------------------------------------------------------

// Initialize
void InitUiState(void); // Initializes the title screen and allocates memory for menu buttons
UiButton InitUiTitle(char *text);
UiButton InitUiButton(char *text, int fontSize, float textPosX, float textPosY);
UiButton *InitUiMenuButton(char *text, int fontSize, float textPosX, float textPosY, UiMenu *menu);
UiButton *InitUiMenuButtonRelative(char* text, int fontSize, UiButton *originButton, float offsetY, UiMenu *menu);
void FreeUiMenuButtons(void);

// Update / User Input
void UpdateUiFrame(void); // Updates the menu for the current frame
void UpdateUiMenuTraverse(void); // Updates the cursor for movement by user input
void UpdateUiButtonMouseHover(UiButton *button); // Draw cursor when mouse is over button
void UpdateUiButtonSelect(UiButton *button); // Selects a button by user input
bool IsMouseWithinUiButton(Vector2 mousePos, UiButton *button);
void ChangeUiMenu(UiMenuState newMenu); // Change from one menu to another

// Draw
void DrawUiFrame(void); // Draws the menu for the current frame
void DrawUiElement(UiButton *button);
void DrawUiCursor(UiButton *selected); // Draw the cursor at the given button
void DrawUiFieldLines(bool isPaused, bool isDemoMode);
void DrawUiScores(void);
void DrawUiWinnerMessage(int scoreL, int scoreR, Color fadeColor); // Draw the "Winner" text for the current winner

#endif // PONG_MENU_HEADER_GUARD
