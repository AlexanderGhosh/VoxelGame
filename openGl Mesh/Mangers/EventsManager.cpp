#include "EventsManager.h"
#include <glfw3.h>


void EventsManager::onContact(const CollisionCallback::CallbackData& e)
{
	onCollision.fire({});
}

bool EventsManager::isPressed(unsigned int keyCode) const
{
	if (!_window) return false;
	return glfwGetKey(_window, keyCode);
}
