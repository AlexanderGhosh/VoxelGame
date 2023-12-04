#pragma once
#include "Helpers/Constants.h"

class BlockStore
{
private: // properties
	HeightMap heightMap;
public:
	BlockStore();
	BlockStore(glm::vec2 pos, unsigned int seed);
	BlockStore(glm::vec2 pos, unsigned int seed, bool limitSamples);
	
	const BlocksEncoded& getBlocksAt(glm::vec2 pos) const;
	const BlocksEncoded& getBlocksAt(float x, float z) const;
	const Block getBlock(glm::vec3 relativePos, bool check = true) const;
};

