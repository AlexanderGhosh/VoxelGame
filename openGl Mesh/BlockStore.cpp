#include "BlockStore.h"

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

std::unordered_map<glm::vec3, Blocks>& BlockStore::getEditedBlocks()
{
	return editedBlocks;
}

std::vector<Block_Count>& BlockStore::getBlocksAt(glm::vec2 pos)
{
	return heightMap[pos.x][pos.y];
}

std::vector<Block_Count>& BlockStore::getBlocksAt(float x, float z)
{
	return heightMap[x][z];
}

const Blocks BlockStore::getBlock(glm::vec3 pos, bool worldPosition, bool checkEdited) const
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
	std::vector<Block_Count> encodes;
	glm::vec2 hPos(relativePos.x, relativePos.z);
	if (glm::all(glm::lessThan(hPos, glm::vec2(CHUNK_SIZE)) && glm::greaterThanEqual(hPos, { 0, 0 }))) {
		encodes = heightMap[hPos.x][hPos.y];
	}
	for (Block_Count& encoded : encodes) {
		relativePos.y -= encoded.second;
		if (relativePos.y <= 0) {
			return encoded.first;
		}
	}
	return Blocks::AIR;
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

void BlockStore::setEditedBlocks(std::unordered_map<glm::vec3, Blocks>& edited)
{
	editedBlocks = edited;
}

void BlockStore::setHaveTrees(bool trees)
{
	hasTrees = trees;
}
