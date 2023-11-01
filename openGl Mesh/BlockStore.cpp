#include "BlockStore.h"
#include "Helpers/Functions.h"
#include "Game/World/world_generation.h"

BlockStore::BlockStore() : heightMap() { }

BlockStore::BlockStore(glm::vec2 pos, unsigned int seed) : BlockStore()
{
	world_generation::createHeightMap(pos, seed, heightMap);
}

const BlocksEncoded& BlockStore::getBlocksAt(glm::vec2 pos) const
{
	return heightMap[columnIndex(pos.x, pos.y)];
}

const BlocksEncoded& BlockStore::getBlocksAt(float x, float z) const
{
	return heightMap[columnIndex(x, z)];
}

const Block BlockStore::getBlock(glm::vec3 relativePos) const
{
	// get from height map
	BlocksEncoded encodes;
	glm::vec2 hPos(relativePos.x, relativePos.z);
	if (glm::all(glm::lessThan(hPos, glm::vec2(CHUNK_SIZE)) && glm::greaterThanEqual(hPos, { 0, 0 }))) {
		encodes = heightMap[columnIndex(hPos.x, hPos.y)];
	}
	return encodes[relativePos.y];
}