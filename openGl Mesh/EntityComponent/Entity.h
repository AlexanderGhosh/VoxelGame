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

	void addComponent(Component& component);
	void addComponent(Component* component);

	Component* getComponent(unsigned int index);
	
	template<class T>
	T* getComponent();
};

template<class T>
inline T* Entity::getComponent()
{
	ComponentManager& manager = ComponentManager::getInstance();

	for (unsigned int id : _componentIds) {
		Component* base = manager.getComponent(id);
		T* component = reinterpret_cast<T*>(base);
		if (component) {
			return component;
		}
	}
	return nullptr;
}
