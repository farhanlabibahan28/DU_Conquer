#include <raylib.h>
#include "tileson.hpp"
#include <map>
#include <tuple>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

map<int, Texture2D> tileTextures;
Camera2D camera = { 0 };

Vector2 isometricToScreen(const Vector2& tilePos, float tileWidth, float tileHeight) {
    return {
        (tilePos.x - tilePos.y) * (tileWidth / 2.0f),
        (tilePos.x + tilePos.y) * (tileHeight / 2.0f)
    };
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Isometric Tiled Map");
    SetTargetFPS(60);

    // Initialize camera
    camera.target = { 0, 0 };
    camera.offset = { screenWidth/2.0f, screenHeight/2.0f };
    camera.zoom = 1.0f;

    // Load map
    tson::Tileson parser;
    unique_ptr<tson::Map> map = parser.parse("please.json");
    
    if (map->getStatus() != tson::ParseStatus::OK) {
        cerr << "Map load error: " << map->getStatusMessage() << endl;
        CloseWindow();
        return -1;
    } else {
        cout << "Map loaded successfully! Size: " 
             << map->getSize().x << "x" << map->getSize().y << endl;
    }

    // Load textures
    for (auto& tileset : map->getTilesets()) {
        string imagePath = tileset.getImagePath();
        cout << "Loading tileset: " << imagePath << endl;
        
        Image tilesetImage = LoadImage(imagePath.c_str());
        if (tilesetImage.data == NULL) {
            cerr << "Failed to load: " << imagePath << endl;
            continue;
        }

        int firstGid = tileset.getFirstgid();
        int tileWidth = tileset.getTileSize().x;
        int tileHeight = tileset.getTileSize().y;
        int columns = tileset.getColumns();
        int tileCount = tileset.getTileCount();
        
        cout << "Processing " << tileCount << " tiles..." << endl;
        
        for (int i = 0; i < tileCount; i++) {
            int gid = firstGid + i;
            int x = (i % columns) * tileWidth;
            int y = (i / columns) * tileHeight;
            
            Rectangle tileRect = {
                (float)x, (float)y,
                (float)tileWidth, (float)tileHeight
            };
            
            Image tileImage = ImageFromImage(tilesetImage, tileRect);
            tileTextures[gid] = LoadTextureFromImage(tileImage);
            UnloadImage(tileImage);
        }
        
        UnloadImage(tilesetImage);
    }

    cout << "Loaded " << tileTextures.size() << " tile textures" << endl;

    // Main game loop
    while (!WindowShouldClose()) {
        // Camera controls
        if (IsKeyDown(KEY_RIGHT)) camera.target.x += 5;
        if (IsKeyDown(KEY_LEFT)) camera.target.x -= 5;
        if (IsKeyDown(KEY_UP)) camera.target.y -= 5;
        if (IsKeyDown(KEY_DOWN)) camera.target.y += 5;
        camera.zoom = fmaxf(0.1f, fminf(camera.zoom + GetMouseWheelMove()*0.1f, 3.0f));

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        
        // Draw all layers
        for (auto& layer : map->getLayers()) {
            if (layer.getType() == tson::LayerType::TileLayer) {
                for (const auto& [pos, tile] : layer.getTileData()) {
                    int gid = tile->getGid();
                    if (gid == 0 || tileTextures.find(gid) == tileTextures.end()) continue;

                    float tileW = (float)map->getTileSize().x;
                    float tileH = (float)map->getTileSize().y;
                    
                    Vector2 screenPos = isometricToScreen(
                        { (float)get<0>(pos), (float)get<1>(pos) },
                        tileW, tileH
                    );

                    DrawTextureV(tileTextures[gid], screenPos, WHITE);
                }
            }
        }
        
        EndMode2D();

        // Debug info
        DrawText(TextFormat("Camera: (%.1f, %.1f)", camera.target.x, camera.target.y), 10, 10, 20, WHITE);
        DrawText(TextFormat("Zoom: %.1f", camera.zoom), 10, 40, 20, WHITE);
        DrawFPS(10, 70);

        EndDrawing();
    }

    // Cleanup
    for (auto& [id, tex] : tileTextures) {
        UnloadTexture(tex);
    }

    CloseWindow();
    return 0;
}