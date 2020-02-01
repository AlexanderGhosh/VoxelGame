#include "PhysicsEngine.h"
namespace Physics {
	std::vector<Update> Engine::updateBuffer;
	std::vector<Object> Engine::objects;
	std::vector<Material> Engine::materials;
	std::vector<BoxCollider> Engine::colliders;

	void Engine::doUpdates(World& world) {
		for (auto& update : Engine::updateBuffer) {
			if (update.Tag == TAG::COLLISION) {

			}
		}
		updateBuffer.clear();
	}
	void Engine::addUpdate(Update& update) {
		if (update.null) return;
		Engine::updateBuffer.push_back(update);
	}
	void Engine::addObject(const Object& object) {
		Engine::objects.push_back(object);
	}
};