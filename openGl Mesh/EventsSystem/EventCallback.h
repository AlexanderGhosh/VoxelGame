#pragma once
#include "IEventCallback.h"

template<typename T>
class EventCallback : public IEventCallback {
public:
	EventCallback(T* instance, void (T::*function)()) : _instance(instance), _function(function) { }

	override void operator()() {
		(_instance->*_function)();
	}
	override bool operator==(IEventCallback* b) {
		EventCallback* c = reinterpret_cast<EventCallback*>(b);
		if (!c) {
			return false;
		}
		return _instance == b->_instance && _function == b->_function;
	}
private:
	T* _instance;
	void (T::* _function)();
};
