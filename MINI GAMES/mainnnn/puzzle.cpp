#include "raylib.h"
#include "puzzle.h"
#include <stdlib.h>
#include <time.h>
#include "game_states.h"  
extern GameState currentGameState;

#define GRID_SIZE 4
#define TILE_SIZE 100
#define TILE_GAP 5
#define WINDOW_WIDTH (GRID_SIZE * (TILE_SIZE + TILE_GAP) + TILE_GAP)
#define WINDOW_HEIGHT (GRID_SIZE * (TILE_SIZE + TILE_GAP) + TILE_GAP + 50)

static int tiles[GRID_SIZE * GRID_SIZE];
static int emptyIndex;
static int moveCount = 0;
static bool initialized = false;
static Texture2D fullImage;
static bool gameWon = false;

static void DrawTile(int number, int x, int y, Texture2D texture) {
    if (number == 0) return;

    int index = number - 1;
    int srcCol = index % GRID_SIZE;
    int srcRow = index / GRID_SIZE;

    float srcTileWidth = (float)texture.width / GRID_SIZE;
    float srcTileHeight = (float)texture.height / GRID_SIZE;

    Rectangle source = {
        srcCol * srcTileWidth,
        srcRow * srcTileHeight,
        srcTileWidth,
        srcTileHeight
    };

    Rectangle dest = {
        (float)x, (float)y,
        TILE_SIZE, TILE_SIZE
    };

    DrawTexturePro(texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

static bool IsSolved() {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE - 1; i++) {
        if (tiles[i] != i + 1) return false;
    }
    return tiles[GRID_SIZE * GRID_SIZE - 1] == 0;
}

static void ShuffleTiles() {
    do {
        for (int i = 0; i < GRID_SIZE * GRID_SIZE - 1; i++) {
            tiles[i] = i + 1;
        }
        tiles[GRID_SIZE * GRID_SIZE - 1] = 0;
        emptyIndex = GRID_SIZE * GRID_SIZE - 1;

        for (int i = GRID_SIZE * GRID_SIZE - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = tiles[i];
            tiles[i] = tiles[j];
            tiles[j] = temp;
            if (tiles[i] == 0) emptyIndex = i;
            if (tiles[j] == 0) emptyIndex = j;
        }
    } while (IsSolved());

    moveCount = 0;
}

static bool CanMove(int tileIndex) {
    int row = tileIndex / GRID_SIZE;
    int col = tileIndex % GRID_SIZE;
    int emptyRow = emptyIndex / GRID_SIZE;
    int emptyCol = emptyIndex % GRID_SIZE;

    return (row == emptyRow && (col == emptyCol - 1 || col == emptyCol + 1)) ||
           (col == emptyCol && (row == emptyRow - 1 || row == emptyRow + 1));
}

static void MoveTile(int tileIndex) {
    if (CanMove(tileIndex)) {
        tiles[emptyIndex] = tiles[tileIndex];
        tiles[tileIndex] = 0;
        emptyIndex = tileIndex;
        moveCount++;
    }
}

void PlayPUZZLE() {
    if (!initialized) {
        srand(time(NULL));
        fullImage = LoadTexture("circuit.png");
        ShuffleTiles();
        initialized = true;
        gameWon = false;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
    initialized = false;  // Reset for next time
    UnloadTexture(fullImage);
    currentGameState = MAIN_ROOM;
    return;
}


    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !gameWon) {
        Vector2 mousePos = GetMousePosition();
        int col = (int)(mousePos.x) / (TILE_SIZE + TILE_GAP);
        int row = (int)(mousePos.y) / (TILE_SIZE + TILE_GAP);

        if (col >= 0 && col < GRID_SIZE && row >= 0 && row < GRID_SIZE) {
            int index = row * GRID_SIZE + col;
            MoveTile(index);
        }
    }

    if (IsKeyPressed(KEY_S)) {
        ShuffleTiles();
        gameWon = false;
    }

    if (IsSolved()) {
        gameWon = true;
    }

    // --- Drawing ---
    ClearBackground(RAYWHITE);

    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        int row = i / GRID_SIZE;
        int col = i % GRID_SIZE;
        int x = TILE_GAP + col * (TILE_SIZE + TILE_GAP);
        int y = TILE_GAP + row * (TILE_SIZE + TILE_GAP);
        DrawTile(tiles[i], x, y, fullImage);
    }

    int fontSize = 20;
    const char *buttonText = TextFormat("Moves: %d", moveCount);

    DrawText(buttonText, 10, WINDOW_HEIGHT - 40, fontSize, DARKGRAY);

    if (gameWon) {
        DrawText("You solved the puzzle!", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 20, 30, GREEN);
    }
}

