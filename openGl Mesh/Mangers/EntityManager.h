#pragma once
#include <list>
#include "Manager.h"
#include "../EntityComponent/Entity.h"
#include "reactphysics3d/engine/EventListener.h"

#include "../EventsSystem/EventCallback.h"
#include "../EventsSystem/EventDetails/CollisionEventInfo.h"
#include "../EventsSystem/EventDetails/ClickEventInfo.h"
#include "../EventsSystem/EventDetails/KeyEventInfo.h"

class Entity;

class EntityManager : public Manager<EntityManager> {
	friend class Manager;
private:
	std::list<Entity*> _entities;
	unsigned int _numEntitysCreated; // used for the entity ids
	EventCallback<EntityManager, CollisionEventInfo> onCollisionCB;
	EventCallback<EntityManager, ClickEventInfo> onClickEventCB;
	EventCallback<EntityManager, KeyEventInfo> onKeyEventCB;

	EntityManager();
public:
	void awakeEvent();
	void startEvent();
	void updateEvent(const float deltaTime);
	void preFixedUpdateEvent();
	void postFixedUpdateEvent();
	void renderEvent();
	void destroyEvent();

	// true events
	void onCollideEvent(CollisionEventInfo info);
	void onClickEvent(ClickEventInfo info);
	void onKeyEvent(KeyEventInfo info);


	void removeEntity(unsigned int id);

	void addEntity(Entity* entity);

	Entity& getEntity(unsigned int id);
	const Entity& getEntity(unsigned int id) const;

	template<class T>
		requires std::is_base_of<Entity, T>::value
	T& getEntity(unsigned int id);
	template<class T>
		requires std::is_base_of<Entity, T>::value
	const T& getEntity(unsigned int id) const;

	void destroy() override;
};

template<class T>
	requires std::is_base_of<Entity, T>::value
T& EntityManager::getEntity(unsigned int id) {
	auto itt = _entities.begin();
	std::advance(itt, id);
	return *reinterpret_cast<T*>(&(*itt));
}

template<class T>
	requires std::is_base_of<Entity, T>::value
const T& EntityManager::getEntity(unsigned int id) const {
	auto itt = _entities.begin();
	std::advance(itt, id);
	return *reinterpret_cast<T*>(&(*itt));
}