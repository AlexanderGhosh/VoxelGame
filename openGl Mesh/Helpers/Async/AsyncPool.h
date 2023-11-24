#pragma once
#include <future>
template <typename Ret_Type, unsigned int size>
class AsyncPool
{
public:
	AsyncPool();
	// returns an avaliable slot
	std::future<Ret_Type>* getAvalible();
	// true if the future in slot is avalible
	bool isDone(const unsigned int& slot);
private:
	std::array<std::future<Ret_Type>, size> pool;
};

template<typename Ret_Type, unsigned int size>
inline AsyncPool<Ret_Type, size>::AsyncPool() : pool()
{
}

template<typename Ret_Type, unsigned int size>
inline std::future<Ret_Type>* AsyncPool<Ret_Type, size>::getAvalible()
{
	for (unsigned int i = 0; i < size; i++) {
		if (isDone(i)) {
			return &pool[i];
		}
	}
	return nullptr;
}

template<typename Ret_Type, unsigned int size>
inline bool AsyncPool<Ret_Type, size>::isDone(const unsigned int& slot)
{
	return pool[slot]._Is_ready();
}
