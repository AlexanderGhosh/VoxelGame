#include "PhysicsEngine.h"
namespace Physics {
	std::vector<Update> Engine::updateBuffer;
	std::vector<Object> Engine::objects;
	std::vector<Material> Engine::materials;
	std::vector<Collider> Engine::colliders;

	Engine::Engine() {

	}
	void Engine::addUpdates(std::vector<Update>& updates) {
		for (auto& update : updates) {
			if (update.Tag == Physics::Null) continue;
			Engine::updateBuffer.push_back(update);
		}
	}
	void Engine::doUpdates(World& world) {
		// return;
		for (auto& update : Engine::updateBuffer) {
			if (update.Tag == TAG::COLLISION) {
				// find chunk standing in
				Chunk& chunk = world.getOccupiedChunk((glm::ivec3)update.Data);
				if (chunk.isNull()) continue;
				
				// check collision | if !collided do update
				if (!chunk.checkCollision(update)) {
					update.Sender->doUpdate(update);
					// std::cout << "Update Passed\n";
				}
				else {
					// std::cout << "Update Failed\n";
				}
			}
		}
		updateBuffer.clear();
	}
	void Engine::addUpdate(Update& update) {
		if (update.Tag == Physics::Null) return;
		Engine::updateBuffer.push_back(update);
	}
	void Engine::addObject(const Object& object) {
		Engine::objects.push_back(object);
	}
};