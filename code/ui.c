// EXPLANATION:
// For managing the user interface
// See ui.h for more documentation/descriptions

#include "ui.h"

#include "raylib.h"
#include "raymath.h" // needed for Vector math

#include "config.h"
#include "input.h"
#include "pong.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void InitUiState(void)
{
    UiState uiDefaults =
    {
        .currentMenu = UI_MENU_TITLE,
        .selectedId = UI_BID_1PLAYER,
        .firstFrame = true,
    };

    // Title menu buttons
    UiMenu *titleMenu = &uiDefaults.menus[UI_MENU_TITLE];

    uiDefaults.title = InitUiTitle("Pong Remake");
    UiButton *onePlayer = InitUiMenuButtonRelative("One Player", UI_TITLE_BUTTON_SIZE, &uiDefaults.title, UI_SPACE_FROM_TITLE, titleMenu);
    UiButton *twoPlayer = InitUiMenuButtonRelative("Two Player", UI_TITLE_BUTTON_SIZE, onePlayer, UI_BUTTON_SPACING, titleMenu);
#if !defined(PLATFORM_WEB)
    UiButton *demo      =
#endif
        InitUiMenuButtonRelative("Demo", UI_TITLE_BUTTON_SIZE, twoPlayer, UI_BUTTON_SPACING, titleMenu);
#if !defined(PLATFORM_WEB)
    InitUiMenuButtonRelative("Exit", UI_TITLE_BUTTON_SIZE, demo, UI_BUTTON_SPACING, titleMenu);
#endif

    // Difficulty buttons
    UiMenu *diffMenu = &uiDefaults.menus[UI_MENU_DIFFICULTY];

    UiButton *easy   = InitUiMenuButtonRelative("Easy", UI_TITLE_BUTTON_SIZE, &uiDefaults.title, UI_SPACE_FROM_TITLE, diffMenu);
    UiButton *medium = InitUiMenuButtonRelative("Medium", UI_TITLE_BUTTON_SIZE, easy, UI_BUTTON_SPACING, diffMenu);
    UiButton *hard   = InitUiMenuButtonRelative("Hard", UI_TITLE_BUTTON_SIZE, medium, UI_BUTTON_SPACING, diffMenu);
    InitUiMenuButtonRelative("Back", UI_TITLE_BUTTON_SIZE, hard, UI_BUTTON_SPACING, diffMenu);

    // Pause button + menu
    UiMenu *pauseMenu = &uiDefaults.menus[UI_MENU_PAUSE];
    char *pauseText = "Pause";
    char *resumeText = "Resume";
    char *toTitleText = "Back to Title";
    const int pauseTextLength = MeasureText(pauseText, UI_PAUSE_SIZE);
    uiDefaults.pause =
        InitUiButton(pauseText, UI_PAUSE_SIZE,
                     (float)RENDER_WIDTH / 4 - pauseTextLength / 2,
                     (float)RENDER_HEIGHT - (UI_PAUSE_SIZE * 2));

    InitUiMenuButtonRelative(resumeText, UI_PAUSE_SIZE, &uiDefaults.pause, -UI_PAUSE_SIZE, pauseMenu);
    InitUiMenuButtonRelative(toTitleText, UI_PAUSE_SIZE, &uiDefaults.pause, -UI_PAUSE_SIZE * 2 - UI_BUTTON_SPACING, pauseMenu);

    pongUi = uiDefaults;
}

UiButton InitUiTitle(char *text)
{
    int fontSize = UI_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float titlePosX = (RENDER_WIDTH - (float)textWidth) / 2;
#if !defined(PLATFORM_WEB) // different spacing for web
    float titlePosY = UI_TITLE_SPACE_FROM_TOP;
#else
    float titlePosY = UI_TITLE_SPACE_FROM_TOP + UI_TITLE_BUTTON_SIZE;
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

UiButton *InitUiMenuButtonRelative(char* text, int fontSize, UiButton *originButton, float offsetY, UiMenu *menu)
{
    float originWidth = MeasureText(originButton->text, originButton->fontSize);
    float originPosX = (originButton->position.x + originWidth / 2);
    float textPosX = originPosX - MeasureText(text, fontSize) / 2;
    float textPosY = originButton->position.y + originButton->fontSize;

    return InitUiMenuButton(text, fontSize, textPosX, textPosY + offsetY, menu);
}

void FreeUiMenuButtons(void)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(pongUi.menus); i++)
        MemFree(pongUi.menus[i].buttons);
}

