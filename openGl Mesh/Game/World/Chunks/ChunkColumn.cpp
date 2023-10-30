#include "ChunkColumn.h"
#include <algorithm>
#include "../../../GeomRendering/GeomData.h"
#include "../world_generation.h"
#include "../../../Helpers/BlockDetails.h"

ChunkColumn::ChunkColumn() : position(0), buffer(), blockStore()
{
	// blocks.fill(Block::AIR);
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed) : ChunkColumn()
{
	position = pos;
	blockStore = BlockStore(pos, seed);
}

const Block getBlockNew(const glm::vec3& worldPos, const WorldMap& map) {
	glm::vec2 chunkPos(worldPos.x, worldPos.z);
	chunkPos.x = ((int)chunkPos.x) / 16;
	chunkPos.y = ((int)chunkPos.y) / 16;
	chunkPos *= 16.f;

	glm::vec3 localPos = worldPos;
	localPos.x -= chunkPos.x;
	localPos.z -= chunkPos.y;
	localPos = abs(localPos);

	auto itt = map.find(chunkPos);
	if (itt == map.end()) {
		return Block::ERROR;
	}
	auto& pair = *itt;
	const BlockStore& chunk = *pair.second;
	return chunk.getBlock(localPos, false, true);
}

void ChunkColumn::populateBuffer(WorldMap* worldMap) {
	const std::list<glm::vec3> offsets = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),

	};
	std::vector<GeomData> bufferData;
	GeomData data{};
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {

			const BlocksEncoded& encodes = blockStore.getBlocksAt(x, z);
			int height = encodes.height();
			for (int r = encodes.size() - 1; r >= 0; r--) {
				const Block b1 = encodes.block(r);
				const unsigned int count1 = encodes.count(r);

				if (b1 == Block::AIR || b1 == Block::WATER) {
					height -= count1;
					continue;
				}
				const BlockDetails& blockDets1 = getDetails(b1);
				data.textureIndex_ = (unsigned int)b1;

				for (unsigned int i = 0; i < count1; i++) {
					data.worldPos_ = glm::vec3(x, height--, z) + glm::vec3(position.x, 0, position.y);
					int j = 0;
					for (const glm::vec3& off : offsets) {
						const glm::vec3 p = data.worldPos_ + off;
						const Block& b = getBlock(p, true, true, worldMap);

						const BlockDetails& blockDets2 = getDetails(b);

						if (blockDets2.isTransparant && (b1 != b)) {
							data.cubeType_ = j;
							bufferData.push_back(data);
						}
						j++;
					}

				}
			}
		}
	}

	buffer.setUp(bufferData.data(), bufferData.size());
}

void ChunkColumn::addTrees() {
	if (blockStore.doesHaveTrees()) return;
	blockStore.setHaveTrees(true);

	//AdjacentMap_p adjacent;
	//std::vector<glm::vec2> treePos = world_generation::getTreePositions(position);
	//for (auto& pos : treePos) {
	//	GLubyte x = pos.x;
	//	GLubyte z = pos.y;
	//	auto t = getHeightAt(pos, GL_TRUE, adjacent);
	//	if (getBlock({ x, std::get<1>(t), z }, 0, 0) != Block::GRASS) {
	//		continue;
	//	}
	//	std::vector<Block_Count>* encoded = std::get<0>(t);
	//	unsigned int maxHeight = std::get<1>(t);
	//	ChunkColumn* column = std::get<2>(t);
	//	if (!column) continue;
	//	encoded->push_back({ Block::LOG, 4 });
	//	encoded->push_back({ Block::LEAF, 1 });
	//	unsigned int row = maxHeight + 2;
	//	/*column->editBlock({ x, row - 1, z }, 0, Block::LOG, adjacentPtr);
	//	column->editBlock({ x, row, z }, 0, Block::LOG, adjacentPtr);
	//	column->editBlock({ x, row + 1, z }, 0, Block::LOG, adjacentPtr);
	//	column->editBlock({ x, row + 2, z }, 0, Block::LOG, adjacentPtr);

	//	column->editBlock({ x, row + 3, z }, 0, Block::LEAF, adjacentPtr);
	//	column->editBlock({ x, row + 4, z }, 0, Block::LEAF, adjacentPtr);*/
	//	// return;
	//	unsigned int diff = 0;
	//	t = getHeightAt({ x + 1, z }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 3/* < 0 ? diff : 0*/ });
	//	}

	//	t = getHeightAt({ x - 1, z }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 3/* < 0 ? diff : 0*/ });
	//	}

	//	t = getHeightAt({ x, z + 1 }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 3/* < 0 ? diff : 0*/ });
	//	}

	//	t = getHeightAt({ x, z - 1 }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 3/* < 0 ? diff : 0*/ });
	//	}
	//	// corners
	//	t = getHeightAt({ x + 1, z + 1 }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 2/* < 0 ? diff : 0*/ });
	//	}

	//	t = getHeightAt({ x + 1, z - 1 }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 2/* < 0 ? diff : 0*/ });
	//	}

	//	t = getHeightAt({ x - 1, z + 1 }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 2/* < 0 ? diff : 0*/ });
	//	}

	//	t = getHeightAt({ x - 1, z - 1 }, GL_TRUE, adjacent);
	//	encoded = std::get<0>(t);
	//	maxHeight = std::get<1>(t);
	//	column = std::get<2>(t);
	//	if (column) {
	//		diff = row - maxHeight;
	//		if (diff > 0) {
	//			encoded->push_back({ Block::AIR, diff });
	//		}
	//		encoded->push_back({ Block::LEAF, 2/* < 0 ? diff : 0*/ });
	//	}
	//}
}

