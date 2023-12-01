#include "ComponentManager.h"
#include "../EntityComponent/Component.h"

ComponentManager::ComponentManager() : Manager<ComponentManager>(), _components(), _numComponentsCreated()
{
}

void ComponentManager::addComponent(Component& component)
{
	_components.push_back(&component);
	component._id = _numComponentsCreated++;
}

void ComponentManager::addComponent(Component* component)
{
	_components.push_back(component);
	component->_id = _numComponentsCreated++;
}

Component* ComponentManager::getComponent(unsigned int id)
{
	return _components[id];
}

void ComponentManager::removeComponent(unsigned int id)
{
	auto itt = _components.begin();
	std::advance(itt, id);
	Component* component = getComponent(id);
	component->destroy();

	_components.erase(itt);
}

void ComponentManager::destroy()
{
	_components.clear();
}
