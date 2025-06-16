#include "map.h"
#include "raylib.h"
#include <math.h>
#include "global.h"

Texture2D map_image,character;
Camera2D camera;
float speed = 3.0f;
float theta = 26.2f * DEG2RAD;
float cosTheta = cosf(theta);
float sinTheta = sinf(theta);

void init_map()
{
    map_image = LoadTexture("resources/map2.png");
    character = LoadTexture("resources/character.png");

}

void logic_draw_map()
{
    camera.target = (Vector2){x_co_ordinate,y_co_ordinate};
    camera.offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
    camera.zoom = 1.0f;

    if(IsKeyPressed(KEY_X))
    {
        state_of_game = LAYER_MENU;
    }

    if (IsKeyDown(KEY_W)) {
            x_co_ordinate -= cosTheta * speed;
            y_co_ordinate -= sinTheta * speed;
        }
        if (IsKeyDown(KEY_S)) {
            x_co_ordinate += cosTheta * speed;
            y_co_ordinate += sinTheta * speed;
        }
        if (IsKeyDown(KEY_A)) {
            x_co_ordinate -= cosTheta * speed;
            y_co_ordinate += sinTheta * speed;
        }
        if (IsKeyDown(KEY_D)) {
            x_co_ordinate += cosTheta * speed;
            y_co_ordinate -= sinTheta * speed;
        }


        // Draw

        BeginMode2D(camera);

        DrawTexture(map_image, 0, 0, WHITE);
        DrawTextureEx(character,
            (Vector2){ x_co_ordinate - (character.width * scale) / 2, y_co_ordinate - (character.height * scale) / 2 },
            0.0f,
            scale*0.3,
            WHITE);

        EndMode2D();

        // text
        DrawText(TextFormat("X: %f Y: %f",x_co_ordinate, y_co_ordinate), 10, 70, 30, RED);
        DrawFPS(10, 40);
}

void unload_map()
{
    if (map_image.id > 0) {
        UnloadTexture(map_image);
        map_image.id = 0; 
    }
    if (character.id > 0) {
        UnloadTexture(character);
        character.id = 0;
    }
}
