#include "ChunkColumn.h"
#include <algorithm>
#include "../../../Helpers/Functions.h"
#include "../world_generation.h"
#include "../../../Helpers/BlockDetails.h"
#include "../world_generation.h"
#include "../../../Helpers/Functions.h"
#include <iostream>

ChunkColumn::ChunkColumn() : position(0), buffer(), seed(), bufferData()
{
	// blocks.fill(Block::AIR);
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map) : ChunkColumn()
{
	this->seed = seed;
	position = pos;
	map[pos] = BlockStore(pos * (float) CHUNK_SIZE, seed);
}

void ChunkColumn::populateBuffer(WorldMap& worldMap) {
	const std::list<glm::vec3> offsets = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),

	};
	const BlockStore& blockStore = worldMap[position];
	GeomData data{};
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			const BlocksEncoded& encodes = blockStore.getBlocksAt(x, z);
			int height = encodes.height();
			unsigned int depth = 0;
			for (int r = encodes.size() - 1; r >= 0; r--) {
				const Block b1 = encodes.block(r);
				const unsigned int count1 = encodes.count(r);

				if (b1 == Block::AIR) {
					height -= count1;
					depth += count1;
					continue;
				}
				const BlockDetails& blockDets1 = getDetails(b1);
				data.textureIndex_ = (unsigned int)b1;
				bool added = false;
				for (unsigned int i = 0; i < count1; i++) {
					data.worldPos_ = glm::vec3(x, height - i, z) + glm::vec3(position.x * CHUNK_SIZE, 0, position.y * CHUNK_SIZE);
					unsigned int j = 0;
					added = false;
					std::vector<unsigned int> added_list;
					for (const glm::vec3& off : offsets) {
						const glm::vec3 p = data.worldPos_ + off;
						const Block& b2 = getBlock(p, true, true, worldMap);

						const BlockDetails& blockDets2 = getDetails(b2);

						if (blockDets2.isTransparant && b1 != b2) {
							markSlot(data.cubeType_, j);
						}
						j++;
					}
					if (data.cubeType_) {
						bufferData.push_back(data);
						added = true;
						data.cubeType_ = 0;
						added_list.clear();
					}
					else {
						break;
					}
				}
				height -= count1;
				if (!added && !blockDets1.isTransparant) break;
			}
		}
	}

	buffer.setUp(bufferData.data(), bufferData.size());
}

const BufferGeom& ChunkColumn::getBuffer() const
{
	return buffer;
}

BufferGeom* ChunkColumn::getBufferPtr()
{
	return &buffer;
}

void ChunkColumn::addBlock(const glm::vec3& worldPos, const Block block)
{
	GeomData toAdd;
	toAdd.worldPos_ = worldPos;
	toAdd.cubeType_ = 0x3f;
	toAdd.textureIndex_ = (unsigned char) block;

	struct RemoveFace {
		glm::vec3 worldPos;
		unsigned char face;
	};
	std::vector<RemoveFace> toRemove;


	const std::list<glm::vec3> offsets = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),

	};
	unsigned char i = 0;
	for (const glm::vec3& off : offsets) {
		// needs to check the world map
		RemoveFace data{};
		data.worldPos = worldPos + off;
		data.face = i++;

		toRemove.push_back(data);
	}


	for (auto itt1 = bufferData.begin(); itt1 != bufferData.end();) {
		GeomData& data = *itt1;
		for (auto itt2 = toRemove.begin(); itt2 != toRemove.end();) {
			const RemoveFace& remove = *itt2;
			if (data.worldPos_ == remove.worldPos) {
				unsigned int mask = 1 << remove.face;
				toAdd.cubeType_ ^= mask; // sets the face to add

				if (remove.face % 2 == 0) {
					mask = 1 << remove.face + 1;
				}
				else {
					mask = 1 << remove.face - 1;
				}

				mask = ~mask; // this is in the wrong slot as the face refers to the new blocks face not the neigbour the slot needs to inccremented or decrmented dependent MAYBE NOT ANY MORE
				data.cubeType_ &= mask; // will set the slot of face to 0
				itt2 = toRemove.erase(itt2);
			}
			else {
				itt2++;
			}
		}
		if (!data.cubeType_) {
			itt1 = bufferData.erase(itt1); // deletes blocks which have no visible faces
			continue;
		}
		if (toRemove.size() == 0) {
			break;
		}
		itt1++;
	}
	// has now removed all the faces needed to place the block
	bufferData.push_back(toAdd);
	buffer.realloc(bufferData.data(), bufferData.size());
}

