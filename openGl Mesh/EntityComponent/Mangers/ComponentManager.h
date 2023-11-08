#pragma once
#include <vector>
#include "Manager.h"
#include "../Component.h"
#include "../Components/Transform.h"

class ComponentManager : public Manager<ComponentManager>
{
	friend class Manager;
private:
	std::vector<Component> _components;
	unsigned int _numComponentsCreated; // used for the components ids
	ComponentManager();
	template<typename T, typename... Args>
	T& createComponent_id(Args... args);
public:
	template<typename T, typename... Args>
	T& createComponent(Args... args);
	template<typename T>
	T& getComponent(unsigned int id);

	void removeComponent(unsigned int id);

	void destroy();
};

template<typename T, typename ...Args>
inline T& ComponentManager::createComponent_id(Args ...args)
{
	_components.push_back(T(args...));
	return (T&)_components.back();
}

template<typename T, typename ...Args>
inline T& ComponentManager::createComponent(Args ...args)
{
	return createComponent_id<T>(_numComponentsCreated++, args...);
}

template<typename T>
inline T& ComponentManager::getComponent(unsigned int id)
{
	return (T&) _components[id];
}