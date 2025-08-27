// EXPLANATION:
// For managing the user interface (text, buttons, menus)
// See ui.h for more documentation/descriptions

#include "ui.h"

#include <stddef.h>
#include "raylib.h"
#include "raymath.h" // needed for Vector math

#include "config.h"
#include "pong.h" // needed to reset game state

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

UiState InitUiState(void)
{
    UiState ui =
    {
        .currentMenu = UI_MENU_TITLE,
        .cursorSize = UI_CURSOR_SIZE,
        .selectedId = UI_ID_1PLAYER,
        .firstFrame = true,
    };

    // Title menu buttons
    UiMenu *titleMenu = &ui.menus[UI_MENU_TITLE];

    ui.title = InitUiTitle("Pong Remake");
    UiButton *onePlayer = InitUiButtonRelative("One Player", &ui.title, UI_SPACE_FROM_TITLE, titleMenu);
    UiButton *twoPlayer = InitUiButtonRelative("Two Player", onePlayer, UI_BUTTON_SPACING, titleMenu);
#if !defined(PLATFORM_WEB)
    UiButton *demo      =
#endif
        InitUiButtonRelative("Demo", twoPlayer, UI_BUTTON_SPACING, titleMenu);
#if !defined(PLATFORM_WEB)
    InitUiButtonRelative("Exit", demo, UI_BUTTON_SPACING, titleMenu);
#endif

    // Difficulty buttons
    UiMenu *diffMenu = &ui.menus[UI_MENU_DIFFICULTY];

    UiButton *easy   = InitUiButtonRelative("Easy", &ui.title, UI_SPACE_FROM_TITLE, diffMenu);
    UiButton *medium = InitUiButtonRelative("Medium", easy, UI_BUTTON_SPACING, diffMenu);
    UiButton *hard   = InitUiButtonRelative("Hard", medium, UI_BUTTON_SPACING, diffMenu);
    InitUiButtonRelative("Back", hard, UI_BUTTON_SPACING, diffMenu);

    // Pause and back-to-title buttons
    const int cornerButtonSize = 50;
    int pauseTextLength = MeasureText("Pause", cornerButtonSize);
    ui.pause =
        InitUiButton("Pause", cornerButtonSize,
                         (float)RENDER_WIDTH / 4 - pauseTextLength / 2,
                         (float)RENDER_HEIGHT - (cornerButtonSize * 2));

    int toTitleTextLength = MeasureText("Unpause", cornerButtonSize);
    ui.unpause =
        InitUiButton("Unpause", cornerButtonSize,
                     (float)RENDER_WIDTH / 4 - toTitleTextLength / 2,
                     (float)RENDER_HEIGHT - (cornerButtonSize * 2));

    return ui;
}

UiButton InitUiTitle(char *text)
{
    int fontSize = UI_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float titlePosX = (RENDER_WIDTH - (float)textWidth) / 2;
#if !defined(PLATFORM_WEB) // different spacing for web
    float titlePosY = UI_TITLE_SPACE_FROM_TOP;
#else
    float titlePosY = UI_TITLE_SPACE_FROM_TOP + UI_BUTTON_SIZE;
#endif

    return InitUiButton(text, fontSize, titlePosX, titlePosY);
}

UiButton InitUiButton(char *text, int fontSize, float textPosX, float textPosY)
{

    UiButton button = { text, fontSize, false, { textPosX, textPosY }, RAYWHITE };

    return button;
}

UiButton *InitUiMenuButton(char *text, int fontSize, float textPosX, float textPosY, UiMenu *menu)
{
    UiButton button = { text, fontSize, false, { textPosX, textPosY }, RAYWHITE };
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

    return InitUiMenuButton(text, fontSize, textPosX, textPosY + offsetY, menu);
}

void FreeUiElements(UiState *ui)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(ui->menus); i++)
        MemFree(ui->menus[i].buttons);
}

void UpdateUiFrame(UiState *ui, GameState *pong)
{
    // Escape or Backspace or Right click to go back
    if (pong->currentScreen == SCREEN_TITLE)
    {
        if ((IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) ||
             IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && ui->currentMenu != UI_MENU_TITLE)
        {
            ui->currentMenu = UI_MENU_TITLE;
            ui->firstFrame = true;
            PlaySound(pong->beeps[BEEP_MENU]);
        }

        UiButton *selectedButton = &ui->menus[ui->currentMenu].buttons[ui->selectedId];
        UpdateUiButtonSelect(selectedButton, ui, pong);
        UpdateUiMenuMove(ui, pong);
    }

    else if (pong->currentScreen == SCREEN_GAMEPLAY)
    {
        if (!pong->isPaused)
        {
            UpdateUiButtonMove(&ui->pause, pong);
            UpdateUiButtonSelect(&ui->pause, ui, pong);
        }
        else
        {
            UpdateUiButtonMove(&ui->unpause, pong);
            UpdateUiButtonSelect(&ui->unpause, ui, pong);
        }
    }
}

