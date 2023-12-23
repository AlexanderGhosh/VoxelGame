#pragma once
#include <vector>
#include "IEventCallback.h"
#include "../Mangers/EntityManager.h"

template<typename T>
class Event {
public:
	Event() = default;
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;

	void addListener(IEventCallback<T>* listener) {
		listeners.push_back(listener);
	}
	void removeListener(IEventCallback<T>* listener)
	{
		listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
	}

	void fire(T info) const {
		for (auto& listener : listeners) {
			(*listener)(info);
		}
	}

	void operator()(T info) {
		fire(info);
	}

	void operator+=(IEventCallback<T>* listener) {
		addListener(listener);
	}
	void operator-=(IEventCallback<T>* listener) {
		removeListener(listener);
	}

	void operator+=(IEventCallback<T>& listener) {
		addListener(&listener);
	}
	void operator-=(IEventCallback<T>& listener) {
		removeListener(&listener);
	}

private:
	std::vector<IEventCallback<T>*> listeners;
};


template<>
class Event<void> {
public:
	Event() = default;

	void addListener(IEventCallback<void>* listener) {
		listeners.push_back(listener);
	}
	void removeListener(IEventCallback<void>* listener)
	{
		listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
	}

	void fire() const {
		for (auto& listener : listeners) {
			(*listener)();
		}
	}

	void operator()() {
		fire();
	}

	void operator+=(IEventCallback<void>* listener) {
		addListener(listener);
	}
	void operator-=(IEventCallback<void>* listener) {
		removeListener(listener);
	}

	void operator+=(IEventCallback<void>& listener) {
		addListener(&listener);
	}
	void operator-=(IEventCallback<void>& listener) {
		removeListener(&listener);
	}

private:
	std::vector<IEventCallback<void>*> listeners;
};