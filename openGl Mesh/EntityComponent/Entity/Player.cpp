#include "Player.h"
#include "../Components/Transform.h"
#include "../Components/Camera.h"
#include "../Components/FlightControls.h"

using namespace Entities;

Player::Player() : Entity(), transform(), camera(), controls()
{
}

const glm::mat4 Player::getViewMatrix() const
{
	return camera->getViewMatrix(transform->position);
}

void Entities::Player::start()
{
	transform = getComponent<Components::Transform>();
	camera = getComponent<Components::Camera>();
	controls = getComponent<Components::FlightControls>();

	controls->setTransform(transform);
}
