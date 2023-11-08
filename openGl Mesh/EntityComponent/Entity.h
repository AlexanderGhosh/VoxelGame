#pragma once
#include <list>

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
};

template<typename T>
inline void Entity::addComponent(T component)
{
	_componentIds.push_back(component.getId());
}
