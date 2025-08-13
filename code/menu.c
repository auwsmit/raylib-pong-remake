// EXPLANATION:
// For managing the title screen menu
// See menu.h for more documentation/descriptions

#include "menu.h"

#include "raylib.h"
#include "raymath.h" // needed for Vector math

#include "config.h"

MenuState InitMenuState(void)
{
    MenuButton title     = InitMenuTitleButton("Pong Remake");

    // Main buttons
    MenuButton onePlayer = InitMenuButton("One Player", &title, MENU_SPACE_FROM_TITLE);
    MenuButton twoPlayer = InitMenuButton("Two Player", &onePlayer, MENU_BUTTON_SPACING);
    MenuButton demo      = InitMenuButton("Demo", &twoPlayer, MENU_BUTTON_SPACING);
#if !defined(PLATFORM_WEB)
    MenuButton exitGame  = InitMenuButton("Exit", &demo, MENU_BUTTON_SPACING);
#endif

    // Difficulty buttons
    MenuButton easy   = InitMenuButton("Easy", &title, MENU_SPACE_FROM_TITLE);
    MenuButton medium = InitMenuButton("Medium", &easy, MENU_BUTTON_SPACING);
    MenuButton hard   = InitMenuButton("Hard", &medium, MENU_BUTTON_SPACING);
    MenuButton back   = InitMenuButton("Back", &hard, MENU_BUTTON_SPACING);

    MenuState state =
    {
        MENU_SS_DEFAULT, // currentScreen
        title, // title
        { // options
            onePlayer, twoPlayer, demo,
#if !defined(PLATFORM_WEB)
            exitGame
#endif
        },
        { // difficulties
            easy, medium, hard, back
        },
        0,     // selectedIndex
        MENU_CURSOR_SIZE, // cursorSize
        0.0f,  // keyHeldTime
        true, // firstFrame
        false, // autoScroll
        false, // mouseWithinButton
    };

    return state;
}

MenuButton InitMenuTitleButton(char *text)
{
    int fontSize = MENU_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float textPosX = (RENDER_WIDTH - (float)textWidth) / 2;
#if !defined(PLATFORM_WEB) // different spacing for web
    float textPosY = MENU_TITLE_SPACE_FROM_TOP;
#else
    float textPosY = MENU_TITLE_SPACE_FROM_TOP + MENU_BUTTON_SIZE;
#endif

    MenuButton button =
    {
        text, fontSize,
        { textPosX, textPosY },
        { 0, 0 }, // offset
        RAYWHITE, // color
    };

    return button;
}

MenuButton InitMenuButton(char* text, MenuButton *originButton, float offsetY)
{
    int fontSize = MENU_BUTTON_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float textPosX = (RENDER_WIDTH - (float)textWidth) / 2;
    float textPosY = originButton->position.y + originButton->fontSize + originButton->offset.y;

    MenuButton button =
    {
        text, fontSize,
        { textPosX, textPosY },
        { 0.0f, offsetY },
        RAYWHITE,
    };

    return button;
}

void UpdateTitleMenuFrame(MenuState *menu, GameState *pong)
{
    // Escape or Backspace to go back
    if ((IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) &&
        menu->currentScreen != MENU_SS_DEFAULT)
    {
        menu->currentScreen = MENU_SS_DEFAULT;
        menu->firstFrame = true;
    }

    UpdateMenuCursorSelect(menu, pong);
    UpdateMenuCursorMove(menu);
}

void UpdateMenuCursorMove(MenuState *menu)
{
    if (menu->firstFrame == true )
    {
        if (menu->currentScreen == MENU_SS_DEFAULT)
            menu->selectedIndex = 0;
        else if (menu->currentScreen == MENU_SS_DIFFICULTY)
            menu->selectedIndex = (MenuOption)DIFFICULTY_MEDIUM;
    }

    int totalButtons;
    if (menu->currentScreen == MENU_SS_DEFAULT)
        totalButtons = ARRAY_SIZE(menu->buttons);
    else if (menu->currentScreen == MENU_SS_DIFFICULTY)
        totalButtons = ARRAY_SIZE(menu->difficulties);

    // Move cursor via mouse
    bool mouseMoved = Vector2Length(GetMouseDelta()) > 0;
    if (mouseMoved || menu->firstFrame)
    {
        menu->mouseWithinButton = false;
        Vector2 scaleFactor = { (float)RENDER_WIDTH / GetScreenWidth(),
            (float)RENDER_HEIGHT / GetScreenHeight() };
        Vector2 mousePos = Vector2Multiply(GetMousePosition(), scaleFactor);

        for (int i = 0; i < totalButtons; i++)
        {
            MenuButton *currentButton;
            if (menu->currentScreen == MENU_SS_DEFAULT)
                currentButton = &menu->buttons[i];
            else if (menu->currentScreen == MENU_SS_DIFFICULTY)
                currentButton = &menu->difficulties[i];

            if (isMouseWithinButton(mousePos, currentButton))
            {
                menu->mouseWithinButton = true;
                menu->selectedIndex = i;
                menu->autoScroll = false;
            }
        }
    }

    // Move cursor via keyboard
    bool isMenuInputUp = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP));
    bool isMenuInputDown = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN));
    const float autoScrollInitPause = 0.6f;

    if ((!menu->autoScroll && menu->keyHeldTime == 0) ||
        (menu->autoScroll && menu->keyHeldTime >= 0.1f))
    {
        if (isMenuInputUp)
        {
            if (menu->selectedIndex > 0)
                menu->selectedIndex--;
            else
                menu->selectedIndex = totalButtons - 1;
            menu->keyHeldTime = 0;
        }
        if (isMenuInputDown)
        {
            if ((int)menu->selectedIndex < totalButtons - 1)
                menu->selectedIndex++;
            else
                menu->selectedIndex = 0;
            menu->keyHeldTime = 0.0f;
        }
    }

    // Update auto-scroll timer when holding keys
    if (isMenuInputUp || isMenuInputDown)
    {
        menu->keyHeldTime += GetFrameTime();
        if (menu->keyHeldTime >= autoScrollInitPause)
        {
            menu->autoScroll = true;
        }
    }
    else
    {
        menu->keyHeldTime = 0;
        menu->autoScroll = false;
    }

    menu->firstFrame = false;
}

