#pragma once

class ComponentManager;

class Component {
	friend class ComponentManager;
protected:
	unsigned int _id;
public:
	Component();

	unsigned int getId() const;

	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void fixedUpdate();
	virtual void render();
	virtual void destroy();
};
