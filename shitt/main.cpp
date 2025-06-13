#include "raylib.h"
#include "enum.h"
#include "button.h"
#include "menu.h"
#include "utilities/map.h"

GameScreen currentScreen = SCREEN_MENU;


int main() {
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "DU_Conquer");
    ToggleFullscreen();
    SetTargetFPS(60);
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Texture2D menuBg = LoadTexture("resources/main_menu.png");
    Texture2D mapBg = LoadTexture("resources/map2.png");
    Vector2 character_pos = {4523, 2873};
    Texture2D characterTex = LoadTexture("resources/character.png");
    // In your main.cpp or where you initialize:
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    Shader nightShader = LoadShader(0, "utilities/night.fs");

    Button buttons[] = {
    Button(screenW * 0.79f, screenH * 0.5f, 250, 40, "New Game"),
    Button(screenW * 0.79f, screenH * 0.5f + 50, 250, 40, "High Score"),
    Button(screenW * 0.79f, screenH * 0.5f + 100, 250, 40, "Help"),
    Button(screenW * 0.79f, screenH * 0.5f + 150, 250, 40, "Map"),
    Button(screenW * 0.79f, screenH * 0.5f + 200, 250, 40, "Saved Games"),
    Button(screenW * 0.79f, screenH * 0.5f + 250, 250, 40, "Exit")
};


    Button backButton(20, 20, 100, 40, "Menu");  // x = 20, y = 20 for top-left corner


    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
            case SCREEN_MENU:
                UpdateMenu(currentScreen, menuBg, buttons, 6);
                break;
            case SCREEN_MAP:
                UpdateMap(currentScreen, mapBg, characterTex, character_pos, backButton, target, nightShader);

                break;
        }

        EndDrawing();
    }

    UnloadTexture(menuBg);
    UnloadTexture(characterTex);
    UnloadTexture(mapBg);
    CloseWindow();
    return 0;
}
