#pragma once
#include <list>
#include "Manager.h"
#include "../EntityComponent/Entity.h"

class Entity;

class EntityManager : public Manager<EntityManager> {
	friend class Manager;
private:
	std::list<Entity> _entities;
	unsigned int _numEntitysCreated; // used for the entity ids
	EntityManager();
public:
	void awakeEvent();
	void startEvent();
	void updateEvent();
	void fixedUpdateEvent();
	void renderEvent();
	void destroyEvent();

	Entity& createEntity();
	template<class T>
		requires std::is_base_of<Entity, T>::value
	T& createEntity();
	void removeEntity(unsigned int id);

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
T& EntityManager::createEntity() {
	_entities.emplace_back(_numEntitysCreated++);
	return *reinterpret_cast<T*>(&_entities.back());
}


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