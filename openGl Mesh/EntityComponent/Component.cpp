#include "Component.h"
#include "ComponentTypes.h"
#include "../Mangers/ComponentManager.h"
#include "../EventsSystem/EventDetails/CollisionEventInfo.h"
#include "../EventsSystem/EventDetails/ClickEventInfo.h"
#include "../EventsSystem/EventDetails/KeyEventInfo.h"

Component::Component() : _id()
{
	ComponentManager::getInstance().addComponent(this);
}

void Component::awake()
{
}

void Component::start()
{
}

void Component::update(const float detlaTime)
{
}

void Component::preFixedUpdate()
{
}

void Component::postFixedUpdate()
{
}

void Component::render()
{
}

void Component::destroy()
{
}

void Component::onCollision(CollisionEventInfo)
{
}

void Component::onClick(ClickEventInfo)
{
}

void Component::onKeyPress(KeyEventInfo)
{
}