void UpdateUiMenuMove(UiState *ui, GameState *pong)
{
    UiMenu *menu = &ui->menus[ui->currentMenu];

    // Set default position for cursor on first menu frame
    if (ui->firstFrame == true)
    {
        if (ui->currentMenu == UI_MENU_TITLE)
            ui->selectedId = UI_ID_1PLAYER;
        else if (ui->currentMenu == UI_MENU_DIFFICULTY)
            ui->selectedId = (UiTitleMenuId)DIFFICULTY_MEDIUM;
    }

    UiTitleMenuId prevId = ui->selectedId; // used to play beep

    // Move cursor via mouse
    bool mouseMoved = (Vector2Length(GetMouseDelta()) > 0);
    if (mouseMoved || ui->firstFrame)
    {
        Vector2 mouse = GetMousePosition();
        float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);
        Vector2 mousePos = { 0 };
        mousePos.x = (mouse.x - (GetScreenWidth() - (RENDER_WIDTH*scale))*0.5f)/scale;
        mousePos.y = (mouse.y - (GetScreenHeight() - (RENDER_HEIGHT*scale))*0.5f)/scale;
        mousePos = Vector2Clamp(mousePos, (Vector2){ 0, 0 }, (Vector2){ (float)RENDER_WIDTH, (float)RENDER_HEIGHT });

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

    if (ui->selectedId != prevId && !ui->firstFrame)
        PlaySound(pong->beeps[BEEP_MENU]);

    ui->firstFrame = false;
}

void UpdateUiButtonMove(UiButton *button, GameState *pong)
{
    bool mouseMoved = (Vector2Length(GetMouseDelta()) > 0);
    if (!mouseMoved) return;

    Vector2 mouse = GetMousePosition();
    float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);
    Vector2 mousePos = { 0 };
    mousePos.x = (mouse.x - (GetScreenWidth() - (RENDER_WIDTH*scale))*0.5f)/scale;
    mousePos.y = (mouse.y - (GetScreenHeight() - (RENDER_HEIGHT*scale))*0.5f)/scale;
    mousePos = Vector2Clamp(mousePos, (Vector2){ 0, 0 }, (Vector2){ (float)RENDER_WIDTH, (float)RENDER_HEIGHT });

    if (IsMouseWithinButton(mousePos, button))
    {
        if (!button->mouseHovered)
            PlaySound(pong->beeps[BEEP_MENU]);
        button->mouseHovered = true;
    }
    else
    {
        button->mouseHovered = false;
    }
}

void UpdateUiButtonSelect(UiButton *button, UiState *ui, GameState *pong)
{
    Vector2 mouse = GetMousePosition();
    float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);
    Vector2 mousePos = { 0 };
    mousePos.x = (mouse.x - (GetScreenWidth() - (RENDER_WIDTH*scale))*0.5f)/scale;
    mousePos.y = (mouse.y - (GetScreenHeight() - (RENDER_HEIGHT*scale))*0.5f)/scale;
    mousePos = Vector2Clamp(mousePos, (Vector2){ 0, 0 }, (Vector2){ (float)RENDER_WIDTH, (float)RENDER_HEIGHT });

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
        (IsGestureDetected(GESTURE_TAP) &&
         (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && IsMouseWithinButton(mousePos, button))))
    {

        if (pong->currentScreen == SCREEN_GAMEPLAY)
        {
            if (!pong->isPaused)
                pong->isPaused = true;
            else
            {
                pong->isPaused = false;
            }
        }

        // assumed that (currentScreen == SCREEN_TITLE)
        else if (ui->currentMenu == UI_MENU_TITLE)
        {
            if (ui->selectedId == UI_ID_EXIT)
            {
                pong->gameShouldExit = true;
            }
            else if (ui->selectedId == UI_ID_1PLAYER)
            {
                // Main menu -> difficulty menu
                pong->currentMode = (GameMode)ui->selectedId;
                ui->currentMenu = UI_MENU_DIFFICULTY;
                ui->firstFrame = true;
            }
            else
            {
                // Main menu -> pong gameplay
                pong->currentMode = (GameMode)ui->selectedId;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }

        else if (ui->currentMenu == UI_MENU_DIFFICULTY)
        {
            if (ui->selectedId == UI_ID_BACK)
            {
                // Difficulty menu -> main menu
                ui->currentMenu = UI_MENU_TITLE;
                ui->firstFrame = true;
            }
            else
            {
                // Main menu -> pong gameplay
                pong->difficulty = (GameDifficulty)ui->selectedId;
                pong->currentScreen = SCREEN_GAMEPLAY;
            }
        }

        PlaySound(pong->beeps[BEEP_MENU]);
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

void DrawUiFrame(UiState *ui, GameState *pong)
{
    if (pong->currentScreen == SCREEN_TITLE)
    {
        DrawUiElement(&ui->title);
        UiMenu *menu = &ui->menus[ui->currentMenu];

        for (unsigned int i = 0; i < menu->buttonCount; i++)
            DrawUiElement(&menu->buttons[i]);

        UiButton *selectedButton = &ui->menus[ui->currentMenu].buttons[ui->selectedId];
        DrawUiCursor(ui, selectedButton);
    }

    if (pong->currentScreen == SCREEN_GAMEPLAY)
    {
        UiButton *cornerButton;
        if (pong->isPaused)
            cornerButton = &ui->unpause;
        else
            cornerButton = &ui->pause;

        DrawUiElement(cornerButton);
        if (cornerButton->mouseHovered)
            DrawUiCursor(ui, cornerButton);
    }

    // Debug:
    // DrawText(TextFormat("cursor selected: %i", menu->selectedId), 0, 40, 40, WHITE);
}

void DrawUiElement(UiButton *button)
{
    DrawText(button->text, (int)button->position.x, (int)button->position.y,
             button->fontSize, RAYWHITE);
}

void DrawUiCursor(UiState *ui, UiButton *selected)
{
    float size = ui->cursorSize;

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50.0f, (float)selected->fontSize / 2};
    selectPointPos = Vector2Add(selected->position, cursorOffset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}
