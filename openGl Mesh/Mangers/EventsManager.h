#pragma once
#include "Manager.h"
#include "../EventsSystem/Event.h"
#include "reactphysics3d/engine/EventListener.h"
#include "../EventsSystem/EventDetails/CollisionEventInfo.h"

struct KeyEventInfo;
struct ClickEventInfo;
struct CollisionEventInfo;

class GLFWwindow;
class EventsManager : public Manager<EventsManager>, public reactphysics3d::EventListener
{
	friend class Manager;
private:
	EventsManager() = default;
	void onContact(const CollisionCallback::CallbackData& e) override;
	GLFWwindow* _window;
public:
	// called on every key press release and repeat
	Event<KeyEventInfo> keyPress;
	Event<CollisionEventInfo> onCollision;
	// mouse
	// will fire once per button
	Event<ClickEventInfo> click;
	Event<void> mouseMove;
	Event<void> mouseScroll;

	// MIGHT NOT USE
	Event<void> awake, start, update, fixedUpdate, render;

	// takes the GLFW key code, returns true if held down
	bool isPressed(unsigned int keyCode) const;

	inline void setWindow(GLFWwindow* window) { _window = window; };
};

