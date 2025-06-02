#include "menu.h"
#include <cmath>

void UpdateMenu(GameScreen& currentScreen, Texture2D& background, Button buttons[], int buttonCount) {
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    // Draw background
    float scale = fminf(screenW / background.width, screenH / background.height);
    float destW = background.width * scale;
    float destH = background.height * scale;
    float offsetX = (screenW - destW) / 2;
    float offsetY = (screenH - destH) / 2;

    DrawTexturePro(background,
        (Rectangle){ 0, 0, (float)background.width, (float)background.height },
        (Rectangle){ offsetX, offsetY, destW, destH },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    DrawText("DU Conquer", screenW * 0.03f, screenH * 0.3f, 50, RED);


    for (int i = 0; i < buttonCount; i++) {
        buttons[i].DrawButton(24, (Color){0, 0, 0, 128});
        if (buttons[i].IsClicked()) {
            if (i == 0) currentScreen = SCREEN_MAP; // "New Game"
            if (i == 5) CloseWindow();               // "Exit"
        }
    }
}
