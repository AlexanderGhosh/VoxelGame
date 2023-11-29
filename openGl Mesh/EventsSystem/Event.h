#pragma once
#include <vector>

class IEventCallback;
class Event {
public:
	Event();

	void addListener(IEventCallback* listener);
	void removeListener(IEventCallback* listener);

	void fire();

	void operator()();
	void operator+=(IEventCallback* listener);
	void operator-=(IEventCallback* listener);
	void operator+=(IEventCallback& listener);
	void operator-=(IEventCallback& listener);

private:
	std::vector<IEventCallback*> listeners;
};
