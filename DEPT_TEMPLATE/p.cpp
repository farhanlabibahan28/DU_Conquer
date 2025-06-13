#include "raylib.h"       // <-- REQUIRED for Texture2D, Vector2, etc.
#include <string>         // Optional, if you're using strings
using namespace std;      // Optional

int main() {
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Raylib Example");
    
    Texture2D texture = LoadTexture("character.png");
    Vector2 pos = {10, GetScreenHeight() - texture.height - 10};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(texture, pos.x, pos.y, WHITE);

        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
