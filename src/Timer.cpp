#include "Timer.hpp"

Timer::Timer(int durationMs) : duration(durationMs), start(std::chrono::steady_clock::now()) {}

void Timer::reset() {
	start = std::chrono::steady_clock::now();
}

bool Timer::isFinished() const {
	return getElapsedMs() >= duration;
}

int Timer::getElapsedMs() const {
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

float Timer::getProgress() const {
	return (float)getElapsedMs() / duration;
}

void Timer::setDuration(int newDuration) {
	//int elapsed = getElapsedMs();
	// Maintains the same start time, changes how long the timer waits to finish.
	duration = newDuration;
}