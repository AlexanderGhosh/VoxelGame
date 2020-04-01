#include "EntityHander.h"

EntityHander::EntityHander() {
}

void EntityHander::render(Camera& cam, glm::mat4 projection, std::vector<ChunkColumn*> occupied) {
	int i = 0;
	for (auto& e : entitys) {
		if (!occupied[i++]) continue;
		e.render(projection, &cam);
	}
}

void EntityHander::addEntity(Entity& entity, GLboolean create) {
	entitys.push_back(entity);
	if(create) entitys.back().create();
}

void EntityHander::updatePositions(GLfloat deltaTime, std::vector<ChunkColumn*> occupied, std::vector<std::vector<ChunkColumn*>> adjacentChunks) {
	std::string collision = "";
	int i = 0;
	int j = 0;
	for (auto& e : entitys) {
		if (!occupied[j++]) continue;
		e.updatePosition(deltaTime, adjacentChunks[i++], collision);
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

std::vector<Entity>& EntityHander::getEntitys()
{
	return entitys;
}