void UpdateUiFrame(void)
{
    // Input to go back
    if (pongUi.currentMenu != UI_MENU_GAMEPLAY)
    {
        if ((IsInputActionPressed(INPUT_ACTION_BACK) ||
             IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && pongUi.currentMenu != UI_MENU_TITLE)
        {
            ChangeUiMenu(UI_MENU_TITLE);
            PlaySound(pongGame.beeps[BEEP_MENU]);
        }

        UiButton *selectedButton = &pongUi.menus[pongUi.currentMenu].buttons[pongUi.selectedId];
        UpdateUiButtonSelect(selectedButton);
        UpdateUiMenuTraverse();
    }
    else if (!pongGame.isPaused)
    {
        UpdateUiButtonMouseHover(&pongUi.pause);
        UpdateUiButtonSelect(&pongUi.pause);
    }

    // Update pause fade animation
    static float fadeLength = 1.5f; // Fade in and out at this rate in seconds
    static bool fadingOut = false;
    float fadeIncrement = (1.0f / fadeLength) * GetFrameTime();

    if (pongUi.textFade >= 1.0f)
        fadingOut = true;
    else if (pongUi.textFade <= 0.0f)
        fadingOut = false;
    if (fadingOut)
        fadeIncrement *= -1;

    pongUi.textFade += fadeIncrement;
}

void UpdateUiMenuTraverse(void)
{
    if (pongUi.currentMenu == UI_MENU_GAMEPLAY)
        return;
    UiMenu *menu = &pongUi.menus[pongUi.currentMenu];

    UiTitleMenuId prevId = pongUi.selectedId; // used to play beep

    // Move cursor via mouse
    bool mouseMoved = (Vector2Length(GetMouseDelta()) > 0);
    if (mouseMoved || (pongUi.firstFrame && pongUi.lastSelectWithMouse))
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

            if (IsMouseWithinUiButton(mousePos, currentButton))
            {
                pongUi.selectedId = i;
                pongUi.autoScroll = false;
                pongUi.lastSelectWithMouse = true;
            }
        }
    }

    // Move cursor via keyboard
    bool isInputUp =
        IsInputActionDown(INPUT_ACTION_P1_UP) ||
        IsInputActionDown(INPUT_ACTION_P2_UP);
    bool isInputDown =
        IsInputActionDown(INPUT_ACTION_P1_DOWN) ||
        IsInputActionDown(INPUT_ACTION_P2_DOWN);
    const float autoScrollInitPause = 0.6f;

    bool initialKeyPress = (!pongUi.autoScroll && pongUi.keyHeldTime == 0);
    bool heldLongEnoughToRepeat = (pongUi.autoScroll && pongUi.keyHeldTime >= 0.1f);
    if (initialKeyPress || heldLongEnoughToRepeat)
    {
        if (isInputUp)
        {
            if (pongUi.selectedId > 0)
                pongUi.selectedId--;
            else
                pongUi.selectedId = menu->buttonCount - 1;
            pongUi.keyHeldTime = 0;
            pongUi.lastSelectWithMouse = false;
        }
        if (isInputDown)
        {
            if ((unsigned int)pongUi.selectedId < menu->buttonCount - 1)
                pongUi.selectedId++;
            else
                pongUi.selectedId = 0;
            pongUi.keyHeldTime = 0.0f;
            pongUi.lastSelectWithMouse = false;
        }
    }

    // Update auto-scroll timer when holding keys
    if (isInputUp || isInputDown)
    {
        pongUi.keyHeldTime += GetFrameTime();
        if (pongUi.keyHeldTime >= autoScrollInitPause)
        {
            pongUi.autoScroll = true;
        }
    }
    else
    {
        pongUi.keyHeldTime = 0;
        pongUi.autoScroll = false;
    }

    if (pongUi.selectedId != prevId && !pongUi.firstFrame)
        PlaySound(pongGame.beeps[BEEP_MENU]);

    pongUi.firstFrame = false;
}

void UpdateUiButtonMouseHover(UiButton *button)
{
    bool mouseMoved = (Vector2Length(GetMouseDelta()) > 0);
    if (!mouseMoved) return;

    Vector2 mouse = GetMousePosition();
    float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);
    Vector2 mousePos = { 0 };
    mousePos.x = (mouse.x - (GetScreenWidth() - (RENDER_WIDTH*scale))*0.5f)/scale;
    mousePos.y = (mouse.y - (GetScreenHeight() - (RENDER_HEIGHT*scale))*0.5f)/scale;
    mousePos = Vector2Clamp(mousePos, (Vector2){ 0, 0 }, (Vector2){ (float)RENDER_WIDTH, (float)RENDER_HEIGHT });

    if (IsMouseWithinUiButton(mousePos, button))
    {
        if (!button->mouseHovered)
            PlaySound(pongGame.beeps[BEEP_MENU]);
        button->mouseHovered = true;
    }
    else
    {
        button->mouseHovered = false;
    }
}

