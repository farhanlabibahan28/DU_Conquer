#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

Color red = {255, 0, 0, 255};
Color blackTransparent = {0, 0, 0, 128};
Vector2 cloudPos = {0,0};

Sound clickSound;
Music bgmMenu;
bool wasHoveredNew = false, wasHoveredHigh = false, wasHoveredHelp = false, wasHoveredMap = false, wasHoveredSaved = false, wasHoveredExit = false;

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
          textColorHover(MAGENTA) {}

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
    

    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "DU_Conquer_Menu");
    ToggleFullscreen();
    InitAudioDevice();
    clickSound = LoadSound("resources/click.mp3");
    bgmMenu = LoadMusicStream("resources/bgm_menu.mp3");
    SetMusicVolume(bgmMenu, 0.2f);
    PlayMusicStream(bgmMenu);

    const int screenWidth = GetMonitorWidth(0);
    const int screenHeight = GetMonitorHeight(0);

    Texture2D main_menu = LoadTexture("resources/main_menu.png");
    Texture2D cloud = LoadTexture("resources/cloud.png");
    float scale = (float)GetMonitorHeight(0) / main_menu.height;

    int buttonWidth = 250 * scale;
    int buttonHeight = 40 * scale;
    int spacing = 40 * scale;
    int startY = GetMonitorHeight(0)-150;
    int startX = GetMonitorWidth(0)-330;
    

    Button btnNew(startX, startY - spacing * 6, buttonWidth, buttonHeight, "New Game");
    Button btnHigh(startX, startY - spacing * 5, buttonWidth, buttonHeight, "High Score");
    Button btnHelp(startX, startY - spacing * 4, buttonWidth, buttonHeight, "Help");
    Button btnMap(startX, startY - spacing * 3, buttonWidth, buttonHeight, "Map");
    Button btnSaved(startX, startY - spacing * 2, buttonWidth, buttonHeight, "Saved Games");
    Button btnExit(startX, startY - spacing * 1, buttonWidth, buttonHeight, "Exit");

    const char* clickedMessage = nullptr;
    bool high_score_b = false;

    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        // Dialog Box setup
        int boxWidth = 500*scale;
        int boxHeight = 300*scale;
        int x = screenWidth / 2 - boxWidth / 2;
        int y = screenHeight - 350*scale ;
        Rectangle closeBtn = { x + boxWidth - 35, y + 10, 25, 25 };

        // Mouse Clicks
        if (btnNew.IsHovered() && !wasHoveredNew) PlaySound(clickSound);
        wasHoveredNew = btnNew.IsHovered();
        if (btnNew.IsClicked()) { PlaySound(clickSound); clickedMessage = "New Game clicked!"; }

        if (btnHigh.IsHovered() && !wasHoveredHigh) PlaySound(clickSound);
        wasHoveredHigh = btnHigh.IsHovered();
        if (btnHigh.IsClicked()) { PlaySound(clickSound); clickedMessage = "High Scores"; high_score_b = true; }

        if (btnHelp.IsHovered() && !wasHoveredHelp) PlaySound(clickSound);
        wasHoveredHelp = btnHelp.IsHovered();
        if (btnHelp.IsClicked()) { PlaySound(clickSound); clickedMessage = "Help clicked!"; }

        if (btnMap.IsHovered() && !wasHoveredMap) PlaySound(clickSound);
        wasHoveredMap = btnMap.IsHovered();
        if (btnMap.IsClicked()) { PlaySound(clickSound); clickedMessage = "Map clicked!"; }

        if (btnSaved.IsHovered() && !wasHoveredSaved) PlaySound(clickSound);
        wasHoveredSaved = btnSaved.IsHovered();
        if (btnSaved.IsClicked()) { PlaySound(clickSound); clickedMessage = "Saved Games clicked!"; }

        if (btnExit.IsHovered() && !wasHoveredExit) PlaySound(clickSound);
        wasHoveredExit = btnExit.IsHovered();
        if (btnExit.IsClicked()) { PlaySound(clickSound); CloseWindow(); return 0; }

        // Box Close
        if (high_score_b &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), closeBtn)) {
            high_score_b = false;
            clickedMessage = nullptr;
        }

        cloudPos.y = -120*scale;
        // Bidirectional cloud oscillation
        static float cloudSpeed = 20.0f;
        static bool movingRight = true;
        if (movingRight) {
            cloudPos.x += cloudSpeed * GetFrameTime();
            if (cloudPos.x >= GetMonitorWidth(0) - cloud.width * scale)
                movingRight = false;
        } else {
            cloudPos.x -= cloudSpeed * GetFrameTime();
            if (cloudPos.x <= 0)
                movingRight = true;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
        main_menu,
        (Rectangle){0, 0, (float)main_menu.width, (float)main_menu.height},
        (Rectangle){0, 0, (float)GetMonitorWidth(0), (float)GetMonitorHeight(0)},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

        DrawTextureEx(cloud, cloudPos, 3.0f, 1.1 *scale, MAGENTA);
        DrawText("DU Conquer", startX-15, startY - spacing * 8, 55 * scale, MAGENTA);

        int fontSize = 27 * scale;

        btnNew.DrawButton(fontSize, BLANK);
        btnHigh.DrawButton(fontSize, BLANK);
        btnHelp.DrawButton(fontSize, BLANK);
        btnMap.DrawButton(fontSize, BLANK);
        btnSaved.DrawButton(fontSize, BLANK);
        btnExit.DrawButton(fontSize, BLANK);

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

            DrawText("High Scores", x + 20, y + 20, 28 * scale, MAGENTA);

            int yp = y + 80;
            for (auto& it : scores) {
                string toShow = it.first + "    " + to_string(it.second);
                Color score_color = WHITE;
                if (it.second >= 1000) { toShow += "  (DU Conqured)"; score_color = GREEN;}
                else if (it.second == 0) {toShow += "  (Failed)"; score_color = RED;}

                DrawText(toShow.c_str(), x + 20, yp, 24 * scale, score_color);
                yp += 30;
            }
        }

        UpdateMusicStream(bgmMenu);
        EndDrawing();
        // cout<<"Screen Width: "<<screenWidth<<endl;
    }

    UnloadTexture(main_menu);
    UnloadSound(clickSound);
    UnloadMusicStream(bgmMenu);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}