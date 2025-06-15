
#include "window.hpp"
#include "SpriteAnimations.hpp"

#include <random>


int main(int argc, char* argv[]) {
	Window win("Decaying Island Game", 1024, 576, SDL_WINDOW_RESIZABLE);

	VFS::init();

	Spritesheet grass(win.renderer, "grass", VFS::resolve("/Assets/Tiles/Grass/Grass_Tiles_3.png").string());
	grass.addSpritesheet(win.renderer, "grass_middle", VFS::resolve("/Assets/Tiles/Grass/grass_middle_decor.png").string());
	grass.pushSubTexture("grassHole", {0,0,48,48});
	grass.pushSubTexture("grass", { 48,0,16,16 });
	//grass.pushSubTexture("grass_middle", { 0,0,256,256 });

	SpriteAnimation player(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string());
	loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string(), player);


	AnimatedPlayer pT(win.renderer, "playerSprite", VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), {250.0f, 150.0f});
	pT.initAnimations(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string());

	std::string currentAnim = "look_around";
	player.setAnimation(currentAnim);

	Timer swap(6000);

	int currentFrame = 0;
	int frameCount = 6;

	std::vector<std::string> idleAnims = {
		"idle_forward",
		"idle_backward",
		"idle_left",
		"idle_right",
		"walk_forward",
		"walk_backward",
		"walk_left",
		"walk_right",
		"death_right",
		"death_left",
		"hit_front",
		"hit_right",
		"hit_left",
		"hit_back",
		"sleep_front",
		"sleep_right",
		"sleep_left",
		"look_around"
	};

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(0, idleAnims.size() - 1);


	auto checkered = createFallbackTexture(win.renderer, 32, 32);
	SDL_Rect checkeredPos{ 300,300,checkered->w, checkered->h };

	int count = 0;
	for (const auto& it : TextureMngr::textures) {
		std::printf("[DEBUG]: Loaded Texture %i, name: %s.", count, it.first.c_str());
		if (it.second->tex == nullptr) {
			std::printf("Texture is invalid!\n");
		}
		else {
			std::printf("Texture valid\n");
		}
		count++;
	}

	while (win.appState) {
		win.update();
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

		//pT.update(win.deltaTime);

		//SDL_RenderCopy(win.renderer, checkered->tex, NULL, &checkeredPos);

		//grass.render("grassHole", { 200,150 });
		grass.render("grass_middle", { 150,300 });

		/*for (int x = 0; x <= 10; x++) {
			for (int y = 0; y <= 10; y++) {
				grass.render("grass_middle", { x * 16, y * 16 }, "grass_middle");
			}
		}*/

		if (swap.isFinished()) {
			std::string nextAnim = idleAnims[dist(rng)];

			// Only changes if anim is different to prevent unnecessary anim set calls.
			if (nextAnim != currentAnim) {
				currentAnim = nextAnim;
				player.setAnimation(currentAnim, false);
			}

			swap.reset();
		}


		//player.play({ 200, 150 });

		//pT.render("");

		SDL_RenderPresent(win.renderer);
	}
	return 0;
}