#include "Component.h"
#include "ComponentTypes.h"

Component::Component() : _id()
{
}

Component::Component(unsigned int id) : _id(id)
{
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
