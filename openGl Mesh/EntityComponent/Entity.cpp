#include "Entity.h"

Entity::Entity() : _id(0)
{
}

Entity::Entity(unsigned int id) : _id(id)
{
}

void Entity::addComponent(Component& component)
{
}

void Entity::addComponent(Component* component)
{
}

Component* Entity::getComponent(unsigned int index)
{
    return nullptr;
}
