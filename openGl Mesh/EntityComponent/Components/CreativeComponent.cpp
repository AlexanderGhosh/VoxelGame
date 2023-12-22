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
	float speed = _speed;

	if (keysPressed[GLFW_KEY_LEFT_CONTROL]) {
		speed *= 1.5f;
	}

	if (keysPressed[GLFW_KEY_W]) {
		_transform->position += fwd * speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_S]) {
		_transform->position -= fwd * speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_A]) {
		_transform->position -= right * speed * deltaTime;
	}
	if (keysPressed[GLFW_KEY_D]) {
		_transform->position += right * speed * deltaTime;
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
				_rigidbody->addVelocity({ 0, 2, 0 });
				_justJumped = true;
				_jumpCooldown = JUMP_COOLDOWN_MAX;
			}
			else if (_justJumped && JUMP_COOLDOWN_MAX - _jumpCooldown != 1) {
				_mode = Mode::WALK;
				_rigidbody->hasGravity(true);
			}
			else {
				// currently flying and holding space
				_transform->position.y += speed * deltaTime;
			}
		}
	}

	if (keysPressed[GLFW_KEY_LEFT_SHIFT]) {

		if (_isGrounded) {
			// shift
		}
		else if (_mode == Mode::FLY) {
			_transform->position.y -= speed * deltaTime;
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
