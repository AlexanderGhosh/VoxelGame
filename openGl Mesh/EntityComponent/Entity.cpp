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

void Entity::update()
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

void Entity::componentsAwake()
{
    awake();
    for (auto& component : _components) {
        component->awake();
    }
}

void Entity::componentsStart()
{
    start();
    for (auto& component : _components) {
        component->start();
    }
}

void Entity::componentsUpdate()
{
    update();
    for (auto& component : _components) {
        component->update();
    }
}

void Entity::componentsFixedUpdate()
{
    fixedUpdate();
    for (auto& component : _components) {
        component->fixedUpdate();
    }
}

void Entity::componentsRender()
{
    render();
    for (auto& component : _components) {
        component->render();
    }
}

void Entity::componentsDestroy()
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
