#include "raylib.h"
#include <stdio.h>
#include <math.h>

typedef struct Zone {
    Rectangle rect;
    const char* name;
    Color color;
} Zone;

Vector2 character_pos = {4523, 2873};
float speed = 4.0f;

int main() {
    InitWindow(1200, 800, "Image with Camera and Zones");
    SetTargetFPS(60);

    // Load the background image
    Image img = LoadImage("map2.png"); // Replace with your image
    Texture2D character = LoadTexture("character.png");
    Texture2D background = LoadTextureFromImage(img);
    UnloadImage(img); // Free original image

    // Define camera
    Camera2D camera = { 0 };
    camera.target = character_pos;
    camera.offset = (Vector2){ GetScreenWidth()/2, GetScreenHeight()/2 };
    camera.zoom = 1.0f;

    // Define zones
    Zone zones[] = {
        {{100, 100, 200, 150}, "Lava Zone", RED},
        {{400, 300, 180, 120}, "Safe Zone", GREEN},
        {{700, 500, 150, 100}, "Trigger Zone", ORANGE}
    };
    int zoneCount = sizeof(zones) / sizeof(zones[0]);

    float theta = 26.2f * DEG2RAD;
    float cosTheta = cosf(theta);  
    float sinTheta = sinf(theta);  

    while (!WindowShouldClose()) {
        // --- MOVEMENT INPUT ---
        Vector2 move = {0};

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
        const char* currentZone = "None";
        for (int i = 0; i < zoneCount; i++) {
            if (CheckCollisionPointRec(character_pos, zones[i].rect)) {
                currentZone = zones[i].name;
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(background, 0, 0, WHITE); // Draw map
        DrawRectangle(0, 0, background.width, background.height, Fade(BLACK, 0.3f));


        // Draw zones
        for (int i = 0; i < zoneCount; i++) {
            DrawRectangleRec(zones[i].rect, Fade(zones[i].color, 0.3f));
            DrawRectangleLinesEx(zones[i].rect, 2, zones[i].color);
            DrawText(zones[i].name, zones[i].rect.x + 4, zones[i].rect.y + 4, 10, zones[i].color);
        }

        float scale = 0.25f;

        // --- GLOW EFFECT ---
        // Draw lamp-style fading glow
        // Smooth, subtle lamp-style glow
        int layers = 20;
        float baseRadius = (character.width * scale) * 0.11f;

        for (int i = 0; i < layers; i++) {
            float radius = baseRadius * (1.0f + i * 0.15f);
            float alpha = 0.25f * (1.0f - (float)i / layers); // Stronger inside, fades out
            DrawCircleV(character_pos, radius, Fade((Color){255, 200, 90, 255}, alpha));

        }


        // Draw the character on top of the glow
        DrawTextureEx(character,
            (Vector2){ character_pos.x - (character.width * scale)/2, character_pos.y - (character.height * scale)/2 },
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

    UnloadTexture(background);
    UnloadTexture(character);
    CloseWindow();
    return 0;
}
