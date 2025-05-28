#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#define TILE_SIZE 24
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
#define NUM_MINES 40
#define HEADER_HEIGHT 80
#define MAX_LIVES 3
#define MAX_TIME 300 // 5 minutes

#define VIRTUAL_WIDTH (GRID_WIDTH * TILE_SIZE)
#define VIRTUAL_HEIGHT (GRID_HEIGHT * TILE_SIZE + HEADER_HEIGHT)

typedef enum {
    STATE_SPLASH,
    STATE_PLAYING,
    STATE_END
} GameState;

typedef struct {
    bool revealed;
    bool flagged;
    bool hasMine;
    int adjacentMines;
} Tile;

Tile grid[GRID_HEIGHT][GRID_WIDTH];
GameState gameState = STATE_SPLASH;
bool gameWon = false;
int flagsLeft = NUM_MINES;
int livesLeft = MAX_LIVES;
bool firstClick = true;
float gameTimer = 0;
float gameStartTime = 0;

const int dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
const int dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

void PlaceMines(int avoidX, int avoidY) {
    int placed = 0;
    while (placed < NUM_MINES) {
        int x = rand() % GRID_WIDTH;
        int y = rand() % GRID_HEIGHT;

        bool skip = false;
        for (int i = 0; i < 8; i++) {
            int nx = avoidX + dx[i];
            int ny = avoidY + dy[i];
            if ((x == avoidX && y == avoidY) || (x == nx && y == ny)) skip = true;
        }
        if (skip || grid[y][x].hasMine) continue;

        grid[y][x].hasMine = true;
        placed++;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x].hasMine) continue;
            int count = 0;
            for (int i = 0; i < 8; i++) {
                int nx = x + dx[i], ny = y + dy[i];
                if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT && grid[ny][nx].hasMine)
                    count++;
            }
            grid[y][x].adjacentMines = count;
        }
    }
}

void ResetGame() {
    flagsLeft = NUM_MINES;
    livesLeft = MAX_LIVES;
    firstClick = true;
    gameTimer = 0;
    gameStartTime = 0;
    gameWon = false;
    gameState = STATE_SPLASH;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = (Tile){0};
        }
    }
}

void Reveal(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return;
    Tile *tile = &grid[y][x];
    if (tile->revealed || tile->flagged) return;

    tile->revealed = true;

    if (tile->hasMine) {
        livesLeft--;
        if (livesLeft <= 0) {
            gameState = STATE_END;
        }
        return;
    }

    if (tile->adjacentMines == 0) {
        for (int i = 0; i < 8; i++) {
            Reveal(x + dx[i], y + dy[i]);
        }
    }
}

void CheckWinCondition() {
    int revealedCount = 0;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (!grid[y][x].hasMine && grid[y][x].revealed) {
                revealedCount++;
            }
        }
    }
    if (revealedCount == GRID_WIDTH * GRID_HEIGHT - NUM_MINES) {
        gameWon = true;
        gameState = STATE_END;
    }
}

