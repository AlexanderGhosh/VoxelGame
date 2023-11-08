#pragma once
#include <list>
#include "Mangers/ComponentManager.h"

class Component;
class EntityManager;

class Entity {
	friend class EntityManager;
private:
	unsigned int _id;
	std::list<Component*> _components;
public:
	Entity();
	Entity(unsigned int id);

	void addComponent(Component& component);
	void addComponent(Component* component);
	
	template<class T>
	T* getComponent();


	void awake();
	void start();
	void update();
	void fixedUpdate();
	void render();
	void destroy();
};

template<class T>
inline T* Entity::getComponent()
{
	ComponentManager& manager = ComponentManager::getInstance();

	for (Component* base : _components) {
		T* component = reinterpret_cast<T*>(base);
		if (component) {
			return component;
		}
	}
	return nullptr;
}
