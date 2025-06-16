#include "Textures.hpp"

std::unordered_map<std::string, std::shared_ptr<Texture>> TextureMngr::textures;
std::shared_ptr<Texture> TextureMngr::checkeredTexture = nullptr;

Texture::~Texture() {
    SDL_DestroyTexture(tex);
}

std::shared_ptr<Texture> loadTexture(SDL_Renderer* renderer, const std::string& texturePath) {
    auto obj = std::make_shared<Texture>();

    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << texturePath << std::endl;
        return nullptr;
    }

    SDL_Texture* texPtr = SDL_CreateTextureFromSurface(renderer, surface);
    obj->w = static_cast<float>(surface->w);
    obj->h = static_cast<float>(surface->h);
    SDL_FreeSurface(surface);

    if (!texPtr) {
        std::cerr << "Failed to convert surface to texture: " << texturePath << std::endl;
        return nullptr;
    }

    // Store the texture
    obj->tex = texPtr;

    return obj;
}

std::shared_ptr<Texture> createFallbackTexture(SDL_Renderer* renderer, int width, int height, int cellSize) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif
    );
    auto obj = std::make_shared<Texture>();

    if (!surface) {
        std::cout << "[Fatal-Error]: Failed to create a fallback surface: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    //Pink & White colours
    Uint32 pink = SDL_MapRGB(surface->format, 255, 0, 255);
    Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);

    //Fill the surface with checker pattern
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool isPink = ((x / cellSize) + (y / cellSize)) % 2 == 0;
            Uint32 color = isPink ? pink : white;

            Uint32* pixels = (Uint32*)surface->pixels;
            pixels[(y * surface->w) + x] = color;
        }
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    //Assign texture size:
    obj->w = surface->w; obj->h = surface->h;
    SDL_FreeSurface(surface);

    if (!texture) {
        std::printf("[Fatal-Error]: Failed to create a fallback texture: %s\n", SDL_GetError());
        return nullptr;;
    }

    obj->tex = texture;
    
    return obj;


}


void TextureMngr::unload(const std::string& textureID) {
    if (textures.find(textureID) == textures.end()) {
        //No need to output a warning. The end-user is expecting the texture to be removed anyway.
        return;
    }

    textures.erase(textureID); // Does the internal SDL_Texture get freed by Texture destructor?
    return;
}

std::shared_ptr<Texture> TextureMngr::resolve(SDL_Renderer* renderer, const std::string& textureID, const std::string& filePath) {
    //Requires a renderer context!
    if (renderer == nullptr) {
        //std::printf("[Error]: Global Texture Manager has no assigned renderer!\n");
        return nullptr;
    }

    // Check to see if a fall-back texture has been generated, if not generate one now!
    if (checkeredTexture == nullptr) {
        checkeredTexture = createFallbackTexture(renderer, 32, 32);
    }

    // check the cache for a loaded texture:
    if (textures.find(textureID) != textures.end()) {
        // One exists, dont relaod it.
        return textures[textureID];
    }

    //No cached texture, load it now.
    // Load texture and confirm state
    std::shared_ptr<Texture> textPtr = loadTexture(renderer, filePath);
    if (textPtr == nullptr || textPtr->tex == nullptr) {
#ifdef _DEBUG
        std::printf("[Error]: Failed to load '%s', defaulting to checkered texture\n", filePath.c_str());
#endif
        return nullptr;
    }

    // Cache the newly loaded texture for future use.
    textures[textureID] = std::move(textPtr);

    return textures[textureID];
}

bool TextureMngr::queryTexture(const std::string& textureID) {
    if (textures.find(textureID) != textures.end()) {
        return true;
    }
    else { return false; }
}


Spritesheet::Spritesheet(SDL_Renderer* renderer, const std::string& texID, const std::string& texturePath) : renderer(renderer) {
    //texture = std::make_unique<Texture>(loadTexture(renderer, texturePath));
    if (TextureMngr::queryTexture(texID)) {
        // Texture exists, no need to resolve it.
        textureIDs.push_back(texID);
    }
    else {
        // Can't find texture, resolve it
        if (TextureMngr::resolve(renderer, texID, texturePath)) {
            textureIDs.push_back(texID); // pushes back the textureID if the state can resolve the texture. Otherwise, quietly ignores the texture.
        }
        else {
#ifdef _DEBUG
            std::printf("[WARNING]: Spritesheet texture '%s' failed to resolve. Spritesheet will have no texture, add one through addSpritehseet()", texID.c_str());
#endif
        }
    }
}

std::string Spritesheet::resolveSpriteID(std::optional<std::string> ID) {
    return ID.value_or(textureIDs.empty() ? "" : textureIDs[0]);
}



