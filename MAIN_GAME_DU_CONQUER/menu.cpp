#include "raylib.h"
#include "global.h"
#include<bits/stdc++.h>
using namespace std;

Color red = {255, 0, 0, 255};
Color blackTransparent = {0, 0, 0, 128};
Vector2 cloudPos = {0,0};

const int MAX_HOVER_SOUNDS = 4;
Sound hoverSounds[MAX_HOVER_SOUNDS];
int currentHoverSound = 0;

void InitHoverSounds() {
    Sound baseSound = LoadSound("resources/click.mp3");
    for (int i = 0; i < MAX_HOVER_SOUNDS; i++) {
        hoverSounds[i] = baseSound;
    }
}

void PlayHoverSound() {
    PlaySound(hoverSounds[currentHoverSound]);
    currentHoverSound = (currentHoverSound + 1) % MAX_HOVER_SOUNDS;
}

Music bgmMenu;
bool wasHoveredNew = false, wasHoveredHigh = false, wasHoveredHelp = false, wasHoveredMap = false, wasHoveredSaved = false, wasHoveredExit = false;

// Declare all globals here, above all functions
Texture2D main_menu;
Texture2D cloud;
int buttonWidth, buttonHeight, spacing, startY, startX;

bool showMap = false;
bool showHelp = false;
Texture2D mapTexture;
string helpText = "Use arrow keys to move.\nPress SPACE to shoot.\nCollect power-ups and avoid enemies.";

class Button {
public:
    Rectangle bounds;
    const char* label;
    Color textColorNormal;
    Color textColorHover;

