// EXPLANATION:
// For managing the user interface (text, buttons, menus)
// See ui.h for more documentation/descriptions

#include "ui.h"

#include <stddef.h>
#include "raylib.h"
#include "raymath.h" // needed for Vector math

#include "config.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

UiState InitUiState(void)
{
    UiState ui =
    {
        .menus = { 0 },
        .selectedId = MENUID_1PLAYER,
        .cursorSize = UI_CURSOR_SIZE,
        .keyHeldTime = 0.0f,
        .firstFrame = true,
        .autoScroll = false,
    };

    // Title menu buttons
    UiMenu *titleMenu = &ui.menus[MENU_TITLE];

    UiButton *title = InitUiText("Pong Remake", &ui, MENU_TITLE);
    UiButton *onePlayer = InitUiButtonRelative("One Player", title, UI_SPACE_FROM_TITLE, titleMenu);
    UiButton *twoPlayer = InitUiButtonRelative("Two Player", onePlayer, UI_BUTTON_SPACING, titleMenu);
    UiButton *demo      = InitUiButtonRelative("Demo", twoPlayer, UI_BUTTON_SPACING, titleMenu);
#if !defined(PLATFORM_WEB)
    InitUiButtonRelative("Exit", demo, UI_BUTTON_SPACING, titleMenu);
#endif

    // Difficulty buttons
    UiMenu *diffMenu = &ui.menus[MENU_DIFFICULTY];
    UiButton *easy   = InitUiButtonRelative("Easy", title, UI_SPACE_FROM_TITLE, diffMenu);
    UiButton *medium = InitUiButtonRelative("Medium", easy, UI_BUTTON_SPACING, diffMenu);
    UiButton *hard   = InitUiButtonRelative("Hard", medium, UI_BUTTON_SPACING, diffMenu);
    InitUiButtonRelative("Back", hard, UI_BUTTON_SPACING, diffMenu);

    return ui;
}

UiButton *InitUiText(char *text, UiState *ui, UiMenuId menuId)
{
    int fontSize = UI_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float titlePosX = (RENDER_WIDTH - (float)textWidth) / 2;
#if !defined(PLATFORM_WEB) // different spacing for web
    float titlePosY = UI_TITLE_SPACE_FROM_TOP;
#else
    float titlePosY = UI_TITLE_SPACE_FROM_TOP + UI_BUTTON_SIZE;
#endif

    UiButton button = { text, fontSize, { titlePosX, titlePosY }, RAYWHITE };
    ui->text[menuId] = button;

    return &ui->text[menuId];
}

UiButton *InitUiButton(char *text, int fontSize, float textPosX, float textPosY, UiMenu *menu)
{
    UiButton button = { text, fontSize, { textPosX, textPosY }, RAYWHITE };
    menu->buttonCount++;
    menu->buttons = MemRealloc(menu->buttons, menu->buttonCount * sizeof(UiButton));
    menu->buttons[menu->buttonCount - 1] = button;

    return &menu->buttons[menu->buttonCount - 1];
}

UiButton *InitUiButtonRelative(char* text, UiButton *originButton, float offsetY, UiMenu *menu)
{
    int fontSize = UI_BUTTON_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float textPosX = (RENDER_WIDTH - (float)textWidth) / 2;
    float textPosY = originButton->position.y + originButton->fontSize;

    return InitUiButton(text, fontSize, textPosX, textPosY + offsetY, menu);
}

void FreeUiElements(UiState *ui)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(ui->menus); i++)
        MemFree(ui->menus[i].buttons);
}

void UpdateUiFrame(UiState *ui, GameState *pong)
{
    // Escape or Backspace or Right click to go back
    if ((IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) ||
         IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && ui->currentMenu != MENU_TITLE)
    {
        ui->currentMenu = MENU_TITLE;
        ui->firstFrame = true;
    }

    UpdateUiCursorSelect(ui, pong);
    UpdateUiCursorMove(ui, pong);
}

