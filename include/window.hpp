#ifndef WINDOW_HPP
#define WINDOW_HPP

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>
#include <filesystem>

#include <iostream>
#include <Windows.h> // Used to resolve exec path.

enum PathType {
	ABS_PATH,
	REL_PATH
};

class Window {
private:
	float width = 800; float height = 600;
	SDL_Event event;

	Uint32 lastTime;
	Uint32 currentTime;

public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool appState = false;

	Uint32 deltaTime;

	Window(std::string windowTitle, float windowWidth, float windowHeight, SDL_WindowFlags windowFlags);
	~Window();
	
	void update();
};

class VFS {
public:
	//Configures the basePath for exec working dir.
	static void init();
	static std::filesystem::path resolve(const std::string& relativePath);

	//Checks if either a relative or absolute path exists
	static bool exists(const std::string& path, const PathType);

	static std::filesystem::path basePath;
};

#endif