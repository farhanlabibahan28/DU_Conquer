#include "raylib.h"
#include "lights_on.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Texture2D lightOn;
Texture2D lightOff;

#define N 5
#define CELL_SIZE 80
#define UI_HEIGHT 60
#define GRID_HEIGHT (N * CELL_SIZE)
#define WINDOW_WIDTH (N * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT + UI_HEIGHT)

static int grid[N][N] = {0};

static int moveCount = 0;
static float elapsedTime = 0;
static float flashTimer = 0;
static bool gameStarted = false;
static bool win = false;

static Rectangle restartBtn = {WINDOW_WIDTH - 130, 15, 100, 30};

bool lightsOnActive = false;
bool lightsOnBlocked = false;
static float blockMessageTimer = 0.0f;

static void toggle(int x, int y) {
    int dx[] = {0, 1, -1, 0, 0};
    int dy[] = {0, 0, 0, 1, -1};
    for (int d = 0; d < 5; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
            grid[nx][ny] ^= 1;
        }
    }
}

static void randomizeGrid() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    for (int i = 0; i < 10; i++) {
        int x = GetRandomValue(0, N - 1);
        int y = GetRandomValue(0, N - 1);
        toggle(x, y);
    }
}

static bool checkWin() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (grid[i][j] == 0)
                return false;
    return true;
}

bool checkLightsOnWinStatus() {
    return win;
}

bool IsLightsOnBlocked() {
    return lightsOnBlocked;
}

static bool IsMouseOver(Rectangle rect) {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, rect);
}

void InitLightsOn() {
    lightOn = LoadTexture("light_on.png");
    lightOff = LoadTexture("light_off.png");
    randomizeGrid();
    moveCount = 0;
    elapsedTime = 0;
    flashTimer = 0;
    gameStarted = false;
    win = false;
    lightsOnBlocked = false;
    blockMessageTimer = 0.0f;
}

void UpdateLightsOn() {
    int screenW = GetMonitorWidth(0);
    int screenH = GetMonitorHeight(0);
    int centerX = (screenW - WINDOW_WIDTH) / 2;
    int centerY = (screenH - WINDOW_HEIGHT) / 2;

    Vector2 mouse = GetMousePosition();

    if (!win && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!IsMouseOver((Rectangle){centerX + restartBtn.x, centerY + restartBtn.y, restartBtn.width, restartBtn.height}) &&
            mouse.y > centerY + UI_HEIGHT && mouse.y < centerY + UI_HEIGHT + GRID_HEIGHT &&
            mouse.x > centerX && mouse.x < centerX + WINDOW_WIDTH) {

            int x = (int)((mouse.y - centerY - UI_HEIGHT) / CELL_SIZE);
            int y = (int)((mouse.x - centerX) / CELL_SIZE);
            toggle(x, y);
            moveCount++;
            gameStarted = true;
        }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        IsMouseOver((Rectangle){centerX + restartBtn.x, centerY + restartBtn.y, restartBtn.width, restartBtn.height})) {
        randomizeGrid();
        moveCount = 0;
        elapsedTime = 0;
        flashTimer = 0;
        gameStarted = false;
        win = false;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        lightsOnActive = false;
    }

    if (gameStarted && !win) elapsedTime += GetFrameTime();
    if (!win && checkWin()) {
        win = true;
        lightsOnActive = false;  // Automatically close popup on win
        lightsOnBlocked = true;
        blockMessageTimer = 3.0f; // Show block message for 3 seconds
    }
    if (win) flashTimer += GetFrameTime();

    if (lightsOnBlocked && blockMessageTimer > 0.0f) {
        blockMessageTimer -= GetFrameTime();
    }
}

void DrawLightsOn() {
    int screenW = GetMonitorWidth(0);
    int screenH = GetMonitorHeight(0);
    int centerX = (screenW - WINDOW_WIDTH) / 2;
    int centerY = (screenH - WINDOW_HEIGHT) / 2;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    if (lightsOnBlocked && blockMessageTimer > 0.0f) {
        float alpha = blockMessageTimer / 3.0f;
        Color fadedRed = Fade(RED, alpha);
        DrawText("Lights On already completed!", screenW / 2 - 160, screenH / 2 - 10, 20, fadedRed);
        return;
    }

    DrawRectangle(centerX, centerY, WINDOW_WIDTH, WINDOW_HEIGHT, DARKGRAY);
    DrawRectangleLines(centerX, centerY, WINDOW_WIDTH, WINDOW_HEIGHT, RAYWHITE);

    BeginScissorMode(centerX, centerY, WINDOW_WIDTH, WINDOW_HEIGHT);

    DrawRectangle(centerX, centerY, WINDOW_WIDTH, UI_HEIGHT, GRAY);
    DrawRectangleRec((Rectangle){centerX + restartBtn.x, centerY + restartBtn.y, restartBtn.width, restartBtn.height}, GRAY);
    DrawText("Restart", centerX + restartBtn.x + 10, centerY + restartBtn.y + 5, 20, WHITE);

    DrawText(TextFormat("Moves: %d", moveCount), centerX + 10, centerY + 15, 20, WHITE);
    DrawText(TextFormat("Time: %.1f sec", elapsedTime), centerX + 160, centerY + 15, 20, WHITE);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Vector2 pos = { (float)(centerX + j * CELL_SIZE), (float)(centerY + UI_HEIGHT + i * CELL_SIZE) };
            Texture2D tex = grid[i][j] ? lightOn : lightOff;

            if (grid[i][j] && win && ((int)(flashTimer * 5) % 2 == 0)) {
                DrawRectangle(pos.x, pos.y, CELL_SIZE, CELL_SIZE, GOLD);
            }

            DrawTextureEx(tex, pos, 0.0f, (float)CELL_SIZE / tex.width, WHITE);
        }
    }

    if (win) {
        DrawText("You Win!", centerX + WINDOW_WIDTH / 2 - 100, centerY + UI_HEIGHT + 20, 40, GREEN);
    }

    EndScissorMode();
}

void UnloadLightsOn() {
    UnloadTexture(lightOn);
    UnloadTexture(lightOff);
}
