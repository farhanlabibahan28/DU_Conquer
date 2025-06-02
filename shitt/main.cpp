#include "raylib.h"
#include "enum.h"
#include "button.h"
#include "menu.h"
#include "map.h"

GameScreen currentScreen = SCREEN_MENU;


int main() {
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "DU_Conquer");
    ToggleFullscreen();
    SetTargetFPS(60);

    Texture2D menuBg = LoadTexture("resources/main_menu.png");
    Texture2D mapBg = LoadTexture("resources/map.png");
    Vector2 character_pos = {4523, 2873};
    Texture2D characterTex = LoadTexture("resources/character.png");

    Button buttons[] = {
        Button(40, 420, 250, 40, "New Game"),
        Button(40, 460, 250, 40, "High Score"),
        Button(40, 500, 250, 40, "Help"),
        Button(40, 540, 250, 40, "Map"),
        Button(40, 580, 250, 40, "Saved Games"),
        Button(40, 620, 250, 40, "Exit")
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
                UpdateMap(currentScreen, mapBg, characterTex, character_pos, backButton);

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
