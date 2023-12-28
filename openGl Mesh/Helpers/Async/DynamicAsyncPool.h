#pragma once
#include <future>
#include <vector>

template <typename Ret_Type>
class DynamicAsyncPool
{
public:
	DynamicAsyncPool();
	// retunrs a reference to the new future
	std::future<Ret_Type>& add();
	// true if the future in slot is avalible
	bool isDone(const unsigned int& slot) const;
	bool allDone() const;
	bool allRunning() const;
	void remove(const unsigned int& slot);
	// removes all finished asyncs
	void cull();

	// returns all .get() on all the done
	std::list<Ret_Type> getAllDone(bool remove);
	void removeAll(std::list<std::future<Ret_Type>*> toRemove);
private:
	std::vector<std::future<Ret_Type>> pool;
};

template<typename Ret_Type>
inline DynamicAsyncPool<Ret_Type>::DynamicAsyncPool() : pool()
{
}

template<typename Ret_Type>
inline std::future<Ret_Type>& DynamicAsyncPool<Ret_Type>::add()
{
	pool.push_back({ });
	return pool.back();
}

template<typename Ret_Type>
inline bool DynamicAsyncPool<Ret_Type>::isDone(const unsigned int& slot) const
{
	return pool[slot]._Is_ready();
}

template<typename Ret_Type>
inline bool DynamicAsyncPool<Ret_Type>::allDone() const
{
	for (unsigned int i = 0; i < pool.size(); i++) {
		if (!isDone(i)) {
			return false;
		}
	}
	return true;
}

template<typename Ret_Type>
inline bool DynamicAsyncPool<Ret_Type>::allRunning() const
{
	for (unsigned int i = 0; i < pool.size(); i++) {
		if (isDone(i)) {
			return false;
		}
	}
	return false;
}

template<typename Ret_Type>
inline void DynamicAsyncPool<Ret_Type>::remove(const unsigned int& slot)
{
	auto itt = pool.begin();
	std::advance(itt, slot);
	if ((*itt)._Is_ready()) {
		pool.erase(itt);
	}
}

template<typename Ret_Type>
inline void DynamicAsyncPool<Ret_Type>::cull()
{
	for (auto itt = pool.begin(); itt != pool.end();) {
		if ((*itt)._Is_ready()) {
			itt++;
		}
		else {
			pool.erase(itt);
		}
	}
}

template<typename Ret_Type>
inline std::list<Ret_Type> DynamicAsyncPool<Ret_Type>::getAllDone(bool remove)
{
	std::list<Ret_Type> res;

	for (auto itt = pool.begin(); itt != pool.end();) {
		if ((*itt).valid() && (*itt)._Is_ready()) {
			res.push_back((*itt).get());
			if (remove) {
				itt = pool.erase(itt);
				continue;
			}
		}
		itt++;
	}
	return res;
}

template<typename Ret_Type>
inline void DynamicAsyncPool<Ret_Type>::removeAll(std::list<std::future<Ret_Type>*> toRemove)
{
	for (auto itt1 = pool.begin(); itt1 != pool.end();) {
		for (auto itt2 = toRemove.begin(); itt2 != toRemove.end();) {
			if (*itt2 == &*itt1) {
				itt1 = pool.erase(itt1);
				itt2 = toRemove.erase(itt2);
				continue;
			}
			else {
				itt2++;
			}
		}
		itt1++;
	}
}
