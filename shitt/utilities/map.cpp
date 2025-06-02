#include "map.h"
#include <cmath>

typedef struct Zone {
    Rectangle rect;
    const char* name;
    Color color;
} Zone;

static const float speed = 4.0f;
static const float theta = 26.2f * DEG2RAD;
static const float cosTheta = cosf(theta);
static const float sinTheta = sinf(theta);

// Define zones statically
static Zone zones[] = {
    {{100, 100, 200, 150}, "Lava Zone", RED},
    {{400, 300, 180, 120}, "Safe Zone", GREEN},
    {{700, 500, 150, 100}, "Trigger Zone", ORANGE}
};
static const int zoneCount = sizeof(zones) / sizeof(zones[0]);
void UpdateMap(GameScreen& currentScreen, Texture2D& mapTexture, Texture2D& characterTex, Vector2& character_pos, Button& backButton)
 {
    Camera2D camera = { 0 };
    camera.target = character_pos;
    camera.offset = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() / 2 };
    camera.zoom = 1.0f;

    // Movement
    if (IsKeyDown(KEY_W)) {
        character_pos.x -= cosTheta * speed;
        character_pos.y -= sinTheta * speed;
    }
    if (IsKeyDown(KEY_S)) {
        character_pos.x += cosTheta * speed;
        character_pos.y += sinTheta * speed;
    }
    if (IsKeyDown(KEY_A)) {
        character_pos.x -= cosTheta * speed;
        character_pos.y += sinTheta * speed;
    }
    if (IsKeyDown(KEY_D)) {
        character_pos.x += cosTheta * speed;
        character_pos.y -= sinTheta * speed;
    }

    camera.target = character_pos;

    // Zone detection
    const char* currentZone = "None";
    for (int i = 0; i < zoneCount; i++) {
        if (CheckCollisionPointRec(character_pos, zones[i].rect)) {
            currentZone = zones[i].name;
        }
    }

    // --- DRAWING ---
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    float imgW = (float)mapTexture.width;
    float imgH = (float)mapTexture.height;
    float scaleBg = fminf(screenW / imgW, screenH / imgH);
    float destW = imgW * scaleBg;
    float destH = imgH * scaleBg;
    float offsetX = (screenW - destW) / 2;
    float offsetY = (screenH - destH) / 2;

    BeginMode2D(camera);
    DrawTexturePro(mapTexture,
        (Rectangle){ 0, 0, imgW, imgH },
        (Rectangle){ 0, 0, imgW, imgH },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    for (int i = 0; i < zoneCount; i++) {
        DrawRectangleRec(zones[i].rect, Fade(zones[i].color, 0.3f));
        DrawRectangleLinesEx(zones[i].rect, 2, zones[i].color);
        DrawText(zones[i].name, zones[i].rect.x + 4, zones[i].rect.y + 4, 10, zones[i].color);
    }

    float scale = 0.25f;
    DrawTextureEx(characterTex,
        (Vector2){
            character_pos.x - (characterTex.width * scale) / 2,
            character_pos.y - (characterTex.height * scale) / 2
        },
        0.0f, scale, WHITE);

    EndMode2D();

    DrawText(TextFormat("Current Zone: %s", currentZone), 10, 10, 20, YELLOW);
    DrawText(TextFormat("X: %f Y: %f", character_pos.x, character_pos.y), 10, 70, 30, RED);
    DrawFPS(10, 40);
    backButton.DrawButton(24, (Color){0, 0, 0, 128});
    if (backButton.IsClicked()) {
        currentScreen = SCREEN_MENU;
    }

}
