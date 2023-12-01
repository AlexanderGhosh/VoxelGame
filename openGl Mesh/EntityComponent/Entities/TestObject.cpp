#include "TestObject.h"
#include "../Components/Transform.h"
#include "../Components/RigidBody.h"
#include "../../Helpers/Functions.h"
#include "../../Game/World/Chunks/ChunkColumn.h"
#include "../../Mangers/PhysicsManager.h"

Entities::TestObject::TestObject() : Entity(), _transform(), _rigidbody()
{
}
void Entities::TestObject::setPosition(const glm::vec3& pos)
{
	_transform->position = pos;
}

Components::RigidBody* Entities::TestObject::getRigidBody()
{
	return _rigidbody;
}

void Entities::TestObject::start()
{
	_transform = getComponent<Components::Transform>();
	_rigidbody = getComponent<Components::RigidBody>();

	_transform->scale = glm::vec3(1, 2, 1) * .8f;

	_rigidbody->setTransform(_transform);

	Components::RigidBody_Details rbDetails;
	rbDetails._hasGravity = true;
	rbDetails._bodyType = Components::RigidBody_Details::DYNAMIC;
	rbDetails._collider = PhysicsManager::getInstance().createBoxShape({ HALF_VOXEL_SIZE * .8, VOXEL_SIZE * .8, HALF_VOXEL_SIZE * .8 });
	_rigidbody->setDetails(rbDetails);
}
