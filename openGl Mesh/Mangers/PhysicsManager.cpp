#include "PhysicsManager.h"
#include "../EntityComponent/Components/Transform.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Helpers/Functions.h"

PhysicsManager::PhysicsManager() : Manager<PhysicsManager>(), _physCommon(), _physWorld(), _terrainBody(), _columnCollider()
{
}

void PhysicsManager::setPhysCommon(rp3d::PhysicsCommon* common)
{
	_physCommon = common;
	_physWorld = _physCommon->createPhysicsWorld();
	reactphysics3d::Transform bodyOffset;
	reactphysics3d::Vector3 offsetPos(0.5, 0, 0);
	bodyOffset.setPosition(offsetPos);

	_terrainBody = _physWorld->createRigidBody(bodyOffset);
	_terrainBody->setType(reactphysics3d::BodyType::STATIC);
	_columnCollider = _physCommon->createBoxShape({ HALF_VOXEL_SIZE, HALF_VOXEL_SIZE * WORLD_HEIGHT, HALF_VOXEL_SIZE });
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
			reactphysics3d::Transform offset;
			offset.setPosition({x * VOXEL_SIZE, -(float)WORLD_HEIGHT, z * VOXEL_SIZE}); // pos in scaled
			// -WORLD_HEIGHT is used so the collider isnt aligned with anything while generating
			_terrainBody->addCollider(_columnCollider, offset);
		}
	}

#ifdef PHYSICS_DEBUG_RENDERER
	_physWorld->setIsDebugRenderingEnabled(true);
	_debugRenderer = &_physWorld->getDebugRenderer();
	_debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
	// _debugRenderer->setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
#endif // PHYSICS_DEBUG_RENDERER

}

reactphysics3d::RigidBody* PhysicsManager::createRigidBody(Components::Transform* transform) const
{
	reactphysics3d::Transform t;
	const auto& pos = t.getPosition();
	memcpy(&transform->position, &pos, sizeof(glm::vec3)); // syncs the position in react to the render position

	return _physWorld->createRigidBody(t);
}

void PhysicsManager::removeRigidBody(reactphysics3d::RigidBody* rb) const
{
	_physWorld->destroyRigidBody(rb);
}

reactphysics3d::BoxShape* PhysicsManager::createBoxShape(const glm::vec3& extent) const
{
	reactphysics3d::Vector3 e;
	memcpy(&e, &extent, sizeof(glm::vec3));
	return _physCommon->createBoxShape(e);
}

#ifdef PHYSICS_DEBUG_RENDERER
	const reactphysics3d::DebugRenderer* PhysicsManager::getDebugRenderer() const
	{
		return _debugRenderer;
	}
#endif // PHYSICS_DEBUG_RENDERER


void PhysicsManager::setTerrain(const ChunkColumn& chunk) const
{
	return;
	reactphysics3d::Transform bodyOffset = _terrainBody->getTransform();
	glm::vec2 chunkWorldPos = chunk.getWorldPosition2D() * VOXEL_SIZE;
	auto p = bodyOffset.getPosition();
	if (p.x == chunkWorldPos.x && p.z == chunkWorldPos.y) {
		// prevents the same chunk's collider from being re made
		return;
	}
	// world pos scaled
	bodyOffset.setPosition({ chunkWorldPos.x, 0, chunkWorldPos.y });
	_terrainBody->setTransform(bodyOffset);

	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
			unsigned int idx = columnIndex(x, z);
			reactphysics3d::Collider* collider = _terrainBody->getCollider(idx);

			reactphysics3d::Transform offset;
			reactphysics3d::Vector3 offsetPos(x * VOXEL_SIZE, 0, z * VOXEL_SIZE);
			offsetPos.y = -(WORLD_HEIGHT * .5 - chunk.getHeight(x, z)) * VOXEL_SIZE + HALF_VOXEL_SIZE;
			offset.setPosition(offsetPos); // pos in scaled

			collider->setLocalToBodyTransform(offset);
			reactphysics3d::Material& mat = collider->getMaterial();
			mat.setBounciness(0);
			mat.setFrictionCoefficient(1);
		}
	}
}

void PhysicsManager::step()
{
	_physWorld->update(FIXED_DELTA_TIME);
}

void PhysicsManager::destroy()
{
}
