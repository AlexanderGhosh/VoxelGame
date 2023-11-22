#include "Timer.h"
#include <iostream>
#include <common.hpp>

Timer::Timer() : name(), start_(), stop_(), markedPoints()
{
}

void Timer::printTime(std::string name, bool inFrames) const
{
	std::string unit = " secconds";
	double time = (double)getTime() / 1e9;
	if (inFrames) {
		time *= 60.f;
		unit = " frames";
	}
	std::cout << "Timer " + name + ": " + std::to_string(time) + unit << std::endl;
}

unsigned int Timer::maxSize()
{
	unsigned int max_ = std::max(name.size(), std::string("Average").size());
	for (const auto& [name, _] : markedPoints) {
		max_ = std::max(max_, name.size());
	}
	return max_;
}

void Timer::normaliseNameSize(std::string& a, unsigned int size)
{
	while (a.size() < size) {
		a += " ";
	}
}

Timer::Timer(const std::string& name) : Timer()
{
	this->name = name;
	start();
}

void Timer::reName(const std::string& newName)
{
	name = newName;
}

void Timer::start()
{
	start_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	stop_ = std::chrono::high_resolution_clock::now();
}

void Timer::log(std::string name, bool inFrames)
{
	mark(name);
	printTime(name, inFrames);
}

void Timer::mark(std::string name)
{
	stop();
	markedPoints.emplace(name, getTime());
}

long long Timer::getTime() const
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(stop_ - start_).count();
}

void Timer::showTime(bool inFrames)
{
	stop();
	printTime(name, inFrames);
	markedPoints.clear();
}

void Timer::showDetails(unsigned int ammount)
{
	stop();
	double total = getTime();
	double totalSecconds = total * 1e-9;
	double totalFrames = totalSecconds * 60.;

	const unsigned int maxSize = this->maxSize();
	normaliseNameSize(name, maxSize);
	std::string avg_name = "Average";
	normaliseNameSize(avg_name, maxSize);

	std::cout << std::setprecision(3) << std::fixed;
	std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << name << ": " << totalSecconds << " Secconds | " << totalFrames << " Frames" << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	double prevDuration = 0;
	for (const auto& [mp_name, mp_duration] : markedPoints) {
		double currentDuration = mp_duration - prevDuration;
		prevDuration = mp_duration;
		double percent = currentDuration;
		percent /= total;
		percent *= 100.f;
		double secconds = currentDuration * 1e-9;
		double frames = secconds * 60.;
		std::string mp_nameCopy(mp_name);
		normaliseNameSize(mp_nameCopy, maxSize);
		std::cout << mp_nameCopy << ": " << secconds << " Secconds | " << frames << " Frames | " << percent << "%" << std::endl;
	}
	double avgSecconds = totalSecconds / (double)ammount;
	double avgFrames = totalFrames / (double)ammount;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << avg_name << ": " << avgSecconds << " Secconds | " << avgFrames << " Frames | Count: " << ammount << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
	markedPoints.clear();
}