void UpdateUiButtonSelect(UiButton *button)
{

    Vector2 mouse = GetMousePosition();
    float scale = MIN((float)GetScreenWidth()/RENDER_WIDTH, (float)GetScreenHeight()/RENDER_HEIGHT);
    Vector2 mousePos = { 0 };
    mousePos.x = (mouse.x - (GetScreenWidth() - (RENDER_WIDTH*scale))*0.5f)/scale;
    mousePos.y = (mouse.y - (GetScreenHeight() - (RENDER_HEIGHT*scale))*0.5f)/scale;
    mousePos = Vector2Clamp(mousePos, (Vector2){ 0, 0 }, (Vector2){ (float)RENDER_WIDTH, (float)RENDER_HEIGHT });

    // Select pause button
    if (pongUi.currentMenu == UI_MENU_GAMEPLAY && IsGestureDetected(GESTURE_TAP) &&
         (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && IsMouseWithinUiButton(mousePos, button)))
    {
        ChangeUiMenu(UI_MENU_PAUSE);
    }

    // Select a menu button
    else if (IsInputActionPressed(INPUT_ACTION_CONFIRM) ||
        (IsGestureDetected(GESTURE_TAP) &&
         (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && IsMouseWithinUiButton(mousePos, button))))
    {
        if (pongUi.currentMenu == UI_MENU_GAMEPLAY && !pongGame.isPaused)
            return; // not a menu

        if (pongUi.currentMenu == UI_MENU_PAUSE && !pongUi.firstFrame)
        {
            if (pongUi.selectedId == UI_BID_RESUME)
            {
                pongGame.isPaused = false;
                pongUi.currentMenu = UI_MENU_GAMEPLAY;
            }
            else if (pongUi.selectedId == UI_BID_BACKTOTITLE)
            {
                ChangeUiMenu(UI_MENU_TITLE);
            }
        }

        else if (pongUi.currentMenu == UI_MENU_TITLE)
        {
            if (pongUi.selectedId == UI_BID_EXIT)
                pongGame.gameShouldExit = true;
            else if (pongUi.selectedId == UI_BID_1PLAYER)
                ChangeUiMenu(UI_MENU_DIFFICULTY);
            else
                ChangeUiMenu(UI_MENU_GAMEPLAY);
        }

        else if (pongUi.currentMenu == UI_MENU_DIFFICULTY)
        {
            if (pongUi.selectedId == UI_BID_BACK)
                ChangeUiMenu(UI_MENU_TITLE);
            else
                ChangeUiMenu(UI_MENU_GAMEPLAY);
        }

        PlaySound(pongGame.beeps[BEEP_MENU]);
    }
}

bool IsMouseWithinUiButton(Vector2 mousePos, UiButton *button)
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

void ChangeUiMenu(UiMenuState newMenu)
{
    if (newMenu == UI_MENU_TITLE)
    {
        // Clear old game state if returning from gameplay
        if (pongGame.currentScreen == SCREEN_GAMEPLAY)
        {
            FreeBeeps();
            InitGameState();
            pongGame.currentScreen = SCREEN_TITLE;
        }

        pongUi.selectedId = UI_BID_1PLAYER;
    }

    else if (newMenu == UI_MENU_DIFFICULTY)
    {
        pongUi.selectedId = UI_BID_MEDIUM;
    }

    else if (newMenu == UI_MENU_PAUSE)
    {
        pongGame.isPaused = true;
        pongUi.selectedId = UI_BID_RESUME;
    }

    else if (newMenu == UI_MENU_GAMEPLAY)
    {
        if (pongUi.currentMenu == UI_MENU_DIFFICULTY)
            pongGame.difficulty = (GameDifficulty)pongUi.selectedId;
        else
            pongGame.currentMode = (GameMode)pongUi.selectedId;
        pongGame.currentScreen = SCREEN_GAMEPLAY;
    }

    pongUi.currentMenu = newMenu;
    pongUi.firstFrame = true;
}

