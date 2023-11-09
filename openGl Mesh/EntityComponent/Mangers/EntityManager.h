#pragma once
#include <vector>
#include "Manager.h"
#include "../Entity.h"

class Entity;

class EntityManager : public Manager<EntityManager> {
	friend class Manager;
private:
	std::vector<Entity> _entities;
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
	void removeEntity(unsigned int id);

	Entity& getEntity(unsigned int id);
	const Entity& getEntity(unsigned int id) const;

	void destroy() override;
};
