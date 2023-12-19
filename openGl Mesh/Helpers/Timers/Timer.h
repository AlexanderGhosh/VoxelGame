#pragma once
#include <chrono>
#include <string>
#include <map>

// in nano secconds
// asumes 60 fps
class TimerGroup;
class Timer {
	friend class TimerGroup;
private:
	std::string name;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop_;

	std::map<std::string, unsigned long long> markedPoints;

	void printTime(std::string name, bool inFrames) const;
	unsigned int maxSize();
	// pads all a untill its size is == size
	static void normaliseNameSize(std::string& a, unsigned int size);
public:
	Timer();
	// creates a timer and starts it
	Timer(const std::string& name);

	void reName(const std::string& newName);
	// starts the timer
	void start();
	// stops the timer
	void stop();
	// ouputs the time and continues
	void log(std::string name, bool inFrames = 0);
	// marks this time internaly and continues
	void mark(std::string name);
	// stops and returns the raw time in nanosecconds 1e-9
	long long getTime() const;
	// stops and ouputs the total elapsed time
	// clears mark point
	void showTime(bool inFrames = 0);
	// stops and outputs the total time elapsed, all marked times (as percentage of total) all times are provided in secconds and frames 
	// ammount is used to caculate the avg time 
	// clears mark point
	void showDetails(unsigned int ammount);
};
