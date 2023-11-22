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

void Timer::normaliseNameSizes(std::string& a)
{
	unsigned int max_ = name.size();
	for (const auto& mp : markedPoints) {
		max_ = std::max(mp.name.size(), max_);
	}
	while (name.size() < max_) {
		name += " ";
	}
	while (a.size() < max_) {
		a += " ";
	}
	for (auto& mp : markedPoints) {
		while (mp.name.size() < max_) {
			mp.name += " ";
		}
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
	MarkPoint mp;
	mp.name = name;
	mp.duration = getTime();
	markedPoints.push_back(mp);
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

	std::string avg_name = "Average";
	normaliseNameSizes(avg_name);

	std::cout << std::setprecision(3) << std::fixed;
	std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << name << ": " << totalSecconds << " Secconds | " << totalFrames << " Frames" << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	double prevDuration = 0;
	for (unsigned int i = 0; i < markedPoints.size(); i++) {
		const MarkPoint& mp = markedPoints[i];
		double currentDuration = mp.duration - prevDuration;
		prevDuration = mp.duration;
		double percent = currentDuration;
		percent /= total;
		percent *= 100.f;
		double secconds = currentDuration * 1e-9;
		double frames = secconds * 60.;
		std::cout << mp.name << ": " << secconds << " Secconds | " << frames << " Frames | " << percent << "%" << std::endl;		
	}
	double avgSecconds = totalSecconds / (double)ammount;
	double avgFrames = totalFrames / (double)ammount;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << avg_name << ": " << avgSecconds << " Secconds | " << avgFrames << " Frames | Count: " << ammount << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
	markedPoints.clear();
}
