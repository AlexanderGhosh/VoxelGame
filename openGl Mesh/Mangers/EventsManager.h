#pragma once
#include "Manager.h"
#include "../EventsSystem/Event.h"
#include "reactphysics3d/engine/EventListener.h"
#include "../EventsSystem/EventDetails/CollisionEventInfo.h"

struct KeyDownEventInfo;
struct KeyUpEventInfo;
struct ClickEventInfo;
struct CollisionEventInfo;

class EventsManager : public Manager<EventsManager>, public reactphysics3d::EventListener
{
	friend class Manager;
private:
	EventsManager() = default;
	void onContact(const CollisionCallback::CallbackData& e) override { onCollision.fire(CollisionEventInfo()); };
public:
	// triggered every frame a key is down
	Event<KeyDownEventInfo> keyDown;
	// triggered every time a key is released
	Event<KeyUpEventInfo> keyUp;
	Event<CollisionEventInfo> onCollision;
	// mouse
	// will fire once per button
	Event<ClickEventInfo> click;
	Event<void> mouseMove;
	Event<void> mouseScroll;

	// MIGHT NOT USE
	Event<void> awake, start, update, fixedUpdate, render;
};

