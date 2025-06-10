
#include "window.hpp"
#include "Textures.hpp"


int main(int argc, char* argv[]) {
	Window win("Decaying Island Game", 1024, 576, SDL_WINDOW_RESIZABLE);

	VFS::init();

	dummyTest();

	//Texture grass = loadTexture(win.renderer, VFS::resolve("Assets/Tiles/Grass/Grass_Tiles_3.png").string());

    //SDL_Rect pos = { 200, 150, grass.w, grass.h };

	while (win.appState) {
		win.update();
		SDL_SetRenderDrawColor(win.renderer, 0, 0, 0, 255);
		SDL_RenderClear(win.renderer);

        //SDL_RenderCopy(win.renderer, grass.tex.get(), NULL, &pos);

		SDL_RenderPresent(win.renderer);
	}
	return 0;
}