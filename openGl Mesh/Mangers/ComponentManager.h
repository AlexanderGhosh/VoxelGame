#pragma once
#include <vector>
#include "Manager.h"
#include "../EntityComponent/Component.h"
#include "../EntityComponent/Components/Transform.h"

class ComponentManager : public Manager<ComponentManager>
{
	friend class Manager;
	friend class Component;
private:
	std::vector<Component*> _components;
	unsigned int _numComponentsCreated; // used for the components ids
	ComponentManager();

	void addComponent(Component& component);
	void addComponent(Component* component);
public:
	Component* getComponent(unsigned int id);

	void removeComponent(unsigned int id);

	void destroy();
};