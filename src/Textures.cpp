#include "Textures.hpp"

void dummyTest() {
    std::printf("TextureLoader.cpp is compiled\n");
}

Texture loadTexture(SDL_Renderer* renderer, const std::string& texturePath) {
    Texture obj;

    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << texturePath << std::endl;
        return Texture{};
    }

    SDL_Texture* texPtr = SDL_CreateTextureFromSurface(renderer, surface);
    obj.w = static_cast<float>(surface->w);
    obj.h = static_cast<float>(surface->h);
    SDL_FreeSurface(surface);

    if (!texPtr) {
        std::cerr << "Failed to convert surface to texture: " << texturePath << std::endl;
        return Texture{};
    }

    // Store the texture in the unique_ptr with SDL_DestroyTexture deleter
    obj.tex.reset(texPtr);

    return obj;
}