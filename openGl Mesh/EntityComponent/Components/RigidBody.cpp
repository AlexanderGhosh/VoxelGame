#include "RigidBody.h"
#include "Transform.h"
#include "../../Helpers/Constants.h"

Components::RigidBody::RigidBody() : Component(), _transform(), _velocity()
{
}

void Components::RigidBody::setTransform(Transform* transform)
{
	_transform = transform;
}

void Components::RigidBody::intergrateVelocity(const float deltaTime) const
{
	_transform->position += _velocity * deltaTime;
}

void Components::RigidBody::applyGravity(const float deltaTime)
{
	_velocity += GRAVITY * deltaTime;
}

void Components::RigidBody::update(const float deltaTime)
{
	applyGravity(deltaTime);
	intergrateVelocity(deltaTime);
}