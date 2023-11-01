#include "ChunkColumn.h"
#include <algorithm>
#include "../../../Helpers/Functions.h"
#include "../../../GeomRendering/GeomData.h"
#include "../world_generation.h"
#include "../../../Helpers/BlockDetails.h"
#include "../world_generation.h"

ChunkColumn::ChunkColumn() : position(0), buffer(), seed()
{
	// blocks.fill(Block::AIR);
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map) : ChunkColumn()
{
	this->seed = seed;
	position = pos;
	map[pos] = BlockStore(pos, seed);
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
	std::vector<GeomData> bufferData;
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
				if (b1 == Block::WATER) {
					int t = 0;
				}
				const BlockDetails& blockDets1 = getDetails(b1);
				data.textureIndex_ = (unsigned int)b1;
				bool added = false;
				for (unsigned int i = 0; i < count1; i++) {
					data.worldPos_ = glm::vec3(x, height - i, z) + glm::vec3(position.x, 0, position.y);
					int j = 0;
					added = false;
					for (const glm::vec3& off : offsets) {
						const glm::vec3 p = data.worldPos_ + off;
						const Block& b2 = getBlock(p, true, true, worldMap);

						const BlockDetails& blockDets2 = getDetails(b2);

						if (blockDets2.isTransparant && b1 != b2) {
							data.cubeType_ = j;
							bufferData.push_back(data);
							added = true;
						}
						j++;
					}
					if (!added) break;
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
	return worldPos - position;
}

const glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos) const
{
	return relativePos + position;
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
		chunkPositionToLookAt.x -= CHUNK_SIZE;
	}
	else if (relativePostion.x > CHUNK_SIZE - 1) {
		relativePostion.x -= CHUNK_SIZE;
		chunkPositionToLookAt.x += CHUNK_SIZE;
	}

	if (relativePostion.z < 0) {
		relativePostion.z += CHUNK_SIZE;
		chunkPositionToLookAt.y -= CHUNK_SIZE;
	}
	else if (relativePostion.z > CHUNK_SIZE - 1) {
		relativePostion.z -= CHUNK_SIZE;
		chunkPositionToLookAt.y += CHUNK_SIZE;
	}
	if (worldMap.size() > 0) {
		auto found = worldMap.find(chunkPositionToLookAt);
		if (found == worldMap.end()) return Block::ERROR;
		auto foundBlock = (*found).second.getBlock(relativePostion);
		return foundBlock;
	}
	return Block::ERROR;
}