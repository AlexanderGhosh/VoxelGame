#include "TestObject.h"
#include "../Components/Transform.h"
#include "../Components/RigidBody.h"
#include "../Components/RenderMesh.h"
#include "../../Helpers/Functions.h"
#include "../../Game/World/Chunks/ChunkColumn.h"
#include "../../Mangers/PhysicsManager.h"

Entities::TestObject::TestObject() : Entity(), _transform(), _renderMesh()
{
}
void Entities::TestObject::setPosition(const glm::vec3& pos)
{
	_transform->position = pos;
}

void Entities::TestObject::start()
{
	_transform = getComponent<Components::Transform>();
	_renderMesh = getComponent<Components::RenderMesh>();

	_transform->scale = glm::vec3(1);

	_renderMesh->setTransform(_transform);
}
