#include "Player.h"
#include "../Components/Transform.h"
#include "../Components/Camera.h"
#include "../Components/FlightControls.h"
#include "../../Helpers/Functions.h"

using namespace Entities;

Player::Player() : Entity(), _transform(), _camera(), _controls()
{
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

void Entities::Player::processKeys(const std::array<bool, 1024>& keysPressed, const float deltaTime)
{
	// prevents the player from moving up or down with out space or shift
	glm::vec3 fwd = _camera->getFront();
	fwd.y = 0;
	fwd = glm::normalize(fwd);
	// ^^^
	_controls->processKeys(keysPressed, fwd, deltaTime);
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

	_controls->setTransform(_transform);
}
