#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "button.h"
#include "enum.h"

void UpdateMenu(GameScreen& currentScreen, Texture2D& background, Button buttons[], int buttonCount);

#endif
