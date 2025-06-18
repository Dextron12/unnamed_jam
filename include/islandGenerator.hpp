#ifndef ISLAND_GENERATOR_HPP
#define ISLAND_GENERATOR_HPP

#include <FastNoise.h>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>

#include "Textures.hpp"

static int TILE_SIZE = 16;
static SDL_Point MAP_SIZE;

enum class TileType {
	Water, Sand, Grass, Path, Rock
};

class worldCamera {
public:
	SDL_Point cameraOffset;

	void updateCameraToCenter(int screenWidth, int screenHeight);
};

struct Tile {
	TileType type;
	float elevation;

	SDL_FPoint worldPos; // Assumes all tiles within a map are of the same size.
};

class World {
public:
	World(int width, int height, int tileSize);
	void generate();
	void render(SDL_Renderer* renderer, SDL_Point cameraOffset, int screenWidth, int screenHeight);

private:
	int width, height, tileSize;
	std::vector<std::vector<Tile>> map;
	FastNoiseLite noise;
};

#endif