#include "window.hpp"
#include <iostream>

#include "window.hpp"
#include <cmath>

struct Vec2 {
	float x, y;

	Vec2 operator+(const Vec2& rhs) const { return { x + rhs.x, y + rhs.y }; }
	Vec2 operator-(const Vec2& rhs) const { return { x - rhs.x, y - rhs.y }; }
	Vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
	Vec2& operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }

	float length() const { return std::sqrt(x * x + y * y); }
	Vec2 normalized() const {
		float len = length();
		if (len == 0) return { 0,0 };
		return { x / len, y / len };
	}
};

int main(int argc, char* argv[]) {
	WindowContext win("Decaying Island Jam", { 1280, 720 }, SDL_WINDOW_RESIZABLE);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	const float speed = 200.0f; // pixels per second

	Vec2 playerPos = { LOGICAL_WIDTH / 2.0f, LOGICAL_HEIGHT / 2.0f };
	Vec2 cameraOffset = { 482.0f, 200.0f };

	SDL_Rect blackBox = { (LOGICAL_WIDTH / 2) - 150, (LOGICAL_HEIGHT / 2) - 75, 300, 150 };

	while (win.appState) {
		win.startFrame();

		SDL_SetRenderDrawColor(win.renderer, 30, 30, 30, 255);
		SDL_RenderClear(win.renderer);

		// Render world (red tiles)
		SDL_SetRenderDrawColor(win.renderer, 250, 30, 30, 255);
		for (int y = 0; y <= 20; y++) {
			for (int x = 0; x <= 20; x++) {
				SDL_Rect pos = {
					static_cast<int>(cameraOffset.x + x * 16),
					static_cast<int>(cameraOffset.y + y * 16),
					16, 16
				};
				SDL_RenderFillRect(win.renderer, &pos);
			}
		}

		// Render player (blue)
		SDL_SetRenderDrawColor(win.renderer, 30, 30, 250, 255);
		SDL_Rect playerRect = { static_cast<int>(playerPos.x), static_cast<int>(playerPos.y), 16, 16 };
		SDL_RenderFillRect(win.renderer, &playerRect);

		// Draw black box boundary
		SDL_SetRenderDrawColor(win.renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(win.renderer, &blackBox);

		// Input handling
		Vec2 movement = { 0, 0 };
		const Uint8* keys = SDL_GetKeyboardState(nullptr);
		if (keys[SDL_SCANCODE_W]) movement.y -= 1;
		if (keys[SDL_SCANCODE_S]) movement.y += 1;
		if (keys[SDL_SCANCODE_A]) movement.x -= 1;
		if (keys[SDL_SCANCODE_D]) movement.x += 1;

		if (movement.x != 0 || movement.y != 0) {
			movement = movement.normalized();
			Vec2 nextPlayerPos = playerPos + (movement * speed * win.deltaTime);

			// X-axis: clamp player or scroll world
			if (nextPlayerPos.x >= blackBox.x && nextPlayerPos.x + 16 <= blackBox.x + blackBox.w) {
				playerPos.x = nextPlayerPos.x;
			}
			else {
				cameraOffset.x -= movement.x * speed * win.deltaTime;
			}

			// Y-axis: clamp player or scroll world
			if (nextPlayerPos.y >= blackBox.y && nextPlayerPos.y + 16 <= blackBox.y + blackBox.h) {
				playerPos.y = nextPlayerPos.y;
			}
			else {
				cameraOffset.y -= movement.y * speed * win.deltaTime;
			}
		}

		win.endFrame();
	}

	return 0;
}



/*
const int BASE_WIDTH = 1280;
const int BASE_HEIGHT = 720;


int main(int argc, char* argv[]) {

	Window win("decaying island game", BASE_WIDTH, BASE_HEIGHT, SDL_WINDOW_RESIZABLE);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_Texture* renderTarget = SDL_CreateTexture(win.renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		BASE_WIDTH, BASE_HEIGHT);

	while (win.appState) {
		SDL_SetRenderTarget(win.renderer, renderTarget);
		win.update();

		SDL_SetRenderDrawColor(win.renderer, 30, 30, 30, 255);
		SDL_RenderClear(win.renderer);

		//Stop renderering here. Resets render target to window
		SDL_SetRenderTarget(win.renderer, nullptr);
		SDL_RenderClear(win.renderer);

		// Calcukate scale to fill window completely without letterboxing (may crop)
		float scaleX = (float)win.width / BASE_WIDTH;
		float scaleY = (float)win.height / BASE_HEIGHT;
		float scale = std::max<float>(scaleX, scaleY);

		int destW = (int)(BASE_WIDTH * scale);
		int destH = (int)(BASE_HEIGHT * scale);

		SDL_Rect destRect;
		destRect.x = (win.width - destW) / 2; // Center horizontally (negative values crop)
		destRect.y = (win.height - destH) / 2; // Center vertically (negative valeus crop)

		destRect.w = destW;
		destRect.h = destH;

		// Render the scaled texture to the window
		SDL_RenderCopy(win.renderer, renderTarget, nullptr, &destRect);
		SDL_RenderPresent(win.renderer);
	}

}

*/

/*
int main(int argc, char* argv[]) {

	// Create a window in resizable mode first to get SDL subsystems up.
	Window window("Decaying Island Game", 1280, 720, SDL_WINDOW_RESIZABLE);

	// Get desktop display mode to find fullscreen resolution
	SDL_DisplayMode desktopMode;
	if (SDL_GetDesktopDisplayMode(0, &desktopMode) != 0) {
		std::cerr << "Failed to get desktop display mode: " << SDL_GetError() << std::endl;
		return -1;
	}

	// Set window to borderless fullscreen desktop mode
	SDL_SetWindowFullscreen(window.window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	// Update window size to desktop resolution (fullscreen)
	window.width = desktopMode.w;
	window.height = desktopMode.h;

	SDL_Rect playerPos = { window.width / 2, window.height / 2, 16, 16 };
	SDL_Rect worldPos = { 0, 0, 16, 16 };

	const int speed = 300; // pixels per second
	Uint32 lastTicks = SDL_GetTicks();

	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = false;
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
		}

		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime = (currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		const Uint8* keys = SDL_GetKeyboardState(nullptr);
		if (keys[SDL_SCANCODE_W]) worldPos.y += (int)(speed * deltaTime);
		if (keys[SDL_SCANCODE_S]) worldPos.y -= (int)(speed * deltaTime);
		if (keys[SDL_SCANCODE_A]) worldPos.x += (int)(speed * deltaTime);
		if (keys[SDL_SCANCODE_D]) worldPos.x -= (int)(speed * deltaTime);

		SDL_SetRenderDrawColor(window.renderer, 30, 30, 30, 255);
		SDL_RenderClear(window.renderer);

		SDL_SetRenderDrawColor(window.renderer, 255, 0, 0, 255);

		// Calculate scaling factors based on current fullscreen resolution and logical 1280x720
		float scaleX = (float)window.width / 1280.0f;
		float scaleY = (float)window.height / 720.0f;

		// Draw a 3x3 tile map in red
		for (int y = 0; y <= 2; y++) {
			for (int x = 0; x <= 2; x++) {
				SDL_Rect t_pos = {
					static_cast<int>((worldPos.x + x * 16) * scaleX),
					static_cast<int>((worldPos.y + y * 16) * scaleY),
					static_cast<int>(16 * scaleX),
					static_cast<int>(16 * scaleY)
				};
				SDL_RenderFillRect(window.renderer, &t_pos);
			}
		}

		SDL_SetRenderDrawColor(window.renderer, 30, 30, 250, 255);
		SDL_Rect scaledPlayerPos = {
			static_cast<int>(playerPos.x * scaleX),
			static_cast<int>(playerPos.y * scaleY),
			static_cast<int>(playerPos.w * scaleX),
			static_cast<int>(playerPos.h * scaleY)
		};
		SDL_RenderFillRect(window.renderer, &scaledPlayerPos);

		SDL_RenderPresent(window.renderer);
	}

	return 0;
}

*/


// Apect ratio (16:() windowed method
/*
int main(int argc, char* argv[]) {
	Window window("Decaying Island Game", 1280, 720, SDL_WINDOW_RESIZABLE);

	// Fixed logical size for 16:9
	const int LOGICAL_WIDTH = 1280;
	const int LOGICAL_HEIGHT = 720;
	SDL_RenderSetLogicalSize(window.renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);

	SDL_Rect playerPos = { LOGICAL_WIDTH / 2, LOGICAL_HEIGHT / 2, 16, 16 };
	SDL_Rect worldPos = { 0, 0, 16, 16 };

	const int speed = 1;

	while (window.appState) {
		window.update();

		// Calculate viewport to maintain aspect ratio anchored top-left
		int winW = window.width;
		int winH = window.height;

		float winAspect = (float)winW / winH;
		float logicalAspect = (float)LOGICAL_WIDTH / LOGICAL_HEIGHT;

		SDL_Rect viewport = { 0, 0, winW, winH };

		if (winAspect > logicalAspect) {
			// Window is wider than logical aspect ratio: letterbox right side
			viewport.w = (int)(winH * logicalAspect);
			viewport.h = winH;
			viewport.x = 0; // Anchor to left
			viewport.y = 0;
		}
		else {
			// Window is taller or equal: letterbox bottom
			viewport.w = winW;
			viewport.h = (int)(winW / logicalAspect);
			viewport.x = 0;
			viewport.y = 0; // Anchor top
		}

		SDL_RenderSetViewport(window.renderer, &viewport);

		// Clear background — fill full window with dark gray
		SDL_SetRenderDrawColor(window.renderer, 30, 30, 30, 255);
		SDL_RenderClear(window.renderer);

		// Draw the map tiles in red
		SDL_SetRenderDrawColor(window.renderer, 255, 0, 0, 255);
		for (int y = 0; y <= 2; y++) {
			for (int x = 0; x <= 2; x++) {
				SDL_Rect tileRect = { worldPos.x + x * 16, worldPos.y + y * 16, 16, 16 };
				SDL_RenderFillRect(window.renderer, &tileRect);
			}
		}

		// Draw the player tile in blue
		SDL_SetRenderDrawColor(window.renderer, 30, 30, 250, 255);
		SDL_RenderFillRect(window.renderer, &playerPos);

		// Simple movement
		const Uint8* keys = SDL_GetKeyboardState(nullptr);
		if (keys[SDL_SCANCODE_W]) worldPos.y += speed * window.deltaTime;
		if (keys[SDL_SCANCODE_S]) worldPos.y -= speed * window.deltaTime;
		if (keys[SDL_SCANCODE_A]) worldPos.x += speed * window.deltaTime;
		if (keys[SDL_SCANCODE_D]) worldPos.x -= speed * window.deltaTime;

		SDL_RenderPresent(window.renderer);
	}

	return 0;
}
*/






















































/*#include "window.hpp"
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
	grass.addSpritesheet(win.renderer, "grassMiddle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());
	grass.addSpritesheet(win.renderer, "water", VFS::resolve("/Assets/Tiles/Water/Water_Tile_3.png").string());
	grass.pushSubTexture("Sand", { 16,92,16,16 });

	//Water around sand anim
	//SpriteAnimation water(win.renderer, "waterAnim", VFS::resolve("/Assets/Tiles/Beach/Beach_Tiles.png").string());
	//loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Tiles/Beach.json").string(), water);
	//water.setAnimation("centerCenter");

	SpriteAnimation grassyWater(win.renderer, "grassyWaterAnim", VFS::resolve("/Assets/Tiles/Water/Water_Tile_3_Anim.png").string());
	loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Tiles/stagWater.json").string(), grassyWater);
	grassyWater.setAnimation("topCenter");

	SpriteAnimation stonyWater(win.renderer, "stonyWaterAnim", VFS::resolve("/Assets/Tiles/Water/Water_Stone_Tile_3_Anim.png").string());
	loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Tiles/stagWater.json").string(), stonyWater);
	stonyWater.setAnimation("topCenter");

	TextureMngr::resolve(win.renderer, "grassMiddle", VFS::resolve("/Assets/Tiles/Grass/Grass_3_Middle.png").string());

	// Player
	SDL_FPoint startPos = { win.width / 2.0f - 16.0f, win.height / 2.0f - 16.0f };

	//Camera camera({ startPos.x - win.width / 2.0f, startPos.y - win.height / 2.0f });
	//AnimatedPlayer Player(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), startPos);
	//Player.initAnimations(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string());

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

	worldCamera cam{};
	SDL_Point oldWindowSize = { win.width, win.height };

	World world(4, 4, 16);
	world.generate();

	SDL_Rect t_test = { 0,0,64,64 };


	while (win.appState) {
		win.update(); // Updates window size, deltaTime, input, flags


		// Clear screen
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

		//Player.update(win.deltaTime);

		if (oldWindowSize.x != win.width || oldWindowSize.y != win.height) {
			cam.updateCameraToCenter(win.width, win.height);
			//SDL_RenderSetLogicalSize(win.renderer, win.width, win.height);
			std::cout << "windowWidth: " << win.width << ", " << "windowHeight: " << win.height << std::endl;
			std::cout << "x: " << cam.cameraOffset.x << ", y: " << cam.cameraOffset.y << std::endl;
			oldWindowSize = { (int)win.width, (int)win.height };
		}

		world.render(win.renderer, cam.cameraOffset, win.width, win.height);

		SDL_SetRenderDrawColor(win.renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(win.renderer, &t_test);

		//water.play({200,200});

		//stagWater.render("topLeft", { 300,300 });

		// Handle window resize: update camera position if needed (e.g. keep player centered)
		/*if (win.windowResized) {
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
					//SDL_RenderCopy(win.renderer, TextureMngr::textures["grassMiddle"]->tex, NULL, &dest);
				}
				else if (tile.type == TileType::Sand) {
					//grass.render("Sand", { screenPos.x, screenPos.y });
				}
				else if (tile.type == TileType::Water) {
					//grass.render("water", { screenPos.x, screenPos.y }, "water");
				}
			}
		}


		//Player.render(camera);

		// Draw any UI or blackbox here

		SDL_RenderPresent(win.renderer);
	}






	return 0;
}


*/

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