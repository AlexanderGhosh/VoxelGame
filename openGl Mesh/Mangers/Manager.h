#pragma once

template<class T>
class Manager {
protected:
	Manager() {}
	~Manager() {}
public:
	Manager(const Manager&) = delete;
	Manager& operator=(const Manager&) = delete;

	static T& getInstance() {
		static T instance;
		return instance;
	}

	virtual void destroy() {}
};