#pragma once
#include "Game/World/constants.h"

class BlockStore
{
private: // properties
	HeightMap heightMap;
public:
	BlockStore();
	BlockStore(glm::vec2 pos, unsigned int seed);
	
	const BlocksEncoded& getBlocksAt(glm::vec2 pos) const;
	const BlocksEncoded& getBlocksAt(float x, float z) const;
	const Block getBlock(glm::vec3 relativePos) const;
};

