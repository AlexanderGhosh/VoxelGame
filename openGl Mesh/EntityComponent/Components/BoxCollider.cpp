#include "BoxCollider.h"
#include "../../Helpers/Functions.h"
#include "Transform.h"
#include "../../Helpers/Constants.h"
#include "../../Mangers/CollisionManager.h"

Components::BoxCollider::BoxCollider() : Component(), _transform()
{
	CollisionManager::getInstance().addCollider(this);
}

bool Components::BoxCollider::colliding(const BoxCollider* b) const
{
	const glm::vec3 aMin = _transform->position - _transform->scale * .5f;
	const glm::vec3 aMax = _transform->position + _transform->scale * .5f;
	const glm::vec3 bMin = b->_transform->position - b->_transform->scale * .5f;
	const glm::vec3 bMax = b->_transform->position + b->_transform->scale * .5f;

	return aMin.x <= bMax.x && aMax.x >= bMin.x
		&& aMin.y <= bMax.y && aMax.y >= bMin.y
		&& aMin.z <= bMax.z && aMax.z >= bMin.z;
}

bool Components::BoxCollider::colliding(const glm::vec3& pos) const
{
	const glm::vec3 aMin = _transform->position - _transform->scale * .5f;
	const glm::vec3 aMax = _transform->position + _transform->scale * .5f;
	const glm::vec3 bMin = pos - glm::vec3(VOXEL_SZIE) * .5f;
	const glm::vec3 bMax = pos + glm::vec3(VOXEL_SZIE) * .5f;

	return aMin.x <= bMax.x && aMax.x >= bMin.x
		&& aMin.y <= bMax.y && aMax.y >= bMin.y
		&& aMin.z <= bMax.z && aMax.z >= bMin.z;
}

void Components::BoxCollider::setTransform(Transform* transform)
{
	_transform = transform;
}

void Components::BoxCollider::destroy()
{
	CollisionManager::getInstance().removeCollider(this);
}
