#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 4
#define TILE_SIZE 100
#define TILE_GAP 5
#define WINDOW_WIDTH (GRID_SIZE * (TILE_SIZE + TILE_GAP) + TILE_GAP)
#define WINDOW_HEIGHT (GRID_SIZE * (TILE_SIZE + TILE_GAP) + TILE_GAP + 50)

int tiles[GRID_SIZE * GRID_SIZE];
int emptyIndex;
int moveCount = 0;

void DrawTile(int number, int x, int y, Texture2D texture) {
    if (number == 0) return;  // Don't draw the empty tile

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

bool IsSolved() {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE - 1; i++) {
        if (tiles[i] != i + 1) return false;
    }
    return tiles[GRID_SIZE * GRID_SIZE - 1] == 0;
}

// void ShuffleTiles() {
//     {
//         for (int i = 0; i < GRID_SIZE * GRID_SIZE - 1; i++) {
//             tiles[i] = i + 1;
//         }
//         tiles[GRID_SIZE * GRID_SIZE - 1] = 0;
//         emptyIndex = GRID_SIZE * GRID_SIZE - 1;

//         for (int i = 0; i <shuffleMoves; i++) {
//             int j = rand() % (i + 1);
//             int temp = tiles[i];
//             tiles[i] = tiles[j];
//             tiles[j] = temp;
//             if (tiles[i] == 0) emptyIndex = i;
//             if (tiles[j] == 0) emptyIndex = j;
//         }
//     } while (IsSolved());

//     moveCount = 0;
// }



bool CanMove(int tileIndex) {
    int row = tileIndex / GRID_SIZE;
    int col = tileIndex % GRID_SIZE;
    int emptyRow = emptyIndex / GRID_SIZE;
    int emptyCol = emptyIndex % GRID_SIZE;

    return (row == emptyRow && (col == emptyCol - 1 || col == emptyCol + 1)) ||
           (col == emptyCol && (row == emptyRow - 1 || row == emptyRow + 1));
}

void MoveTile(int tileIndex) {
    if (CanMove(tileIndex)) {
        tiles[emptyIndex] = tiles[tileIndex];
        tiles[tileIndex] = 0;
        emptyIndex = tileIndex;
        moveCount++;
    }
}

void ShuffleTiles() {
    // Start from the solved state
    for (int i = 0; i < GRID_SIZE * GRID_SIZE - 1; i++) {
        tiles[i] = i + 1;
    }
    tiles[GRID_SIZE * GRID_SIZE - 1] = 0;
    emptyIndex = GRID_SIZE * GRID_SIZE - 1;

    // Make a series of valid random moves
    int shuffleMoves = 1000;  // You can adjust this
    for (int i = 0; i < shuffleMoves; i++) {
        int emptyRow = emptyIndex / GRID_SIZE;
        int emptyCol = emptyIndex % GRID_SIZE;

        int directions[4][2] = {
            { 0, -1 }, // left
            { 0,  1 }, // right
            { -1, 0 }, // up
            { 1,  0 }  // down
        };

        int validMoves[4];
        int numValidMoves = 0;

        for (int d = 0; d < 4; d++) {
            int newRow = emptyRow + directions[d][0];
            int newCol = emptyCol + directions[d][1];

            if (newRow >= 0 && newRow < GRID_SIZE && newCol >= 0 && newCol < GRID_SIZE) {
                validMoves[numValidMoves++] = newRow * GRID_SIZE + newCol;
            }
        }

        if (numValidMoves > 0) {
            int chosen = validMoves[rand() % numValidMoves];
            MoveTile(chosen);  // Use your existing MoveTile function
        }
    }

    moveCount = 0;
}

int main(void) {
    srand(time(NULL));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sliding Tile Puzzle");
    SetTargetFPS(60);

    Texture2D fullImage = LoadTexture("circuit.png");

    ShuffleTiles();

    bool gameWon = false;

    while (!WindowShouldClose()) {
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

        BeginDrawing();
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
        const char *buttonHighScore = "High Score";
        const char *buttonHelp = "Help";
        const char *buttonExit = "Exit";

        int textWidthNewGame = MeasureText(buttonText, fontSize);
        int textWidthHighScore = MeasureText(buttonHighScore, fontSize);
        int textWidthHelp = MeasureText(buttonHelp, fontSize);
        int textWidthExit = MeasureText(buttonExit, fontSize);

        DrawText(buttonText,
                 10 + (100 - textWidthNewGame) / 2,
                 WINDOW_HEIGHT - 40 + (30 - fontSize) / 2,
                 fontSize,
                 DARKGRAY);

        DrawText(buttonHighScore,
                 120 + (100 - textWidthHighScore) / 2,
                 WINDOW_HEIGHT - 40 + (30 - fontSize) / 2,
                 fontSize,
                 DARKGRAY);

        DrawText(buttonHelp,
                 230 + (100 - textWidthHelp) / 2,
                 WINDOW_HEIGHT - 40 + (30 - fontSize) / 2,
                 fontSize,
                 DARKGRAY);

        DrawText(buttonExit,
                 WINDOW_WIDTH - 100 + (100 - textWidthExit) / 2,
                 WINDOW_HEIGHT - 40 + (30 - fontSize) / 2,
                 fontSize,
                 DARKGRAY);

        if (gameWon) {
            DrawText("You solved the puzzle!", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 20, 30, GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(fullImage);
    CloseWindow();

    return 0;
}
