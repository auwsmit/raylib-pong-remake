#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "menu.h"

MenuState InitMenuState(void)
{
    MenuButton title     = InitMenuButtonTitle("Pong Remake");

    // Main buttons
    MenuButton onePlayer = InitMenuButtonOption("One Player", &title, 200); // 200 pixels below title button
    MenuButton twoPlayer = InitMenuButtonOption("Two Player", &onePlayer, 50); // 50 pixels below onePlayer button
    MenuButton demo      = InitMenuButtonOption("Demo", &twoPlayer, 50);
    MenuButton exitGame  = InitMenuButtonOption("Exit", &demo, 50);

    // Difficulty buttons
    MenuButton easy   = InitMenuButtonOption("Easy", &title, 200);
    MenuButton medium = InitMenuButtonOption("Medium", &easy, 50);
    MenuButton hard   = InitMenuButtonOption("Hard", &medium, 50);

    MenuState state =
    {
        MENU_SS_DEFAULT, // currentScreen
        title, // title
        { // options
            onePlayer, twoPlayer, demo, exitGame
        },
        { // difficulties
            easy, medium, hard
        },
        25, // cursorSize
        0, // selectedIndex
    };
    return state;
}

MenuButton InitMenuButtonTitle(char *text)
{
    int fontSize = 180;
    int textWidth = MeasureText(text, fontSize);
    int textPosX = (RENDER_WIDTH - textWidth) / 2;
    // int textPosY = (RENDER_HEIGHT / 2) - RENDER_HEIGHT / 2.5f;
    int textPosY = 100;
    MenuButton button =
    {
        text, fontSize,
        { textPosX, textPosY },
        { 0, 0 }, // offset
        RAYWHITE, // color
    };
    return button;
}

MenuButton InitMenuButtonOption(char* text, MenuButton *originButton, int offsetY)
{
    int fontSize = 80;
    int textWidth = MeasureText(text, fontSize);
    int textPosX = (RENDER_WIDTH - textWidth) / 2;
    int textPosY = originButton->position.y + originButton->fontSize + originButton->offset.y;
    MenuButton button =
    {
        text, fontSize,
        { textPosX, textPosY },
        { 0, offsetY },
        RAYWHITE,
    };
    return button;
}

void UpdateMenuCursorMove(MenuState *menu)
{
    int indexLimit = MENU_TOTAL_OPTIONS - 1;
    if (menu->currentScreen == MENU_SS_DIFFICULTY)
        indexLimit = MENU_TOTAL_DIFFS - 1;

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
                pong->gameMode = (GameMode)selectedChoice;
                menu->currentScreen = MENU_SS_DIFFICULTY;
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
             button->position.x + button->offset.x,
             button->position.y + button->offset.y,
             button->fontSize, RAYWHITE);
}

void DrawCursor(MenuState *menu)
{
    int size = menu->cursorSize;
    MenuButton *selected; // the option that the cursor is currently pointing at
    selected = &menu->options[menu->selectedIndex];
    if (menu->currentScreen == MENU_SS_DIFFICULTY)
        selected = &menu->difficulties[menu->selectedIndex];

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50, selected->fontSize / 2};
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
        for (int i = 0; i < MENU_TOTAL_DIFFS; i++)
        {
            DrawMenuElement(&menu->difficulties[i]);
        };
    }

    DrawCursor(menu);

    // Debug:
    // DrawText(TextFormat("cursor selected: %i", menu->selectedIndex), 0, 40, 40, WHITE);
}