int main(void) {
    srand(time(NULL));
    InitWindow(VIRTUAL_WIDTH * 2, VIRTUAL_HEIGHT * 2, "Minesweeper");
    SetWindowMinSize(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Texture2D bombTexture = LoadTexture("explosion.png");
    Texture2D flagTexture = LoadTexture("flag.png");
    Texture2D splashTexture = LoadTexture("splash_bomb.png");

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

    ResetGame();

    while (!WindowShouldClose()) {
        float scaleX = (float)GetScreenWidth() / VIRTUAL_WIDTH;
        float scaleY = (float)GetScreenHeight() / VIRTUAL_HEIGHT;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = {
            (mouse.x - (GetScreenWidth() - VIRTUAL_WIDTH * scale) / 2) / scale,
            (mouse.y - (GetScreenHeight() - VIRTUAL_HEIGHT * scale) / 2) / scale
        };

        BeginTextureMode(target);
        ClearBackground(BLACK);
if (gameState == STATE_SPLASH || gameState == STATE_END) {
    // Scale splash image to fit screen
    float scaleFactor = fminf(
        (float)VIRTUAL_WIDTH / splashTexture.width * 0.8f,
        (float)(VIRTUAL_HEIGHT - HEADER_HEIGHT) / splashTexture.height * 0.8f
    );

    int splashW = splashTexture.width * scaleFactor;
    int splashH = splashTexture.height * scaleFactor;
    int splashX = VIRTUAL_WIDTH / 2 - splashW / 2;
    int splashY = VIRTUAL_HEIGHT / 2 - splashH / 2;

    Rectangle destRect = { (float)splashX,(float) splashY, (float)splashW,(float) splashH };

    // Draw splash bomb
    DrawTexturePro(
        splashTexture,
        (Rectangle){0, 0, (float)splashTexture.width, (float)splashTexture.height},
        destRect,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

    // Title text
    const char* msg = gameState == STATE_SPLASH ? "Click the Bomb or Start" :
                      (gameWon ? "You Win!" : "Game Over!");
    DrawText(msg, VIRTUAL_WIDTH / 2 - MeasureText(msg, 30) / 2, 30, 30, WHITE);

    // Start / Restart Button
    Rectangle startBtn = { VIRTUAL_WIDTH / 2 - 60, VIRTUAL_HEIGHT - 80, 120, 40 };
    DrawRectangleRec(startBtn, GREEN);
    DrawRectangleLinesEx(startBtn, 2, DARKGREEN);
    DrawText("Start", startBtn.x + 25, startBtn.y + 10, 20, WHITE);

    // Click detection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        (CheckCollisionPointRec(virtualMouse, destRect) || CheckCollisionPointRec(virtualMouse, startBtn))) {
        ResetGame();
        gameState = STATE_PLAYING;
        gameStartTime = GetTime();
    }
}

        
          else if (gameState == STATE_PLAYING) {
            gameTimer = GetTime() - gameStartTime;
            if (gameTimer >= MAX_TIME) gameState = STATE_END;

            DrawRectangle(0, 0, VIRTUAL_WIDTH, HEADER_HEIGHT, DARKGRAY);
            DrawText(TextFormat("Flags Left: %d", flagsLeft), 10, 10, 20, RAYWHITE);
            DrawText(TextFormat("Lives: %d", livesLeft), 10, 40, 20, YELLOW);
            DrawText(TextFormat("Time: %02d:%02d", (int)(MAX_TIME - gameTimer) / 60, (int)(MAX_TIME - gameTimer) % 60),
                     VIRTUAL_WIDTH - 130, 10, 20, RAYWHITE);

            if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))) {
                int x = virtualMouse.x / TILE_SIZE;
                int y = (virtualMouse.y - HEADER_HEIGHT) / TILE_SIZE;
                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    if (firstClick) {
                        PlaceMines(x, y);
                        firstClick = false;
                    }

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        Reveal(x, y);
                        CheckWinCondition();
                    } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        Tile *tile = &grid[y][x];
                        if (!tile->revealed) {
                            tile->flagged = !tile->flagged;
                            flagsLeft += tile->flagged ? -1 : 1;
                        }
                    }
                }
            }

            for (int y = 0; y < GRID_HEIGHT; y++) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                    int px = x * TILE_SIZE;
                    int py = y * TILE_SIZE + HEADER_HEIGHT;
                    Rectangle tileRect = { (float)px,(float) py, TILE_SIZE, TILE_SIZE };
                    Tile tile = grid[y][x];

                    if (tile.revealed) {
                        if (tile.hasMine) {
                            DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, RED);
                            DrawTextureEx(bombTexture, (Vector2){(float)px,(float) py}, 0.0f, (float)TILE_SIZE / bombTexture.width, WHITE);
                        } else {
                            DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, WHITE);
                            if (tile.adjacentMines > 0) {
                                Color numColor;
                                switch (tile.adjacentMines) {
                                    case 1: numColor = BLUE; break;
                                    case 2: numColor = GREEN; break;
                                    case 3: numColor = RED; break;
                                    case 4: numColor = PURPLE; break;
                                    case 5: numColor = MAROON; break;
                                    case 6: numColor = DARKBLUE; break;
                                    case 7: numColor = BLACK; break;
                                    case 8: numColor = GRAY; break;
                                    default: numColor = DARKGRAY; break;
                                }
                                DrawText(TextFormat("%d", tile.adjacentMines), px + 8, py + 5, 20, numColor);
                            }
                        }
                    } else {
                        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, BLACK);
                        if (tile.flagged) {
                            DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, ORANGE);
                            DrawTextureEx(flagTexture, (Vector2){(float)px,(float) py}, 0.0f, (float)TILE_SIZE / flagTexture.width, WHITE);
                        } else if (CheckCollisionPointRec(virtualMouse, tileRect)) {
                            DrawRectangleLinesEx(tileRect, 2, BLUE);
                        }
                    }

                    DrawRectangleLinesEx(tileRect, 1, GRAY);
                }
            }
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(
            target.texture,
            (Rectangle){0, 0, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT},
            (Rectangle){
                (GetScreenWidth() - VIRTUAL_WIDTH * scale) / 2,
                (GetScreenHeight() - VIRTUAL_HEIGHT * scale) / 2,
                VIRTUAL_WIDTH * scale,
                VIRTUAL_HEIGHT * scale
            },
            (Vector2){0, 0}, 0.0f, WHITE
        );
        EndDrawing();
    }

    UnloadTexture(bombTexture);
    UnloadTexture(flagTexture);
    UnloadTexture(splashTexture);
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}