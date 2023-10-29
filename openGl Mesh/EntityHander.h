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
	void render(Camera& cam, glm::mat4 projection, std::vector<ChunkColumn*> occupied);
	void addEntity(Entity& entity, bool create = 1);
	void updatePositions(float deltaTime, std::vector<ChunkColumn*> occupied, std::vector<std::vector<ChunkColumn*>> adjacentChunks);
	void update(glm::mat4 projection, Camera& cam, std::vector<std::vector<ChunkColumn*>> adjacent, std::vector<ChunkColumn*> occuped, float deltaTime);
	void attackPlayer(Entity& player);

	// ai
	void setTarget(glm::vec3 target);
	void getNewTarget();
	void moveToTarget();

	// getters
	std::vector<Entity>& getEntitys();
};

