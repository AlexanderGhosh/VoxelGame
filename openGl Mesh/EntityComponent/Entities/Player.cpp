#include "Player.h"
#include "../Components/Transform.h"
#include "../Components/Camera.h"
#include "../Components/FlightControls.h"
#include "../Components/BoxCollider.h"
#include "../Components/RigidBody.h"
#include "../../Helpers/Functions.h"
#include "../../Mangers/CollisionManager.h"
#include "../../Game/World/Chunks/ChunkColumn.h"

using namespace Entities;

Player::Player() : Entity(), _transform(), _camera(), _controls(), _collider(), _rigidbody(), _noClip(false)
{
}

Entities::Player::Player(bool noClip) : Player()
{
	_noClip = noClip;
}

const glm::mat4 Player::getViewMatrix() const
{
	return _camera->getViewMatrix(_transform->position);
}

void Entities::Player::setPosition(const glm::vec3& pos)
{
	_transform->position = pos;
}

const glm::ivec2 Entities::Player::getChunkPosition() const
{
	glm::vec3 p = glm::floor(_transform->position);
	glm::ivec2 pos(p.x, p.z);
	reduceToMultiple(pos, CHUNK_SIZE); // chunk pos world space (scaled)
	pos /= CHUNK_SIZE; // unscaled space
	return pos;
}

const glm::vec3& Entities::Player::getViewDirection() const
{
	return _camera->getFront();
}

const glm::vec3& Entities::Player::getPosition() const
{
	return _transform->position;
}

const glm::vec3& Entities::Player::getFront() const
{
	return _camera->getFront();
}

void Entities::Player::processKeys(const std::array<bool, 1024>& keysPressed, const float deltaTime, const std::list<ChunkColumn*>& neighbours)
{
	CollisionManager& manager = CollisionManager::getInstance();
	// prevents the player from moving up or down with out space or shift
	glm::vec3 fwd = _camera->getFront();
	fwd.y = 0;
	fwd = glm::normalize(fwd);
	// ^^^
	glm::vec3 prevPos = _transform->position;
	_controls->processKeys(keysPressed, fwd, deltaTime);

	if (_noClip) {
		return;
	}
	// reverts movement if it has caused a collision
	for (ChunkColumn* chunk : neighbours) {
		if (manager.checkCollision(_collider, chunk->getMeshData(), chunk->getWorldPosition2D())) {
			_transform->position = prevPos;
			break;
		}
	}
	prevPos = _transform->position;
	_transform->position.y -= 10 * deltaTime;
	for (ChunkColumn* chunk : neighbours) {
		if (manager.checkCollision(_collider, chunk->getMeshData(), chunk->getWorldPosition2D())) {
			_transform->position = prevPos;
			break;
		}
	}
}

void Entities::Player::processMouse(const glm::vec2& mouseOffsets)
{
	_camera->processMouseMovement(mouseOffsets.x, mouseOffsets.y);
}

void Entities::Player::start()
{
	_transform = getComponent<Components::Transform>();
	_camera = getComponent<Components::Camera>();
	_controls = getComponent<Components::FlightControls>();
	_collider = getComponent<Components::BoxCollider>();
	_rigidbody = getComponent<Components::RigidBody>();

	_transform->scale = glm::vec3(1, 2, 1);

	_controls->setTransform(_transform);
	_collider->setTransform(_transform);
	_rigidbody->setTransform(_transform);
}
