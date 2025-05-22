#include <raylib.h>


int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Hello Raylib");
   

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
