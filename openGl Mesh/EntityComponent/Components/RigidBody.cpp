#include "RigidBody.h"
#include <reactphysics3d/reactphysics3d.h>
#include "Transform.h"
#include "../../Helpers/Constants.h"
#include "../../Mangers/PhysicsManager.h"

Components::RigidBody::RigidBody() : Component(), _transform(), _manager(), _reactRigidBody(), _details()
{
	_manager = &PhysicsManager::getInstance();
}

void Components::RigidBody::setTransform(Transform* transform)
{
	_transform = transform;
}

void Components::RigidBody::setDetails(const RigidBody_Details& detials)
{
	_details = detials;
}

void Components::RigidBody::start()
{
	_reactRigidBody = _manager->createRigidBody(_transform);

	_reactRigidBody->setAngularDamping(10000);
	_reactRigidBody->enableGravity(_details._hasGravity);
	_reactRigidBody->setType((reactphysics3d::BodyType)_details._bodyType);
	if (_details._collider) {
		_reactRigidBody->addCollider(_details._collider, {});
	}
}

void Components::RigidBody::preFixedUpdate()
{
	reactphysics3d::Transform transform = _reactRigidBody->getTransform();
	reactphysics3d::Vector3 pos(_transform->position.x, _transform->position.y, _transform->position.z);
	reactphysics3d::Quaternion i = reactphysics3d::Quaternion::identity();
	transform.setPosition(pos);
	transform.setOrientation(i);

	_reactRigidBody->setTransform(transform);
}

void Components::RigidBody::postFixedUpdate()
{
	const auto& transform = _reactRigidBody->getTransform();
	const auto& pos = transform.getPosition();
	memcpy(&_transform->position, &pos, sizeof(glm::vec3));
}

void Components::RigidBody::addVelocity(const glm::vec3& delta)
{
	reactphysics3d::Vector3 vel = _reactRigidBody->getLinearVelocity();
	vel.x += delta.x;
	vel.y += delta.y;
	vel.z += delta.z;
	_reactRigidBody->setLinearVelocity(vel);
}

void Components::RigidBody::setVelocity(const glm::vec3& delta)
{
	reactphysics3d::Vector3 vel;
	memcpy(&vel, &delta, sizeof(glm::vec3));
	_reactRigidBody->setLinearVelocity(vel);
}
const glm::vec3 Components::RigidBody::getVelocity() const
{
	auto v = _reactRigidBody->getLinearVelocity();
	glm::vec3 vel(0);
	memcpy(&vel, &v, sizeof(glm::vec3));
	return vel;
}

void Components::RigidBody::hasGravity(bool hasGravity)
{
	_reactRigidBody->enableGravity(hasGravity);
}

reactphysics3d::Material& Components::RigidBody::getMaterial()
{
	return _reactRigidBody->getCollider(0)->getMaterial();
}

void Components::RigidBody::destroy()
{
}
