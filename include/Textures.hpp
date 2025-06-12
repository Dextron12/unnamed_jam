#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <SDL2/SDL.h>
#include <SDL2\SDL_image.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

class Texture {
public:
	float w=0, h=0; // The size of the texture
	std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> tex;

	Texture() : tex(nullptr, SDL_DestroyTexture) {}

};

class TextureMngr {
public:

	//Either returns a laoded texture object, if not loaded, will lazy laod the texture (searches in Assets/) if no texture can be located it will return nullptr
	static std::shared_ptr<Texture> resolve(const std::string& relPath);

	static void unload(const std::string& textureID);

protected:
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};

class Spritesheet{
public:
	std::shared_ptr<Texture> texture;

	Spritesheet(SDL_Renderer* renderer, const std::string& texturePath);

	void pushSubTexture(const std::string& SubTextureName, int x, int y, int w, int h);
	void pushSubTexture(const std::string& SubTextureName, SDL_Rect pxPos);

	void popSubTexture(const std::string& textureName);

	void render(const std::string& SubTexture, SDL_Point destRect);
	void renderEx(const std::string& SubTexture, SDL_Point destPos, float angle = 0.0f, SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE);

protected:
	SDL_Renderer* renderer;

	std::unordered_map<std::string, SDL_Rect> subTextures;
	
};

// Loads a texture from a file.	
Texture loadTexture(SDL_Renderer* renderer, const std::string& texturePath);

#endif