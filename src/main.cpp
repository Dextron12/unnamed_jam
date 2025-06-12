
#include "window.hpp"
#include "SpriteAnimations.hpp"

#include <random>


int main(int argc, char* argv[]) {
	Window win("Decaying Island Game", 1024, 576, SDL_WINDOW_RESIZABLE);

	VFS::init();

	Spritesheet grass(win.renderer, VFS::resolve("/Assets/Tiles/Grass/Grass_Tiles_3.png").string());
	grass.pushSubTexture("grassHole", {0,0,48,48});
	grass.pushSubTexture("grass", { 48,0,16,16 });

	SpriteAnimation player(win.renderer, VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string());
	loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string(), player);


	AnimatedPlayer pT(win.renderer, VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string(), { 250.0f, 150.0f });
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

	while (win.appState) {
		win.update();
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

		pT.update(win.deltaTime);

		grass.render("grassHole", { 200,150 });

		for (int x = 0; x <= 10; x++) {
			for (int y = 0; y <= 10; y++) {
				grass.render("grass", { x * 16, y * 16 });
			}
		}

		if (swap.isFinished()) {
			std::string nextAnim = idleAnims[dist(rng)];

			// Only changes if anim is different to prevent unnecessary anim set calls.
			if (nextAnim != currentAnim) {
				currentAnim = nextAnim;
				player.setAnimation(currentAnim, false);
			}

			swap.reset();
		}


		player.play({ 200, 150 });

		pT.render("");

		SDL_RenderPresent(win.renderer);
	}
	return 0;
}