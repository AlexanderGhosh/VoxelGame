#include "Component.h"
#include "ComponentTypes.h"
#include "../Mangers/ComponentManager.h"

Component::Component() : _id()
{
	ComponentManager::getInstance().addComponent(this);
}

unsigned int Component::getId() const
{
	return _id;
}

void Component::awake()
{
}

void Component::start()
{
}

void Component::update()
{
}

void Component::fixedUpdate()
{
}

void Component::render()
{
}

void Component::destroy()
{
}
