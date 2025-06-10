#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <SDL2/SDL.h>
#include <SDL2\SDL_image.h>

#include <iostream>
#include <string>
#include <memory>

void dummyTest();

class Texture {
public:
	float w=0, h=0; // The size of the texture
	std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> tex;

	Texture() : tex(nullptr, SDL_DestroyTexture) {}

};

// Loads a texture from a file.	
Texture loadTexture(SDL_Renderer* renderer, const std::string& texturePath);

#endif