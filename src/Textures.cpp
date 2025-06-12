#include "Textures.hpp"

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

void TextureMngr::setRenderer(SDL_Renderer* rendererPtr) {
    renderer = rendererPtr;
}

void TextureMngr::unload(const std::string& textureID) {
    if (textures.find(textureID) == textures.end()) {
        //No need to output a warning. The end-user is expecting the texture to be removed anyway.
        return;
    }

    textures.erase(textureID); // Does the internal SDL_Texture get freed by Texture destructor?
    return;
}

Spritesheet::Spritesheet(SDL_Renderer* renderer, const std::string& texturePath) : renderer(renderer){
    texture = std::make_unique<Texture>(loadTexture(renderer, texturePath));
}

void Spritesheet::pushSubTexture(const std::string& SubTextureName, int x, int y, int w, int h){
    SDL_Rect px = SDL_Rect{x,y,w,h};
    subTextures.emplace(SubTextureName, px);
}

void Spritesheet::pushSubTexture(const std::string& SubTextureName, SDL_Rect pxPos){
    subTextures.emplace(SubTextureName, pxPos);
}

void Spritesheet::popSubTexture(const std::string& textureName){
    subTextures.erase(textureName);
}

void Spritesheet::render(const std::string& name, SDL_Point destRect){
    if (subTextures.count(name) != 0) {
        SDL_Rect subR = subTextures[name];
        SDL_Rect pos = SDL_Rect{ destRect.x, destRect.y, subR.w, subR.h };
        SDL_RenderCopy(renderer, texture->tex.get(), &subR, &pos);
    }
    else {
        std::printf("%s is not a valid subTexture", name.c_str());
    }
}

void Spritesheet::renderEx(const std::string& name, SDL_Point destPos, float angle, SDL_RendererFlip flip){
    if (subTextures.count(name) != 0) {
        SDL_Rect subR = subTextures[name];
        SDL_Rect pos = SDL_Rect{ destPos.x, destPos.y, subR.w, subR.h };
        SDL_RenderCopyEx(renderer, texture->tex.get(), &subR, &pos, angle, NULL, flip);
    }
    else {
        std::printf("%s is not a valid Subtexture", name.c_str());
    }
}