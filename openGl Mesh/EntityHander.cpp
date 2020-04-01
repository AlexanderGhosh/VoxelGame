#include "EntityHander.h"

EntityHander::EntityHander() {
}

void EntityHander::render(Camera& cam, glm::mat4 projection, World& world) {
	for (auto& e : entitys) {
		if (!world.getChunkOccupied(e.getPosition())) continue;
		e.render(projection, &cam);
	}
}

void EntityHander::addEntity(Entity& entity) {
	entitys.push_back(entity);
	entitys.back().create();
}

void EntityHander::updatePositions(GLfloat deltaTime, World& world) {
	std::string collision = "";
	for (auto& e : entitys) {
		if (!world.getChunkOccupied(e.getPosition())) continue;
		e.updatePosition(deltaTime, world, collision);
	}
}

void EntityHander::create() {
	for (auto& e : entitys) {
		e.create();
	}
}

void EntityHander::setTarget(glm::vec3 target) {
	for (Entity& e : entitys) {
		e.setTarget(target);
	}
}

void EntityHander::getNewTarget() {
	for (Entity& e : entitys) {
		e.getNewTarget();
	}
}
void EntityHander::moveToTarget() {
	for (Entity& e : entitys) {
		e.moveToTarget();
	}
}