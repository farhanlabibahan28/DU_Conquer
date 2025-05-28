#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game_states.h"
extern GameState currentGameState;


#define N 5
#define CELL_SIZE 80
#define UI_HEIGHT 60
#define GRID_HEIGHT (N * CELL_SIZE)
#define WINDOW_WIDTH (N * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT + UI_HEIGHT)

static int grid[N][N] = {0};

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
            grid[i][j] = GetRandomValue(0, 1);
}

static bool checkWin() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (grid[i][j] == 0)
                return false;
    return true;
}

static bool IsMouseOver(Rectangle rect) {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, rect);
}

void PlayLIGHTSON() {
    // Load textures
    Texture2D lightOn = LoadTexture("light_on.png");
    Texture2D lightOff = LoadTexture("light_off.png");

    if (lightOn.id == 0 || lightOff.id == 0) {
        printf("Error: Could not load textures.\n");
        return;
    }

    randomizeGrid();

    int moveCount = 0;
    float elapsedTime = 0;
    float flashTimer = 0;
    bool gameStarted = false;
    bool win = false;

    Rectangle restartBtn = {WINDOW_WIDTH - 130, 15, 100, 30};

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
    currentGameState = MAIN_ROOM;
    break;
}

        if (!win && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            if (!IsMouseOver(restartBtn) && mouse.y > UI_HEIGHT) {
                int row = (mouse.y - UI_HEIGHT) / CELL_SIZE;
                int col = mouse.x / CELL_SIZE;
                toggle(row, col);
                moveCount++;
                gameStarted = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsMouseOver(restartBtn)) {
            randomizeGrid();
            moveCount = 0;
            elapsedTime = 0;
            flashTimer = 0;
            win = false;
            gameStarted = false;
        }

        if (gameStarted && !win) {
            elapsedTime += GetFrameTime();
        }

        if (!win && checkWin()) {
            win = true;
        }

        if (win) {
            flashTimer += GetFrameTime();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangle(0, 0, WINDOW_WIDTH, UI_HEIGHT, DARKGRAY);
        DrawRectangleRec(restartBtn, GRAY);
        DrawText("Restart", restartBtn.x + 10, restartBtn.y + 5, 20, WHITE);

        char timeStr[64];
        snprintf(timeStr, sizeof(timeStr), "Time: %.1f sec", elapsedTime);
        DrawText(timeStr, 10, 15, 20, WHITE);

        char moveStr[64];
        snprintf(moveStr, sizeof(moveStr), "Moves: %d", moveCount);
        DrawText(moveStr, 150, 15, 20, WHITE);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                Vector2 pos = { j * CELL_SIZE, i * CELL_SIZE + UI_HEIGHT };
                if (grid[i][j]) {
                    if (win && ((int)(flashTimer * 5) % 2 == 0)) {
                        DrawRectangle(pos.x, pos.y, CELL_SIZE, CELL_SIZE, GOLD);
                    }
                    DrawTextureEx(lightOn, pos, 0.0f, (float)CELL_SIZE / lightOn.width, WHITE);
                } else {
                    DrawTextureEx(lightOff, pos, 0.0f, (float)CELL_SIZE / lightOff.width, WHITE);
                }
            }
        }

        if (win) {
            DrawText("You Win!", WINDOW_WIDTH / 2 - 100, UI_HEIGHT + 20, 40, GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(lightOn);
    UnloadTexture(lightOff);
}
