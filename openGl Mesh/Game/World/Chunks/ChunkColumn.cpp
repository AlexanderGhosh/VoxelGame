#include "ChunkColumn.h"
#include <algorithm>
#include "../../../Buffer.h"
#include "../../../GeomRendering/GeomData.h"

ChunkColumn::ChunkColumn() : position(0), buffer(), blockStore()
{
	// blocks.fill(Blocks::AIR);
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed) : ChunkColumn()
{
	position = pos;
	blockStore = BlockStore(pos, seed);
}

const Blocks getBlockNew(const glm::vec3& worldPos, const WorldMap& map) {
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
		return Blocks::ERROR;
	}
	auto pair = *itt;
	const BlockStore& chunk = *pair.second;
	return chunk.getBlock(localPos, false, true);
}

void ChunkColumn::createMeshNew(WorldMap* worldMap) {
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

			std::vector<Block_Count>& encodes = blockStore.getBlocksAt(x, z);
			int height = 0;
			for (const Block_Count& bc : encodes) {
				height += bc.second;
			}

			for (auto itt = encodes.rbegin(); itt != encodes.rend(); itt++) {
				Block_Count& bc = *itt;
				if (bc.first == Blocks::AIR || bc.first == Blocks::WATER) {
					height -= bc.second;
					continue;
				}
				const BlockDet& blockDets1 = getDets(bc.first);
				data.textureIndex_ = (unsigned int)bc.first;

				for (unsigned int i = 0; i < bc.second; i++) {
					data.worldPos_ = glm::vec3(x, height--, z) + glm::vec3(position.x, 0, position.y);
					if (data.worldPos_.x == 13 && data.worldPos_.z == 5) {
						int j = 0;
					}
					int j = 0;
					for (const glm::vec3& off : offsets) {
						const glm::vec3 p = data.worldPos_ + off;
						const Blocks& b = getBlock(p, true, true, worldMap);

						const BlockDet& blockDets2 = getDets(b);

						if (blockDets2.isTransparant && (bc.first != b)) {
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
	blockStore.setHaveTrees(1);
	AdjacentMap_p adjacent;
	std::vector<glm::vec2> treePos = world_generation::getTreePositions(position);
	for (auto& pos : treePos) {
		GLubyte x = pos.x;
		GLubyte z = pos.y;
		auto t = getHeightAt(pos, GL_TRUE, adjacent);
		if (getBlock({ x, std::get<1>(t), z }, 0, 0) != Blocks::GRASS) {
			continue;
		}
		std::vector<Block_Count>* encoded = std::get<0>(t);
		unsigned int maxHeight = std::get<1>(t);
		ChunkColumn* column = std::get<2>(t);
		if (!column) continue;
		encoded->push_back({ Blocks::LOG, 4 });
		encoded->push_back({ Blocks::LEAF, 1 });
		unsigned int row = maxHeight + 2;
		/*column->editBlock({ x, row - 1, z }, 0, Blocks::LOG, adjacentPtr);
		column->editBlock({ x, row, z }, 0, Blocks::LOG, adjacentPtr);
		column->editBlock({ x, row + 1, z }, 0, Blocks::LOG, adjacentPtr);
		column->editBlock({ x, row + 2, z }, 0, Blocks::LOG, adjacentPtr);

		column->editBlock({ x, row + 3, z }, 0, Blocks::LEAF, adjacentPtr);
		column->editBlock({ x, row + 4, z }, 0, Blocks::LEAF, adjacentPtr);*/
		// return;
		unsigned int diff = 0;
		t = getHeightAt({ x + 1, z }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x - 1, z }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x, z + 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x, z - 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}
		// corners
		t = getHeightAt({ x + 1, z + 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x + 1, z - 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x - 1, z + 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x - 1, z - 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}
	}
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

Blocks ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, bool checkEdited)
{
	return blockStore.getBlock(pos, worldPos, checkEdited);
}

glm::vec3 ChunkColumn::getRelativePosition(glm::vec3 worldPos)
{
	return worldPos - position;
}

glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos)
{
	return relativePos + position;
}

std::tuple<std::vector<Block_Count>*, unsigned int, ChunkColumn*> ChunkColumn::getHeightAt(glm::vec2 pos, bool safe, AdjacentMap_p& adjacent)
{
	std::tuple<std::vector<Block_Count>*, unsigned int, ChunkColumn*> t = { nullptr, 0, nullptr };
	if (!this) return t;
	unsigned int maxHeight = 0;
	if (glm::all(glm::greaterThanEqual(pos, { 0, 0 }) && glm::lessThan(pos, { CHUNK_SIZE, CHUNK_SIZE }))) {
		auto& encoded = blockStore.getBlocksAt(pos);
		std::for_each(encoded.begin(), encoded.end(), [&](Block_Count& encode) { maxHeight += encode.second; });
		return { &encoded, maxHeight, this };
	}
	if (!safe) return t;
	glm::vec2 chunkPositionToLookAt = position;
	if (pos.x < 0) {
		pos.x += CHUNK_SIZE;
		chunkPositionToLookAt.x -= CHUNK_SIZE;
	}
	else if (pos.x > CHUNK_SIZE - 1) {
		pos.x -= CHUNK_SIZE;
		chunkPositionToLookAt.x += CHUNK_SIZE;
	}

	if (pos.y < 0) {
		pos.y += CHUNK_SIZE;
		chunkPositionToLookAt.y -= CHUNK_SIZE;
	}
	else if (pos.y > CHUNK_SIZE - 1) {
		pos.y -= CHUNK_SIZE;
		chunkPositionToLookAt.y += CHUNK_SIZE;
	}
	if (adjacent.size() > 0) {

		return adjacent[chunkPositionToLookAt]->getHeightAt(pos, 0, adjacent);
	}
	return t;
}

// editors
void ChunkColumn::addBlock(glm::vec3 position, bool worldPos, Blocks block, WorldMap* worldMap) {
	/*if (block == Blocks::AIR) return;
	glm::vec3 relativePos = worldPos ? getRelativePosition(position) : position;
	const float& x = relativePos.x;
	const float& y = relativePos.y;
	const float& z = relativePos.z;

	glm::vec3 worldPosition = worldPos ? position : getWorldPosition(position);
	auto addToMesh = [](Buffer* buffer, Texture* texture, glm::vec3 position, std::unordered_map<unsigned int, FaceB_p>& mesh) {
		unsigned int key = std::pow(buffer->type + 1, 2) * std::pow(texture->getTexMap() + 1, 3);
		glm::mat4 model = translate(glm::mat4(1), position);
		auto& faces = mesh[key];
		if (std::get<0>(faces) == nullptr) {
			faces = { buffer, texture, { model } };
		}
		else {
			std::get<2>(faces).push_back(model);
		}
	};

	unsigned int tex_index = (unsigned int)getTexture(block);

	glm::vec3 delta(1);
	Buffer* faces[] = { 
		FACES[RIGHT],
		FACES[TOP],
		FACES[FRONT],
		FACES[LEFT],
		FACES[BOTTOM],
		FACES[BACK]
	};
	for (GLubyte i = 0; i < 6; i++) {
		glm::vec3 lookingAt = relativePos;
		Buffer* face = faces[i];
		if (i > 2) {
			lookingAt[i - 3] -= delta[i - 3];
		}
		else {
			lookingAt[i] += delta[i];
		}
		Blocks found = getBlock(lookingAt, 0, 1, worldMap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh(face, TEXTURES[tex_index], worldPosition, mesh);
			}
		}
	}*/
}

// getters
Blocks ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap* worldMap) {
	glm::vec3 relativePostion = worldPos ? getRelativePosition(pos) : pos;

	if (glm::all(glm::greaterThanEqual(relativePostion, { 0, 0, 0 }))) {
		if (glm::all(glm::lessThan(relativePostion, { CHUNK_SIZE, WORLD_HEIGHT, CHUNK_SIZE }))) {
			return getBlock(pos, worldPos);
		}
	}
	if (!safe) return Blocks::ERROR;
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
		if (found == worldMap->end()) return Blocks::ERROR;
		auto foundBlock = (*found).second->getBlock(relativePostion, false, true);
		return foundBlock;
	}
	return Blocks::ERROR;
}

FaceB_p Tuple_::toFace() {
	Texture* tex = nullptr;
	for (auto& t : TEXTURES) {
		if (t->getTexMap() == data.textureMap) {
			tex = t;
			break;
		}
	}if (!tex) tex = TEXTURES[0];
	FaceB_p face = { FACES[data.bufferType], tex, models };
	return face;
}

std::pair<glm::vec3, Blocks> BlockData::toBlockD()
{
	return { pos, toBlock(block) };
}
