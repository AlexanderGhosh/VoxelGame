#include "Player.h"
#include "../Components/Transform.h"
#include "../Components/Camera.h"
#include "../Components/CreativeComponent.h"
#include "../Components/FlightControls.h"
#include "../Components/RigidBody.h"
#include "../../Helpers/Functions.h"
#include "../../Game/World/Chunks/ChunkColumn.h"
#include "../../Mangers/PhysicsManager.h"

using namespace Entities;

Player::Player() : Entity(), _transform(), _camera(), _controls(), _rigidbody(), _noClip(false)
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

Components::RigidBody* Entities::Player::getRigidBody()
{
	return _rigidbody;
}

const glm::ivec2 Entities::Player::getChunkPosition() const
{
	glm::vec3 p = glm::floor(_transform->position * VOXEL_SIZE_INV); // my * VOXEL_SIZE_INV it translates world space to unscaled space
	glm::ivec2 pos(p.x, p.z);
	reduceToMultiple(pos, CHUNK_SIZE); // chunk pos world space
	pos /= CHUNK_SIZE; // local space
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
}

void Entities::Player::processMouse(const glm::vec2& mouseOffsets)
{
	_camera->processMouseMovement(mouseOffsets.x, mouseOffsets.y);
}

void Entities::Player::start()
{
	_transform = getComponent<Components::Transform>(); 
	_camera = getComponent<Components::Camera>();
	_controls = getComponent<Components::CreativeComponent>();
	_rigidbody = getComponent<Components::RigidBody>();

	_transform->scale = glm::vec3(1, 2, 1);

	_controls->setTransform(_transform);
	_controls->setRigidbody(_rigidbody);

	_rigidbody->setTransform(_transform);

	Components::RigidBody_Details rbDetails;
	rbDetails._hasGravity = !_noClip;
	if (!_noClip) {
		rbDetails._bodyType = Components::RigidBody_Details::DYNAMIC;
		rbDetails._collider = PhysicsManager::getInstance().createBoxShape(glm::vec3(HALF_VOXEL_SIZE, VOXEL_SIZE, HALF_VOXEL_SIZE) * .9f);
	}
	else {
		rbDetails._bodyType = Components::RigidBody_Details::KINEMATIC;
	}
	_rigidbody->setDetails(rbDetails);
}
