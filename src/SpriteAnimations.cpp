#include "SpriteAnimations.hpp"

SDL_RendererFlip flipFromString(const std::string& str) {
	if (str == "horizontal") return SDL_FLIP_HORIZONTAL;
	if (str == "vertical") return SDL_FLIP_VERTICAL;
	if (str == "both") return (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	return SDL_FLIP_NONE;

}

SpriteAnimation::SpriteAnimation(SDL_Renderer* renderer, const std::string& spriteID, const std::string& texturePath) : Spritesheet(renderer, spriteID, texturePath), timer(Timer(0.0)), currentAnimation(std::string("")) {}

void SpriteAnimation::addAnimation(const std::string& name, const std::vector<AnimationFrame>& frames) {
	if (m_frames.find(name) != m_frames.end()) {
		std::printf("Animation '%s' already exists!\n", name.c_str());
	}
	else {
		//Create a new animation sequence
		m_frames[name] = frames;
	}
	// Optionally, can add overwrite flag for flexibility. So that the animations can be changed.
}

void SpriteAnimation::remAnimation(const std::string& name) {
	if (m_frames.find(name) != m_frames.end()) {
		//Frame exists, remvoe it.
		m_frames.erase(name);
	}
	else {
		std::printf("Aniamtion '%s' was not found. Delete operation cannot be completed\n", name.c_str());
	}
}

void SpriteAnimation::play(const SDL_Point& pos, std::optional<std::string> spriteID) {
	auto it = m_frames.find(currentAnimation);
	if (it == m_frames.end()) {
		//std::printf("Animation '%s' not found!\n", currentAnimation.c_str());
		return;
	}

	std::string texID = resolveSpriteID(spriteID);

	const auto& frames = it->second;

	if (frames.empty()) return;

	if (currentFrame >= frames.size()) {
		currentFrame = frames.size() - 1; // Clamp the frameSize if end-user isnt resetting ebtween frmaes and we switch to a smaller animation size.
	}

	const AnimationFrame& frame = frames[currentFrame];

	//Sync timer to this frames duration.
	timer.setDuration(frame.duration);

	//Src & destRect's
	const SDL_Rect& srcRect = frame.rect;
	const SDL_Rect destRect{ pos.x, pos.y, srcRect.w, srcRect.h };

	//Render sprite
	auto tex = TextureMngr::resolve(renderer, texID, "");
	SDL_RenderCopyEx(renderer, tex->tex, &srcRect, &destRect, NULL, NULL, frame.flip);

	//Advance to next frame if timer has expired
	if (isPlaying && timer.isFinished()) {
		currentFrame++;
		if (isLooping) {
			currentFrame %= frames.size(); //wrap around
		}
		else if (currentFrame >= frames.size()) {
			currentFrame = frames.size() - 1; // Stop at last frame
			isPlaying = false;
		}

		timer.reset();
	}
}

void SpriteAnimation::setAnimation(const std::string& name, bool resetFrame, bool loop) {
	if (m_frames.find(name) == m_frames.end()) {
		std::printf("Animation '%s' not found!\n", name.c_str());
		return;
	}

	if (name != currentAnimation) {
		currentAnimation = name;
		if (resetFrame) {
			currentFrame = 0;
		}
		timer.reset();
		isLooping = loop;
	}

	if (currentFrame >= m_frames[name].size()) {
		currentFrame = 0;
	}

	isPlaying = true;
}

void SpriteAnimation::pause() {
	isPlaying = false;
}

void SpriteAnimation::resume() {
	isPlaying = true;
}

void loadAnimationsFromJSON(const std::string& jsonPath, SpriteAnimation& anim) {
	std::ifstream file(jsonPath);
	if (!file.is_open()) {
		std::printf("Could not open JSON Animation file: '%s'", jsonPath.c_str());
		return;
	}

	nlohmann::json j;
	file >> j;

	const int globalDefaultDuration = 100; //fallabck if no duration anywhere

	if (!j.contains("animations") || !j["animations"].is_object()) {
		std::printf("Animation Script('%s') is missing 'animations' object", std::filesystem::path(jsonPath).filename().string().c_str());
		return;
	}

	for (auto& [animName, animData] : j["animations"].items()) {
		std::string spriteOnFlip;
		SDL_RendererFlip flip = SDL_FLIP_NONE;

		// Read optional animation-wide default duration
		int animDefaultDuration = globalDefaultDuration;
		if (animData.contains("duration") && animData["duration"].is_number()) {
			animDefaultDuration = animData["duration"];
		}

		//Read optional flip
		if (animData.contains("sprite_flip") && animData["sprite_flip"].is_string()) {
			flip = flipFromString(animData["sprite_flip"].get<std::string>());
		}

		if (!animData.contains("frames") || !animData["frames"].is_array()) {
			std::printf("Animation '%s' is missing 'frames' array", animName.c_str());
			return;
		}

		std::vector<AnimationFrame> frameVec;
		for (auto& frame : animData["frames"]) {
			AnimationFrame af;

			af.rect.x = frame.value("x", 0);
			af.rect.y = frame.value("y", 0);
			af.rect.w = frame.value("w", 0);
			af.rect.h = frame.value("h", 0);

			//Use per-frame duration if exists, else animation default, esle globald default
			if (frame.contains("duration") && frame["duration"].is_number()) {
				af.duration = frame.value("duration", globalDefaultDuration);
			}
			else {
				af.duration = animDefaultDuration;
			}

			af.flip = flip; // Persist flip data (per sequence, player already set up to handle it)

			frameVec.push_back(af);
		}

		anim.addAnimation(animName, frameVec);
	}
}

AnimatedPlayer::AnimatedPlayer(SDL_Renderer* renderer, const std::string& spriteID, const std::string& spritePath, SDL_FPoint initPos) : SpriteAnimation(renderer, spriteID, spritePath), pos({ initPos.x, initPos.y, 0.0f, 0.0f }) {
}

void AnimatedPlayer::initAnimations(const std::string& AnimScript) {
	loadAnimationsFromJSON(AnimScript, *this);
}

void AnimatedPlayer::update(const Uint32& deltaTime) {

	//Get current tiem
	Uint32 now = SDL_GetTicks();

	//Get keyboard state
	const auto* keys = SDL_GetKeyboardState(NULL);

	bool up = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
	bool down = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
	bool left = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
	bool right = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];

	SDL_FPoint acceleration = { 0, 0 };

	Direction newFacing = facing;

	if (up)    acceleration.y -= 1;
	if (down)  acceleration.y += 1;
	if (left)  acceleration.x -= 1;
	if (right) acceleration.x += 1;

	// Determine facing direction by last direction key pressed (prioritize vertical > horizontal)
	if (up)         newFacing = Direction::UP;
	else if (down)  newFacing = Direction::DOWN;
	else if (left)  newFacing = Direction::LEFT;
	else if (right) newFacing = Direction::RIGHT;


	if (newFacing != facing) {
		facing = newFacing;
	}

	//Normalise acceleration then multiply by speed
	float accelFactor = 0.0006f;
	velocityVec.x += acceleration.x * accelFactor * deltaTime;
	velocityVec.y += acceleration.y * accelFactor * deltaTime;

	//Dampen movement
	velocityVec.x *= 0.85f;
	velocityVec.y *= 0.85f;

	pos.x += velocityVec.x * deltaTime;
	pos.y += velocityVec.y * deltaTime;

	bool moving = (acceleration.x != 0 || acceleration.y != 0);

	// Choose animation based on direction and movement
	if (moving) {
		lastMoveTime = now;
		switch (facing) {
		case Direction::UP: setAnimation("walk_backward"); break;
		case Direction::DOWN: setAnimation("walk_forward"); break;
		case Direction::LEFT: setAnimation("walk_left"); break;
		case Direction::RIGHT: setAnimation("walk_right"); break;
		}
	}
	else if (asleep) {
		switch (facing) {
		case UP: setAnimation("sleep_front"); break;
		case DOWN: setAnimation("sleep_front"); break;
		case LEFT: setAnimation("sleep_left"); break;
		case RIGHT: setAnimation("sleep_right"); break;
		}
	}
	else {
		switch (facing) {
		case Direction::UP: setAnimation("idle_backward"); break;
		case Direction::DOWN: setAnimation("idle_forward"); break;
		case Direction::LEFT: setAnimation("idle_left"); break;
		case Direction::RIGHT: setAnimation("idle_right"); break;
		}
	}

	if (!asleep && (now - lastMoveTime > sleepDelay)) {
		asleep = true;
	}
}

void AnimatedPlayer::render(const std::string& animID) {

	play({ (int)pos.x, (int)pos.y });
}