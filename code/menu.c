// EXPLANATION:
// For managing the title screen menu.
// See menu.h for more documentation/descriptions.

#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "menu.h"

MenuState InitMenuState(void)
{
    MenuButton title     = InitMenuButtonTitle("Pong Remake");

    // Main buttons
    MenuButton onePlayer = InitMenuButtonOption("One Player", &title, MENU_SPACE_FROM_TITLE);
    MenuButton twoPlayer = InitMenuButtonOption("Two Player", &onePlayer, MENU_OPTION_SPACING);
    MenuButton demo      = InitMenuButtonOption("Demo", &twoPlayer, MENU_OPTION_SPACING);
#if !defined(PLATFORM_WEB)
    MenuButton exitGame  = InitMenuButtonOption("Exit", &demo, MENU_OPTION_SPACING);
#endif

    // Difficulty buttons
    MenuButton easy   = InitMenuButtonOption("Easy", &title, MENU_SPACE_FROM_TITLE);
    MenuButton medium = InitMenuButtonOption("Medium", &easy, MENU_OPTION_SPACING);
    MenuButton hard   = InitMenuButtonOption("Hard", &medium, MENU_OPTION_SPACING);

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
            easy, medium, hard
        },
        0,     // selectedIndex
        MENU_CURSOR_SIZE, // cursorSize
        0.0f,  // keyHeldTime
        0,     // lastKeyHeld
        false, // autoScroll
    };
    return state;
}

MenuButton InitMenuButtonTitle(char *text)
{
    int fontSize = MENU_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float textPosX = (RENDER_WIDTH - (float)textWidth) / 2;
    // int textPosY = (RENDER_HEIGHT / 2) - RENDER_HEIGHT / 2.5f;
#if !defined(PLATFORM_WEB)
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

MenuButton InitMenuButtonOption(char* text, MenuButton *originButton, float offsetY)
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

void UpdateMenuCursorMove(MenuState *menu)
{
    int indexLimit; // used to determine the beginning/end of list
    if (menu->currentScreen == MENU_SS_DEFAULT)
        indexLimit = MENU_TOTAL_OPTIONS - 1;
    if (menu->currentScreen == MENU_SS_DIFFICULTY)
        indexLimit = ARRAY_SIZE(menu->difficulties) - 1;


    // Move cursor
    bool menuInputUp = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP));
    bool menuInputDown = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN));
    const float autoScrollInitPause = 0.6f;

    if ((!menu->autoScroll && menu->keyHeldTime == 0) ||
        (menu->autoScroll && menu->keyHeldTime >= 0.1f))
    {
        if (menuInputUp)
        {
            if (menu->selectedIndex > 0)
                menu->selectedIndex--;
            else
                menu->selectedIndex = indexLimit;
            menu->keyHeldTime = 0;
        }
        else if (menuInputDown)
        {
            if ((int)menu->selectedIndex < indexLimit)
                menu->selectedIndex++;
            else
                menu->selectedIndex = 0;
            menu->keyHeldTime = 0.0f;
        }
    }

    // Update data needed for auto scrolling after holding Up or Down
    if (menuInputUp || menuInputDown)
    {
        menu->keyHeldTime += GetFrameTime();
        if (IsKeyDown(menu->lastKeyHeld))
        {
            if (menu->keyHeldTime >= autoScrollInitPause)
            {
                menu->autoScroll = true;
            }
        }

        // Update last key held
        int currentKey = GetKeyPressed();
        if (currentKey != 0)
            menu->lastKeyHeld = currentKey;
    }
    else
    {
        menu->lastKeyHeld = 0;
        menu->keyHeldTime = 0;
        menu->autoScroll = false;
    }
}

void UpdateMenuCursorSelect(MenuState *menu, GameState *pong)
{
    MenuOption selectedChoice = menu->selectedIndex;
    if (IsKeyPressed(KEY_ENTER))
    {
        if (menu->currentScreen == MENU_SS_DEFAULT)
        {
            if (selectedChoice == MENU_EXIT)
                pong->gameShouldExit = true;
            else if (selectedChoice == MENU_1PLAYER)
            {
                pong->currentMode   = (GameMode)selectedChoice;
                menu->currentScreen = MENU_SS_DIFFICULTY;
                menu->selectedIndex = (MenuOption)DIFFICULTY_MEDIUM;
            }
            else
            {
                pong->currentMode = (GameMode)selectedChoice;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }
        else if (menu->currentScreen == MENU_SS_DIFFICULTY)
        {
            pong->difficulty = (Difficulty)selectedChoice;
            pong->currentScreen = SCREEN_GAMEPLAY;
        }
    }
}

void UpdateTitleMenuFrame(MenuState *menu, GameState *pong)
{

    UpdateMenuCursorMove(menu);
    UpdateMenuCursorSelect(menu, pong);

    // Escape or Backspace to go back
    if ((IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) &&
        menu->currentScreen != MENU_SS_DEFAULT)
    {
        menu->currentScreen = MENU_SS_DEFAULT;
        menu->selectedIndex = 0;
    }
}

void DrawMenuElement(MenuButton *button)
{
    DrawText(button->text,
             (int)button->position.x + (int)button->offset.x,
             (int)button->position.y + (int)button->offset.y,
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
