#include "EntityManager.h"
#include "ComponentManager.h"
#include "../Component.h"
#include "../Entity.h"
#include "../Components/Transform.h"

EntityManager::EntityManager() : Manager<EntityManager>(), _entities(), _numEntitysCreated(0)
{
}

Entity& EntityManager::createEntity(bool hasTransform)
{
	_entities.emplace_back(_numEntitysCreated++);
	if (hasTransform) {
		ComponentManager& manager = ComponentManager::getInstance();
		Transform tranform = manager.createComponent<Transform>();
		_entities.back().addComponent(tranform);
	}
	return _entities.back();
}

void EntityManager::removeEntity(unsigned int id)
{
	Entity& entity = getEntity(id);
	for (auto& componentId : entity._componentIds)
	{
		ComponentManager::getInstance().removeComponent(id);
	}
}

Entity& EntityManager::getEntity(unsigned int id)
{
	return _entities[id];
}

const Entity& EntityManager::getEntity(unsigned int id) const
{
	return _entities[id];
}