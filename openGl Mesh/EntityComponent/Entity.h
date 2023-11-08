#pragma once
#include <list>
#include "Mangers/ComponentManager.h"

class Component;
class EntityManager;

class Entity {
	friend class EntityManager;
private:
	unsigned int _id;
	std::list<unsigned int> _componentIds;
public:
	Entity();
	Entity(unsigned int id);
	template<typename T>
	void addComponent(T component);
	template<typename T>
	T& getComponent(unsigned int index);
};

template<typename T>
inline void Entity::addComponent(T component)
{
	_componentIds.push_back(component.getId());
}

template<typename T>
inline T& Entity::getComponent(unsigned int index)
{
	ComponentManager& manager = ComponentManager::getInstance();
	return (T&)manager.getComponent<T>(*(_componentIds.begin()++));
}
