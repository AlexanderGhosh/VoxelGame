#include "EntityHander.h"

EntityHander::EntityHander()
{
}

void EntityHander::render(Camera& cam, glm::mat4 projection)
{
	for (auto& entiy : entitys) {
		entiy.render(projection, &cam);
	}
}

void EntityHander::addEntity(Entity entity)
{
	entitys.push_back(entity);
}

void EntityHander::updatePositions(GLfloat deltaTime, World& world, std::string& collision)
{
	for (auto& e : entitys) {
		e.updatePosition(deltaTime, world, collision);
	}
}
