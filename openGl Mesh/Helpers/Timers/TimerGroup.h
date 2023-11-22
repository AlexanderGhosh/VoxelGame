#pragma once
#include <string>
#include <vector>

class Timer;

// will produce details for the timers (assumes that all timers have the same marked slots with the same names (no more no less)
class TimerGroup
{
private:
	std::vector<Timer> timers;
	std::vector<TimerGroup> timerGroups;
	std::string name;
public:
	TimerGroup();
	TimerGroup(const std::string& name);

	void add(const Timer& timer);
	void add(const TimerGroup& timerGroup);
	void showDetails();
};

