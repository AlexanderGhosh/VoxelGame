#pragma once

class IEventCallback {
public:
	virtual void operator()() = 0;
	virtual bool operator==(IEventCallback* b) = 0;
};

