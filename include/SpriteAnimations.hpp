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

// A 2D-blackbox style camera
class Camera {
public:
	SDL_FPoint pos = { 0,0 };		// camera position in world space (top-left corner of viewport)

	Camera(float x = 0.0f, float y = 0.0f);

	// Move camera by delta (for smooth movement)
	void move(float dx, float dy);

	void setPosition(float x, float y);

	//Convert world position to screen position given camera offset.
	SDL_Point worldToScreen(const SDL_FPoint& worldPos) const;

	// Convert screen position to world
	SDL_FPoint screenToWorld(const SDL_Point& screen) const;
};

class SpriteAnimation : public Spritesheet {
public:

	SpriteAnimation(SDL_Renderer* renderer, const std::string& spriteID, const std::string& texturePath);

	void addAnimation(const std::string& name, const std::vector<AnimationFrame>& frames);
	void remAnimation(const std::string& name);

	void setAnimation(const std::string& name, bool resetFrame = true, bool loop = true);

	void pause();
	void resume();

	//By default plays an animation on loop.
	void play(const SDL_Point& pos, std::optional<std::string> = std::nullopt);


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
	AnimatedPlayer(SDL_Renderer* renderer, const std::string& spriteID, const std::string& spriteSheet, SDL_FPoint initPos);

	void update(const Uint32& deltaTime);
	void render(const Camera& camera);

	void initAnimations(const std::string& AnimScript);

	SDL_FRect pos = { 0, 0, 0, 0};

protected:

	SDL_FPoint velocityVec = { 0,0 };

	const float maxSpeed = 800.0f;
	const float accelFactor = 800.0f;

	Direction facing = UP;

	Uint32 lastMoveTime = 0;
	bool asleep = false;
	Uint32 sleepDelay = 10000; // 10 seconds

};


#endif // !SPRITE_ANIMATIONS_HPP
