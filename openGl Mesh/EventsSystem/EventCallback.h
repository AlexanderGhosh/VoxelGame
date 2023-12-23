#pragma once
#include "IEventCallback.h"

template<typename T, typename U>
class EventCallback : public IEventCallback<U> {
public:
	EventCallback(T* instance, void (T::* function)(U)) : _instance(instance), _function(function) { }

	void operator()(U info) override {
		(_instance->*_function)(info);
	}
	bool operator==(IEventCallback<U>* b) override {
		EventCallback<T, U>* c = reinterpret_cast<EventCallback<T, U>*>(b);
		if (!c) {
			return false;
		}
		return _instance == c->_instance && _function == c->_function;
	}
private:
	T* _instance;
	void (T::* _function)(U);
};