#include "window.hpp"

std::filesystem::path VFS::basePath;

Window::Window(std::string windowTitle, float windowWidth, float windowHeight, SDL_WindowFlags windowFlags) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Failed to init SDL2" << SDL_GetError() << std::endl;
		return;
	}

	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);
	if (!window) {
		std::cerr << "Failed to create a SDL window" << SDL_GetError() << std::endl;
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)	{
		std::cerr << "Faield to create a SDL renderer" << SDL_GetError() << std::endl;
		return;
	}

	//Render controls
	SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // no smoothing (pixel perfect)
	SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

	//Init SDL_Image
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cerr << "failed to init required image formats: " << IMG_GetError() << std::endl;
		return;
	}

	width = windowWidth;
	height = windowHeight;

	appState = true;

	lastTime = SDL_GetTicks();
}

Window::~Window() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::update() {
	//Reste flags
	windowResized = false;
	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_QUIT) {
			appState = false;
		}

		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				appState = false;
			}
		}

		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				width = event.window.data1;
				height = event.window.data2;
				windowResized = true;
			}
		}
	}

	//Update dT
	currentTime = SDL_GetTicks();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
}

void VFS::init() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	basePath = std::filesystem::path(buffer).parent_path();

	//Step out of debug directory if debugging
#ifdef _DEBUG
	basePath = basePath.parent_path(); // Debug/
	basePath = basePath.parent_path(); //x64/
	//Should now be in project root dir
#endif

	// Suggested: Other OS operations to get execPath, if OS not known, it defaults to argv[0]
}

// Resolves relative paths into absolute paths. Returns an empty path on failure.
std::filesystem::path VFS::resolve(const std::string& relativePath) {
	std::filesystem::path abs;
	try {
		// Attemtps to resolve abs path, if not errors out.
		abs = std::filesystem::canonical(basePath / std::filesystem::path(relativePath).relative_path());
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cout << "The path does not exist. " << std::endl;
	}
	return abs;
}

bool VFS::exists(const std::string& path, PathType type) {
	switch (type)
	{
	case ABS_PATH:
		return std::filesystem::exists(path);
	case REL_PATH:
		return std::filesystem::exists(basePath / path);
	default:
		break;
	}

	return false;
}