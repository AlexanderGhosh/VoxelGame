#pragma once
#include "Game/Player/Entity.h"
class EntityHander
{
private: // properties
	std::vector<Entity> entitys;
public: // functions
	// constructors
	EntityHander();

	// operations
	void render(Camera& cam, glm::mat4 projection);
	void addEntity(Entity& entity);
	void updatePositions(GLfloat deltaTime, World& world);
	void create();

	// ai
	void setTarget(glm::vec3 target);
	void getNewTarget();
	void moveToTarget();
};

