#include "ChunkColumn.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <format>
#include "../../../Helpers/Functions.h"
#include "../world_generation.h"
#include "../../../Helpers/BlockDetails.h"
#include "../World.h"
#include "../../../Helpers/Timer.h"


ChunkColumn::ChunkColumn() : position(0), buffer(), seed(), bufferData(), editedBlocks()
{
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map) : ChunkColumn()
{
	this->seed = seed;
	position = pos;
	map[pos] = BlockStore(pos * (float) CHUNK_SIZE, seed);
}

void ChunkColumn::buildBlockStore(glm::vec2 pos, unsigned int seed, const std::list<ChunkColumn*>& neibours)
{
	this->seed = seed;
	position = pos;
	BlockStore bs(pos * CHUNK_SIZE_F, seed);
	populateBuffer(neibours, bs);
}

void ChunkColumn::populateBuffer(const std::list<ChunkColumn*>& neibours, const BlockStore& blockStore) {
	auto getBlockNeibour = [&](glm::vec3 localPos, glm::vec3 offset) {
		if (outOfRange(localPos)) {
			glm::vec2 newChunkPos = position + glm::vec2(offset.x, offset.z);
			glm::vec3 newLocalPos;
			for (ChunkColumn* chunk : neibours) {
				if (chunk->getPosition() == newChunkPos) {
					return chunk->getBlock(newLocalPos);
				}
			}
		}
		return blockStore.getBlock(localPos);
	};
	std::unordered_map<glm::vec3, Block> exploredBlocksCache; // map of all the blocs which have already being looked up
	const std::list<glm::vec3> offsets = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),

	};
	bufferData.reserve(CHUNK_SIZE * CHUNK_SIZE);

	GeomData data{};
	glm::vec3 chunkWorldPos(position.x * CHUNK_SIZE, 0, position.y * CHUNK_SIZE);
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			const BlocksEncoded& encodes = blockStore.getBlocksAt(x, z);

			int height = encodes.height();
			for (int r = encodes.size() - 1; r >= 0; r--) {
				const Block b1 = encodes.block(r);
				const unsigned int count1 = encodes.count(r);

				if (b1 == Block::AIR) {
					height -= count1;
					continue;
				}

				const BlockDetails& blockDets1 = getDetails(b1);
				data.textureIndex_ = (unsigned int)b1;
				bool added = false;
				for (unsigned int i = 0; i < count1; i++) {
					glm::vec3 currentLocalPos = glm::vec3(x, height - i, z);
					data.setPos(currentLocalPos);
					unsigned int j = 0;
					added = false;
					std::vector<unsigned int> added_list;
					for (const glm::vec3& off : offsets) {
						const glm::vec3 p = currentLocalPos + off + chunkWorldPos;
						Block b2 = Block::ERROR;
						Block* b2Ptr = getValue(exploredBlocksCache, p);
						if (b2Ptr) {
							b2 = *b2Ptr;
						}
						else {
							glm::vec3 newLocalPosition = currentLocalPos + off;
							b2 = blockStore.getBlock(newLocalPosition);
							if (b2 == Block::ERROR) {
								glm::vec2 newChunkPos = position + glm::vec2(off.x, off.z);
								glm::vec3 newLocalPos = newLocalPosition - glm::sign(newLocalPosition) * CHUNK_SIZE_F;
								for (ChunkColumn* chunk : neibours) {
									if (chunk->getPosition() == newChunkPos) {
										b2 = chunk->getBlock(newLocalPos);
										break;
									}
								}
							}
							exploredBlocksCache.emplace(p, b2);
						}

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
	bufferData.shrink_to_fit();
	// setUpBuffer(bufferData);
}

void ChunkColumn::setUpBuffer()
{
	buffer.setUp(bufferData.data(), bufferData.size());
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

	std::unordered_map<glm::vec3, Block> exploredBlocksCache; // map of all the blocs which have already being looked up

	const BlockStore& blockStore = worldMap[position];
	GeomData data{};
	glm::vec3 chunkWorldPos(position.x * CHUNK_SIZE, 0, position.y * CHUNK_SIZE);
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			const BlocksEncoded& encodes = blockStore.getBlocksAt(x, z);
			int height = encodes.height();
			unsigned int depth = 0;
			for (int r = encodes.size() - 1; r >= 0; r--) {
				const Block b1 = encodes.block(r);

				const unsigned int count1 = encodes.count(r);

				// prob never the case due to lack of caves
				if (b1 == Block::AIR) {
					height -= count1;
					depth += count1;
					continue;
				}
				bool added = false;
				for (unsigned int i = 0; i < count1; i++) {
					glm::vec3 blockPos = glm::vec3(x, height - i, z);
					data.setPos(blockPos);
					unsigned int j = 0;
					added = false;
					std::vector<unsigned int> added_list;
					for (const glm::vec3& off : offsets) {
						const glm::vec3 p = blockPos + off + chunkWorldPos;

						Block b2 = Block::ERROR;
						if (exploredBlocksCache.find(p) != exploredBlocksCache.end()) {
							b2 = exploredBlocksCache.at(p);
						}
						else {
							b2 = getBlock(p, true, true, worldMap);
							exploredBlocksCache.emplace(p, b2);
						}

						const BlockDetails& blockDets2 = getDetails(b2);

						if (blockDets2.isTransparant && b1 != b2) {
							markSlot(data.cubeType_, j);
						}
						j++;
					}
					if (data.cubeType_) {
						data.textureIndex_ = (unsigned char) b1;
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
				const BlockDetails& blockDets1 = getDetails(b1);
				if (!added && !blockDets1.isTransparant) 
					break;
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
	toAdd.setPos(toLocal(worldPos));
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
		glm::vec3 dataWorldPos = data.getPos() + getWorldPos();
		for (auto itt2 = toRemove.begin(); itt2 != toRemove.end();) {
			const RemoveFace& remove = *itt2;
			if (dataWorldPos == remove.worldPos) {
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

	editedBlocks[worldPos] = block;
	bufferData.push_back(toAdd);
	buffer.realloc(bufferData.data(), bufferData.size());
}

void ChunkColumn::removeBlock(const glm::vec3& worldPos, World* world)
{
	glm::vec3 localPos = toLocal(worldPos);
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
		data.offset = off;
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
		glm::vec3 dataWorldPos = data.getPos() + getWorldPos();
		if (dataWorldPos == worldPos) {
			itt1 = bufferData.erase(itt1);
			removed = true;
			continue;
		}
		for (auto itt2 = toAdd.begin(); itt2 != toAdd.end();) {
			const AddFaces& add = *itt2;
			if (dataWorldPos == add.worldPos) {
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

	std::list<ChunkColumn*> neibours;
	if (toAdd.size() > 0) {
		for (const AddFaces& add : toAdd) {
			if (outOfRange(add.worldPos - position * CHUNK_SIZE_F)) {
				if (!neibours.size()) {
					neibours = world->getNeibours(position);
				}
				glm::vec2 newChunkPos = position;
				newChunkPos.x += add.offset.x;
				newChunkPos.y += add.offset.z;
				for (ChunkColumn* chunk : neibours) {
					if (chunk->getPosition() == newChunkPos) {
						chunk->addFace(add);
					}
				}
			}
			else {

				// the reson why more faces are added then needed is beacuse it doesnt check to see if that blokcs had being broken before
				const Block b = getBlock(add.worldPos);
				if (b == Block::AIR) {
					continue;
				}
				GeomData data;
				data.setPos(toLocal(add.worldPos));
				data.textureIndex_ = (unsigned char) b;
				data.cubeType_ = 1 << add.face;
				bufferData.push_back(data);
			}
		}
	}
	editedBlocks[worldPos] = Block::AIR;

	buffer.realloc(bufferData.data(), bufferData.size());
}

const glm::vec2& ChunkColumn::getPosition() const
{
	return position;
}

void ChunkColumn::save() const
{
	// seed
	// size of chunk data 
	// chunk data (will contain block edits built in)
	union vec2_to_char
	{
		glm::vec2 a;
		char b[8];
		vec2_to_char(glm::vec2 a) :a(a) { }
	};
	union uint_to_char
	{
		unsigned int a;
		char b[4];
		uint_to_char(unsigned int a) :a(a) { }
	};
	union GeomData_to_char
	{
		GeomData a;
		char b[4];
		GeomData_to_char(GeomData a) :a(a) { }
	};
	std::string name = std::format("Chunks/c.{}.{}.chunk", (int)position.x, (int)position.y);
	std::ofstream fs(name, std::ios::out | std::ios::binary);

	char* data = &(uint_to_char(seed).b)[0];
	fs.write(data, 4);

	data = &(uint_to_char(bufferData.size()).b)[0];
	fs.write(data, 4);

	for (const GeomData& d : bufferData) {
		data = &(GeomData_to_char(d).b)[0];
		fs.write(data, sizeof(GeomData));
	}
	fs.close();
}

void ChunkColumn::load(const glm::vec2& chunkPos)
{
	position = chunkPos;
	std::string name = std::format("Chunks/c.{}.{}.chunk", (int)position.x, (int)position.y);
	std::ifstream fs(name, std::ios::in | std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(fs), {});
	unsigned int index = 0;
	seed = *(reinterpret_cast<unsigned int*>(&buffer[index]));
	index += 4;
	unsigned int bufferDataSize = *(reinterpret_cast<unsigned int*>(&buffer[index]));
	index += 4;

	bufferData.reserve(bufferDataSize);

	for (unsigned int i = index; i < buffer.size(); i += sizeof(GeomData)) {
		GeomData* d = reinterpret_cast<GeomData*>(&buffer[i]);
		bufferData.push_back(*d);
	}

	this->buffer.setUp(bufferData.data(), bufferData.size());
}

const Block ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const
{
	if (worldPos) {
		return blockStore.getBlock(getRelativePosition(pos));
	}
	return blockStore.getBlock(pos);
}

const Block ChunkColumn::getBlock(const glm::vec3& worldPos) {
	if (editedBlocks.size() > 0 && editedBlocks.find(worldPos) != editedBlocks.end()) {
		return editedBlocks.at(worldPos);
	}
	return Block::ERROR;
	const BlocksEncoded column = world_generation::getColumn({ worldPos.x, worldPos.z }, seed);
	return column[worldPos.y];
}

const Block ChunkColumn::getBlock(const glm::vec3& worldPos, const std::vector<ChunkColumn*>& neibours, const BlockStore& bs)
{
	if (outOfRange(worldPos)) {
		const glm::vec3 local = toLocal(worldPos);
		const glm::vec3 delta = sign(local - (CHUNK_SIZE_F - 1.f));
		const glm::vec2 newChunkPos = position + glm::vec2(delta.x, delta.z);
		for (ChunkColumn* chunk : neibours) {
			if (chunk->getPosition() == newChunkPos) {
				return chunk->getBlock(worldPos);
			}
		}
	}
	else {
		return getBlock(worldPos, true, bs);
	}
	return Block::AIR;
}

const glm::vec3 ChunkColumn::getRelativePosition(glm::vec3 worldPos)const 
{
	return worldPos - position * CHUNK_SIZE_F;
}

const glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos) const
{
	return relativePos + position * CHUNK_SIZE_F;
}

void ChunkColumn::addFace(const AddFaces& add) {
	for (GeomData& data : bufferData) {
		if (getWorldPosition(data.getPos()) == add.worldPos) {
			markSlot(data.cubeType_, add.face);
			buffer.realloc(bufferData.data(), bufferData.size());
			return;
		}
	}
	// will reach hear if the block doesnt exist in the mesh
	GeomData data;
	data.setPos(toLocal(add.worldPos));
	markSlot(data.cubeType_, add.face);;
	glm::vec3 pos = data.getPos();
	data.textureIndex_ = (unsigned char)getBlock(add.worldPos);
	if (data.textureIndex_ == (unsigned char) Block::AIR || data.textureIndex_ == (unsigned char)Block::ERROR) {
		return;
	}
	bufferData.push_back(data);
	buffer.realloc(bufferData.data(), bufferData.size());
}

bool ChunkColumn::outOfRange(const glm::vec3& localPos)
{
	return glm::any(glm::greaterThanEqual(localPos, glm::vec3(CHUNK_SIZE_F, WORLD_HEIGHT, CHUNK_SIZE_F)) || glm::lessThan(localPos, glm::vec3(0)));
}

glm::vec3 ChunkColumn::getWorldPos() const
{
	return glm::vec3(position.x, 0, position.y) * CHUNK_SIZE_F;
}

glm::vec3 ChunkColumn::toLocal(const glm::vec3& p) const
{
	return p - getWorldPos();
}

// getters
const Block ChunkColumn::getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const {
	glm::vec3 relativePostion = worldPos ? getRelativePosition(pos) : pos;

	const BlockStore& blockStore = worldMap[position];

	if (glm::all(glm::greaterThanEqual(relativePostion, { 0, 0, 0 }))) {
		if (glm::all(glm::lessThan(relativePostion, { CHUNK_SIZE_F, WORLD_HEIGHT, CHUNK_SIZE_F }))) {
			return getBlock(pos, worldPos, blockStore);
		}
	}
	if (!safe) return Block::AIR;
	// safe

	glm::vec2 chunkPositionToLookAt = position;
	if (relativePostion.x < 0) {
		relativePostion.x += CHUNK_SIZE_F;
		chunkPositionToLookAt.x -= 1;
	}
	else if (relativePostion.x > CHUNK_SIZE_F - 1) {
		relativePostion.x -= CHUNK_SIZE_F;
		chunkPositionToLookAt.x += 1;
	}

	if (relativePostion.z < 0) {
		relativePostion.z += CHUNK_SIZE_F;
		chunkPositionToLookAt.y -= 1;
	}
	else if (relativePostion.z > CHUNK_SIZE_F - 1) {
		relativePostion.z -= CHUNK_SIZE_F;
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