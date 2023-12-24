#pragma once

class ComponentManager;

struct CollisionEventInfo;
struct ClickEventInfo;
struct KeyEventInfo;
class Entity;

class Component {
	friend class Entity;
	friend class ComponentManager;
protected:
	unsigned int _id;
	Entity* _parent;
public:
	Component();

	unsigned int getId() const { return _id;}

	// the following have an empty defintion (they are only defined in the cpp because some of the paramas need #includes)
	virtual void awake();
	virtual void start();
	virtual void update(const float detlaTime);
	virtual void preFixedUpdate();
	virtual void postFixedUpdate();
	virtual void render();
	virtual void destroy();

	// real events
	virtual void onCollision(CollisionEventInfo);
	virtual void onClick(ClickEventInfo);
	virtual void onKeyPress(KeyEventInfo);
};
