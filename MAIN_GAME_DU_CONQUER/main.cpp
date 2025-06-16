#include "raylib.h"
#include "global.h"
#include "menu.h"
#include "map.h"
#include <iostream>

using namespace std;

int main() {
    

    InitWindow(GetScreenWidth(), GetScreenHeight(), "DU_Conquer");
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    ToggleFullscreen();
    SetTargetFPS(144);

    init_map();
    init_menu();

    cout<<"Initial state: "<<state_of_game<<endl;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("DU_CONQUER", screenWidth / 2, screenHeight / 2, 50, MAGENTA);

        if(state_of_game==LAYER_MENU) logic_draw_menu();
        else if(state_of_game==LAYER_MAP) logic_draw_map();
        else if(state_of_game==LAYER_EXIT) break;
        
        

        cout<<"Game State: "<<state_of_game<<endl;
        EndDrawing();
    }

    unload_map();
    unload_menu();
    CloseWindow();

    
    return 0;
}