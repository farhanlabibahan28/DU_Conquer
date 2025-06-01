#include "raylib.h"
#include "tileson.hpp"
#include <filesystem>
namespace fs = std::filesystem;

// using namespace tson;

int main() {
    InitWindow(1200, 800, "Isometric Map Viewer");
    SetTargetFPS(60);

    // Load tile texture
    Texture2D tileset = LoadTexture("grass.png");

    // Parse map.json
    tson::Tileson parser;
    auto map = parser.parse(fs::path("please.json"));
    if (map->getStatus() != tson::ParseStatus::OK) {
        TraceLog(LOG_ERROR, "Failed to load map: %s", map->getStatusMessage().c_str());
        CloseWindow();
        return 1;
    }

    int tileWidth = map->getTileSize().x;
    int tileHeight = map->getTileSize().y;

    // Initialize camera
    Camera2D camera = { 0 };
    camera.target = { 0, 0 };
    camera.offset = { 600, 400 }; // center of screen
    camera.zoom = 1.0f;

    // Game loop
    while (!WindowShouldClose()) {
        // Camera movement
        if (IsKeyDown(KEY_RIGHT)) camera.target.x += 10;
        if (IsKeyDown(KEY_LEFT)) camera.target.x -= 10;
        if (IsKeyDown(KEY_DOWN)) camera.target.y += 10;
        if (IsKeyDown(KEY_UP)) camera.target.y -= 10;

        camera.zoom += GetMouseWheelMove() * 0.1f;
        if (camera.zoom < 0.2f) camera.zoom = 0.2f;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);  // <-- Camera starts here

        for (auto& layer : map->getLayers()) {
            if (layer.getType() != tson::LayerType::TileLayer) continue;

            for (const auto& item : layer.getTileData()) {
                auto pos = item.first;
                tson::Tile* tile = item.second;
                if (!tile) continue;

                const tson::Tileset* ts = tile->getTileset();
                if (!ts) continue;

                // Get source rect from tileset
                tson::Rect rect = const_cast<tson::Tileset*>(ts)->getTile(tile->getId())->getDrawingRect();
                Rectangle src = {
                    (float)rect.x,
                    (float)rect.y,
                    (float)rect.width,
                    (float)rect.height
                };

                // Convert to isometric position
                int x = std::get<0>(pos);
                int y = std::get<1>(pos);

                float isoX = (x - y) * (tileWidth / 2.0f);
                float isoY = (x + y) * (tileHeight / 2.0f) / 2;

                Rectangle dest = {
                    isoX,
                    isoY,
                    (float)tileWidth,
                    (float)tileHeight
                };

                DrawTexturePro(tileset, src, dest, {0, 0}, 0.0f, WHITE);
            }
        }

        EndMode2D(); // <-- Camera ends here

        // Debug info
        DrawText(TextFormat("Camera: (%.1f, %.1f)", camera.target.x, camera.target.y), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Zoom: %.2f", camera.zoom), 10, 40, 20, DARKGRAY);
        DrawFPS(10, 70);

        EndDrawing();
    }

    UnloadTexture(tileset);
    CloseWindow();
    return 0;
}