#pragma once
#include <vector>
#include "Manager.h"
#include "../Component.h"

class ComponentManager : public Manager<ComponentManager>
{
	friend class Manager;
private:
	std::vector<unsigned char> rawData;
	std::vector<Component> _components;
	unsigned int _numComponentsCreated; // used for the components ids
	ComponentManager();
public:
	template<typename T, typename... Args>
	T& createComponent(Args... args);
	template<typename T>
	T& getComponent(unsigned int id);

	void removeComponent(unsigned int id);

	void destroy();
};

template<typename T, typename ...Args>
inline T& ComponentManager::createComponent(Args ...args)
{
	_components.emplace_back(_numComponentsCreated++, args...);
	
	return (T&)_components.back();
}

template<typename T>
inline T& ComponentManager::getComponent(unsigned int id)
{
	return _components[id];
}