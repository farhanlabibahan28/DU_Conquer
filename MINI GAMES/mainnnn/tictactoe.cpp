#include "raylib.h"
#include <string>
#include <array>
#include "game_states.h"
extern GameState currentGameState;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int BOARD_SIZE = 600;
const int CELL_SIZE = BOARD_SIZE / 3;
const int BOARD_POS_X = (WINDOW_WIDTH - BOARD_SIZE) / 2;
const int BOARD_POS_Y = (WINDOW_HEIGHT - BOARD_SIZE) / 2;

enum class TicTacToeState { EnteringNames, Playing, GameOver };

void PlayTICTACTOE() {
    static TicTacToeState ticState = TicTacToeState::EnteringNames;
    static std::string playerName = "";
    static std::string botName = "Bot";
    static bool xTurn = true;  // Player is X
    static std::array<std::array<std::string, 3>, 3> board = { { {"", "", ""}, {"", "", ""}, {"", "", ""} } };
    static std::string winner = "";

    static Rectangle restartButton = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 70, 200, 50 };

    // ESC key to return to Main Room
    if (IsKeyPressed(KEY_ESCAPE)) {
        ticState = TicTacToeState::EnteringNames;
        playerName.clear();
        xTurn = true;
        winner.clear();
        for (auto &row : board)
            for (auto &cell : row)
                cell.clear();
        currentGameState = MAIN_ROOM;
        return;
    }

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
        ticState = TicTacToeState::Playing;
        xTurn = true;
        winner.clear();
    };

    // Input handling
    if (ticState == TicTacToeState::EnteringNames) {
        int key = GetKeyPressed();
        if (key > 0 && playerName.size() < 20) {
            if (key == KEY_BACKSPACE && !playerName.empty()) {
                playerName.pop_back();
            }
            else if (key >= 32 && key < 128) {
                playerName.push_back((char)key);
            }
        }
        if (IsKeyPressed(KEY_ENTER) && !playerName.empty()) {
            ticState = TicTacToeState::Playing;
        }
    }
    else if (ticState == TicTacToeState::Playing) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xTurn) {
            int mouseX = GetMouseX() - BOARD_POS_X;
            int mouseY = GetMouseY() - BOARD_POS_Y;

            if (mouseX >= 0 && mouseX < BOARD_SIZE && mouseY >= 0 && mouseY < BOARD_SIZE) {
                int col = mouseX / CELL_SIZE;
                int row = mouseY / CELL_SIZE;

                if (board[row][col].empty()) {
                    board[row][col] = "X";

                    if (checkWin()) {
                        winner = playerName;
                        ticState = TicTacToeState::GameOver;
                    }
                    else if (boardFull()) {
                        winner = "Draw";
                        ticState = TicTacToeState::GameOver;
                    }
                    else {
                        xTurn = false;
                        botMove();

                        if (checkWin()) {
                            winner = botName;
                            ticState = TicTacToeState::GameOver;
                        }
                        else if (boardFull()) {
                            winner = "Draw";
                            ticState = TicTacToeState::GameOver;
                        }
                        else {
                            xTurn = true;
                        }
                    }
                }
            }
        }
    }
    else if (ticState == TicTacToeState::GameOver) {
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

    if (ticState == TicTacToeState::EnteringNames) {
        std::string displayName = playerName;
        float time = GetTime();
        if (((int)(time * 2) % 2) == 0)
            displayName += "_";

        DrawText("Enter Your Name:", 50, 100, 28, WHITE);
        DrawText(displayName.c_str(), 50, 160, 28, YELLOW);
    }
    else {
        // Grid
        for (int i = 1; i < 3; ++i) {
            DrawRectangle(BOARD_POS_X, BOARD_POS_Y + i * CELL_SIZE, BOARD_SIZE, 4, (Color){180, 180, 180, 255});
            DrawRectangle(BOARD_POS_X + i * CELL_SIZE, BOARD_POS_Y, 4, BOARD_SIZE, (Color){180, 180, 180, 255});
        }

        // X and O
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (!board[r][c].empty()) {
                    Color col = board[r][c] == "X" ? RED : SKYBLUE;
                    int offsetX = 55;
                    int offsetY = 25;
                    DrawText(board[r][c].c_str(),
                        BOARD_POS_X + c * CELL_SIZE + offsetX,
                        BOARD_POS_Y + r * CELL_SIZE + offsetY,
                        100, col);
                }
            }
        }

        // Status text
        if (ticState == TicTacToeState::GameOver) {
            if (winner == "Draw")
                DrawText("It's a Draw!", 10, 10, 24, WHITE);
            else
                DrawText((winner + " Wins!").c_str(), 10, 10, 24, WHITE);

            DrawRectangleRec(restartButton, (Color){100, 200, 100, 255});
            DrawText("Restart", restartButton.x + 50, restartButton.y + 10, 24, BLACK);
        }
        else {
            std::string turn = xTurn ? playerName : botName;
            DrawText((turn + "'s Turn (" + (xTurn ? "X" : "O") + ")").c_str(), 10, 10, 24, WHITE);
        }
    }

    EndDrawing();
}