void Spritesheet::addSpritesheet(SDL_Renderer* renderer, std::optional<std::string> textureID, std::optional<std::string> texturePath) {
    if (textureID.has_value()) {
        const std::string& id = textureID.value();

        if (TextureMngr::queryTexture(id)) {
            textureIDs.push_back(id);
            return;
        }

        if (texturePath.has_value()) {
            const std::string& path = texturePath.value();
            if (TextureMngr::resolve(renderer, id, path) != nullptr) {
                textureIDs.push_back(id);
                return;
            }
        }

#ifdef _DEBUG
        std::printf("[Texture Debug]: Failed to resolve and add textureID: '%s': Path: '%s'\n",
            id.c_str(),
            texturePath.value_or("<null>").c_str());
#endif
    }

#ifdef _DEBUG
    std::printf("[Spritesheet Error]: Cannot resolve texture '%s', check params\n",
        textureID.value_or("<null>").c_str());
#endif
}


void Spritesheet::remSpritesheet(const std::string& textureID, bool unloadTexture) {
    // Attempts to handle textureID abuse or overuse of thsi function
    auto it = std::find(textureIDs.begin(), textureIDs.end(), textureID);
    if (it != textureIDs.end()) {
        // textureID exists, remove it and optionally unlaod the texture
        textureIDs.erase(it);
        if (unloadTexture) {
            TextureMngr::unload(textureID);
        }
        return;
    }

#ifdef _DEBUG
    std::printf("[DEBUG_STATUS]: remSpritesheet is being called with an invalid textureID. Source of optimization\n");
#endif
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

void Spritesheet::render(const std::string& name, SDL_Point destRect, std::optional<std::string> spriteID){

    // Allow end-user to change spritesheets. There is no check for subTextures, so the end-suer needs to eb careful to choose the right one for the spriteID

    std::string texID;
    if (spriteID == std::nullopt) {
        //No spriteID specified, use default
        texID = resolveSpriteID(spriteID);
    }
    else {
        texID = resolveSpriteID(name);
    }


    // Render a part of the requested spritesheet, no attempt is made by the program to ensure subTextures are rendered per their given spritesheet, sot eh end-user must eb careful if using mutliple spritesheets in one object.

    // This function assumes, the end-user knows what they are doing. There are no checks to see if the user is rendering the correct sprite, other than blindly rendering the requested sprite.
    // All textures run through the texture resolve protocol and are either erused or lazy-laoded if the texture has sicne been dumped. On failure of lazy-laod a fallback checkered texture is used.
    auto& tex = TextureMngr::resolve(renderer, texID, "");
    if (tex == nullptr) {
#ifdef _DEBUG
        std::printf("[DEBUG]: The texture '%s' is invalid\n", texID.c_str());
        return;
#endif
    }

    if (tex->tex == nullptr) {
#ifdef _DEBUG 
        std::printf("Failed to resolve texture: '%s'. Defaulting to fallback texture\n", texID.c_str());
        
        //Check fi a fallback texture has already been generatied, if not, generate.
        if (TextureMngr::checkeredTexture == nullptr) {
            TextureMngr::checkeredTexture = createFallbackTexture(renderer, 32, 32);
        }
        tex = TextureMngr::checkeredTexture;
#endif
    }

    const SDL_Rect* subR;
    SDL_Rect pos;
    
    auto it = subTextures.find(name);
    if (it != subTextures.end()) {
            // A subtexture exists, assign it
            subR = &it->second;
            pos = { destRect.x, destRect.y, subR->w, subR->h };
    }
    else {
        // No subTexture, use full sprite surface
        subR = nullptr;
        pos = { destRect.x, destRect.y, (int)tex->w, (int)tex->h };
    }

    SDL_RenderCopy(renderer, tex->tex, subR, &pos);

}

void Spritesheet::renderEx(const std::string& name, SDL_Point destPos, std::optional<std::string> spriteID, float angle, SDL_RendererFlip flip){
    std::string texID = resolveSpriteID(spriteID);

    if (subTextures.count(name) != 0) {
        SDL_Rect subR = subTextures[name];
        SDL_Rect pos = { destPos.x, destPos.y, subR.x, subR.y };


        auto tex = TextureMngr::resolve(nullptr, texID, "");

        // Render fall-back texture if texture obj is invalid
        if (tex == nullptr) {
            if (TextureMngr::checkeredTexture == nullptr) {
                // generate one
                tex = createFallbackTexture(renderer, 32, 32);
            }
            else {
                tex = TextureMngr::checkeredTexture;
            }
        }
#ifdef _DEBUG
        std::printf("[DEBUG_STATUS]: Using fall-back texture for sprite(sheet) '%s'\n", texID.c_str());
#endif

        //Render Sprite
        SDL_RenderCopyEx(renderer, tex->tex, &subR, &pos, angle, NULL, flip);
    }

}