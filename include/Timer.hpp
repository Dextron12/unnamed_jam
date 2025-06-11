#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer {
public:
	Timer(int durationMs);

	void reset();

	bool isFinished() const;
	int getElapsedMs() const;
	float getProgress() const;

	void setDuration(int newDurationMs);

private:
	int duration;
	std::chrono::steady_clock::time_point start;
};

#endif