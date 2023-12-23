#pragma once
#include "IEventCallback.h"

template<typename T, typename U>
class EventCallback : public IEventCallback<U> {
public:
	EventCallback() = default;
	EventCallback(T* instance, void (T::* function)(U)) : _instance(instance), _function(function) { }

	EventCallback(EventCallback&& other) {
		_instance = other._instance;
		_function = other._function;
		other._instance = nullptr;
		other._function = nullptr;
	}
	EventCallback& operator=(EventCallback&& other) {
		_instance = other._instance;
		_function = other._function;
		other._instance = nullptr;
		other._function = nullptr;
		return *this;
	}

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