void UpdateUiCursorMove(UiState *ui, GameState *pong)
{
    UiMenu *menu = &ui->menus[ui->currentMenu];

    // Set default position for cursor on first menu frame
    if (ui->firstFrame == true)
    {
        if (ui->currentMenu == MENU_TITLE)
            ui->selectedId = MENUID_1PLAYER;
        else if (ui->currentMenu == MENU_DIFFICULTY)
            ui->selectedId = (UiOptionId)DIFFICULTY_MEDIUM;
    }

    UiOptionId prevId = ui->selectedId; // used to play beep

    // Move cursor via mouse
    bool mouseMoved = Vector2Length(GetMouseDelta()) > 0;
    if (mouseMoved || ui->firstFrame)
    {
        Vector2 scaleFactor = { (float)RENDER_WIDTH / GetScreenWidth(),
            (float)RENDER_HEIGHT / GetScreenHeight() };
        Vector2 mousePos = Vector2Multiply(GetMousePosition(), scaleFactor);

        for (unsigned int i = 0; i < menu->buttonCount; i++)
        {
            UiButton *currentButton = 0;
            currentButton = &menu->buttons[i];

            if (IsMouseWithinButton(mousePos, currentButton))
            {
                ui->selectedId = i;
                ui->autoScroll = false;
            }
        }
    }

    // Move cursor via keyboard
    bool isInputUp = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP));
    bool isInputDown = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN));
    const float autoScrollInitPause = 0.6f;

    if ((!ui->autoScroll && ui->keyHeldTime == 0) ||
        (ui->autoScroll && ui->keyHeldTime >= 0.1f))
    {
        if (isInputUp)
        {
            if (ui->selectedId > 0)
                ui->selectedId--;
            else
                ui->selectedId = menu->buttonCount - 1;
            ui->keyHeldTime = 0;
        }
        if (isInputDown)
        {
            if ((unsigned int)ui->selectedId < menu->buttonCount - 1)
                ui->selectedId++;
            else
                ui->selectedId = 0;
            ui->keyHeldTime = 0.0f;
        }
    }

    // Update auto-scroll timer when holding keys
    if (isInputUp || isInputDown)
    {
        ui->keyHeldTime += GetFrameTime();
        if (ui->keyHeldTime >= autoScrollInitPause)
        {
            ui->autoScroll = true;
        }
    }
    else
    {
        ui->keyHeldTime = 0;
        ui->autoScroll = false;
    }

    ui->firstFrame = false;

    if (ui->selectedId != prevId)
        PlaySound(pong->beeps[BEEP_MENU]);
}

void UpdateUiCursorSelect(UiState *ui, GameState *pong)
{
    UiMenu *menu            = &ui->menus[ui->currentMenu];
    UiButton *currentButton = &menu->buttons[ui->selectedId];

    Vector2 scaleFactor = { (float)RENDER_WIDTH / GetScreenWidth(),
                            (float)RENDER_HEIGHT / GetScreenHeight() };
    Vector2 mousePos = Vector2Multiply(GetMousePosition(), scaleFactor);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
        (IsGestureDetected(GESTURE_TAP) &&
         (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && IsMouseWithinButton(mousePos, currentButton))))
    {

        if (ui->currentMenu == MENU_TITLE)
        {
            if (ui->selectedId == MENUID_EXIT)
                pong->gameShouldExit = true;
            else if (ui->selectedId == MENUID_1PLAYER)
            {
                // Main menu -> difficulty menu
                pong->currentMode = (GameMode)ui->selectedId;
                ui->currentMenu = MENU_DIFFICULTY;
                ui->firstFrame = true;
            }
            else
            {
                // Main menu -> pong gameplay
                pong->currentMode = (GameMode)ui->selectedId;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }
        else if (ui->currentMenu == MENU_DIFFICULTY)
        {
            if (ui->selectedId == MENUID_BACK)
            {
                // Difficulty menu -> main menu
                ui->currentMenu = MENU_TITLE;
                ui->firstFrame = true;
            }
            else
            {
                // Main menu -> pong gameplay
                pong->difficulty = (GameDifficulty)ui->selectedId;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }

    }
}

bool IsMouseWithinButton(Vector2 mousePos, UiButton *button)
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

void DrawUiFrame(UiState *ui, UiMenuId menuId)
{
    DrawUiElement(&ui->text[menuId]);
    UiMenu *menu = &ui->menus[ui->currentMenu];

    for (unsigned int i = 0; i < menu->buttonCount; i++)
        DrawUiElement(&menu->buttons[i]);

    DrawUiCursor(ui);

    // Debug:
    // DrawText(TextFormat("cursor selected: %i", menu->selectedId), 0, 40, 40, WHITE);
}

void DrawUiElement(UiButton *button)
{
    DrawText(button->text, (int)button->position.x, (int)button->position.y,
             button->fontSize, RAYWHITE);
}

void DrawUiCursor(UiState *ui)
{
    UiMenu *menu = &ui->menus[ui->currentMenu];
    float size = ui->cursorSize;
    UiButton *selected = 0; // the button that the cursor is currently pointing at
    selected = &menu->buttons[ui->selectedId];

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50.0f, (float)selected->fontSize / 2};
    selectPointPos = Vector2Add(selected->position, cursorOffset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}
