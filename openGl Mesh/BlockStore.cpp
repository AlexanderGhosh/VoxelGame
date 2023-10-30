#include "BlockStore.h"
#include "Game/World/world_generation.h"

BlockStore::BlockStore() : position(), heightMap(), editedBlocks(), hasTrees(false) { }

BlockStore::BlockStore(glm::vec2 pos, unsigned int seed) : BlockStore()
{
	position = pos;
	heightMap = world_generation::createHeightMap(pos, seed);
}

const glm::vec2& BlockStore::getPosition() const
{
	return position;
}

const HeightMap& BlockStore::getHeightMap() const
{
	return heightMap;
}

std::unordered_map<glm::vec3, Block>& BlockStore::getEditedBlocks()
{
	return editedBlocks;
}

const BlocksEncoded& BlockStore::getBlocksAt(glm::vec2 pos) const
{
	return heightMap[columnIndex(pos.x, pos.y)];
}

const BlocksEncoded& BlockStore::getBlocksAt(float x, float z) const
{
	return heightMap[columnIndex(x, z)];
}

const Block BlockStore::getBlock(glm::vec3 pos, bool worldPosition, bool checkEdited) const
{
	auto getRelativePosition = [&](glm::vec3 pos) -> glm::vec3 {
		return pos - this->getPosition();
	};
	glm::vec3 relativePos = worldPosition ? getRelativePosition(pos) : pos;
	// check changed blocks
	if (checkEdited) {
		unsigned int size = editedBlocks.size();
		if (editedBlocks.find(relativePos) != editedBlocks.cend()) {
			return editedBlocks.at(relativePos);
		}
	}
	// get from height map
	BlocksEncoded encodes;
	glm::vec2 hPos(relativePos.x, relativePos.z);
	if (glm::all(glm::lessThan(hPos, glm::vec2(CHUNK_SIZE)) && glm::greaterThanEqual(hPos, { 0, 0 }))) {
		encodes = heightMap[columnIndex(hPos.x, hPos.y)];
	}
	return encodes[relativePos.y];
	/*
	for (Block_Count& encoded : encodes) {
		relativePos.y -= encoded.second;
		if (relativePos.y <= 0) {
			return encoded.first;
		}
	}
	return Block::AIR;
	*/
}

bool BlockStore::doesHaveTrees()
{
	return hasTrees;
}

void BlockStore::setPosition(glm::vec2 pos)
{
	position = pos;
}

void BlockStore::setHeightMap(const HeightMap& hm)
{
	heightMap = hm;
}

void BlockStore::setEditedBlocks(std::unordered_map<glm::vec3, Block>& edited)
{
	editedBlocks = edited;
}

void BlockStore::setHaveTrees(bool trees)
{
	hasTrees = trees;
}
