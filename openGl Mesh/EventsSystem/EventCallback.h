#pragma once
#include "IEventCallback.h"

template<typename T>
class EventCallback : public IEventCallback {
public:
	EventCallback(T* instance, void (T::* function)());

	void operator()() override;
	bool operator==(IEventCallback* b) override;
private:
	T* _instance;
	void (T::* _function)();
};

template<typename T>
EventCallback<T>::EventCallback<T>(T* instance, void (T::* function)()) : _instance(instance), _function(function) { }

template<typename T>
void EventCallback<T>::operator()() {
	(_instance->*_function)();
}

template<typename T>
bool EventCallback<T>::operator==(IEventCallback* b) {
	EventCallback* c = reinterpret_cast<EventCallback*>(b);
	if (!c) {
		return false;
	}
	return _instance == c->_instance && _function == c->_function;
}