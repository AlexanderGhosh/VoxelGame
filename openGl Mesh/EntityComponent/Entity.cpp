#include "Entity.h"
#include "Component.h"
#include "../Mangers/EntityManager.h"

Entity::Entity() : _id(0)
{
    EntityManager::getInstance().addEntity(this);
}

Entity::Entity(unsigned int id) : _id(id)
{
}

void Entity::addComponent(Component& component)
{
    _components.push_back(&component);
}

void Entity::addComponent(Component* component)
{
    _components.push_back(component);
}

void Entity::awake()
{
}

void Entity::start()
{
}

void Entity::update(const float detlaTime)
{
}

void Entity::fixedUpdate()
{
}

void Entity::render()
{
}

void Entity::destroy()
{
}

void Entity::awakeEvent()
{
    awake();
    for (auto& component : _components) {
        component->awake();
    }
}

void Entity::startEvent()
{
    start();
    for (auto& component : _components) {
        component->start();
    }
}

void Entity::updateEvent(const float deltaTime)
{
    update(deltaTime);
    for (auto& component : _components) {
        component->update(deltaTime);
    }
}

void Entity::fixedUpdateEvent()
{
    fixedUpdate();
    for (auto& component : _components) {
        component->fixedUpdate();
    }
}

void Entity::renderEvent()
{
    render();
    for (auto& component : _components) {
        component->render();
    }
}

void Entity::destroyEvent()
{
    destroy();
    for (auto& component : _components) {
        component->destroy();
    }
}

const unsigned int Entity::getId() const
{
    return _id;
}
