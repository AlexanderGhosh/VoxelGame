#include "EventsManager.h"

void EventsManager::onContact(const CollisionCallback::CallbackData& e)
{
	onCollision.fire({});
}