const BufferGeom& ChunkColumn::getBuffer() const
{
	return buffer;
}

const BlockStore& ChunkColumn::getBlockStore() const
{
	return blockStore;
}

const glm::vec2& ChunkColumn::getPosition() const
{
	return position;
}

const Block ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, bool checkEdited) const
{
	return blockStore.getBlock(pos, worldPos, checkEdited);
}

const glm::vec3 ChunkColumn::getRelativePosition(glm::vec3 worldPos)const 
{
	return worldPos - position;
}

const glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos) const
{
	return relativePos + position;
}

//const std::tuple<std::vector<Block_Count>*, unsigned int, ChunkColumn*> ChunkColumn::getHeightAt(glm::vec2 pos, bool safe, AdjacentMap_p& adjacent) const
//{
//	std::tuple<std::vector<Block_Count>*, unsigned int, ChunkColumn*> t = { nullptr, 0, nullptr };
//	if (!this) return t;
//	if (glm::all(glm::greaterThanEqual(pos, { 0, 0 }) && glm::lessThan(pos, { CHUNK_SIZE, CHUNK_SIZE }))) {
//		auto& encoded = blockStore.getBlocksAt(pos);
//		unsigned int maxHeight = encoded.height();
//		return { &encoded, maxHeight, this };
//	}
//	if (!safe) return t;
//	glm::vec2 chunkPositionToLookAt = position;
//	if (pos.x < 0) {
//		pos.x += CHUNK_SIZE;
//		chunkPositionToLookAt.x -= CHUNK_SIZE;
//	}
//	else if (pos.x > CHUNK_SIZE - 1) {
//		pos.x -= CHUNK_SIZE;
//		chunkPositionToLookAt.x += CHUNK_SIZE;
//	}
//
//	if (pos.y < 0) {
//		pos.y += CHUNK_SIZE;
//		chunkPositionToLookAt.y -= CHUNK_SIZE;
//	}
//	else if (pos.y > CHUNK_SIZE - 1) {
//		pos.y -= CHUNK_SIZE;
//		chunkPositionToLookAt.y += CHUNK_SIZE;
//	}
//	if (adjacent.size() > 0) {
//
//		return adjacent[chunkPositionToLookAt]->getHeightAt(pos, 0, adjacent);
//	}
//	return t;
//}

// getters
const Block ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap* worldMap) const {
	glm::vec3 relativePostion = worldPos ? getRelativePosition(pos) : pos;

	if (glm::all(glm::greaterThanEqual(relativePostion, { 0, 0, 0 }))) {
		if (glm::all(glm::lessThan(relativePostion, { CHUNK_SIZE, WORLD_HEIGHT, CHUNK_SIZE }))) {
			return getBlock(pos, worldPos);
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
	if (worldMap->size() > 0) {
		auto found = worldMap->find(chunkPositionToLookAt);
		if (found == worldMap->end()) return Block::ERROR;
		auto foundBlock = (*found).second->getBlock(relativePostion, false, true);
		return foundBlock;
	}
	return Block::ERROR;
}