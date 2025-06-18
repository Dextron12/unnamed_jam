#include "window.hpp"

int LOGICAL_WIDTH = 1280;
int LOGICAL_HEIGHT = 720;

std::filesystem::path VFS::basePath;

WindowContext::WindowContext(std::string windowTitle, SDL_Point windowSize, SDL_WindowFlags windowFlags, const SDL_Point logicalWindowSize) {
	// Set Logical window sizes:
	LOGICAL_WIDTH = logicalWindowSize.x;
	LOGICAL_HEIGHT = logicalWindowSize.y;
	//Set window size
	width = windowSize.x;
	height = windowSize.y;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Failed to init SDL2" << SDL_GetError() << std::endl;
		return;
	}

	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
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
	//SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // no smoothing (pixel perfect)
	SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

	//Init SDL_Image
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cerr << "failed to init required image formats: " << IMG_GetError() << std::endl;
		return;
	}

	// Set vrtScreen prop:
	virtScreen = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		LOGICAL_WIDTH, LOGICAL_HEIGHT);

	appState = true;

	lastTime = SDL_GetTicks();
}

WindowContext::~WindowContext() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void WindowContext::startFrame() {
	// Chnage the render tartext to virtual atrget
	SDL_SetRenderTarget(renderer, virtScreen);
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
	deltaTime = (currentTime - lastTime) / 1000.0f; // for heavens sakes, keep this in seconds. Physics relies on it being ins econds!
	lastTime = currentTime;
}

void WindowContext::endFrame() {
	// Rendering should have stoppeed. Change atrgets
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_RenderClear(renderer); //Clear the renderer in case of un-initalised pixels ona  window resize!

	// Calculate scale to fill window completely without letterboxxing (may crop)
	float scaleX = (float)width / LOGICAL_WIDTH;
	float scaleY = (float)height / LOGICAL_HEIGHT;
	float scale = std::max<float>(scaleX, scaleY);

	int destW = (int)(LOGICAL_WIDTH * scale);
	int destH = (int)(LOGICAL_HEIGHT * scale);

	SDL_Rect destRect;
	destRect.x = (width - destW) / 2; // centers horizontally (negative values crop)
	destRect.y = (height - destH) / 2; // centers vertically (negative valeus crop)

	destRect.w = destW;
	destRect.h = destH;

	// Render the scaled texture tot he window
	SDL_RenderCopy(renderer, virtScreen, nullptr, &destRect);
	SDL_RenderPresent(renderer);
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