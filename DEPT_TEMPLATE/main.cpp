#include "raylib.h"
#include <string>
#include "lights_on.h"
using namespace std;

typedef enum {
    Dept,
    Game
} dept_state;

Sound conquered_sound,pop_up_sound;
Sound click_sound, error_sound;
Music bgm_eee;
Music walk_music;
bool walk_music_playing = false;

dept_state dept_status = Dept;
bool game_win = false;

Texture2D character,bg_image;
float scale;
bool lightsOnWin = false;
// Vector2 playerPos = {-20, GetScreenHeight()-character.height-10};
Vector2 playerPos = {0};  // <-- this line is REQUIRED
Vector2 game_zone = {1200,700};
Vector2 exit_zone = {50,700};
string pop_up = "Find and Solve the Clue";
string game_pop_up = " ";
string game_rules = "Lights On Game Rules:\nTurn on all the lights to win.\nPress X to exit the game.";

Camera2D camera = {0};

bool show_rules_popup = false;
bool show_ok_button = false;


bool eee_game() 
{
    return game_win;
}



void init_eee()
{
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "DU_Conquer/EEE DEPT");
    InitAudioDevice();
    conquered_sound = LoadSound("conquered.mp3"); 
    pop_up_sound = LoadSound("pop_up.mp3"); 
    click_sound = LoadSound("click.mp3");
    error_sound = LoadSound("error.mp3");
    bgm_eee = LoadMusicStream("bgm_eee.mp3");
    SetMusicVolume(bgm_eee, 0.13f);
    PlayMusicStream(bgm_eee);
    walk_music = LoadMusicStream("walk.mp3");
    SetMusicVolume(walk_music, 1.0f);
    character = LoadTexture("character.png");
    bg_image = LoadTexture("HISTORY_EX.png");
    

    // Now set correct position AFTER loading character
    playerPos = (Vector2){
        10,  // X position from left
        (float)GetScreenHeight() - character.height - 0.0015 * GetScreenHeight()  // Y position from bottom
    };

    camera.target = playerPos;  
    camera.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    ToggleFullscreen();
    SetTargetFPS(60);
    InitLightsOn();
}

void unload_eee()
{
    UnloadLightsOn();
    UnloadTexture(character);
    UnloadTexture(bg_image);
    UnloadSound(pop_up_sound);
    UnloadSound(conquered_sound);
    UnloadSound(click_sound);
    UnloadSound(error_sound);
    UnloadMusicStream(bgm_eee);
    UnloadMusicStream(walk_music);
    CloseAudioDevice();
    CloseWindow();
}

