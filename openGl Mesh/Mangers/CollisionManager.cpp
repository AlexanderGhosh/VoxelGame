#include "CollisionManager.h"
#include "../EntityComponent/Components/BoxCollider.h"
#include "../GeomRendering/GeomData.h"

CollisionManager::CollisionManager() : _colliders()
{
}

bool CollisionManager::checkCollision(Components::BoxCollider* a) const
{
	for (Components::BoxCollider* b : _colliders) {
		if (a == b) {
			continue;
		}
		if (a->colliding(b)) {
			return true;
		}
	}
	return false;
}

bool CollisionManager::checkCollision(Components::BoxCollider* a, const std::vector<GeomData>& chunkData, const glm::vec2& chunkPos) const
{
	for (const GeomData& b : chunkData) {
		glm::vec3 blockPos = b.getPos();
		blockPos.x += chunkPos.x;
		blockPos.z += chunkPos.y;
		if (a->colliding(blockPos)) {
			return true;
		}
	}
	return false;
}

void CollisionManager::addCollider(Components::BoxCollider* collider)
{
	_colliders.push_back(collider);
}

void CollisionManager::removeCollider(Components::BoxCollider* collider)
{
	_colliders.remove(collider);
}

void CollisionManager::destroy()
{
	_colliders.clear();
}
