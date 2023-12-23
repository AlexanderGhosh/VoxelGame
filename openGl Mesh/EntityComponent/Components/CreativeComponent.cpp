#include "CreativeComponent.h"
#include <geometric.hpp>
#include <glfw3.h>
#include "Transform.h"
#include "Rigidbody.h"

Components::CreativeComponent::CreativeComponent(bool hasCollision) : _transform(), _speed(10), _isGrounded(false), _justJumped(false), _mode(Mode::WALK), _rigidbody(), _jumpCooldown()
{
}

void Components::CreativeComponent::processKeys(const std::array<bool, 1024>& keysPressed, const glm::vec3& fwd, const float deltaTime)
{
	const glm::vec3 right = glm::normalize(glm::cross(fwd, glm::vec3(0, 1, 0)));
	float speed = _speed * deltaTime;

	if (keysPressed[GLFW_KEY_LEFT_CONTROL]) {
		speed *= 1.5f;
	}
	glm::vec3 deltaV(0);
	if (keysPressed[GLFW_KEY_W]) {
		deltaV += fwd * speed;
	}
	if (keysPressed[GLFW_KEY_S]) {
		deltaV -= fwd * speed;
	}
	if (keysPressed[GLFW_KEY_A]) {
		deltaV -= right * speed;
	}
	if (keysPressed[GLFW_KEY_D]) {
		deltaV += right * speed;
	}

	_rigidbody->addVelocity(deltaV);
	if (deltaV.x == 0 && deltaV.z == 0) {
		deltaV.y = _rigidbody->getVelocity().y;
		_rigidbody->setVelocity(deltaV);
	}

	if (keysPressed[GLFW_KEY_SPACE]) {
		if (_mode == Mode::WALK) {
			if (_isGrounded) {
				// Jump
				_rigidbody->addVelocity({ 0, 2, 0 });
				_justJumped = true;
				_jumpCooldown = JUMP_COOLDOWN_MAX;
			}
			else if (_justJumped) {
				_mode = Mode::FLY;
				_rigidbody->hasGravity(false);
				_rigidbody->setVelocity({ 0, 0, 0 });
				_justJumped = false;
			}
		}
		else if(_mode == Mode::FLY) {
			if (_isGrounded) {
				// jump
				_mode = Mode::WALK;
				_rigidbody->addVelocity({ 0, speed, 0 });
				_justJumped = true;
				_jumpCooldown = JUMP_COOLDOWN_MAX;
			}
			else if (_justJumped && JUMP_COOLDOWN_MAX - _jumpCooldown != 1) {
				_mode = Mode::WALK;
				_rigidbody->hasGravity(true);
			}
			else {
				// currently flying and holding space
				_rigidbody->addVelocity({ 0, speed, 0 });
			}
		}
	}

	if (keysPressed[GLFW_KEY_LEFT_SHIFT]) {

		if (_isGrounded) {
			// shift
		}
		else if (_mode == Mode::FLY) {
			_rigidbody->addVelocity({ 0, -speed, 0 });
		}
	}
}

void Components::CreativeComponent::update(const float dt)
{
	_isGrounded = _transform->position.y <= 38;

	if (_jumpCooldown <= 0) {
 		_justJumped = false;
		_jumpCooldown = 0;
	}
	else {
		--_jumpCooldown;
	}
}

void Components::CreativeComponent::setTransform(Transform* transform)
{
	_transform = transform;
}

void Components::CreativeComponent::setRigidbody(RigidBody* rigidbody)
{
	_rigidbody = rigidbody;
}

void Components::CreativeComponent::setSpeed(const float speed)
{
	_speed = speed;
}
