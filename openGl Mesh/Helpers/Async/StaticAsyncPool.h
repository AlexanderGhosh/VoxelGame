#pragma once
#include <future>
#include <array>
template <typename Ret_Type, unsigned int size>
class StaticAsyncPool
{
public:
	StaticAsyncPool();
	// returns an avaliable slot
	std::future<Ret_Type>* getAvalible();
	// true if the future in slot is avalible
	bool isDone(const unsigned int& slot) const;
	bool allDone() const;
	bool allRunning() const;
private:
	std::array<std::future<Ret_Type>, size> pool;
};

template<typename Ret_Type, unsigned int size>
inline StaticAsyncPool<Ret_Type, size>::StaticAsyncPool() : pool()
{
}

template<typename Ret_Type, unsigned int size>
inline std::future<Ret_Type>* StaticAsyncPool<Ret_Type, size>::getAvalible()
{
	for (unsigned int i = 0; i < size; i++) {
		if (isDone(i)) {
			return &pool[i];
		}
	}
	return nullptr;
}

template<typename Ret_Type, unsigned int size>
inline bool StaticAsyncPool<Ret_Type, size>::isDone(const unsigned int& slot) const
{
	return pool[slot]._Is_ready();
}

template<typename Ret_Type, unsigned int size>
inline bool StaticAsyncPool<Ret_Type, size>::allDone() const
{
	for (unsigned int i = 0; i < size; i++) {
		if (!isDone(i)) {
			return false;
		}
	}
	return true;
}

template<typename Ret_Type, unsigned int size>
inline bool StaticAsyncPool<Ret_Type, size>::allRunning() const
{
	for (unsigned int i = 0; i < size; i++) {
		if (isDone(i)) {
			return false;
		}
	}
	return false;
}
