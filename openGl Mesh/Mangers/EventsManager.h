#pragma once
#include "Manager.h"
#include "../EventsSystem/Event.h"
#include "reactphysics3d/engine/EventListener.h"

class EventsManager : public Manager<EventsManager>, public reactphysics3d::EventListener
{
	friend class Manager;
private:
	EventsManager() = default;
	void onContact(const CollisionCallback::CallbackData& e) override { onCollision.fire(); };
public:
	// triggered every frame a key is down
	Event keyDown;
	// triggered every time a key is released
	Event keyUp;
	Event onCollision;
	// mouse
	Event leftClick;
	Event rightClick;
	Event middleClick;
	Event mouseMove;
	Event mouseScroll;

	// MIGHT NOT USE
	Event awake, start, update, fixedUpdate, render;
};

