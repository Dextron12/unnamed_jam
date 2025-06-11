#include "SpriteAnimations.hpp"

SpriteAnimation::SpriteAnimation(SDL_Renderer* renderer, const std::string& texturePath) : Spritesheet(renderer, texturePath), timer(Timer(0.0)), currentAnimation(std::string("")) {}

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

void SpriteAnimation::play(const SDL_Point& pos) {
	auto it = m_frames.find(currentAnimation);
	if (it == m_frames.end()) {
		std::printf("Animation '%s' not found!\n", currentAnimation.c_str());
		return;
	}

	const auto& frames = it->second;

	if (frames.empty()) return;

	const AnimationFrame& frame = frames[currentFrame];

	//Sync timer to this frames duration.
	timer.setDuration(frame.duration);

	//Src & destRect's
	const SDL_Rect& srcRect = frame.rect;
	const SDL_Rect destRect{ pos.x, pos.y, srcRect.w, srcRect.h };

	//Render sprite
	SDL_RenderCopyEx(renderer, texture->tex.get(), &srcRect, &destRect, NULL, NULL, frame.flip);

	//Advance to jnext frame if timer has expired
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

void SpriteAnimation::setAnimation(const std::string& name, bool resetFrame, bool loop, SDL_RendererFlip flipFlag) {
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

	isPlaying = true;
	flipState = flipFlag; // If changed from NONE, the picture will flip on that axis.
}

void SpriteAnimation::pause() {
	isPlaying = false;
}

void SpriteAnimation::resume() {
	isPlaying = true;
}

void loadAnimationsFromJSON(const std::string& jsonPath, SpriteAnimation& anim) {
	std::ifstream file(jsonPath);
	nlohmann::json j;
	file >> j;

	// Read optional default duration for the animation
	int defaultDuration = 100;
	if (j.contains("defualt_duration") && j["default_duration"].is_number()) {
		defaultDuration = j["default_duration"];
	}

	// Read optional flip flag
	if (j.contains("spriteOnFlip") && j["spriteOnFlip"].is_boolean()) {
		anim.flipState = flipFromString(j["spriteOnFlip"]);
	}

	for (auto& [animName, frames] : j.items()) {
		std::vector<AnimationFrame> frameVec;

		for (auto& frame : frames) {
			AnimationFrame af;
			af.rect.x = frame["x"].get<int>();
			af.rect.y = frame["y"].get<int>();
			af.rect.w = frame["w"].get<int>();
			af.rect.h = frame["h"].get<int>();
			af.duration = frame["duration"].get<int>();

			frameVec.push_back(af);
		}

		anim.addAnimation(animName, frameVec);
	}
}