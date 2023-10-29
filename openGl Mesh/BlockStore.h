#pragma once
#include "gtx/hash.hpp"
#include "Game/World/world_generation.h"
class BlockStore
{
private: // properties
	glm::vec2 position;
	HeightMap heightMap;
	std::unordered_map<glm::vec3, Blocks> editedBlocks;
	bool hasTrees;
public:
	BlockStore();
	BlockStore(glm::vec2 pos, unsigned int seed);
	
	// getters
	const glm::vec2& getPosition() const;
	const HeightMap& getHeightMap() const;
	std::unordered_map<glm::vec3, Blocks>& getEditedBlocks();
	std::vector<Block_Count>& getBlocksAt(glm::vec2 pos);
	std::vector<Block_Count>& getBlocksAt(float x, float z);
	const Blocks getBlock(glm::vec3 pos, bool worldPosition, bool checkEdited = 1) const;
	bool doesHaveTrees();

	// setters
	void setPosition(glm::vec2 pos);
	void setHeightMap(const HeightMap& hm);
	void setEditedBlocks(std::unordered_map<glm::vec3, Blocks>& edited);
	void setHaveTrees(bool trees);

};

