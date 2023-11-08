#include "Event.h"
#include "IEventCallback.h"

Event::Event() : listeners()
{
}

void Event::addListener(IEventCallback* listener)
{
	listeners.push_back(listener);
}

void Event::removeListener(IEventCallback* listener)
{
	listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
}

void Event::fire()
{
	for (auto& listener : listeners) {
		(*listener)();
	}
}

void Event::operator()()
{
	fire();
}

void Event::operator+=(IEventCallback* listener)
{
	addListener(listener);
}

void Event::operator-=(IEventCallback* listener)
{
	removeListener(listener);
}