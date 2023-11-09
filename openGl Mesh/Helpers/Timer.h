#pragma once
#include <chrono>
#include <string>

// in micro secconds
class Timer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop_;
	std::chrono::nanoseconds duration;
public:
	void start();
	void stop();
	void end();
	long long getTime();
	void showTime(std::string name = "", bool inFrames = 0);
};
