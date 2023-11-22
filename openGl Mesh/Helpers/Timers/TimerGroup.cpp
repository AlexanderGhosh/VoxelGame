#include "TimerGroup.h"
#include "Timer.h"
#include <iostream>

TimerGroup::TimerGroup() : name(), timers()
{
}

TimerGroup::TimerGroup(const std::string& name) : TimerGroup()
{
	this->name = name;
}

void TimerGroup::add(const Timer& timer)
{
	timers.push_back(timer);
}

void TimerGroup::add(const TimerGroup& timerGroup)
{
	timerGroups.push_back(timerGroup);
}

void TimerGroup::showDetails()
{
	double size = 0;
	double total = 0;
	unsigned int maxSize = name.size();
	std::unordered_map<std::string, long long> sections;
	for (const Timer& t : timers) {
		for (const auto& [name, duration] : t.markedPoints) {
			sections[name] += duration;
			total += duration;
			maxSize = std::max(maxSize, name.size());
		}
	}
	size += timers.size();
	for (const TimerGroup& tg : timerGroups) {
		size += tg.timers.size();
		for (const Timer& t : tg.timers) {
			for (const auto& [name, duration] : t.markedPoints) {
				sections[name] += duration;
				total += duration;
				maxSize = std::max(maxSize, name.size());
			}
		}
	}
	double totalSecconds = total * 1e-9;
	double totalFrames = totalSecconds * 60.;

	Timer::normaliseNameSize(name, maxSize);

	std::cout << std::setprecision(3) << std::fixed;
	std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << name << ": " << totalSecconds << " Secconds | " << totalFrames << " Frames" << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	double prevDuration = 0;
	for (const auto& [mp_name, mp_duration] : sections) {
		double currentDuration = mp_duration - prevDuration;
		prevDuration = mp_duration;
		double percent = currentDuration;
		percent /= total;
		percent *= 100.f;
		double avgSecconds = (currentDuration * 1e-9) / size;
		double avgFrames = (avgSecconds * 60.) / size;
		std::string mp_nameCopy(mp_name);
		Timer::normaliseNameSize(mp_nameCopy, maxSize);
		std::cout << mp_nameCopy << ": Average: " << avgSecconds << " Secconds | " << avgFrames << " Frames | " << percent << "%" << std::endl;
	}
	double avgSecconds = totalSecconds / size;
	double avgFrames = totalFrames / size;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << ": Average: " << avgSecconds << " Secconds | " << avgFrames << " Frames | Count: " << timers.size() << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;

}
