#include "raylib.h"
#include <string>
#include <array>
#include <iostream>

using namespace std;

const int WINDOW_SIZE = 600;
const int CELL_SIZE = WINDOW_SIZE / 3;

// Enum to track the current state of the game
enum class GameState { StartScreen, EnteringNames, Playing, GameOver };

int main() {
    // Initialize the window
    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Tic Tac Toe - vs Bot");
    SetTargetFPS(60);

    GameState gameState = GameState::StartScreen;
    string playerName = "";
    string botName = "Bot";
    bool xTurn = true; // Track player's turn
    array<array<string, 3>, 3> board = { "" }; // 3x3 Tic Tac Toe board
    string winner = "";

    // Define buttons
    Rectangle startButton = { WINDOW_SIZE / 2 - 100, WINDOW_SIZE / 2 - 30, 200, 60 };
    Rectangle restartButton = { WINDOW_SIZE / 2 - 100, WINDOW_SIZE - 70, 200, 50 };
    Vector2 statusPos = { 10, 10 }; // Position for status messages

    Font font = GetFontDefault();

    // Function to check if any player has won
    auto checkWin = [&]() -> bool {
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && !board[i][0].empty()) ||
                (board[0][i] == board[1][i] && board[1][i] == board[2][i] && !board[0][i].empty()))
                return true;
        }
        return (!board[0][0].empty() && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
               (!board[0][2].empty() && board[0][2] == board[1][1] && board[1][1] == board[2][0]);
    };

    // Check if the board is full (draw condition)
    auto boardFull = [&]() -> bool {
        for (auto &row : board)
            for (auto &cell : row)
                if (cell.empty()) return false;
        return true;
    };

    // Simple bot move: places 'O' in first empty cell
    auto botMove = [&]() {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                if (board[r][c].empty()) {
                    board[r][c] = "O";
                    return;
                }
    };

    // Reset the game state to initial values
    auto resetGame = [&]() {
        for (auto &row : board)
            for (auto &cell : row)
                cell.clear();
        gameState = GameState::StartScreen;
        xTurn = true;
        winner.clear();
        playerName.clear();
    };

    while (!WindowShouldClose()) {
        // Input Handling
        if (gameState == GameState::StartScreen) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                if (CheckCollisionPointRec(mouse, startButton)) {
                    gameState = GameState::EnteringNames;
                }
            }
        } else if (gameState == GameState::EnteringNames) {
            int key = GetKeyPressed();
            if (key > 0 && playerName.size() < 20) {
                if (key == KEY_BACKSPACE && !playerName.empty()) {
                    playerName.pop_back();
                } else if (key >= 32 && key < 128) {
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
                Vector2 mousePoint = GetMousePosition();
                if (CheckCollisionPointRec(mousePoint, restartButton)) {
                    resetGame();
                }
            }
        }

        // Drawing section
        BeginDrawing();
        ClearBackground({40, 44, 52, 255}); // dark gray background

        // Draw start screen
        if (gameState == GameState::StartScreen) {
            DrawText("Tic Tac Toe", WINDOW_SIZE / 2 - 100, 100, 36, WHITE);
            DrawRectangleRounded(startButton, 0.4f, 10, {255, 182, 193, 255}); // pink button
            DrawText("START", startButton.x + 50, startButton.y + 15, 24, DARKPURPLE);

        // Draw name input screen
        } else if (gameState == GameState::EnteringNames) {
            DrawText("Enter Your Name:", 50, 100, 28, WHITE);
            string displayName = playerName;
            if ((GetFrameTime() * 60) - (int)(GetFrameTime() * 60) < 0.5f)
                displayName += "_"; // blinking cursor
            DrawText(displayName.c_str(), 50, 160, 28, YELLOW);

        // Draw game grid and pieces
        } else {
            for (int i = 1; i < 3; ++i) {
                DrawRectangle(0, i * CELL_SIZE, WINDOW_SIZE, 4, {180, 180, 180, 255});
                DrawRectangle(i * CELL_SIZE, 0, 4, WINDOW_SIZE, {180, 180, 180, 255});
            }

            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    if (!board[r][c].empty()) {
                        Color col = board[r][c] == "X" ? RED : SKYBLUE;
                        DrawText(board[r][c].c_str(), c * CELL_SIZE + 55, r * CELL_SIZE + 25, 100, col);
                    }
                }
            }

            // Display game result and restart button
            if (gameState == GameState::GameOver) {
                if (winner == "Draw")
                    DrawText("It's a Draw!", statusPos.x, statusPos.y, 24, WHITE);
                else
                    DrawText((winner + " Wins!").c_str(), statusPos.x, statusPos.y, 24, WHITE);

                DrawRectangleRec(restartButton, {100, 200, 100, 255});
                DrawText("Restart", restartButton.x + 50, restartButton.y + 10, 24, BLACK);

            // Show current player's turn
            } else {
                string turn = xTurn ? playerName : botName;
                DrawText((turn + "'s Turn (" + (xTurn ? "X" : "O") + ")").c_str(), statusPos.x, statusPos.y, 24, WHITE);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
