#include "EntityManager.h"
#include "ComponentManager.h"
#include "../EntityComponent/Component.h"
#include "../EntityComponent/Components/Transform.h"
#include "EventsManager.h"

EntityManager::EntityManager() : Manager<EntityManager>(), _entities(), _numEntitysCreated(0)
{
	EventsManager& events = EventsManager::getInstance();

	onCollisionCB = EventCallback(this, &EntityManager::onCollideEvent);
	events.onCollision += onCollisionCB;

	onClickEventCB = EventCallback(this, &EntityManager::onClickEvent);
	events.click += onClickEventCB;

	onKeyEventCB = EventCallback(this, &EntityManager::onKeyEvent);
	events.keyPress += onKeyEventCB;
}

void EntityManager::awakeEvent()
{
	for (Entity* entity : _entities) {
		entity->awakeEvent();
	}
}

void EntityManager::startEvent()
{
	for (Entity* entity : _entities) {
		entity->startEvent();
	}
}

void EntityManager::updateEvent(const float deltaTime)
{
	for (Entity* entity : _entities) {
		entity->updateEvent(deltaTime);
	}
}

void EntityManager::preFixedUpdateEvent()
{
	for (Entity* entity : _entities) {
		entity->preFixedUpdateEvent();
	}
}

void EntityManager::postFixedUpdateEvent()
{
	for (Entity* entity : _entities) {
		entity->postFixedUpdateEvent();
	}
}

void EntityManager::renderEvent()
{
	for (Entity* entity : _entities) {
		entity->renderEvent();
	}
}

void EntityManager::destroyEvent()
{
	for (Entity* entity : _entities) {
		entity->destroyEvent();
	}
}

void EntityManager::onCollideEvent(CollisionEventInfo info)
{
	for (Entity* entity : _entities) {
		entity->onCollisionEvent(info);
	}
}

void EntityManager::onClickEvent(ClickEventInfo info)
{
	for (Entity* entity : _entities) {
		entity->onClickEvent(info);
	}
}

void EntityManager::onKeyEvent(KeyEventInfo info)
{
	for (Entity* entity : _entities) {
		entity->onKeyPressEvent(info);
	}
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

void EntityManager::addEntity(Entity* entity)
{
	_entities.push_back(entity);
}

Entity& EntityManager::getEntity(unsigned int id)
{
	auto itt = _entities.begin();
	std::advance(itt, id);
	return **itt;
}

const Entity& EntityManager::getEntity(unsigned int id) const
{
	auto itt = _entities.begin();
	std::advance(itt, id);
	return **itt;
}

void EntityManager::destroy()
{
	_entities.clear();
}
