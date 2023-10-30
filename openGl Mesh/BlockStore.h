#pragma once
#include "Game/World/constants.h"

class BlockStore
{
private: // properties
	HeightMap heightMap;
	bool hasTrees;
	std::unordered_map<glm::vec3, Block> editedBlocks;
public:
	BlockStore();
	BlockStore(glm::vec2 pos, unsigned int seed);
	
	// getters
	const HeightMap& getHeightMap() const;
	std::unordered_map<glm::vec3, Block>& getEditedBlocks();
	const BlocksEncoded& getBlocksAt(glm::vec2 pos) const;
	const BlocksEncoded& getBlocksAt(float x, float z) const;
	const Block getBlock(glm::vec3 relativePos, bool checkEdited = 1) const;
	bool doesHaveTrees();

	// setters
	void setHeightMap(const HeightMap& hm);
	void setEditedBlocks(std::unordered_map<glm::vec3, Block>& edited);
	void setHaveTrees(bool trees);

};

