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

unsigned long Timer::getTime()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(stop_ - start_).count();
}

void Timer::showTime(std::string name, bool inFrames)
{
	std::string unit = " secconds";
	float time = (float)getTime() / 1000000.0f;
	if (inFrames) {
		time /= (1.0f / 60.0f);
		unit = " frames";
	}
	std::cout << "Timer " + name + ": " + std::to_string(time) + unit << std::endl;
}
