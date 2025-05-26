#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_SEGMENTS 100

typedef struct Worm {
    Vector2 position[MAX_SEGMENTS];
    int size;
} Worm;

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int cellSize = 40;
    const float step = cellSize * 0.55f;

    InitWindow(screenWidth, screenHeight, "Feed The Worm");

    Texture2D background = LoadTexture("bg.jpg");
    Texture2D foodImage = LoadTexture("food1.png");
    Texture2D wormHead = LoadTexture("wormhead1.png");
    Texture2D wormBody = LoadTexture("wormbody.png");

    Worm worm = {0};
    worm.size = 2;
    Vector2 currentDirection = (Vector2){step, 0};
    Vector2 nextDirection = currentDirection;

    Vector2 headStart = (Vector2){screenWidth / 2, screenHeight / 2};
    worm.position[0] = headStart;

    for (int i = 1; i < worm.size; i++) {
        worm.position[i] = Vector2Subtract(worm.position[i - 1], currentDirection);
    }

    Vector2 food = {
        (float)(rand() % (screenWidth / cellSize)) * cellSize,
        (float)(rand() % (screenHeight / cellSize)) * cellSize
    };

    bool gameOver = false;

    SetTargetFPS(10);

    while (!WindowShouldClose()) {
        if (!gameOver) {
            // Input
            if (IsKeyPressed(KEY_UP) && currentDirection.y == 0) nextDirection = (Vector2){0, -step};
            if (IsKeyPressed(KEY_DOWN) && currentDirection.y == 0) nextDirection = (Vector2){0, step};
            if (IsKeyPressed(KEY_LEFT) && currentDirection.x == 0) nextDirection = (Vector2){-step, 0};
            if (IsKeyPressed(KEY_RIGHT) && currentDirection.x == 0) nextDirection = (Vector2){step, 0};

            // Move worm
            for (int i = worm.size - 1; i >0; i--) {
                worm.position[i] = worm.position[i - 1];
            }

            worm.position[0] = Vector2Add(worm.position[0], currentDirection);
            currentDirection = nextDirection;

            // Collision: wall
            if (worm.position[0].x < 0 || worm.position[0].x >= screenWidth ||
                worm.position[0].y < 0 || worm.position[0].y >= screenHeight) {
                gameOver = true;
            }

            // Collision: self
            for (int i = 1; i < worm.size; i++) {
                if (Vector2Distance(worm.position[0], worm.position[i]) < step / 2) {
                    gameOver = true;
                    break;
                }
            }

            // Food collision
            Rectangle headRect = { worm.position[0].x, worm.position[0].y, cellSize, cellSize };
            Rectangle foodRect = { food.x, food.y, cellSize, cellSize };

            if (CheckCollisionRecs(headRect, foodRect)) {
                if (worm.size < MAX_SEGMENTS) {
                    worm.position[worm.size] = worm.position[worm.size - 1];
                    worm.size++;
                }

                food = (Vector2){
                    (float)(rand() % (screenWidth / cellSize)) * cellSize,
                    (float)(rand() % (screenHeight / cellSize)) * cellSize
                };
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);
        DrawTexture(foodImage, food.x, food.y, WHITE);

        for (int i = 0; i < worm.size; i++) {
            Vector2 drawPos = worm.position[i];
            Texture2D segmentTexture;
            float rotation = 0.0f;

            if (i == 0) {
                segmentTexture = wormHead;
                if (currentDirection.x > 0) rotation = 0.0f;
                else if (currentDirection.x < 0) rotation = 0.0f;
                else if (currentDirection.y > 0) rotation = 0.0f;
                else if (currentDirection.y < 0) rotation = 0.0f;
            } else {
                segmentTexture = wormBody;
            }

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
                rotation,
                WHITE
            );

           // DrawRectangleLines(drawPos.x, drawPos.y, cellSize, cellSize, RED); // Optional debug
        }

        DrawText(TextFormat("Score: %d", worm.size), 10, 10, 20, DARKGRAY);

        if (gameOver) {
            DrawText("GAME OVER! Press R to restart.", screenWidth / 2 - 230 , screenHeight / 2, 30, RED);
            if (IsKeyPressed(KEY_R)) {
                // Reset worm and food
                worm.size = 2;
                worm.position[0] = headStart;
                for (int i = 1; i < worm.size; i++) {
                    worm.position[i] = Vector2Subtract(worm.position[i - 1], currentDirection);
                }
                food = (Vector2){
                    (float)(rand() % (screenWidth / cellSize)) * cellSize,
                    (float)(rand() % (screenHeight / cellSize)) * cellSize
                };
                currentDirection = (Vector2){step, 0};
                nextDirection = currentDirection;
                gameOver = false;
            }
        }

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(background);
    UnloadTexture(foodImage);
    UnloadTexture(wormHead);
    UnloadTexture(wormBody);
    CloseWindow();

    return 0;
}
