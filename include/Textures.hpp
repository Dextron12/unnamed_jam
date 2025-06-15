#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

#include "window.hpp"

class Texture {
public:
	float w=0, h=0; // The size of the texture
	//std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> tex;
	SDL_Texture* tex;

	~Texture();
};

// Loads a texture from a file.	
std::shared_ptr<Texture> loadTexture(SDL_Renderer* renderer, const std::string& texturePath);

// Generated a pink & white checkered fall-back texture
std::shared_ptr<Texture> createFallbackTexture(SDL_Renderer* renderer, int width = 64, int height = 64, int cellSize = 8);

class TextureMngr {
public:

	//Overlaoded function: returns the shared_ptr of the texture or a
	static std::shared_ptr<Texture> resolve(SDL_Renderer* renderer, const std::string& textureID, const std::string& relPath);


	static void unload(const std::string& textureID);
	
	static bool queryTexture(const std::string& textureID);

	static std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	static std::shared_ptr<Texture> checkeredTexture;
};

class Spritesheet{
public:
	std::vector<std::string> textureIDs;

	// Assumes the suer is creating a Spritesheet so shoudl resolve inital texture. More can be added throu addTexture, whcih will check if the sttae has it cached or not.
	Spritesheet(SDL_Renderer* renderer, const std::string& textureID, const std::string& texturePath);

	void pushSubTexture(const std::string& SubTextureName, int x, int y, int w, int h);
	void pushSubTexture(const std::string& SubTextureName, SDL_Rect pxPos);

	void addSpritesheet(SDL_Renderer* renderer, std::optional<std::string> textureID = std::nullopt, std::optional<std::string> texturePath = std::nullopt);
	void remSpritesheet(const std::string& textureID, bool unloadTexture = true);

	void popSubTexture(const std::string& textureName);

	virtual void render(const std::string& SubTexture, SDL_Point destRect, std::optional<std::string> spriteID = std::nullopt);
	virtual void renderEx(const std::string& SubTexture, SDL_Point destPos, std::optional<std::string> spriteID = std::nullopt, float angle = 0.0f, SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE);

	virtual ~Spritesheet() = default;

protected:
	SDL_Renderer* renderer;

	std::unordered_map<std::string, SDL_Rect> subTextures;

	std::string resolveSpriteID(std::optional<std::string> ID);
	
};

#endif