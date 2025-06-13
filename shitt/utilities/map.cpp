#include<map.h>
#include<cmath>
#include "map.h"
#include <cmath>

// Define constants here
static const float speed = 4.0f;
static const float theta = 26.2f * DEG2RAD;
static const float cosTheta = cosf(theta);
static const float sinTheta = sinf(theta);

// Define zones statically
typedef struct Zone {
    Rectangle rect;
    const char* name;
    Color color;
} Zone;

static Zone zones[] = {
    {{100, 100, 200, 150}, "Lava Zone", RED},
    {{400, 300, 180, 120}, "Safe Zone", GREEN},
    {{700, 500, 150, 100}, "Trigger Zone", ORANGE}
};

static const int zoneCount = sizeof(zones) / sizeof(zones[0]);

void UpdateMap(GameScreen& currentScreen, Texture2D& mapTexture, Texture2D& characterTex,
               Vector2& character_pos, Button& backButton,
               RenderTexture2D& target, Shader& nightshader)
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

    const char* currentZone = "None";
    for (int i = 0; i < zoneCount; i++) {
        if (CheckCollisionPointRec(character_pos, zones[i].rect)) {
            currentZone = zones[i].name;
        }
    }

    float imgW = (float)mapTexture.width;
    float imgH = (float)mapTexture.height;
    float scale = 0.25f; // for character

    // Render to texture
    BeginTextureMode(target);
    ClearBackground(BLANK);
    BeginMode2D(camera);

    // Draw map
    DrawTexturePro(mapTexture,
        (Rectangle){ 0, 0, imgW, imgH },
        (Rectangle){ 0, 0, imgW, imgH },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    // Zones
    for (int i = 0; i < zoneCount; i++) {
        DrawRectangleRec(zones[i].rect, Fade(zones[i].color, 0.3f));
        DrawRectangleLinesEx(zones[i].rect, 2, zones[i].color);
        DrawText(zones[i].name, zones[i].rect.x + 4, zones[i].rect.y + 4, 10, zones[i].color);
    }

    // Ghost glow: multiple fading outlines
    for (int i = 4; i >= 1; i--) {
        float glowScale = scale + i * 0.01f;
        float alpha = 0.15f - i * 0.025f;
        DrawTextureEx(characterTex,
            (Vector2){
                character_pos.x - (characterTex.width * glowScale) / 2,
                character_pos.y - (characterTex.height * glowScale) / 2
            },
            0.0f, glowScale, (Color){160, 100, 255, (unsigned char)(alpha * 255)});
    }

    // Main character
    DrawTextureEx(characterTex,
        (Vector2){
            character_pos.x - (characterTex.width * scale) / 2,
            character_pos.y - (characterTex.height * scale) / 2
        },
        0.0f, scale, WHITE);

    EndMode2D();
    EndTextureMode();
    // Update shader uniforms
    Vector2 screenSize = { (float)GetScreenWidth(), (float)GetScreenHeight() };
    SetShaderValue(nightshader, GetShaderLocation(nightshader, "resolution"), &screenSize, SHADER_UNIFORM_VEC2);

    // Convert character_pos (world) to screen space using camera
    Vector2 ghostScreenPos = GetWorldToScreen2D(character_pos, camera);

    // Normalize to [0, 1] for UV-space
    Vector2 lightPosNorm = {
        ghostScreenPos.x / screenSize.x,
        ghostScreenPos.y / screenSize.y
    };
    SetShaderValue(nightshader, GetShaderLocation(nightshader, "lightPos"), &lightPosNorm, SHADER_UNIFORM_VEC2);

    // Draw the texture with night shader
    BeginShaderMode(nightshader);
    DrawTextureRec(target.texture,
        (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height },
        (Vector2){ 0, 0 },
         WHITE);
    EndShaderMode();

    // HUD
    DrawText(TextFormat("Current Zone: %s", currentZone), 10, 10, 20, YELLOW);
    DrawText(TextFormat("X: %f Y: %f", character_pos.x, character_pos.y), 10, 70, 30, RED);
    DrawFPS(10, 40);

    // Back button
    backButton.DrawButton(24, (Color){0, 0, 0, 128});
    if (backButton.IsClicked()) {
        currentScreen = SCREEN_MENU;
    }
}
