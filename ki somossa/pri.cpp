#include "raylib.h"
#include "tileson.hpp"
#include <filesystem>
namespace fs = std::filesystem;

using namespace tson;

int main() {
    InitWindow(800, 600, "Isometric Map Viewer");
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

    // Game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(::RAYWHITE);

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

                // Convert grid position to isometric screen position
                int x = std::get<0>(pos);
                int y = std::get<1>(pos);

                float isoX = (x - y) * (tileWidth / 2.0f) + 400; // center offset
                float isoY = (x + y) * (tileHeight / 2.0f) / 2 + 100;

                Rectangle dest = {
                    isoX,
                    isoY,
                    (float)tileWidth,
                    (float)tileHeight
                };

                DrawTexturePro(tileset, src, dest, {0, 0}, 0.0f, ::WHITE);
            }
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    CloseWindow();
    return 0;
}
