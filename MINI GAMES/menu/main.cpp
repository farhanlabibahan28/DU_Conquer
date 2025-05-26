#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

Color red = {255, 0, 0, 255};
Color blackTransparent = {0, 0, 0, 128};

class Button {
public:
    Rectangle bounds;
    const char* label;
    Color textColorNormal;
    Color textColorHover;

    Button(float x, float y, float width, float height, const char* text)
        : bounds{ x, y, width, height },
          label(text),
          textColorNormal(WHITE),
          textColorHover(RED) {}

    bool IsHovered() const {
        return CheckCollisionPointRec(GetMousePosition(), bounds);
    }

    bool IsClicked() const {
        return IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void DrawButton(int fontSize, Color bgColor) const {
        DrawRectangleRec(bounds, bgColor);
        Color currentTextColor = IsHovered() ? textColorHover : textColorNormal;
        int textWidth = MeasureText(label, fontSize);
        DrawText(label,
                 bounds.x + (bounds.width - textWidth) / 2,
                 bounds.y + (bounds.height - fontSize) / 2,
                 fontSize,
                 currentTextColor);
    }
};

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "DU_Conquer");
    Texture2D main_menu = LoadTexture("resources/main_menu.png");

    int buttonWidth = 250;
    int buttonHeight = 40;
    int spacing = 40;
    int startY = 400;

    Button btnNew(25, startY + spacing * 0, buttonWidth, buttonHeight, "New Game");
    Button btnHigh(25, startY + spacing * 1, buttonWidth, buttonHeight, "High Score");
    Button btnHelp(25, startY + spacing * 2, buttonWidth, buttonHeight, "Help");
    Button btnMap(25, startY + spacing * 3, buttonWidth, buttonHeight, "Map");
    Button btnSaved(25, startY + spacing * 4, buttonWidth, buttonHeight, "Saved Games");
    Button btnExit(25, startY + spacing * 5, buttonWidth, buttonHeight, "Exit");

    const char* clickedMessage = nullptr;
    bool high_score_b = false;

    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        // Dialog Box setup
        int boxWidth = 500;
        int boxHeight = 300;
        int x = screenWidth / 2 - boxWidth / 2 + 100;
        int y = screenHeight / 2 - boxHeight / 2 + 200;
        Rectangle closeBtn = { x + boxWidth - 35, y + 10, 25, 25 };

        // Mouse Clicks
        if (btnNew.IsClicked()) {
            clickedMessage = "New Game clicked!";
        }

        if (btnHigh.IsClicked()) {
            clickedMessage = "High Scores";
            high_score_b = true;
        }

        if (btnHelp.IsClicked()) {
            clickedMessage = "Help clicked!";
        }

        if (btnMap.IsClicked()) {
            clickedMessage = "Map clicked!";
        }

        if (btnSaved.IsClicked()) {
            clickedMessage = "Saved Games clicked!";
        }

        if (btnExit.IsClicked()) {
            CloseWindow();
            return 0;
        }

        // Box Close
        if (high_score_b &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), closeBtn)) {
            high_score_b = false;
            clickedMessage = nullptr;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(main_menu, 0, 0, WHITE);
        DrawText("DU Conquer", 20, 320, 50, red);

        int fontSize = 24;

        btnNew.DrawButton(fontSize, blackTransparent);
        btnHigh.DrawButton(fontSize, blackTransparent);
        btnHelp.DrawButton(fontSize, blackTransparent);
        btnMap.DrawButton(fontSize, blackTransparent);
        btnSaved.DrawButton(fontSize, blackTransparent);
        btnExit.DrawButton(fontSize, blackTransparent);

        if (high_score_b) {
            DrawRectangleRounded((Rectangle){x, y, boxWidth, boxHeight}, 0.3f, 10, Fade(BLACK, 0.75f));

            // Close Button Button Draw
            DrawRectangleRec(closeBtn, RED);
            DrawText("Quit", closeBtn.x + 6, closeBtn.y + 2, 20, WHITE);

            // Load and draw scores
            vector<pair<string, int>> scores;
            ifstream file("scores.txt");
            string name; int score;

            while (file >> name >> score)
                scores.emplace_back(name, score);

            int yp = y + 80;
            for (auto& it : scores) {
                string toShow = it.first + "    " + to_string(it.second);
                if (it.second >= 1000) toShow += "  (DU Conqured)";
                else if (it.second == 0) toShow += "  (Failed)";

                Color col = (it.second >= 1000) ? GREEN : (it.second == 0 ? WHITE : ORANGE);
                DrawText(toShow.c_str(), x + 20, yp, 20, col);
                yp += 30;
            }

            DrawText(clickedMessage, x + 20, y + 40, 20, RED);
        }

        EndDrawing();
    }

    UnloadTexture(main_menu);
    CloseWindow();

    return 0;
}