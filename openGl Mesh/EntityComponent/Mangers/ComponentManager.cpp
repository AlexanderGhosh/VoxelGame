#include "ComponentManager.h"
#include "../Component.h"

ComponentManager::ComponentManager() : _components(), _numComponentsCreated()
{
}

void ComponentManager::removeComponent(unsigned int id)
{
	auto itt = _components.begin();
	std::advance(itt, id);
	Component component = getComponent<Component>(id);
	component.destroy();

	_components.erase(itt);
}

void ComponentManager::destroy()
{
}
