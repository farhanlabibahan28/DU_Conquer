#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "enum.h"
#include "button.h"

void UpdateMap(GameScreen& currentScreen, Texture2D& mapTexture, Texture2D& characterTex, Vector2& character_pos, Button& backButton);

#endif // MAP_H
