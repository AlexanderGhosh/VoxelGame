#include "EntityHander.h"

EntityHander::EntityHander() {
}

void EntityHander::render(Camera& cam, glm::mat4 projection, std::vector<ChunkColumn*> occupied) {
	int i = 0;
	for (auto& e : entitys) {
		if (!occupied[i++]) continue;
		e.render(projection, cam);
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
		e.updatePosition(deltaTime, adjacentChunks[i++]);
	}
}

void EntityHander::update(glm::mat4 projection, Camera& cam, std::vector<std::vector<ChunkColumn*>> adjacent, std::vector<ChunkColumn*> occuped, GLfloat deltaTime) {
	std::vector<GLuint> victimIndex;
	int i = 0;
	int j = 0;
	int k = 0;
	for (Entity& e : entitys) {
		e.update(projection, cam, adjacent[j++], occuped[k++], deltaTime, entitys[0].getPosition());
		if (e.isDead) {
			victimIndex.push_back(i);
			continue;
		}
		e.attack(entitys[0]);
		i++;
	}
	i = 0;
	for (GLuint& index : victimIndex) {
		entitys.erase(entitys.begin() + index - i);
		i++;
	}
}

void EntityHander::attackPlayer(Entity& player) {
	for (auto& e : entitys) {
		e.attack(player);
	}
}

void EntityHander::setTarget(glm::vec3 target) {
	for (Entity& e : entitys) {
		e.setTarget(target);
	}
}

void EntityHander::getNewTarget() {
	for (Entity& e : entitys) {
		e.getNewTarget(e.getPosition() + glm::vec3(10000));
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
