#pragma once

template<typename T>
class IEventCallback {
public:
	virtual void operator()(T) = 0;
	virtual bool operator==(IEventCallback<T>* b) = 0;
};

template<>
class IEventCallback<void> {
public:
	virtual void operator()() = 0;
	virtual bool operator==(IEventCallback<void>* b) = 0;
};
