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

struct Tile {
	TileType type;
	float elevation;

	SDL_FPoint worldPos; // Assumes all tiles within a map are of the same size.
};

#endif