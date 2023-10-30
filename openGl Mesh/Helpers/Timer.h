#pragma once
#include <chrono>
#include <string>

class Timer {
private:
	std::chrono::time_point<std::chrono::steady_clock> start_;
	std::chrono::time_point<std::chrono::steady_clock> stop_;
	std::chrono::microseconds duration;
public:
	void start();
	void stop();
	void end();
	unsigned long getTime();
	void showTime(std::string name = "", bool inFrames = 0);
};
