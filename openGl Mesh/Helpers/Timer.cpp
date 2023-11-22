#include "Timer.h"
#include <iostream>

void Timer::start()
{
	start_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	stop_ = std::chrono::high_resolution_clock::now();
}

void Timer::end() {
	stop();
}

long long Timer::getTime()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(stop_ - start_).count();
}

void Timer::showTime(std::string name, bool inFrames)
{
	end();
	std::string unit = " secconds";
	float time = (float)getTime() / 1e9;
	if (inFrames) {
		time *= 60.f;
		unit = " frames";
	}
	std::cout << "Timer " + name + ": " + std::to_string(time) + unit << std::endl;
}
