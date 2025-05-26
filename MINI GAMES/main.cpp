#include <SFML/Graphics.hpp>
#include <iostream>

const int WINDOW_SIZE = 600;
const int CELL_SIZE = WINDOW_SIZE / 3;

enum class GameState { EnteringNames, Playing, GameOver };

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Tic Tac Toe - vs Bot");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font load failed.\n";
        return 1;
    }

    // Game state variables
    GameState gameState = GameState::EnteringNames;
    std::string playerName = "", botName = "Bot";
    bool enteringName = true;
    bool xTurn = true;  // Player is X
    std::string board[3][3] = { "" };
    std::string winner = "";

    // UI Elements
    sf::Text namePrompt("Enter Your Name:", font, 28);
    namePrompt.setFillColor(sf::Color::White);
    namePrompt.setPosition(50, 100);

    sf::Text nameInput("", font, 28);
    nameInput.setFillColor(sf::Color::Yellow);
    nameInput.setPosition(50, 160);

    sf::Text statusText("", font, 24);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(10, 10);

    sf::RectangleShape restartButton(sf::Vector2f(200, 50));
    restartButton.setFillColor(sf::Color(100, 200, 100));
    restartButton.setPosition(WINDOW_SIZE / 2 - 100, WINDOW_SIZE - 70);

    sf::Text restartText("Restart", font, 24);
    restartText.setFillColor(sf::Color::Black);
    restartText.setPosition(restartButton.getPosition().x + 50, restartButton.getPosition().y + 10);

    auto checkWin = [&](const std::string& symbol) -> bool {
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) ||
                (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol))
                return true;
        }
        return (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) ||
               (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol);
    };

    auto boardFull = [&]() -> bool {
        for (auto& row : board)
            for (auto& cell : row)
                if (cell == "") return false;
        return true;
    };

    auto botMove = [&]() {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                if (board[r][c] == "") {
                    board[r][c] = "O";
                    return;
                }
    };

    auto resetGame = [&]() {
        for (auto& row : board)
            for (auto& cell : row)
                cell = "";
        gameState = GameState::Playing;
        xTurn = true;
        winner = "";
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == GameState::EnteringNames && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    if (!playerName.empty()) playerName.pop_back();
                } else if (event.text.unicode == '\r') {
                    gameState = GameState::Playing;
                } else if (event.text.unicode < 128) {
                    playerName.push_back(static_cast<char>(event.text.unicode));
                }
                nameInput.setString(playerName);
            }

            if (gameState == GameState::Playing && event.type == sf::Event::MouseButtonPressed) {
                int col = event.mouseButton.x / CELL_SIZE;
                int row = event.mouseButton.y / CELL_SIZE;

                if (row < 3 && col < 3 && board[row][col] == "" && xTurn) {
                    board[row][col] = "X";
                    if (checkWin("X")) {
                        winner = playerName;
                        gameState = GameState::GameOver;
                    } else if (boardFull()) {
                        winner = "Draw";
                        gameState = GameState::GameOver;
                    } else {
                        xTurn = false;
                        botMove();
                        if (checkWin("O")) {
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

            if (gameState == GameState::GameOver && event.type == sf::Event::MouseButtonPressed) {
                if (restartButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    resetGame();
                }
            }
        }

        // Draw
        window.clear(sf::Color(40, 44, 52));  // dark background

        if (gameState == GameState::EnteringNames) {
            window.draw(namePrompt);
            window.draw(nameInput);
        } else {
            // Draw grid
            for (int i = 1; i < 3; ++i) {
                sf::RectangleShape lineH(sf::Vector2f(WINDOW_SIZE, 4));
                lineH.setPosition(0, i * CELL_SIZE);
                lineH.setFillColor(sf::Color(180, 180, 180));
                window.draw(lineH);

                sf::RectangleShape lineV(sf::Vector2f(4, WINDOW_SIZE));
                lineV.setPosition(i * CELL_SIZE, 0);
                lineV.setFillColor(sf::Color(180, 180, 180));
                window.draw(lineV);
            }

            // Draw X and O
            sf::Text cellText("", font, 100);
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    if (board[r][c] != "") {
                        cellText.setString(board[r][c]);
                        cellText.setFillColor(board[r][c] == "X" ? sf::Color::Red : sf::Color::Cyan);
                        cellText.setPosition(c * CELL_SIZE + 55, r * CELL_SIZE + 25);
                        window.draw(cellText);
                    }
                }
            }

            // Status text
            if (gameState == GameState::GameOver) {
                if (winner == "Draw")
                    statusText.setString("It's a Draw!");
                else
                    statusText.setString(winner + " Wins!");
                window.draw(statusText);

                // Draw Restart
                window.draw(restartButton);
                window.draw(restartText);
            } else {
                std::string turn = xTurn ? playerName : botName;
                statusText.setString(turn + "'s Turn (" + (xTurn ? "X" : "O") + ")");
                window.draw(statusText);
            }
        }

        window.display();
    }

    return 0;
}
