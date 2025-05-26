#include "raylib.h"
#include <string>
#include <array>
#include <iostream>

const int WINDOW_SIZE = 600;
const int CELL_SIZE = WINDOW_SIZE / 3;

enum class GameState { EnteringNames, Playing, GameOver };

int main() {
    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Tic Tac Toe - vs Bot");
    SetTargetFPS(60);

    GameState gameState = GameState::EnteringNames;
    std::string playerName = "";
    std::string botName = "Bot";
    bool xTurn = true;  // Player is X
    std::array<std::array<std::string, 3>, 3> board = { "" };
    std::string winner = "";

    // Input related
    bool enteringName = true;
    int nameCursorBlinkTimer = 0;
    const int blinkDelay = 30;

    // Load default font (raylib default)
    Font font = GetFontDefault();

    // Status text position
    Vector2 statusPos = { 10, 10 };

    // Restart button
    Rectangle restartButton = { WINDOW_SIZE / 2 - 100, WINDOW_SIZE - 70, 200, 50 };

    auto checkWin = [&]() -> bool {
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && !board[i][0].empty()) ||
                (board[0][i] == board[1][i] && board[1][i] == board[2][i] && !board[0][i].empty()))
                return true;
        }
        return (!board[0][0].empty() && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
               (!board[0][2].empty() && board[0][2] == board[1][1] && board[1][1] == board[2][0]);
    };

    auto boardFull = [&]() -> bool {
        for (auto &row : board)
            for (auto &cell : row)
                if (cell.empty()) return false;
        return true;
    };

    auto botMove = [&]() {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                if (board[r][c].empty()) {
                    board[r][c] = "O";
                    return;
                }
    };

    auto resetGame = [&]() {
        for (auto &row : board)
            for (auto &cell : row)
                cell.clear();
        gameState = GameState::Playing;
        xTurn = true;
        winner.clear();
    };

    while (!WindowShouldClose()) {
        // Input handling
        if (gameState == GameState::EnteringNames) {
            int key = GetKeyPressed();
            if (key > 0 && playerName.size() < 20) {
                if (key == KEY_BACKSPACE && !playerName.empty()) {
                    playerName.pop_back();
                } else if (key >= 32 && key < 128) { // printable ASCII
                    playerName.push_back((char)key);
                }
            }
            if (IsKeyPressed(KEY_ENTER) && !playerName.empty()) {
                gameState = GameState::Playing;
            }
        } else if (gameState == GameState::Playing) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xTurn) {
                int col = GetMouseX() / CELL_SIZE;
                int row = GetMouseY() / CELL_SIZE;
                if (row < 3 && col < 3 && board[row][col].empty()) {
                    board[row][col] = "X";
                    if (checkWin()) {
                        winner = playerName;
                        gameState = GameState::GameOver;
                    } else if (boardFull()) {
                        winner = "Draw";
                        gameState = GameState::GameOver;
                    } else {
                        xTurn = false;
                        botMove();
                        if (checkWin()) {
                            winner = botName;
                            gameState = GameState::GameOver;
                        } else if (boardFull()) {
                            winner = "Draw";
                            gameState = GameState::GameOver;
                        } else {
                            xTurn = true;
                        }
                    }
                }
            }
        } else if (gameState == GameState::GameOver) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePoint = { (float)GetMouseX(), (float)GetMouseY() };
                if (CheckCollisionPointRec(mousePoint, restartButton)) {
                    resetGame();
                }
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground((Color){40, 44, 52, 255}); // dark background

        if (gameState == GameState::EnteringNames) {
            DrawText("Enter Your Name:", 50, 100, 28, WHITE);
            std::string displayName = playerName;
            // blinking cursor
            if ((GetFrameTime() * 60) - (int)(GetFrameTime() * 60) < 0.5f)
                displayName += "_";
            DrawText(displayName.c_str(), 50, 160, 28, YELLOW);
        } else {
            // Draw grid lines
            for (int i = 1; i < 3; ++i) {
                DrawRectangle(0, i * CELL_SIZE, WINDOW_SIZE, 4, (Color){180, 180, 180, 255});
                DrawRectangle(i * CELL_SIZE, 0, 4, WINDOW_SIZE, (Color){180, 180, 180, 255});
            }

            // Draw X and O
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    if (!board[r][c].empty()) {
                        Color col = board[r][c] == "X" ? RED : SKYBLUE;
                        int offsetX = 55;
                        int offsetY = 25;
                        DrawText(board[r][c].c_str(), c * CELL_SIZE + offsetX, r * CELL_SIZE + offsetY, 100, col);
                    }
                }
            }

            // Draw status text
            if (gameState == GameState::GameOver) {
                if (winner == "Draw")
                    DrawText("It's a Draw!", statusPos.x, statusPos.y, 24, WHITE);
                else
                    DrawText((winner + " Wins!").c_str(), statusPos.x, statusPos.y, 24, WHITE);

                // Draw restart button
                DrawRectangleRec(restartButton, (Color){100, 200, 100, 255});
                DrawText("Restart", restartButton.x + 50, restartButton.y + 10, 24, BLACK);
            } else {
                std::string turn = xTurn ? playerName : botName;
                DrawText((turn + "'s Turn (" + (xTurn ? "X" : "O") + ")").c_str(), statusPos.x, statusPos.y, 24, WHITE);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}