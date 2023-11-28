#pragma once
#include <list>
#include "../Mangers/ComponentManager.h"

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

	// called before componet functions
	// idea is to be used for resources that are used by componets
	virtual void awake();
	// called before componet functions
	// idea is to be used for initalising variables which require the componets to already exist
	virtual void start();
	// called before componet functions
	virtual void update();
	// called before componet functions
	virtual void fixedUpdate();
	// called before componet functions
	virtual void render();
	// called before componet functions
	virtual void destroy();

	// called after entity functions
	void componentsAwake();
	// called after entity functions
	void componentsStart();
	// called after entity functions
	void componentsUpdate();
	// called after entity functions
	void componentsFixedUpdate();
	// called after entity functions
	void componentsRender();
	// called after entity functions
	void componentsDestroy();

	const unsigned int getId() const;
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
