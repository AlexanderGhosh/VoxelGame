#include "BlockStore.h"
#include "Helpers/Functions.h"
#include "Game/World/world_generation.h"

BlockStore::BlockStore() : heightMap() { }

BlockStore::BlockStore(glm::vec2 pos, unsigned int seed) : BlockStore()
{
	world_generation::createHeightMap(pos, seed, heightMap);
}

BlockStore::BlockStore(glm::vec2 pos, unsigned int seed, bool limitSamples)
{
	world_generation::createHeightMapLimitedSamples(pos, seed, heightMap);
}

const BlocksEncoded& BlockStore::getBlocksAt(glm::vec2 pos) const
{
	return heightMap[columnIndex(pos.x, pos.y)];
}

const BlocksEncoded& BlockStore::getBlocksAt(float x, float z) const
{
	return heightMap[columnIndex(x, z)];
}

const Block BlockStore::getBlock(glm::vec3 relativePos, bool check) const
{
	// get from height map
	if (check) {
		BlocksEncoded encodes;
		glm::vec2 hPos(relativePos.x, relativePos.z);
		if (glm::all(glm::lessThan(hPos, glm::vec2(CHUNK_SIZE)) && glm::greaterThanEqual(hPos, { 0, 0 }))) {
			encodes = heightMap[columnIndex(hPos.x, hPos.y)];
			return encodes[relativePos.y];
		}
		return Block::ERROR;
	}
	else {
		const BlocksEncoded& encodes = heightMap[columnIndex(relativePos.x, relativePos.z)];
		return encodes[relativePos.y];
	}
}