void DrawUiFrame(void)
{
    if (pongGame.currentScreen == SCREEN_TITLE)
    {
        DrawUiElement(&pongUi.title);
    }

    if (pongUi.currentMenu != UI_MENU_GAMEPLAY)
    {
        UiMenu *menu = &pongUi.menus[pongUi.currentMenu];
        for (unsigned int i = 0; i < menu->buttonCount; i++)
            DrawUiElement(&menu->buttons[i]);

        UiButton *selectedButton = &pongUi.menus[pongUi.currentMenu].buttons[pongUi.selectedId];
        DrawUiCursor(selectedButton);
    }
    else if (pongGame.currentScreen == SCREEN_GAMEPLAY)
    {
        // Draw pause button
        DrawUiElement(&pongUi.pause);
        if (pongUi.pause.mouseHovered)
            DrawUiCursor(&pongUi.pause);
    }

    if (pongGame.currentScreen == SCREEN_GAMEPLAY)
    {
        // Draw dotted line down middle
        DrawUiFieldLines(pongGame.isPaused, pongGame.currentMode == MODE_DEMO);

        // Draw score
        DrawUiScores();

        // Fade animation
        Color fadeColor = Fade(RAYWHITE, pongUi.textFade);

        // Draw win message
        if (pongGame.playerWon)
            DrawUiWinnerMessage(pongGame.scoreL, pongGame.scoreR, fadeColor);

        // Draw pause message
        char *text;
        if (pongGame.isPaused)
        {
            text = "PAUSED";
            int textOffset = MeasureText(text, SCORE_FONT_SIZE) / 2;
            DrawText(text, RENDER_WIDTH / 2 - textOffset,
                     RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2,
                     SCORE_FONT_SIZE, fadeColor);
        }
        else if (pongGame.currentMode == MODE_DEMO) // Draw demo mode message
        {
            text = "DEMO MODE";
            int textOffset = MeasureText(text, SCORE_FONT_SIZE) / 2;
            DrawText(text, RENDER_WIDTH / 2 - textOffset,
                     RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2,
                     SCORE_FONT_SIZE, fadeColor);
        }

    }

    // Debug:
    // DrawText(TextFormat("cursor selected: %i", menu->selectedId), 0, 40, 40, WHITE);
}

void DrawUiElement(UiButton *button)
{
    DrawText(button->text, (int)button->position.x, (int)button->position.y,
             button->fontSize, RAYWHITE);
}

void DrawUiCursor(UiButton *selectedButton)
{
    float size = UI_CURSOR_SIZE;

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50.0f, (float)selectedButton->fontSize / 2};
    selectPointPos = Vector2Add(selectedButton->position, cursorOffset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}

void DrawUiFieldLines(bool isPaused, bool isDemoMode)
{
    int dashHeight = 40;
    int spaceHeight = 40;

    // Draw top and bottom lines
    DrawRectangle(0, 0,
                  RENDER_WIDTH, FIELD_LINE_WIDTH, RAYWHITE);
    DrawRectangle(0, RENDER_HEIGHT - FIELD_LINE_WIDTH,
                  RENDER_WIDTH, FIELD_LINE_WIDTH, RAYWHITE);

    // Calculate amount of dashes needed for dotted line
    int totalSegmentHeight = dashHeight + spaceHeight;
    int totalSegments = (RENDER_HEIGHT - spaceHeight) / totalSegmentHeight;
    int usedHeight = totalSegments * totalSegmentHeight - spaceHeight;
    int offsetY = (RENDER_HEIGHT - usedHeight) / 2;

    // Dotted line down the middle
    for (int i = 0; i < totalSegments; i++)
    {
        int y = offsetY + i * totalSegmentHeight;

        int pauseMessageYPos = RENDER_HEIGHT / 2 - SCORE_FONT_SIZE / 2;
        if (((isPaused || isDemoMode) &&
             (y+dashHeight > pauseMessageYPos) &&
             (y < pauseMessageYPos + SCORE_FONT_SIZE)))
            continue;

        DrawRectangle(RENDER_WIDTH / 2 - FIELD_LINE_WIDTH / 2, y,
                      FIELD_LINE_WIDTH, dashHeight, RAYWHITE);

    }
}

void DrawUiScores(void)
{
    int fontSize = 180;

    const char *scoreLMsg = TextFormat("%i", pongGame.scoreL);
    int scoreLWidth = MeasureText(scoreLMsg, fontSize);
    int scoreLPosX = RENDER_WIDTH / 4 - scoreLWidth / 2;

    const char *scoreRMsg = TextFormat("%i", pongGame.scoreR);
    int scoreRWidth = MeasureText(scoreRMsg, fontSize);
    int scoreRPosX = RENDER_WIDTH / 4 * 3 - scoreRWidth / 2;

    int scorePosY = 50;
    DrawText(scoreLMsg, scoreLPosX, scorePosY, fontSize, RAYWHITE);
    DrawText(scoreRMsg, scoreRPosX, scorePosY, fontSize, RAYWHITE);
}

void DrawUiWinnerMessage(int scoreL, int scoreR, Color fadeColor)
{
    char *msg = "Winner";
    int fontSize = 100; // this is also the font height because we're using the default font
    int textWidth = MeasureText(msg, fontSize);
    int textPosY = (RENDER_HEIGHT - fontSize) / 4;

    if (scoreL == WIN_SCORE)
    {
        int textPosX = RENDER_WIDTH / 4 - textWidth / 2;
        DrawText(msg, textPosX, textPosY, fontSize, fadeColor);
    }
    if (scoreR == WIN_SCORE)
    {
        int textPosX = RENDER_WIDTH / 4 * 3 - textWidth / 2;
        DrawText(msg, textPosX, textPosY, fontSize, fadeColor);
    }
}

