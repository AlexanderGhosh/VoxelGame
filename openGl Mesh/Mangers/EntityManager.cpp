#include "EntityManager.h"
#include "ComponentManager.h"
#include "../EntityComponent/Component.h"
#include "../EntityComponent/Components/Transform.h"

EntityManager::EntityManager() : Manager<EntityManager>(), _entities(), _numEntitysCreated(0)
{
}

void EntityManager::awakeEvent()
{
	for (Entity& entity : _entities) {
		entity.awake();
	}
}

void EntityManager::startEvent()
{
	for (Entity& entity : _entities) {
		entity.start();
	}
}

void EntityManager::updateEvent()
{
	for (Entity& entity : _entities) {
		entity.update();
	}
}

void EntityManager::fixedUpdateEvent()
{
	for (Entity& entity : _entities) {
		entity.fixedUpdate();
	}
}

void EntityManager::renderEvent()
{
	for (Entity& entity : _entities) {
		entity.render();
	}
}

void EntityManager::destroyEvent()
{
	for (Entity& entity : _entities) {
		entity.destroy(); 
	}
}

Entity& EntityManager::createEntity()
{
	_entities.emplace_back(_numEntitysCreated++);
	return _entities.back();
}

void EntityManager::removeEntity(unsigned int id)
{
	Entity& entity = getEntity(id);
	ComponentManager& manager = ComponentManager::getInstance();
	for (auto& component : entity._components)
	{
		manager.removeComponent(component->getId());
	}
}

Entity& EntityManager::getEntity(unsigned int id)
{
	auto itt = _entities.begin();
	std::advance(itt, id);
	return *itt;
}

const Entity& EntityManager::getEntity(unsigned int id) const
{
	auto itt = _entities.begin();
	std::advance(itt, id);
	return *itt;
}

void EntityManager::destroy()
{
	_entities.clear();
}
