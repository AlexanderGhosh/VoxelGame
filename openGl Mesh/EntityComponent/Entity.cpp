#include "Entity.h"

Entity::Entity() : _id(0)
{
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
    for (auto& component : _components) {
        component->awake();
    }
}

void Entity::start()
{
    for (auto& component : _components) {
        component->start();
    }
}

void Entity::update()
{
    for (auto& component : _components) {
        component->update();
    }
}

void Entity::fixedUpdate()
{
    for (auto& component : _components) {
        component->fixedUpdate();
    }
}

void Entity::render()
{
    for (auto& component : _components) {
        component->render();
    }
}

void Entity::destroy()
{
    for (auto& component : _components) {
        component->destroy();
    }
}
