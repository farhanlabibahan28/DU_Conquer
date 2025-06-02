#include "raylib.h"
#include <stdio.h>

typedef struct Zone {
    Rectangle rect;
    const char* name;
    Color color;
} Zone;

int main() {
    InitWindow(1200, 800, "Image with Camera and Zones");
    SetTargetFPS(60);

    // Load the background image
    Image img = LoadImage("map.png"); // Replace with your image
    Texture2D background = LoadTextureFromImage(img);
    UnloadImage(img); // No longer needed

    // Define camera
    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ GetScreenWidth()/2, GetScreenHeight()/2 };
    camera.zoom = 1.0f;

    // Define player position (in world space)
    Vector2 playerPos = { 200, 200 };
    float playerRadius = 10;

    // Define zones
    Zone zones[] = {
        {{100, 100, 200, 150}, "Lava Zone", RED},
        {{400, 300, 180, 120}, "Safe Zone", GREEN},
        {{700, 500, 150, 100}, "Trigger Zone", ORANGE}
    };
    int zoneCount = sizeof(zones) / sizeof(zones[0]);

    while (!WindowShouldClose()) {
        // --- CAMERA CONTROLS ---
        if (IsKeyDown(KEY_RIGHT)) camera.target.x += 10;
        if (IsKeyDown(KEY_LEFT))  camera.target.x -= 10;
        if (IsKeyDown(KEY_DOWN))  camera.target.y += 10;
        if (IsKeyDown(KEY_UP))    camera.target.y -= 10;

        camera.zoom += GetMouseWheelMove() * 0.1f;
        if (camera.zoom < 0.2f) camera.zoom = 0.2f;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;

        // --- PLAYER CONTROLS (in world space) ---
        if (IsKeyDown(KEY_W)) playerPos.y -= 2;
        if (IsKeyDown(KEY_S)) playerPos.y += 2;
        if (IsKeyDown(KEY_A)) playerPos.x -= 2;
        if (IsKeyDown(KEY_D)) playerPos.x += 2;

        // --- ZONE DETECTION ---
        const char* currentZone = "None";
        for (int i = 0; i < zoneCount; i++) {
            if (CheckCollisionPointRec(playerPos, zones[i].rect)) {
                currentZone = zones[i].name;
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(background, 0, 0, WHITE); // Draw image at (0,0)

        // Draw zones
        for (int i = 0; i < zoneCount; i++) {
            DrawRectangleRec(zones[i].rect, Fade(zones[i].color, 0.3f));
            DrawRectangleLinesEx(zones[i].rect, 2, zones[i].color);
            DrawText(zones[i].name, zones[i].rect.x + 4, zones[i].rect.y + 4, 10, zones[i].color);
        }

        // Draw player
        DrawCircleV(playerPos, playerRadius, BLUE);

        EndMode2D();

        DrawText(TextFormat("Current Zone: %s", currentZone), 10, 10, 20, YELLOW);
        DrawFPS(10, 40);
        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();
    return 0;
}
