#pragma once
#include "gtx/hash.hpp"
#include "Game/World/world_generation.h"
class BlockStore
{
private: // properties
	glm::vec2 position;
	HeightMap heightMap;
	std::unordered_map<glm::vec3, Blocks> editedBlocks;
	GLboolean initalised;
public:
	BlockStore();
	BlockStore(glm::vec2 pos);
	
	// getters
	glm::vec2& getPosition();
	HeightMap& getHeightMap();
	std::unordered_map<glm::vec3, Blocks>& getEditedBlocks();
	std::vector<Block_Count>& getBlocksAt(glm::vec2 pos);
	std::vector<Block_Count>& getBlocksAt(GLfloat x, GLfloat z);
	Blocks getBlock(glm::vec3 pos, GLboolean worldPosition);
	GLboolean isInitilised();

	// setters
	void setPosition(glm::vec2 pos);
	void setHeightMap(const HeightMap& hm);
	void setEditedBlocks(std::unordered_map<glm::vec3, Blocks>& edited);

};

