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
    addComponent(&component);
}

void Entity::addComponent(Component* component)
{
    component->_parent = this;
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

void Entity::preFixedUpdate()
{
}

void Entity::postFixedUpdate()
{
}

void Entity::render()
{
}

void Entity::destroy()
{
}

void Entity::onCollision(CollisionEventInfo)
{
}

void Entity::onClick(ClickEventInfo)
{
}

void Entity::onKeyPress(KeyEventInfo)
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

void Entity::preFixedUpdateEvent()
{
    preFixedUpdate();
    for (auto& component : _components) {
        component->preFixedUpdate();
    }
}

void Entity::postFixedUpdateEvent()
{
    postFixedUpdate();
    for (auto& component : _components) {
        component->postFixedUpdate();
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

void Entity::onCollisionEvent(CollisionEventInfo info)
{
    onCollision(info);
    for (auto& component : _components) {
        component->onCollision(info);
    }
}

void Entity::onClickEvent(ClickEventInfo info)
{
    onClick(info);
    for (auto& component : _components) {
        component->onClick(info);
    }
}

void Entity::onKeyPressEvent(KeyEventInfo info)
{
    onKeyPress(info);
    for (auto& component : _components) {
        component->onKeyPress(info);
    }
}

const unsigned int Entity::getId() const
{
    return _id;
}
