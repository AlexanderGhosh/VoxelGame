#pragma once
template<typename T>
class Manager {
protected:
	static T _instance;
	static bool _initalised;
	Manager();
	template<typename... Args>
	static void createInstance(Args ...args);
public:
	template<typename... Args>
	static T& getInstance(Args... args);

	virtual void destroy() = 0;
};


template<typename T>
inline Manager<T>::Manager()
{
}

template<typename T>
template<typename ...Args>
inline void Manager<T>::createInstance(Args ...args)
{
	_instance = T(args...);
	_initalised = true;
}

template<typename T>
template<typename ...Args>
inline T& Manager<T>::getInstance(Args ...args)
{
	if (!_initalised) {
		createInstance(args...);
	}
	return _instance;
}

