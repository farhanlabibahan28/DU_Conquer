#include "raylib.h"
#include <math.h>
#include <stdio.h>

// ---------- STRUCT ----------
typedef struct Zone {
    Rectangle rect;
    const char* name;
    Color color;
} Zone;

// ---------- GLOBAL VARIABLES ----------
Camera2D camera;
Texture2D background;
Texture2D character;
Vector2 character_pos = {4523, 2873};
float speed = 3.0f;
Zone zones[3];
int zoneCount;
float theta, cosTheta, sinTheta;
const char* currentZone;
int screenWidth, screenHeight;

// ---------- INIT FUNCTION ----------
void init_map() {
    
    InitWindow(screenWidth, screenHeight, "Map View");
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    ToggleFullscreen();
    SetTargetFPS(60);

    Image img = LoadImage("map2.png");
    character = LoadTexture("character.png");
    background = LoadTextureFromImage(img);
    UnloadImage(img);

    camera.target = character_pos;
    camera.offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
    camera.zoom = 1.0f;

    zones[0] = (Zone){ {100, 100, 200, 150}, "Lava Zone", RED };
    zones[1] = (Zone){ {400, 300, 180, 120}, "Safe Zone", GREEN };
    zones[2] = (Zone){ {700, 500, 150, 100}, "Trigger Zone", ORANGE };
    zoneCount = 3;

    theta = 26.2f * DEG2RAD;
    cosTheta = cosf(theta);
    sinTheta = sinf(theta);
}

// ---------- MAIN LOGIC FUNCTION ----------
void logic_draw_map() {
    while (!WindowShouldClose()) {
        // --- MOVEMENT INPUT ---
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

        // --- ZONE DETECTION ---
        currentZone = "None";
        for (int i = 0; i < zoneCount; i++) {
            if (CheckCollisionPointRec(character_pos, zones[i].rect)) {
                currentZone = zones[i].name;
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(background, 0, 0, WHITE);
        DrawRectangle(0, 0, background.width, background.height, Fade(BLACK, 0.3f));

        // Draw zones
        for (int i = 0; i < zoneCount; i++) {
            DrawRectangleRec(zones[i].rect, Fade(zones[i].color, 0.3f));
            DrawRectangleLinesEx(zones[i].rect, 2, zones[i].color);
            DrawText(zones[i].name, zones[i].rect.x + 4, zones[i].rect.y + 4, 10, zones[i].color);
        }

        // Glow Effect
        float scale = 0.25f;
        int layers = 20;
        float baseRadius = (character.width * scale) * 0.11f;

        for (int i = 0; i < layers; i++) {
            float radius = baseRadius * (1.0f + i * 0.15f);
            float alpha = 0.25f * (1.0f - (float)i / layers);
            DrawCircleV(character_pos, radius, Fade((Color){255, 200, 90, 255}, alpha));
        }

        // Draw the character
        DrawTextureEx(character,
            (Vector2){ character_pos.x - (character.width * scale) / 2, character_pos.y - (character.height * scale) / 2 },
            0.0f,
            scale,
            WHITE);

        EndMode2D();

        // UI
        DrawText(TextFormat("Current Zone: %s", currentZone), 10, 10, 20, YELLOW);
        DrawText(TextFormat("X: %f Y: %f", character_pos.x, character_pos.y), 10, 70, 30, RED);
        DrawFPS(10, 40);
        EndDrawing();
    }
}

// ---------- UNLOAD FUNCTION ----------
void unload_map() {
    UnloadTexture(background);
    UnloadTexture(character);
    CloseWindow();
}

int main() {
    init_map();
    logic_draw_map();
    unload_map();
    return 0;
}