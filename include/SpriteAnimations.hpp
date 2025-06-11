#ifndef SPRITE_ANIMATIONS_HPP
#define SPRITE_ANIMATIONS_HPP

#include "Textures.hpp"
#include "Timer.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

struct AnimationFrame {
	SDL_Rect rect;
	int duration;
	SDL_RendererFlip flip;
};

SDL_RendererFlip flipFromString(const std::string& str) {
	if (str == "horizontal") return SDL_FLIP_HORIZONTAL;
	if (str == "vertical") return SDL_FLIP_VERTICAL;
	if (str == "both") return (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	return SDL_FLIP_NONE;

}

class SpriteAnimation : public Spritesheet {
public:

	SpriteAnimation(SDL_Renderer* renderer, const std::string& texturePath);

	void addAnimation(const std::string& name, const std::vector<AnimationFrame>& frames);
	void remAnimation(const std::string& name);

	void setAnimation(const std::string& name, bool resetFrame = false, bool loop = true, SDL_RendererFlip flipFlag = SDL_FLIP_NONE);

	void pause();
	void resume();

	//By default plays an animation on loop.
	void play(const SDL_Point& pos);


private:
	std::unordered_map<std::string, std::vector<AnimationFrame>> m_frames;

	Timer timer;

	std::string currentAnimation;
	size_t currentFrame = 0;
	bool isPlaying = true;
	bool isLooping = true;

	SDL_RendererFlip flipState = SDL_FLIP_NONE; // Used to set the flip orientation. Set it through setAnimation
};

// Capable of laoding animation data from a JSON file and inserting it into the given AnimatedSprite
void loadAnimationsFromJSON(const std::string& jsonPath, SpriteAnimation& anim);

#endif // !SPRITE_ANIMATIONS_HPP
