#include "BlockStore.h"

BlockStore::BlockStore() : initalised(0), hasTrees(0)
{
	
}

BlockStore::BlockStore(glm::vec2 pos) : position(pos), editedBlocks(), initalised(1), hasTrees(0)
{
	heightMap = world_generation::createHeightMap(pos, 0);
}

glm::vec2& BlockStore::getPosition()
{
	return position;
}

HeightMap& BlockStore::getHeightMap()
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

std::vector<Block_Count>& BlockStore::getBlocksAt(GLfloat x, GLfloat z)
{
	return heightMap[x][z];
}

Blocks BlockStore::getBlock(glm::vec3 pos, GLboolean worldPosition)
{
	auto getRelativePosition = [&](glm::vec3 pos) -> glm::vec3 {
		return pos - this->getPosition();
	};
	glm::vec3 relativePos = worldPosition ? getRelativePosition(pos) : pos;
	// check changed blocks
	GLuint size = editedBlocks.size();
	Blocks t = editedBlocks[relativePos];
	if (editedBlocks.size() > size) {
		editedBlocks.erase(relativePos);
	}
	else {
		return t;
	}

	// get from height map
	std::vector<Block_Count> encodes;
	glm::vec2 hPos(relativePos.x, relativePos.z);
	if (glm::all(glm::lessThan(hPos, glm::vec2(CHUNK_SIZE)) && glm::greaterThanEqual(hPos, { 0, 0 }))) {
		encodes = heightMap[hPos.x][hPos.y];
	}
	for (Block_Count& encoded : encodes) {
		relativePos.y -= encoded.second;
		if (relativePos.y < 0) {
			return encoded.first;
		}
	}
	return Blocks::AIR;
}

GLboolean BlockStore::doesHaveTrees()
{
	return hasTrees;
}

GLboolean BlockStore::isInitilised()
{
	return initalised;
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

void BlockStore::setHaveTrees(GLboolean trees)
{
	hasTrees = trees;
}
