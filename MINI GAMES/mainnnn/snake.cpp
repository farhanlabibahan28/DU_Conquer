#include "raylib.h"
#include <stdlib.h>
#include "raymath.h"
#include "game_states.h"
extern GameState currentGameState;


#define MAX_SEGMENTS 100

typedef struct Worm {
    Vector2 position[MAX_SEGMENTS];
    int size;
} Worm;

void PlaySNAKE(void) {
    static bool initialized = false;
    static Texture2D background, foodImage, wormHead, wormBody;
    static Vector2 currentDirection, nextDirection;
    static Vector2 headStart;
    static Vector2 food;
    static bool gameOver = false;

    static const int cellSize = 40;
    static const float step = cellSize * 0.55f;
    static Worm worm = {0};

    if (!initialized) {
        background = LoadTexture("bg.jpg");
        foodImage = LoadTexture("food1.png");
        wormHead = LoadTexture("wormhead1.png");
        wormBody = LoadTexture("wormbody.png");

        worm.size = 2;
        currentDirection = (Vector2){step, 0};
        nextDirection = currentDirection;
        headStart = (Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2};
        worm.position[0] = headStart;

        for (int i = 1; i < worm.size; i++) {
            worm.position[i] = Vector2Subtract(worm.position[i - 1], currentDirection);
        }

        food = (Vector2){
            (float)(rand() % (GetScreenWidth() / cellSize)) * cellSize,
            (float)(rand() % (GetScreenHeight() / cellSize)) * cellSize
        };

        gameOver = false;
        initialized = true;
    }

    if (!gameOver) {
        if (IsKeyPressed(KEY_UP) && currentDirection.y == 0) nextDirection = (Vector2){0, -step};
        if (IsKeyPressed(KEY_DOWN) && currentDirection.y == 0) nextDirection = (Vector2){0, step};
        if (IsKeyPressed(KEY_LEFT) && currentDirection.x == 0) nextDirection = (Vector2){-step, 0};
        if (IsKeyPressed(KEY_RIGHT) && currentDirection.x == 0) nextDirection = (Vector2){step, 0};

        for (int i = worm.size - 1; i > 0; i--) {
            worm.position[i] = worm.position[i - 1];
        }

        worm.position[0] = Vector2Add(worm.position[0], currentDirection);
        currentDirection = nextDirection;

        if (worm.position[0].x < 0 || worm.position[0].x >= GetScreenWidth() ||
            worm.position[0].y < 0 || worm.position[0].y >= GetScreenHeight()) {
            gameOver = true;
        }

        for (int i = 1; i < worm.size; i++) {
            if (Vector2Distance(worm.position[0], worm.position[i]) < step / 2) {
                gameOver = true;
                break;
            }
        }

        Rectangle headRect = { worm.position[0].x, worm.position[0].y, cellSize, cellSize };
        Rectangle foodRect = { food.x, food.y, cellSize, cellSize };

        if (CheckCollisionRecs(headRect, foodRect)) {
            if (worm.size < MAX_SEGMENTS) {
                worm.position[worm.size] = worm.position[worm.size - 1];
                worm.size++;
            }

            food = (Vector2){
                (float)(rand() % (GetScreenWidth() / cellSize)) * cellSize,
                (float)(rand() % (GetScreenHeight() / cellSize)) * cellSize
            };
        }
    }

    ClearBackground(RAYWHITE);
    DrawTexture(background, 0, 0, WHITE);
    DrawTexture(foodImage, food.x, food.y, WHITE);

    for (int i = 0; i < worm.size; i++) {
        Vector2 drawPos = worm.position[i];
        Texture2D segmentTexture = (i == 0) ? wormHead : wormBody;

        Rectangle destRect = {
            drawPos.x + cellSize / 2.0f,
            drawPos.y + cellSize / 2.0f,
            cellSize,
            cellSize
        };

        Vector2 origin = {
            segmentTexture.width / 2.0f,
            segmentTexture.height / 2.0f
        };

        DrawTexturePro(
            segmentTexture,
            (Rectangle){0, 0, segmentTexture.width, segmentTexture.height},
            destRect,
            origin,
            0.0f,   // NO rotation here as you asked
            WHITE
        );
    }

    DrawText(TextFormat("Score: %d", worm.size), 10, 10, 20, DARKGRAY);

    if (gameOver) {
        DrawText("GAME OVER! Press R to restart.", GetScreenWidth() / 2 - 230, GetScreenHeight() / 2, 30, RED);
        if (IsKeyPressed(KEY_R)) {
            worm.size = 2;
            worm.position[0] = headStart;
            for (int i = 1; i < worm.size; i++) {
                worm.position[i] = Vector2Subtract(worm.position[i - 1], currentDirection);
            }

            food = (Vector2){
                (float)(rand() % (GetScreenWidth() / cellSize)) * cellSize,
                (float)(rand() % (GetScreenHeight() / cellSize)) * cellSize
            };

            currentDirection = (Vector2){step, 0};
            nextDirection = currentDirection;
            gameOver = false;
        }
    }
}
