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
        { // buttons
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
        true,  // firstFrame
        false, // autoScroll
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

    MenuButton button = { text, fontSize, { textPosX, textPosY }, RAYWHITE };

    return button;
}

MenuButton InitMenuButton(char* text, MenuButton *originButton, float offsetY)
{
    int fontSize = MENU_BUTTON_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float textPosX = (RENDER_WIDTH - (float)textWidth) / 2;
    float textPosY = originButton->position.y + originButton->fontSize;

    MenuButton button = { text, fontSize, { textPosX, textPosY + offsetY }, RAYWHITE };

    return button;
}

void UpdateTitleMenuFrame(MenuState *menu, GameState *pong)
{
    // Escape or Backspace or Right click to go back
    if ((IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) ||
         IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && menu->currentScreen != MENU_SS_DEFAULT)
    {
        menu->currentScreen = MENU_SS_DEFAULT;
        menu->firstFrame = true;
    }

    UpdateMenuCursorSelect(menu, pong);
    UpdateMenuCursorMove(menu);
}

void UpdateMenuCursorMove(MenuState *menu)
{
    // Set default position for cursor on first menu frame
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
        Vector2 scaleFactor = { (float)RENDER_WIDTH / GetScreenWidth(),
            (float)RENDER_HEIGHT / GetScreenHeight() };
        Vector2 mousePos = Vector2Multiply(GetMousePosition(), scaleFactor);

        for (int i = 0; i < totalButtons; i++)
        {
            MenuButton *currentButton = 0;
            if (menu->currentScreen == MENU_SS_DEFAULT)
                currentButton = &menu->buttons[i];
            else if (menu->currentScreen == MENU_SS_DIFFICULTY)
                currentButton = &menu->difficulties[i];

            if (isMouseWithinButton(mousePos, currentButton))
            {
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
    Vector2 scaleFactor = { (float)RENDER_WIDTH / GetScreenWidth(),
        (float)RENDER_HEIGHT / GetScreenHeight() };
    Vector2 mousePos = Vector2Multiply(GetMousePosition(), scaleFactor);

    MenuButton *currentButton;
    if (menu->currentScreen == MENU_SS_DEFAULT)
        currentButton = &menu->buttons[menu->selectedIndex];
    else if (menu->currentScreen == MENU_SS_DIFFICULTY)
        currentButton = &menu->difficulties[menu->selectedIndex];

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
        (IsGestureDetected(GESTURE_TAP) &&
         (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && isMouseWithinButton(mousePos, currentButton))))
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
}

bool isMouseWithinButton(Vector2 mousePos, MenuButton *button)
{
    int padding = 20; // extra clickable area around the text
    int buttonWidth = MeasureText(button->text, button->fontSize);
    if ((mousePos.x >= button->position.x - padding) &&
        (mousePos.x <= button->position.x + buttonWidth + padding) &&
        (mousePos.y >= button->position.y - padding) &&
        (mousePos.y <= button->position.y + button->fontSize + padding))
        return true;
    else
        return false;
}

void DrawTitleMenuFrame(MenuState *menu)
{
    DrawMenuElement(&menu->title);

    if (menu->currentScreen == MENU_SS_DEFAULT)
    {
        for (int i = 0; i < ARRAY_SIZE(menu->buttons); i++)
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
    DrawText(button->text,
             button->position.x, button->position.y,
             button->fontSize, RAYWHITE);
}

void DrawCursor(MenuState *menu)
{
    float size = menu->cursorSize;
    MenuButton *selected = 0; // the button that the cursor is currently pointing at
    if (menu->currentScreen == MENU_SS_DEFAULT)
        selected = &menu->buttons[menu->selectedIndex];
    else // if (menu->currentScreen == MENU_SS_DIFFICULTY)
        selected = &menu->difficulties[menu->selectedIndex];

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50.0f, (float)selected->fontSize / 2};
    selectPointPos = Vector2Add(selected->position, cursorOffset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}
