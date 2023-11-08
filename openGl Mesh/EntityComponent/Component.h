#pragma once

class Component {
protected:
	unsigned int _id;
public:
	Component();
	Component(unsigned int id);

	unsigned int getId() const;

	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void fixedUpdate();
	virtual void render();
	virtual void destroy();
};
