#pragma once
#include "Manager.h"
#include <list>
#include <vector>
#include <glm.hpp>

namespace Components {
	class BoxCollider;
}
struct GeomData;

class CollisionManager : public Manager<CollisionManager>
{
private:
	std::list<Components::BoxCollider*> _colliders;
public:
	CollisionManager();

	// returns true is collider collides with any other collider
	bool checkCollision(Components::BoxCollider* collider) const;

	// returns true is collider collides with the given mesh (asumes voxel data)
	bool checkCollision(Components::BoxCollider* collider, const std::vector<GeomData>& meshData, const glm::vec2& chunkPos) const;

	void addCollider(Components::BoxCollider* collider);
	void removeCollider(Components::BoxCollider* collider);

	void destroy() override;
};

