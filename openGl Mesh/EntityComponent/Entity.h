#pragma once
#include <list>
#include "Component.h"

class EntityManager;

class Entity {
	friend class EntityManager;
private:
	std::list<Component*> _components;
	unsigned int _id;
protected:
	// called before componet functions
	// idea is to be used for resources that are used by componets
	virtual void awake();
	// called before componet functions
	// idea is to be used for initalising variables which require the componets to already exist
	virtual void start();
	// called before componet functions
	virtual void update(const float detlaTime);
	// called before componet functions
	virtual void preFixedUpdate();
	// called before componet functions
	virtual void postFixedUpdate();
	// called before componet functions
	virtual void render();
	// called before componet functions
	virtual void destroy();

	// real events
	virtual void onCollision(CollisionEventInfo);
	virtual void onClick(ClickEventInfo);
	virtual void onKeyPress(KeyEventInfo);
public:
	Entity();
	Entity(unsigned int id);

	void addComponent(Component& component);
	void addComponent(Component* component);
	
	template<class T>
	T* getComponent();

	// called after entity functions
	void awakeEvent();
	// called after entity functions
	void startEvent();
	// called after entity functions
	void updateEvent(const float deltaTime);
	// called after entity functions
	void preFixedUpdateEvent();
	// called after entity functions
	void postFixedUpdateEvent();
	// called after entity functions
	void renderEvent();
	// called after entity functions (likly redundent as it will be cassed after the componets destructor
	void destroyEvent();


	virtual void onCollisionEvent(CollisionEventInfo);
	virtual void onClickEvent(ClickEventInfo);
	virtual void onKeyPressEvent(KeyEventInfo);

	const unsigned int getId() const;
};

template<class T>
inline T* Entity::getComponent()
{
	for (Component* base : _components) {
		T* component = dynamic_cast<T*>(base);
		if (component) {
			return component;
		}
	}
	return nullptr;
}
