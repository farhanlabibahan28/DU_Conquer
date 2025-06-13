#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "enum.h"
#include "button.h"

void UpdateMap(GameScreen& currentScreen,
               Texture2D& mapTexture,
               Texture2D& characterTex,
               Vector2& character_pos,
               Button& backButton,
               RenderTexture2D& target,
               Shader& nightShader);

#endif // MAP_H
