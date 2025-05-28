#include "raylib.h"
#include "game_states.h"
#include "puzzle.h"
#include "snake.h"
#include "tictactoe.h"
#include "minesweeper.h"
#include "gameoflife.h"
#include "lights_on.h"

GameState currentGameState = MAIN_ROOM;

void DrawButton(Rectangle rect, const char* text, Color color) {
    DrawRectangleRec(rect, color);
    DrawText(text, rect.x + 20, rect.y + 10, 20, BLACK);
}

void DrawMainRoom() {
    DrawText("Click a button to play a mini-game", 50, 30, 20, DARKGRAY);

    // Define buttons
    Rectangle buttons[6] = {
        {100, 100, 120, 40},
        {250, 100, 120, 40},
        {400, 100, 120, 40},
        {100, 160, 120, 40},
        {250, 160, 120, 40},
        {400, 160, 120, 40}
    };

    const char* labels[6] = {
        "PUZZLE GAME", "SNAKE", "TIC-TAC-TOE",
        "MINESWEEPER", "GAME OF LIFE", "LIGHTS ON"
    };

    for (int i = 0; i < 6; i++) {
        DrawButton(buttons[i], labels[i], LIGHTGRAY);

        // Check mouse click
        if (CheckCollisionPointRec(GetMousePosition(), buttons[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentGameState = (GameState)(PUZZLE + i);
        }
    }
}

int main() {
    InitWindow(800, 600, "Game Hub with Mouse Buttons");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentGameState) {
            case MAIN_ROOM:
                DrawMainRoom();
                break;
            case PUZZLE:
                PlayPUZZLE();
                break;
            case SNAKE:
                PlaySNAKE();
                break;
            case TICTACTOE:
                PlayTICTACTOE();
                break;
            case MINESWEEPER:
                PlayMINESWEEPER();
                break;
            case GAMEOFLIFE:
                PlayGAMEOFLIFE();
                break;
            case LIGHTS_ON:
                PlayLIGHTSON();
                break;
            default:
                currentGameState = MAIN_ROOM;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