void logic_draw_eee()
{
    while (!WindowShouldClose()) 
    {
        UpdateMusicStream(bgm_eee);
        if(!game_win) pop_up = "Find and Solve the Clue";
        if (dept_status == Dept) 
        {
            bool moving = false;
            if (IsKeyDown(KEY_A)) {
                playerPos.x -= 3;
                moving = true;
            }
            if (IsKeyDown(KEY_D)) {
                playerPos.x += 3;
                moving = true;
            }

            if (moving && !walk_music_playing) {
                PlayMusicStream(walk_music);
                walk_music_playing = true;
            }
            else if (!moving && walk_music_playing) {
                StopMusicStream(walk_music);
                walk_music_playing = false;
            }

            if (walk_music_playing) {
                UpdateMusicStream(walk_music);
            }

            bool eKeyHandled = false;

            if (CheckCollisionCircles(playerPos, 150.0f,game_zone, 150.0f)) {
                pop_up = "Press E to Solve";
                if(IsKeyPressed(KEY_E) && !game_win && !show_rules_popup)
                {
                    PlaySound(pop_up_sound);
                    PlaySound(click_sound);
                    show_rules_popup = true;
                    show_ok_button = true;
                    eKeyHandled = true;
                }
            }

            if (CheckCollisionCircles(playerPos, 150.0f,exit_zone, 150.0f)) {
                pop_up = "Press E to Exit";
                if(IsKeyPressed(KEY_E))
                {
                    PlaySound(click_sound);
                    unload_eee();
                    eKeyHandled = true;
                }
                else if(IsKeyPressed(KEY_E)) {
                    PlaySound(error_sound);
                }
            }

            if (IsKeyPressed(KEY_E) && !eKeyHandled) {
                PlaySound(error_sound);
            }
        } 
        else if(dept_status == Game)
        {
            UpdateLightsOn();
            if (checkLightsOnWinStatus() || IsKeyDown(KEY_X)) {
                dept_status = Dept;
                game_pop_up = "EEE Conqured!! Abort";
                game_win = true;
                PlaySound(conquered_sound);
                UnloadLightsOn();
            }

            if(IsKeyDown(KEY_Q))
            {
                UnloadLightsOn();
            }
        }

        if(playerPos.x<=-20) playerPos.x = -20;
        else if(playerPos.x>=bg_image.width) playerPos.x = bg_image.width;
    
        scale = (float)GetMonitorHeight(0) / bg_image.height;
        float scaledWidth = bg_image.width * scale;

        float cam = playerPos.x;

        float halfScreen = GetMonitorWidth(0) / 2.0f;

        if (cam < halfScreen) cam = halfScreen;
        if (cam > scaledWidth - halfScreen) cam = scaledWidth - halfScreen;

        camera.target = (Vector2){ cam, GetScreenHeight() / 2.0f};

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        float scale = (float)GetMonitorHeight(0) / bg_image.height;
        // float scaledWidth = image.width * scale;
        DrawTextureEx(bg_image, (Vector2){0, 0}, 0.0f, scale, WHITE);
        DrawTexture(character, playerPos.x, playerPos.y, WHITE);
        DrawCircleV(game_zone, 20, RED);
        EndMode2D();

        
        if (dept_status == Game) DrawLightsOn();

        DrawText(game_pop_up.c_str(), 20, GetMonitorHeight(0)-100, 20, GREEN);

        if (game_win && CheckCollisionCircles(playerPos, 150.0f, exit_zone, 150.0f)) {
            DrawText("Press E to Exit", 20, GetMonitorHeight(0)-70, 20, RAYWHITE);
        } else if (!game_win) {
            DrawText(pop_up.c_str(), 20, GetMonitorHeight(0)-50, 20, RAYWHITE);
        }

        if (show_rules_popup) {
            int screenW = GetMonitorWidth(0);
            int screenH = GetMonitorHeight(0);

            // Draw box behind popup
            DrawRectangle(screenW / 2 - 220, screenH / 2 - 100, 440, 200, Fade(BLACK, 0.9f));
            DrawRectangleLines(screenW / 2 - 220, screenH / 2 - 100, 440, 200, LIGHTGRAY);

            DrawText(game_rules.c_str(), screenW/2 - MeasureText(game_rules.c_str(), 20)/2, screenH/2 - 60, 20, RAYWHITE);

            Rectangle okBtn = { screenW/2 - 50, screenH/2 + 30, 100, 40 };
            Vector2 mouse = GetMousePosition();
            Color btnColor = CheckCollisionPointRec(mouse, okBtn) ? RED : DARKGRAY;
            DrawRectangleRec(okBtn, btnColor);
            DrawText("OK", screenW/2 - MeasureText("OK", 20)/2, screenH/2 + 40, 20, WHITE);

            if (show_ok_button && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                if (CheckCollisionPointRec(mouse, okBtn)) {
                    PlaySound(click_sound);
                    dept_status = Game;
                    show_rules_popup = false;
                    show_ok_button = false;
                }
            }
            else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(error_sound);
            }

            
        }

        EndDrawing();
    }
}


int main()
{
    init_eee();

    logic_draw_eee();

    unload_eee();
}