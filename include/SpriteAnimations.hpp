#ifndef SPRITE_ANIMATIONS_HPP
#define SPRITE_ANIMATIONS_HPP

#include "Textures.hpp"
#include "Timer.hpp"
#include "window.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

struct AnimationFrame {
	SDL_Rect rect;
	int duration;
	SDL_RendererFlip flip;
};

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

SDL_RendererFlip flipFromString(const std::string& str);

class SpriteAnimation : public Spritesheet {
public:

	SpriteAnimation(SDL_Renderer* renderer, const std::string& texturePath);

	void addAnimation(const std::string& name, const std::vector<AnimationFrame>& frames);
	void remAnimation(const std::string& name);

	void setAnimation(const std::string& name, bool resetFrame = true, bool loop = true);

	void pause();
	void resume();

	//By default plays an animation on loop.
	void play(const SDL_Point& pos);


protected:
	std::unordered_map<std::string, std::vector<AnimationFrame>> m_frames;

	Timer timer;

	std::string currentAnimation;
	size_t currentFrame = 0;
	bool isPlaying = true;
	bool isLooping = true;
};

// Capable of laoding animation data from a JSON file and inserting it into the given AnimatedSprite
void loadAnimationsFromJSON(const std::string& jsonPath, SpriteAnimation& anim);


class AnimatedPlayer : public SpriteAnimation {
public:
	AnimatedPlayer(SDL_Renderer* renderer, const std::string& spriteSheet, SDL_FPoint initPos);

	void update(const Uint32& deltaTime);
	void render(const std::string& animID);

	void initAnimations(const std::string& AnimScript);

	SDL_FPoint mapOffset; // Camera offset.

protected:
	SDL_FRect pos;

	SDL_FPoint velocityVec = { 0,0 };

	Direction facing = UP;

	Uint32 lastMoveTime = 0;
	bool asleep = false;
	Uint32 sleepDelay = 10000; // 10 seconds

};


#endif // !SPRITE_ANIMATIONS_HPP
