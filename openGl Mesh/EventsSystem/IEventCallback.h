#pragma once

template<typename T>
class IEventCallback {
public:
	IEventCallback() = default;
	IEventCallback(const IEventCallback&) = delete;
	IEventCallback& operator=(const IEventCallback&) = delete;
	virtual void operator()(T) = 0;
	virtual bool operator==(IEventCallback<T>* b) = 0;
};

template<>
class IEventCallback<void> {
public:
	IEventCallback() = default;
	IEventCallback(const IEventCallback&) = delete;
	IEventCallback& operator=(const IEventCallback&) = delete;
	virtual void operator()() = 0;
	virtual bool operator==(IEventCallback<void>* b) = 0;
};