void UpdateMenuCursorSelect(MenuState *menu, GameState *pong)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
        (IsGestureDetected(GESTURE_TAP) && menu->mouseWithinButton))
    {
        if (menu->currentScreen == MENU_SS_DEFAULT)
        {
            if (menu->selectedIndex == MENU_EXIT)
                pong->gameShouldExit = true;
            else if (menu->selectedIndex == MENU_1PLAYER)
            {
                pong->currentMode   = (GameMode)menu->selectedIndex;
                menu->currentScreen = MENU_SS_DIFFICULTY;
                menu->firstFrame = true;
            }
            else
            {
                pong->currentMode = (GameMode)menu->selectedIndex;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }
        else if (menu->currentScreen == MENU_SS_DIFFICULTY)
        {
            if (menu->selectedIndex == MENU_EXIT)
            {
                    menu->currentScreen = MENU_SS_DEFAULT;
                    menu->firstFrame = true;
            }
            else
            {
                pong->difficulty = (Difficulty)menu->selectedIndex;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }
    }

#if !defined(PLATFORM_WEB)
    if (pong->currentScreen == SCREEN_GAMEPLAY)
        HideCursor();
#endif
}

bool isMouseWithinButton(Vector2 mousePos, MenuButton *button)
{
    int padding = 20;
    int buttonWidth = MeasureText(button->text, button->fontSize);
    Vector2 buttonPos = Vector2Add(button->position, button->offset);
    if ((mousePos.x >= buttonPos.x - padding) &&
        (mousePos.x <= buttonPos.x + buttonWidth + padding) &&
        (mousePos.y >= buttonPos.y - padding) &&
        (mousePos.y <= buttonPos.y + button->fontSize + padding))
        return true;
    else
        return false;
}

void DrawTitleMenuFrame(MenuState *menu)
{
    DrawMenuElement(&menu->title);

    if (menu->currentScreen == MENU_SS_DEFAULT)
    {
#if defined(PLATFORM_WEB)
        const int menuTotalOptions = MENU_TOTAL_OPTIONS - 1;
#else
        const int menuTotalOptions = MENU_TOTAL_OPTIONS;
#endif
        for (int i = 0; i < menuTotalOptions; i++)
        {
            DrawMenuElement(&menu->buttons[i]);
        };
    }
    else if (menu->currentScreen == MENU_SS_DIFFICULTY)
    {
        for (int i = 0; i < ARRAY_SIZE(menu->difficulties); i++)
        {
            DrawMenuElement(&menu->difficulties[i]);
        };
    }

    DrawCursor(menu);

    // Debug:
    // DrawText(TextFormat("cursor selected: %i", menu->selectedIndex), 0, 40, 40, WHITE);
}

void DrawMenuElement(MenuButton *button)
{
    int elementPosX = (int)button->position.x + (int)button->offset.x;
    int elementPosY = (int)button->position.y + (int)button->offset.y;

    DrawText(button->text, elementPosX, elementPosY,
             button->fontSize, RAYWHITE);
}

void DrawCursor(MenuState *menu)
{
    float size = menu->cursorSize;
    MenuButton *selected = 0; // the option that the cursor is currently pointing at
    if (menu->currentScreen == MENU_SS_DEFAULT)
        selected = &menu->buttons[menu->selectedIndex];
    else if (menu->currentScreen == MENU_SS_DIFFICULTY)
        selected = &menu->difficulties[menu->selectedIndex];

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50.0f, (float)selected->fontSize / 2};
    selectPointPos = Vector2Add(selected->position, cursorOffset);
    selectPointPos = Vector2Add(selectPointPos, selected->offset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}
