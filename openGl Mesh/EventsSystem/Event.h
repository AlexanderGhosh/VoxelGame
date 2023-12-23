#pragma once
#include <vector>

class IEventCallback;
class Event {
public:
	Event() = default;

	void addListener(IEventCallback* listener);
	void removeListener(IEventCallback* listener);

	void fire() const;

	void operator()();
	void operator+=(IEventCallback* listener);
	void operator-=(IEventCallback* listener);
	void operator+=(IEventCallback& listener);
	void operator-=(IEventCallback& listener);

private:
	std::vector<IEventCallback*> listeners;
};
