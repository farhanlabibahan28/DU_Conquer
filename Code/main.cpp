#include "raylib.h"
#include <cmath>

const int TILE_WIDTH = 64;
const int TILE_HEIGHT = 32;


int GetTileIndex(int tx, int ty)
{
    return (tx + ty) % 2;
}

Vector2 IsoToScreen(int tx, int ty)
{
    Vector2 pos;
    pos.x = (tx - ty) * (TILE_WIDTH / 2.0f);
    pos.y = (tx + ty) * (TILE_HEIGHT / 2.0f);
    return pos;
}

Vector2 ScreenToIsoTile(float px, float py)
{
    Vector2 tile;
    tile.x = (px / (TILE_WIDTH / 2) + py / (TILE_HEIGHT / 2)) / 2;
    tile.y = (py / (TILE_HEIGHT / 2) - (px / (TILE_WIDTH / 2))) / 2;
    return tile;
}

int main()
{
    InitWindow(800, 600, "Infinite Isometric Tilemap - raylib");

    Texture2D tileset = LoadTexture("Tiles Image/dummy.json"); 

    Rectangle tileRecs[2] = {
        {0, 0, (float)TILE_WIDTH, (float)TILE_HEIGHT},
        {(float)TILE_WIDTH, 0, (float)TILE_WIDTH, (float)TILE_HEIGHT}
    };

    Vector2 cameraPos = {0.0f, 0.0f}; 

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Move camera with arrow keys (pixels)
        if (IsKeyDown(KEY_RIGHT)) cameraPos.x += 5;
        if (IsKeyDown(KEY_LEFT))  cameraPos.x -= 5;
        if (IsKeyDown(KEY_UP))    cameraPos.y -= 5;
        if (IsKeyDown(KEY_DOWN))  cameraPos.y += 5;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Calculate visible tile range based on screen and camera position

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Get top-left and bottom-right screen coords in world coords
        float startX = cameraPos.x - screenWidth/2;
        float startY = cameraPos.y - screenHeight/2;

        float endX = cameraPos.x + screenWidth/2;
        float endY = cameraPos.y + screenHeight/2;

        // Convert top-left and bottom-right screen positions to tile coordinates
        Vector2 startTile = ScreenToIsoTile(startX, startY);
        Vector2 endTile = ScreenToIsoTile(endX, endY);

        // We'll draw tiles from floor(minX) to ceil(maxX) and floor(minY) to ceil(maxY)
        int minTx = (int)floorf(startTile.x) - 2;  // extra buffer
        int maxTx = (int)ceilf(endTile.x) + 2;
        int minTy = (int)floorf(startTile.y) - 2;
        int maxTy = (int)ceilf(endTile.y) + 2;

        Vector2 screenCenter = {(float)screenWidth / 2, (float)screenHeight / 2};

        // Draw tiles visible on screen
        for (int ty = minTy; ty <= maxTy; ty++)
        {
            for (int tx = minTx; tx <= maxTx; tx++)
            {
                // For demo infinite map: no boundaries, tile index cycles every 2
                int tileIndex = GetTileIndex(tx, ty);

                Vector2 pos = IsoToScreen(tx, ty);
                // Adjust for camera and center offset
                pos.x -= cameraPos.x;
                pos.y -= cameraPos.y;
                pos.x += screenCenter.x;
                pos.y += screenCenter.y;

                DrawTextureRec(tileset, tileRecs[tileIndex], pos, WHITE);
            }
        }

        DrawText("Use arrow keys to move camera", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadTexture(tileset);
    CloseWindow();

    return 0;
}