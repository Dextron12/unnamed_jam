
#include "window.hpp"
#include "SpriteAnimations.hpp"


int main(int argc, char* argv[]) {
	Window win("Decaying Island Game", 1024, 576, SDL_WINDOW_RESIZABLE);

	VFS::init();

	Spritesheet grass(win.renderer, VFS::resolve("/Assets/Tiles/Grass/Grass_Tiles_3.png").string());
	grass.pushSubTexture("grassHole", {0,0,48,48});

	SpriteAnimation player(win.renderer, VFS::resolve("/Assets/Player/Player_Static/Player_New/Player_Anim/Player_Idle_Run_Death_Anim.png").string());
	loadAnimationsFromJSON(VFS::resolve("/Assets/Animation scripts/Player/Player_idle.json").string(), player);

	std::string currentAnim = "idle_right";
	player.setAnimation(currentAnim, true, true, SDL_FLIP_HORIZONTAL);

	Timer swap(3000);

	int currentFrame = 0;
	int frameCount = 6;

	while (win.appState) {
		win.update();
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

		grass.render("grassHole", { 200,150 });

		/*if (swap.isFinished()) {
			if (currentAnim == "idle_forward") {
				currentAnim = "idle_backward";
			}
			else {
				currentAnim = "idle_forward";
			}
			player.setAnimation(currentAnim, false);
			swap.reset();
		}*/


		player.play({ 200, 150 });

		SDL_RenderPresent(win.renderer);
	}
	return 0;
}