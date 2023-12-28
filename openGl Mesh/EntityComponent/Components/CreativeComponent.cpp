#include "CreativeComponent.h"
#include <geometric.hpp>
#include <glfw3.h>
#include "../Entity.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "Camera.h"
#include "../../Mangers/EventsManager.h"

void Components::CreativeComponent::setFly()
{
	_mode = Mode::FLY;
	_rigidbody->hasGravity(false);
	_justJumped = false;
	_rigidbody->setVelocity({ 0, 0, 0 });
	_jumpCooldown = 0;
}

void Components::CreativeComponent::setWalk()
{
	_mode = Mode::WALK;
	_rigidbody->hasGravity(true);
	_justJumped = false;
	_rigidbody->setVelocity({ 0, 0, 0 });
	_jumpCooldown = 0;
}

Components::CreativeComponent::CreativeComponent(bool hasCollision) : _transform(), _speed(10), _isGrounded(false), _justJumped(false), _mode(Mode::WALK), _rigidbody(), _jumpCooldown(), _camera(nullptr)
{
}

void Components::CreativeComponent::start()
{
	_camera = _parent->getComponent<Components::Camera>();
}

void Components::CreativeComponent::update(const float deltaTime)
{
	_isGrounded = _transform->position.y <= 38;

	if (_jumpCooldown > 0) {
		--_jumpCooldown;
	}

	const float yVel = _rigidbody->getVelocity().y;

	const EventsManager& events = EventsManager::getInstance();

	glm::vec3 fwd = _camera->getFront();
	fwd.y = 0;
	fwd = glm::normalize(fwd);

	glm::vec3 right = _camera->getRight();
	right.y = 0;
	right = glm::normalize(right);

	float speed = _speed;

	if (events.isPressed(GLFW_KEY_LEFT_CONTROL)) {
		speed *= 2.f;
	}
	glm::vec3 deltaV(0);
	if (events.isPressed(GLFW_KEY_W)) {
		deltaV += fwd * speed;
	}
	else if (events.isPressed(GLFW_KEY_S)) {
		deltaV -= fwd * speed;
	}
	if (events.isPressed(GLFW_KEY_A)) {
		deltaV -= right * speed;
	}
	if (events.isPressed(GLFW_KEY_D)) {
		deltaV += right * speed;
	}


	if (_mode == Mode::FLY && events.isPressed(GLFW_KEY_SPACE)) {
		deltaV.y += speed;
	}

	if (_mode == Mode::FLY && events.isPressed(GLFW_KEY_LEFT_SHIFT)) {
		deltaV.y -= speed;
	}
	if(_mode == Mode::WALK)
		deltaV.y += yVel;
	_rigidbody->setVelocity(deltaV);


	//if (events.isPressed(GLFW_KEY_LEFT_SHIFT)) {

	//	if (_isGrounded) {
	//		// shift
	//	}
	//	else if (_mode == Mode::FLY) {
	//		_rigidbody->addVelocity({ 0, -speed, 0 });
	//	}
	//}
}

void Components::CreativeComponent::onKeyPress(KeyEventInfo info)
{
	if (info._key == GLFW_KEY_SPACE && info._action == GLFW_RELEASE) {
		if (_justJumped && _jumpCooldown > 0) {
			if (_mode == Mode::FLY) {
				setWalk();
			}
			else {
				setFly();
			}
			return;
		}
		else if(_mode == Mode::WALK) {
			_rigidbody->addVelocity({ 0, 7, 0 });
		}
		_justJumped = true;
		_jumpCooldown = JUMP_COOLDOWN_MAX;
	}
}

void Components::CreativeComponent::onCollision(CollisionEventInfo info)
{

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
