#include "islandGenerator.hpp"

#include <algorithm>

// This function is called after the window size is changed.
void worldCamera::updateCameraToCenter(int screenWidth, int screenHeight) {
    SDL_Point worldSize = { 300 * 16, 300 * 16 };  // This is your world size (4800x4800)

    // Calculate camera offset to center the world in the screen
    cameraOffset.x = (worldSize.x - screenWidth) / 2;  // Centers horizontally
    cameraOffset.y = (worldSize.y - screenHeight) / 2; // Centers vertically
}

World::World(int width, int height, int tileSize) : width(width), height(height), tileSize(tileSize) {
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    map.resize(height, std::vector<Tile>(width));
}

void World::generate() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Tile& tile = map[y][x];
            tile.worldPos = { x * (float)tileSize, y * (float)tileSize };
            tile.elevation = noise.GetNoise(tile.worldPos.x, tile.worldPos.y);
        }
    }
}

void World::render(SDL_Renderer* renderer, SDL_Point cameraOffset, int screenWidth, int screenHeight) {
    // Calculate which tiles are visible based on the current camera offset and screen size
    int firstVisibleTileX = std::max<int>(0, static_cast<int>(std::floor(cameraOffset.x / (float)tileSize)));
    int firstVisibleTileY = std::max<int>(0, static_cast<int>(std::floor(cameraOffset.y / (float)tileSize)));

    int lastVisibleTileX = std::min<int>(width - 1, static_cast<int>(std::floor((cameraOffset.x + screenWidth) / (float)tileSize)));
    int lastVisibleTileY = std::min<int>(height - 1, static_cast<int>(std::floor((cameraOffset.y + screenHeight) / (float)tileSize)));

    // Render only the visible tiles in the current window
    for (int y = firstVisibleTileY; y <= lastVisibleTileY; y++) {
        for (int x = firstVisibleTileX; x <= lastVisibleTileX; x++) {
            const Tile& tile = map[y][x];
            SDL_Point screenPos = { (int)tile.worldPos.x - cameraOffset.x, (int)tile.worldPos.y - cameraOffset.y };


            // Check if the tile is within the screen bounds
            if (screenPos.x < screenWidth && screenPos.x + tileSize > 0 &&
                screenPos.y < screenHeight && screenPos.y + tileSize > 0) {

                SDL_Rect rect = { screenPos.x, screenPos.y, tileSize, tileSize };
                if (tile.elevation > 0.4f) {
                    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);  // Grass
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);  // Water
                }
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}
