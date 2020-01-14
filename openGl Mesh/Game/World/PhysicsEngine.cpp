#include "PhysicsEngine.h"
namespace Physics {
	std::vector<Update> Engine::updateBuffer;
	std::vector<Object> Engine::objects;
	std::vector<Material> Engine::materials;
	std::vector<BoxCollider> Engine::colliders;

	void Engine::doUpdates(World& world) {
		for (auto& update : Engine::updateBuffer) {
			if (update.Tag == TAG::COLLISION) {
				// get the closest chunk
				Chunk* chunk = world.getChunkOccupied(update.Data);
				// check its collison mesh
				if (chunk == nullptr) continue;
				GLboolean collided = chunk->checkCollision(*update.Sender);
				// if collied dont do update
				// else do update
				if (!collided) {
					update.Sender->doUpdate(update);
				}
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