    Button() = default;  // default constructor

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

Button btnNew, btnHigh, btnHelp, btnMap, btnSaved, btnExit;

const char* clickedMessage;
bool high_score_b;

void init_menu() {
    InitAudioDevice();
    InitHoverSounds();
    bgmMenu = LoadMusicStream("resources/bgm_menu.mp3");
    SetMusicVolume(bgmMenu, 0.2f);
    PlayMusicStream(bgmMenu);

    main_menu = LoadTexture("resources/main_menu.png");
    cloud = LoadTexture("resources/cloud.png");
    scale = (double) screenHeight / main_menu.height;

    buttonWidth = 250 * scale;
    buttonHeight = 40 * scale;
    spacing = 40 * scale;
    startY = screenHeight - 150;
    startX = screenWidth - 330;

    btnNew = Button(startX, startY - spacing * 6, buttonWidth, buttonHeight, "New Game");
    btnHigh = Button(startX, startY - spacing * 5, buttonWidth, buttonHeight, "High Score");
    btnHelp = Button(startX, startY - spacing * 4, buttonWidth, buttonHeight, "Help");
    btnMap = Button(startX, startY - spacing * 3, buttonWidth, buttonHeight, "Map");
    btnSaved = Button(startX, startY - spacing * 2, buttonWidth, buttonHeight, "Saved Games");
    btnExit = Button(startX, startY - spacing * 1, buttonWidth, buttonHeight, "Exit");

    clickedMessage = nullptr;
    high_score_b = false;

    mapTexture = LoadTexture("resources/map.png");

    SetTargetFPS(60);
}

void logic_draw_menu() {
    
    // while (true) {
        if (clickedMessage && strcmp(clickedMessage, "New Game clicked!") == 0) 
        {
            state_of_game = LAYER_MAP;
            // return;
        }

        
        int boxWidth = (int)(500 * scale);
        int boxHeight = (int)(300 * scale);
        float x = (float)(screenWidth / 2 - boxWidth / 2);
        float y = (float)(screenHeight - (int)(350 * scale));

        Rectangle closeBtn = { x + boxWidth - 35.0f, y + 10.0f, 25.0f, 25.0f };

        // Mouse Clicks
        if (btnNew.IsHovered() && !wasHoveredNew) PlayHoverSound();
        wasHoveredNew = btnNew.IsHovered();
        if (btnNew.IsClicked()) { PlaySound(hoverSounds[0]); clickedMessage = "New Game clicked!"; }

        if (btnHigh.IsHovered() && !wasHoveredHigh) PlayHoverSound();
        wasHoveredHigh = btnHigh.IsHovered();
        if (btnHigh.IsClicked()) { PlaySound(hoverSounds[0]); clickedMessage = "High Scores"; high_score_b = true; }

        if (btnHelp.IsHovered() && !wasHoveredHelp) PlayHoverSound();
        wasHoveredHelp = btnHelp.IsHovered();
        if (btnHelp.IsClicked()) { PlaySound(hoverSounds[0]); showHelp = true; }

        if (btnMap.IsHovered() && !wasHoveredMap) PlayHoverSound();
        wasHoveredMap = btnMap.IsHovered();
        if (btnMap.IsClicked()) { PlaySound(hoverSounds[0]); showMap = true; }

        if (btnSaved.IsHovered() && !wasHoveredSaved) PlayHoverSound();
        wasHoveredSaved = btnSaved.IsHovered();
        if (btnSaved.IsClicked()) { PlaySound(hoverSounds[0]); clickedMessage = "Saved Games clicked!"; }

        if (btnExit.IsHovered() && !wasHoveredExit) PlayHoverSound();
        wasHoveredExit = btnExit.IsHovered();
        if (btnExit.IsClicked()) { 
            PlaySound(hoverSounds[0]); 
            state_of_game = LAYER_EXIT;
            // return; 
        }

        // Box Close
        if (high_score_b &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), closeBtn)) {
            PlayHoverSound();
            high_score_b = false;
            clickedMessage = nullptr;
        }

        cloudPos.y = -120 * scale;
        // Cloud
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
        // BeginDrawing();
        // ClearBackground(BLACK);

        // Static variables for background oscillation and rotation
        static float bgOffsetX = 0;
        static float bgRotation = 0;
        static bool bgMovingRight = true;
        float bgMaxOffset = 10 * scale;
        float bgMoveSpeed = 7.5f * GetFrameTime();
        float bgRotationSpeed = 0.45f * GetFrameTime();

        if (bgMovingRight) {
            bgOffsetX += bgMoveSpeed;
            bgRotation += bgRotationSpeed;
            if (bgOffsetX >= bgMaxOffset) bgMovingRight = false;
        } else {
            bgOffsetX -= bgMoveSpeed;
            bgRotation -= bgRotationSpeed;
            if (bgOffsetX <= -bgMaxOffset) bgMovingRight = true;
        }

        DrawTexturePro(
            main_menu,
            (Rectangle){0, 0, (float)main_menu.width, (float)main_menu.height},
            (Rectangle){bgOffsetX, 0, (float)GetMonitorWidth(0), (float)GetMonitorHeight(0)},
            (Vector2){0, 0},
            bgRotation,
            WHITE
        );

        Color fadedWhite = {255, 255, 255, 80}; // semi-transparent white
        DrawTextureEx(cloud, cloudPos, 0.0f, 1.1f * scale, MAGENTA);

        DrawText("DU Conquer", startX - 15, startY - spacing * 8, (int)(55 * scale), MAGENTA);

        int fontSize = (int)(27 * scale);

        btnNew.DrawButton(fontSize, BLANK);
        btnHigh.DrawButton(fontSize, BLANK);
        btnHelp.DrawButton(fontSize, BLANK);
        btnMap.DrawButton(fontSize, BLANK);
        btnSaved.DrawButton(fontSize, BLANK);
        btnExit.DrawButton(fontSize, BLANK);

        if (high_score_b) {
            DrawRectangleRounded((Rectangle){x, y, (float)boxWidth, (float)boxHeight}, 0.3f, 10, Fade(BLACK, 0.75f));

            // Close Button Button Draw
            DrawRectangleRec(closeBtn, RED);
            DrawText("X", (int)(closeBtn.x + 8), (int)(closeBtn.y + 5), 20, WHITE);

            // Load and draw scores
            vector<pair<string, int>> scores;
            ifstream file("scores.txt");
            string name; 
            int score;

            while (file >> name >> score)
                scores.emplace_back(name, score);

            DrawText("High Scores", (int)(x + 20), (int)(y + 20), (int)(28 * scale), MAGENTA);

            int yp = (int)(y + 80);
            for (auto& it : scores) {
                string toShow = it.first + "    " + to_string(it.second);
                Color score_color = WHITE;
                if (it.second >= 1000) { toShow += "  (DU Conqured)"; score_color = GREEN;}
                else if (it.second == 0) {toShow += "  (Failed)"; score_color = RED;}

                DrawText(toShow.c_str(), (int)(x + 20), yp, (int)(24 * scale), score_color);
                yp += 30;
            }
        }

        if (showMap) {
            DrawRectangle(0, 0, GetMonitorWidth(0), GetMonitorHeight(0), Fade(BLACK, 0.8f));
            DrawText("Dhaka University Map", 40, 40, (int)(40 * scale), WHITE);

            // Static variables for oscillation and rotation
            static float mapOffsetX = 0;
            static float mapRotation = 0;
            static bool movingRight = true;
            float maxOffset = 25 * scale;
            float moveSpeed = 25 * GetFrameTime();
            float rotationSpeed = 0.5f * GetFrameTime();

            if (movingRight) {
                mapOffsetX += moveSpeed;
                mapRotation += rotationSpeed;
                if (mapOffsetX >= maxOffset) movingRight = false;
            } else {
                mapOffsetX -= moveSpeed;
                mapRotation -= rotationSpeed;
                if (mapOffsetX <= -maxOffset) movingRight = true;
            }

            DrawTexturePro(mapTexture,
                           (Rectangle){0, 0, (float)mapTexture.width, (float)mapTexture.height},
                           (Rectangle){mapOffsetX, 0, (float)GetMonitorWidth(0), (float)GetMonitorHeight(0)},
                           (Vector2){0, 0},
                           mapRotation,
                           WHITE);

            Rectangle closeBtn = { GetMonitorWidth(0) - 45.0f, 15.0f, 30.0f, 30.0f };
            DrawRectangleRec(closeBtn, RED);
            DrawText("X", (int)(closeBtn.x + 8), (int)(closeBtn.y + 5), 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), closeBtn)) {
                PlayHoverSound();
                showMap = false;
            }
        }

        if (showHelp) {
            int helpBoxWidth = (int)(600 * scale);
            int helpBoxHeight = (int)(300 * scale);
            int helpBoxX = (GetMonitorWidth(0) - helpBoxWidth) / 2;
            int helpBoxY = (GetMonitorHeight(0) - helpBoxHeight) / 2;
            DrawRectangle(helpBoxX, helpBoxY, helpBoxWidth, helpBoxHeight, Fade(BLACK, 0.8f));
            DrawText(helpText.c_str(), helpBoxX + 20, helpBoxY + 20, (int)(22 * scale), RAYWHITE);

            Rectangle closeBtn = { helpBoxX + helpBoxWidth - 35.0f, helpBoxY + 5.0f, 30.0f, 30.0f };
            DrawRectangleRec(closeBtn, RED);
            DrawText("X", (int)(closeBtn.x + 8), (int)(closeBtn.y + 5), 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), closeBtn)) {
                PlayHoverSound();
                showHelp = false;
            }
        }

        UpdateMusicStream(bgmMenu);
        // EndDrawing();
    }
// }

// void unload_menu() {
//     UnloadTexture(main_menu);
//     UnloadTexture(mapTexture);
//     UnloadTexture(cloud);
//     for (int i = 0; i < MAX_HOVER_SOUNDS; i++) {
//         UnloadSound(hoverSounds[i]);
//     }
//     UnloadMusicStream(bgmMenu);
//     CloseAudioDevice();
// }

void unload_menu() {
    if (main_menu.id != 0) {
        UnloadTexture(main_menu);
        main_menu.id = 0;
    }

    if (cloud.id != 0) {
        UnloadTexture(cloud);
        cloud.id = 0;
    }

    if (mapTexture.id != 0) {
        UnloadTexture(mapTexture);
        mapTexture.id = 0;
    }

    for (int i = 0; i < MAX_HOVER_SOUNDS; i++) {
        if (hoverSounds[i].frameCount > 0) {
            UnloadSound(hoverSounds[i]);
            hoverSounds[i].frameCount = 0;
        }
    }

    if (bgmMenu.frameCount > 0) {
        UnloadMusicStream(bgmMenu);
        bgmMenu.frameCount = 0;
    }

    CloseAudioDevice();
}



void MAIN_MENU() {
    init_menu();
    logic_draw_menu();
    unload_menu();
}