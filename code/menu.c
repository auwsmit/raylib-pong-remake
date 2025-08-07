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
        25.0f, // cursorSize
        0, // selectedIndex
    };
    return state;
}

MenuButton InitMenuButtonTitle(char *text)
{
    int fontSize = MENU_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float textPosX = (RENDER_WIDTH - (float)textWidth) / 2;
    // int textPosY = (RENDER_HEIGHT / 2) - RENDER_HEIGHT / 2.5f;
    float textPosY = MENU_TITLE_SPACE_FROM_TOP;
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
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
    {
        if (menu->selectedIndex > 0)
            menu->selectedIndex--;
        else
            menu->selectedIndex = indexLimit;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
    {
        if ((int)menu->selectedIndex < indexLimit)
            menu->selectedIndex++;
        else
            menu->selectedIndex = 0;
    }

    // TODO: hold key to keep moving after a small pause
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
                pong->gameMode      = (GameMode)selectedChoice;
                menu->currentScreen = MENU_SS_DIFFICULTY;
                menu->selectedIndex = (MenuOption)DIFFICULTY_MEDIUM;
            }
            else
            {
                pong->gameMode = (GameMode)selectedChoice;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }
        else if (menu->currentScreen == MENU_SS_DIFFICULTY)
        {
            pong->difficulty = (Difficulty)selectedChoice;
            pong->currentScreen = SCREEN_GAMEPLAY;
        }
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
    {
        menu->currentScreen = MENU_SS_DEFAULT;
        menu->selectedIndex = 0;
    }
}

void UpdateStartMenu(MenuState *menu, GameState *pong)
{
    UpdateMenuCursorMove(menu);
    UpdateMenuCursorSelect(menu, pong);
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
    MenuButton *selected; // the option that the cursor is currently pointing at
    if (menu->currentScreen == MENU_SS_DEFAULT)
        selected = &menu->options[menu->selectedIndex];
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

void DrawStartMenu(MenuState *menu)
{
    DrawMenuElement(&menu->title);

    if (menu->currentScreen == MENU_SS_DEFAULT)
    {
        for (int i = 0; i < MENU_TOTAL_OPTIONS; i++)
        {
            DrawMenuElement(&menu->options[i]);
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