void ChunkColumn::removeBlock(const glm::vec3& worldPos)
{
	struct AddFaces {
		glm::vec3 worldPos;
		unsigned char face;
	};
	std::vector<AddFaces> toAdd;


	const std::list<glm::vec3> offsets = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),

	};
	unsigned char i = 0;
	for (const glm::vec3& off : offsets) {
		// needs to check the world map
		AddFaces data{};
		data.worldPos = worldPos + off;
		data.face = i++;
		if (data.face % 2 == 0) {
			data.face += 1;
		}
		else {
			data.face -= 1;
		}

		toAdd.push_back(data);
	}

	bool removed = false;
	for (auto itt1 = bufferData.begin(); itt1 != bufferData.end();) {
		GeomData& data = *itt1;
		if (data.worldPos_ == worldPos) {
			itt1 = bufferData.erase(itt1);
			removed = true;
			continue;
		}
		for (auto itt2 = toAdd.begin(); itt2 != toAdd.end();) {
			const AddFaces& add = *itt2;
			if (data.worldPos_ == add.worldPos) {
				unsigned int mask = 1 << add.face;

				data.cubeType_ |= mask; // will set the slot of face to 0
				itt2 = toAdd.erase(itt2);
			}
			else {
				itt2++;
			}
		}
		if (!data.cubeType_) {
			itt1 = bufferData.erase(itt1); // deletes blocks which have no visible faces
			continue;
		}
		if (toAdd.size() == 0 && removed) {
			break;
		}
		itt1++;
	}

	buffer.realloc(bufferData.data(), bufferData.size());
}

const glm::vec2& ChunkColumn::getPosition() const
{
	return position;
}

const Block ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const
{
	if (worldPos) {
		return blockStore.getBlock(getRelativePosition(pos));
	}
	return blockStore.getBlock(pos);
}

const glm::vec3 ChunkColumn::getRelativePosition(glm::vec3 worldPos)const 
{
	return worldPos - position * (float) CHUNK_SIZE;
}

const glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos) const
{
	return relativePos + position * (float)CHUNK_SIZE;
}

// getters
const Block ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const {
	glm::vec3 relativePostion = worldPos ? getRelativePosition(pos) : pos;

	const BlockStore& blockStore = worldMap[position];

	if (glm::all(glm::greaterThanEqual(relativePostion, { 0, 0, 0 }))) {
		if (glm::all(glm::lessThan(relativePostion, { CHUNK_SIZE, WORLD_HEIGHT, CHUNK_SIZE }))) {
			return getBlock(pos, worldPos, blockStore);
		}
	}
	if (!safe) return Block::AIR;
	// safe

	glm::vec2 chunkPositionToLookAt = position;
	if (relativePostion.x < 0) {
		relativePostion.x += CHUNK_SIZE;
		chunkPositionToLookAt.x -= 1;
	}
	else if (relativePostion.x > CHUNK_SIZE - 1) {
		relativePostion.x -= CHUNK_SIZE;
		chunkPositionToLookAt.x += 1;
	}

	if (relativePostion.z < 0) {
		relativePostion.z += CHUNK_SIZE;
		chunkPositionToLookAt.y -= 1;
	}
	else if (relativePostion.z > CHUNK_SIZE - 1) {
		relativePostion.z -= CHUNK_SIZE;
		chunkPositionToLookAt.y += 1;
	}
	if (worldMap.size() > 0) {
		auto found = worldMap.find(chunkPositionToLookAt);
		if (found == worldMap.end()) return Block::ERROR;
		auto foundBlock = (*found).second.getBlock(relativePostion);
		return foundBlock;
	}
	return Block::ERROR;
}