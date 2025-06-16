#include "window.hpp"
#include "SpriteAnimations.hpp"
#include "islandGenerator.hpp"

#include <unordered_map>
#include <SDL.h>
#include <utility>
#include <functional>
#include <FastNoise.h>

struct PairHash {
	std::size_t operator()(const std::pair<int, int>& p) const noexcept {
		return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
	}
};

Tile generateTile(int x, int y, FastNoiseLite& noise, int tileSize) {
	Tile tile;
	tile.worldPos = { (float)x, (float)y };

	float nx = x * tileSize * 0.3f;
	float ny = y * tileSize * 0.4f;
	tile.elevation = (noise.GetNoise(nx, ny) + 1.0f) * 0.5f;

	if (tile.elevation < 0.3f)
		tile.type = TileType::Water;
	else if (tile.elevation < 0.4f)
		tile.type = TileType::Sand;
	else
		tile.type = TileType::Grass;

	return tile;
}

int main() {
	Window win("Infinite World", 1024, 576, SDL_WINDOW_RESIZABLE);
	VFS::init();

	const int tileSize = 16;

	Spritesheet grass(win.renderer, "grass", VFS::resolve("/Assets/Tiles/Grass/Grass_Tiles_3.png").string());
	grass.addSpritesheet(win.renderer, "grass_middle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());
	grass.addSpritesheet(win.renderer, "water", VFS::resolve("/Assets/Tiles/Water/Water_Tile_3.png").string());
	grass.pushSubTexture("Sand", { 16,92,16,16 });
	grass.pushSubTexture("water", { 16,16,16,16 });

	TextureMngr::resolve(win.renderer, "grassMiddle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());

	// Player
	SDL_FPoint startPos = { win.width / 2.0f - 16.0f, win.height / 2.0f - 16.0f };

	Camera camera({ startPos.x - win.width / 2.0f, startPos.y - win.height / 2.0f });
	AnimatedPlayer Player(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), startPos);
	Player.initAnimations(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string());

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	std::unordered_map<std::pair<int, int>, Tile, PairHash> tileMap;


	float cameraLerpSpeed = 5.0f; //Higher = faster follow
	SDL_Rect blackBox = { (win.width / 2) -(win.width / 8), (win.height / 2) - (win.height / 8), win.width/4, win.height/4};

	// Custom hash for pair<int,int>
	struct IntPairHash {
		std::size_t operator()(const std::pair<int, int>& p) const noexcept {
			return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
		}
	};

	while (win.appState) {
		win.update(); // Updates window size, deltaTime, input, flags

		// Clear screen
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

		Player.update(win.deltaTime);

		// Handle window resize: update camera position if needed (e.g. keep player centered)
		if (win.windowResized) {
			// Example: recenter camera so player stays at center of screen
			camera.pos.x = Player.pos.x - win.width / 2.0f;
			camera.pos.y = Player.pos.y - win.height / 2.0f;

			std::cout << "WIndow resizxed" << std::endl;

			win.windowResized = false; // Reset flag
		}
		else {
			// Smoothly lerp camera towards target position normally
			SDL_FPoint targetPos = { Player.pos.x - win.width / 2.0f, Player.pos.y - win.height / 2.0f };
			camera.pos.x += (targetPos.x - camera.pos.x) * cameraLerpSpeed * (win.deltaTime / 1000.0f);
			camera.pos.y += (targetPos.y - camera.pos.y) * cameraLerpSpeed * (win.deltaTime / 1000.0f);
		}

		// Calculate visible tile range dynamically EVERY FRAME
		int startX = static_cast<int>(camera.pos.x) / tileSize - 1;
		int startY = static_cast<int>(camera.pos.y) / tileSize - 1;
		int endX = (static_cast<int>(camera.pos.x) + win.width) / tileSize + 1;
		int endY = (static_cast<int>(camera.pos.y) + win.height) / tileSize + 1;

		// Generate and render visible tiles for current window size and camera pos
		for (int y = startY; y <= endY; ++y) {
			for (int x = startX; x <= endX; ++x) {
				std::pair<int, int> key = { x, y };

				if (tileMap.find(key) == tileMap.end()) {
					tileMap[key] = generateTile(x, y, noise, tileSize);
				}

				Tile& tile = tileMap[key];
				SDL_Point screenPos = camera.worldToScreen({ static_cast<float>(x * tileSize), static_cast<float>(y * tileSize) });

				if (tile.type == TileType::Grass) {
					SDL_Rect dest = { screenPos.x, screenPos.y, tileSize, tileSize };
					SDL_RenderCopy(win.renderer, TextureMngr::textures["grassMiddle"]->tex, NULL, &dest);
				}
				else if (tile.type == TileType::Sand) {
					grass.render("Sand", { screenPos.x, screenPos.y });
				}
				else if (tile.type == TileType::Water) {
					grass.render("water", { screenPos.x, screenPos.y }, "water");
				}
			}
		}

		Player.render(camera);

		// Draw any UI or blackbox here...

		SDL_RenderPresent(win.renderer);
	}






	return 0;
}




/*
int main(int argc, char* argv[]) {
	Window win("Decaying Island Game", 1024, 576, SDL_WINDOW_RESIZABLE);

	VFS::init();

	Spritesheet grass(win.renderer, "grass", VFS::resolve("/Assets/Tiles/Grass/Grass_Tiles_3.png").string());
	grass.addSpritesheet(win.renderer, "grass_middle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());
	grass.addSpritesheet(win.renderer, "water", VFS::resolve("/Assets/Tiles/Water/Water_Tile_3.png").string());
	grass.pushSubTexture("grassHole", {0,0,48,48});
	grass.pushSubTexture("Sand", { 16,92,16,16 });
	grass.pushSubTexture("water", { 16,16,16,16 });

	TextureMngr::resolve(win.renderer, "grassMiddle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());

	SpriteAnimation player(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string());
	loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string(), player);

	// Camera: (pos.x pos.h, viewport.x, viewport.y
	SDL_Rect camera = { (win.width / 2) - 16, (win.height / 2) - 16, win.width, win.height };
	SDL_Rect blackBox = { camera.w / 4, camera.h / 4, camera.w / 2, camera.h / 2 };
	SDL_FPoint worldPos;
	AnimatedPlayer Player(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), {win.width / 2, win.height / 2});
	Player.initAnimations(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string());

	//AnimatedPlayer pT(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), {250.0f, 150.0f});
	//pT.initAnimations(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string());


	int tileSize = 16;
	SDL_Point mapSize = { win.width / tileSize, win.height / tileSize };

	std::vector<std::vector<Tile>> tileGrid;

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	//Generate map
	tileGrid.resize(mapSize.y);
	for (int y = 0; y < mapSize.y; y++) {
		for (int x = 0; x < mapSize.x; x++) {
			tileGrid[y].resize(mapSize.x);
			Tile& tile = tileGrid[y][x];
			tile.worldPos = { (float)x, (float)y };

			float xoff = (noise.GetNoise(((float)x) * tileSize * 0.3, ((float)y) * tileSize * 0.4) + 1.0f) * 0.5f; // Mapts to [0, 1]

			if (xoff < 0.3f) {
				tile.type = TileType::Water;
			}
			else if (xoff < 0.4f) {
				tile.type = TileType::Sand;
			}
			else {
				tile.type = TileType::Grass;
			}

			tile.elevation = xoff;
		}
	}

	float time = 0.0f;
	SDL_Point cameraOffset = { 0,0 };

	while (win.appState) {
		win.update();
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

		Player.update(win.deltaTime);

		time += win.deltaTime * 0.1f;


		//Black box checking
		SDL_Point playerScreenPos = {
			((int)Player.pos.x / tileSize) * tileSize - cameraOffset.x,
			((int)Player.pos.y / tileSize) * tileSize - cameraOffset.y
		};

		if (playerScreenPos.x < blackBox.x)
			cameraOffset.x -= blackBox.x - playerScreenPos.x;
		else if (playerScreenPos.x > blackBox.x + blackBox.w)
			cameraOffset.x += playerScreenPos.x - (blackBox.x + blackBox.w);

		if (playerScreenPos.y < blackBox.y)
			cameraOffset.y -= blackBox.y - playerScreenPos.y;
		else if (playerScreenPos.y > blackBox.y + blackBox.h)
			cameraOffset.y += playerScreenPos.y - (blackBox.y + blackBox.h);

		for (int y = 0; y < mapSize.y; ++y) {
			for (int x = 0; x < mapSize.x; ++x) {
				Tile& tile = tileGrid[y][x];

				//float nx = ((float)x) * tileSize * 0.3f - cameraOffset.x;
				//float ny = ((float)y) * tileSize * 0.4f - cameraOffset.y;

				worldPos.x = (tile.worldPos.x * tileSize - cameraOffset.x) * 0.3f;
				worldPos.y = (tile.worldPos.y * tileSize - cameraOffset.y) * 0.4f;

				tile.elevation = (noise.GetNoise(worldPos.x, worldPos.y) + 1.0) * 0.5f;

				if (tile.elevation < 0.3f)
					tile.type = TileType::Water;
				else if (tile.elevation < 0.4f)
					tile.type = TileType::Sand;
				else
					tile.type = TileType::Grass;

				int screenX = tile.worldPos.x * tileSize - cameraOffset.x;
				int screenY = tile.worldPos.y * tileSize - cameraOffset.y;
				if (tile.type == TileType::Grass) {
					//grass.render("Sand", { x * tileSize, y * tileSize });
					SDL_Rect dest = { screenX, screenY, 16, 16 };
					SDL_RenderCopy(win.renderer, TextureMngr::textures["grassMiddle"]->tex, NULL, &dest);
				}
				else if (tile.type == TileType::Sand) {
					grass.render("Sand", {screenX, screenY});
				}
				else if (tile.type == TileType::Water) {
					grass.render("water", {screenX, screenY}, "water");
				}
			}
		}


		/*for (int x = 0; x <= 10; x++) {
			for (int y = 0; y <= 10; y++) {
				grass.render("grass_middle", { x * 16, y * 16 }, "grass_middle");
			}
		}


		//player.play({ 200, 150 });
		Player.render("");
		SDL_RenderDrawRect(win.renderer, &blackBox);

		SDL_RenderPresent(win.renderer);
	}
	return 0;
}

*/