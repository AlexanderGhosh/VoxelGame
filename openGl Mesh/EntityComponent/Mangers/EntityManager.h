#pragma once
#include <vector>
#include "Manager.h"

class Entity;

class EntityManager : public Manager<EntityManager> {
	friend class Manager;
private:
	std::vector<Entity> _entities;
	unsigned int _numEntitysCreated; // used for the entity ids
	EntityManager();
public:
	Entity& createEntity();
	void removeEntity(unsigned int id);

	Entity& getEntity(unsigned int id);
	const Entity& getEntity(unsigned int id) const;
	void destroy();
};
