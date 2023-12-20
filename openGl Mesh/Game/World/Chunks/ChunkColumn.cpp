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
#include "../../../GreedyRendering/GreedyData.h"
#include "../../../Helpers/Timers/Timer.h"


ChunkColumn::ChunkColumn() : position(0), buffer_(), seed(), bufferData(), editedBlocks()
{
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed) : ChunkColumn()
{
	this->seed = seed;
	position = pos;
}

ChunkColumn::ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map) : ChunkColumn(pos, seed)
{
	map[pos] = BlockStore(pos * (float) CHUNK_SIZE, seed);
}

ChunkColumn::ChunkColumn(const ChunkColumn& other)
{
	position = other.position;
	seed = other.seed;
	bufferData = other.bufferData;
	editedBlocks = other.editedBlocks;
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyBufferData = other.greedyBufferData;
#endif
}

ChunkColumn ChunkColumn::operator=(const ChunkColumn& other)
{
	position = other.position;
	seed = other.seed;
	bufferData = other.bufferData;
	editedBlocks = other.editedBlocks;
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyBufferData = other.greedyBufferData;
#endif
	return *this;
}

ChunkColumn::ChunkColumn(ChunkColumn&& other) noexcept
{
	position = other.position;
	buffer_ = std::move(other.buffer_);
	seed = other.seed;
	bufferData = std::move(other.bufferData);
	editedBlocks = std::move(other.editedBlocks);
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyBufferData = std::move(other.greedyBufferData);
	greedyBuffer = std::move(other.greedyBuffer);
#endif
}

#ifdef ALWAYS_USE_SLOW_MESH
void ChunkColumn::generateChunkData(glm::vec2 pos, unsigned int seed, const std::list<ChunkColumn*>& neibours)
{
	this->seed = seed;
	position = pos;
	BlockStore bs(pos * CHUNK_SIZE_F, seed);

#ifdef ALWAYS_USE_SLOW_MESH
	populateBufferFromNeibours(neibours, bs);
#elif defined(ALWAYS_USE_GREEDY_MESH)
	greedyMesh(neibours, bs);
#endif
}
#endif

#ifdef ALWAYS_USE_SLOW_MESH
void ChunkColumn::populateBufferFromNeibours(const std::list<ChunkColumn*>& neibours, const BlockStore& blockStore) {
	std::unordered_map<glm::vec3, Block> exploredBlocksCache; // map of all the blocs which have already being looked up

	bufferData.reserve(CHUNK_AREA); // the chunk will have at least this many top faces
	GeomData data{};
	const glm::vec3 chunkWorldPos_U = getWorldPosition3D();

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
				data.textureIndex_ = (unsigned char)b1;
				bool added = false;
				for (unsigned int i = 0; i < count1; i++) {
					glm::vec3 currentLocalPos = glm::vec3(x, height - i, z);
					data.setPos(currentLocalPos);
					unsigned int j = 0;
					added = false;
					for (const glm::vec3& off : OFFSETS_3D) {
						glm::vec3 newLocalPosition = currentLocalPos + off;
						const glm::vec3 newWorldPosition_U = newLocalPosition + chunkWorldPos_U;
						Block b2 = Block::ERROR;
						Block* b2Ptr = getValue(exploredBlocksCache, newWorldPosition_U);
						if (b2Ptr) {
							b2 = *b2Ptr;
						}
						else {
							b2 = blockStore.getBlock(newLocalPosition);
							if (b2 == Block::ERROR) { //  means that it is outside this chunk
								// when this chunk has a face visble from an other chunk eg it is ataller
								glm::vec2 newChunkPos = position + glm::vec2(off.x, off.z);
								for (ChunkColumn* chunk : neibours) {
									if (chunk->getPosition2D() == newChunkPos) {
										b2 = chunk->getBlock(newWorldPosition_U);
										break;
									}
								}
							}
							exploredBlocksCache.emplace(newWorldPosition_U, b2);
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
	// fix seems of neibours
	// looks along the edge of the nebour chunk that borders the generated chunk and aims to add faces to fix the seems
	for (unsigned int i = 0; i < 4; i++) {
		glm::vec3 off = OFFSETS_3D[i];
		const glm::vec2 neighbourChunkPos = position + glm::vec2(off.x, off.z);

		glm::vec3 mask = -off; // is +-1 along the axis which is the direction of the neibour
		// start point in unscaled local coords
		const glm::vec3 startPoint = glm::max(glm::vec3(CHUNK_SIZE_F - 1) * mask, glm::vec3(0)); // is the coord of the starting point on the neibours chunk (local coords to the neibour)
		mask = 1.f - glm::abs(mask); // (1 - is because need the perpendicular line in order to advance along)
		mask.y = 0;
		
		for (ChunkColumn* chunk : neibours) {
			if (chunk->getPosition2D() == neighbourChunkPos) {

				for (unsigned int j = 0; j < CHUNK_SIZE; j++) {
					glm::vec3 delta(j);
					delta *= mask; // the axis with a value is == to the axis that borders the chunk (this is what is incremented in the loop
					glm::vec3 neighbourLocalPos = startPoint + delta;

					glm::vec3 neighbourWorldPos_U = neighbourLocalPos + glm::vec3(neighbourChunkPos.x * CHUNK_SIZE_F, 0, neighbourChunkPos.y * CHUNK_SIZE_F);
					neighbourLocalPos.y = world_generation::heightOfColumn({ neighbourWorldPos_U.x, neighbourWorldPos_U.z }, seed);
					neighbourWorldPos_U.y = neighbourLocalPos.y;

					// check if the face needs to be added at all
					glm::vec3 localPos = (neighbourLocalPos - off); // the local position of the block that is next to the nebours block
					localPos.x = (int)localPos.x % CHUNK_SIZE;
					localPos.z = (int)localPos.z % CHUNK_SIZE;

					for (unsigned int k = localPos.y; k > 0; k--) {
						localPos.y = k;

						Block b = blockStore.getBlock(localPos, false); // the block that is next to the neibours block in the current chunk
						BlockDetails blockDets = getDetails(b);
						if (blockDets.isTransparant) {
							AddFaces toAdd{};
							toAdd.worldPos = neighbourWorldPos_U; // the scaled world pos to the block to add
							toAdd.worldPos.y = k;
							toAdd.face = i; // turns it to the oposite face
							if (toAdd.face % 2 == 0) {
								toAdd.face++;
							}
							else {
								toAdd.face--;
							}
							chunk->addFace(toAdd, false);
						}
						else {
							break;
						}
					}
				}
			}
		}
	}
}
#endif

#ifndef ALWAYS_USE_GREEDY_MESH
void ChunkColumn::setUpBuffer()
{
	buffer_.setUp(bufferData.data(), bufferData.size());
}
#endif

#ifdef ALWAYS_USE_GREEDY_MESH
void ChunkColumn::greedyMesh(const std::unordered_map<glm::vec2, BlockStore>& neibours, const BlockStore& blockStore)
{
#ifdef GENERATE_INDEX_DATA_GREEDY
	auto lessThanVec3 = [](glm::vec3 a, glm::vec3 b) { return a.x < b.x && a.y < b.y && a.z < b.z; };
	// maps vertex to index
	unsigned int index = 0;
	std::unordered_map<glm::vec3, unsigned int> vertices;
	std::vector<unsigned int> indices;
#endif // GENERATE_INDEX_DATA_GREEDY

	const BlockStore* pz = nullptr;
	const BlockStore* nz = nullptr;
	const BlockStore* px = nullptr;
	const BlockStore* nx = nullptr;

	for (auto& [p, chunk] : neibours) {
		glm::vec2 delta = p - position;
		if (delta.x == 1 && delta.y == 0) {
			px = &chunk;
		}
		else if (delta.x == -1 && delta.y == 0) {
			nx = &chunk;
		}
		else if (delta.x == 0 && delta.y == 1) {
			pz = &chunk;
		}
		else if (delta.x == 0 && delta.y == -1) {
			nz = &chunk;
		}

	}

	auto show = [&](Block a, Block b) {
		auto& d = getDetails(b);
		return d.isTransparant && a != b;
	};

	// unscaled local space
	auto isVisiblePY = [&](Block a, int x, int y, int z) -> bool {
		Block b = blockStore.getBlock({ x, y + 1, z }, false);
		return show(a, b);
	};

	auto isVisiblePZ = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (z + 1 == CHUNK_SIZE) {
			if(pz)
				b = pz->getBlock({ x, y, 0 }, false);
			else
				return false;
		}
		else {
			b = blockStore.getBlock({ x, y, z + 1 }, false);
		}
		return show(a, b);
	};
	auto isVisibleNZ = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (z == 0) {
			if(nz)
				b = nz->getBlock({ x, y, CHUNK_SIZE - 1 }, false);
			else
				return false;
		}
		else {
			b = blockStore.getBlock({ x, y, z - 1 }, false);
		}
		return show(a, b);
	};

	auto isVisiblePX = [&](Block a, int x, int y, int z) -> bool {
		Block b; 
		if (x == CHUNK_SIZE - 1) {
			if (px)
				b = px->getBlock({ 0, y, z }, false);
			else
				return false;
		}
		else {
			b = blockStore.getBlock({ x + 1, y, z }, false);
		}
		return show(a, b);
	};
	auto isVisibleNX = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (x == 0) {
			if (nx)
				b = nx->getBlock({ CHUNK_SIZE - 1, y, z }, false);
			else
				return false;
		}
		else {
			b = blockStore.getBlock({ x - 1, y, z }, false);
		}
		return show(a, b);
	};

#ifdef GENERATE_INDEX_DATA_GREEDY
	auto addVert = [&](glm::vec3 vert) {
		auto [itt, success] = vertices.emplace(vert, index);
		if (success) {
			indices.push_back(index);
			index++;
		}
		else {
			indices.push_back((*itt).second);
		}
		};
#endif // GENERATE_INDEX_DATA_GREEDY


	auto addPY = [&](Block b, int mkPoint, int x, int y, int z) {
		if (b != Block::AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y, z);
			glm::vec3 faceMax(x, y, z + 1);

			GreedyData data;
			data._normal = glm::vec3(0, 1, 0);
			data._materialIdx = (unsigned int) b;
			data._corner0 = faceMin;
			data._corner1 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner2 = { faceMin.x, faceMin.y, faceMax.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);

#ifdef GENERATE_INDEX_DATA_GREEDY
			addVert(data._corner0);
			addVert(data._corner1);
			addVert(data._corner2);
			addVert(data._corner3);
#endif // GENERATE_INDEX_DATA_GREEDY

		}
	};
	auto addPZ = [&](Block b, int mkPoint, int x, int y, int z) {
		// add face
		if (b != Block::AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y - 1, z);
			glm::vec3 faceMax(x, y, z);
			faceMin.z += 1.f;
			faceMax.z += 1.f;

			GreedyData data;
			data._normal = glm::vec3(0, 0, 1);
			data._materialIdx = (unsigned int)b;
			data._corner0 = faceMin;
			data._corner1 = { faceMin.x, faceMax.y, faceMax.z };
			data._corner2 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner3 = faceMax;
			greedyBufferData.push_back(data);


#ifdef GENERATE_INDEX_DATA_GREEDY
			addVert(data._corner0);
			addVert(data._corner1);
			addVert(data._corner2);
			addVert(data._corner3);
#endif // GENERATE_INDEX_DATA_GREEDY
		}
	};
	auto addNZ = [&](Block b, int mkPoint, int x, int y, int z) {
		if (b != Block::AIR && mkPoint != x) {
			// add face
			glm::vec3 faceMin(mkPoint, y - 1, z);
			glm::vec3 faceMax(x, y, z);
			// faceMin.z += 1.f;
			// faceMax.z += 1.f;

			GreedyData data;
			data._normal = glm::vec3(0, 0, -1);
			data._materialIdx = (unsigned int)b;
			data._corner0 = faceMin;
			data._corner1 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner2 = { faceMin.x, faceMax.y, faceMax.z };
			data._corner3 = faceMax;
			greedyBufferData.push_back(data);


#ifdef GENERATE_INDEX_DATA_GREEDY
			addVert(data._corner0);
			addVert(data._corner1);
			addVert(data._corner2);
			addVert(data._corner3);
#endif // GENERATE_INDEX_DATA_GREEDY
		}
	};
	
	//////////////////////////////////////////

	for (int y = WORLD_HEIGHT - 1; y >= 0; y--) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				Block prevBlock = blockStore.getBlock({ x, y, z }, false);
				if (prevBlock == Block::AIR) continue;
				int mkPointPY = x; // the start point of a run
				int mkPointPZ = x;
				int mkPointNZ = x;
				// visited_px[idx] = true;
				for (; x < CHUNK_SIZE; x++) {
					// visited_px[idx] = true;
					const Block currentBlock = blockStore.getBlock({ x, y, z }, false);

					// if next is visible and same block
					//	increment without changing block
					// if not the same block but is visible
					//	add move mark 1
					//	then incremnet
					// if the same block but no visible
					//	add move mark 2
					//	then increment
					// if not visible and not the same
					//	no add increment 2

#ifdef MINIMAL_GREEDY_MESH
					bool pyVisible = isVisiblePY(currentBlock, x, y, z);
					bool pzVisible = isVisiblePZ(currentBlock, x, y, z);
					bool nzVisible = isVisibleNZ(currentBlock, x, y, z);
#else
					bool pyVisible = currentBlock != Block::AIR;
					bool pzVisible = currentBlock != Block::AIR;
					bool nzVisible = currentBlock != Block::AIR;
#endif // MINIMAL_GREEDY_MESH
					if (pyVisible && pzVisible && nzVisible) break;

					if (prevBlock == currentBlock) {
						// PY
						if (pyVisible) {
							// doesnt change mark spot
						}
						else {
							addPY(currentBlock, mkPointPY, x, y, z);
							mkPointPY = x + 1;
						}

						// PZ
						if (pzVisible) {
							// doesnt change mark spot
						}
						else {
							addPZ(currentBlock, mkPointPZ, x, y, z);
							mkPointPZ = x + 1;
						}

						// NZ
						if (nzVisible) {
							// doesnt change mark spot
						}
						else {
							addNZ(currentBlock, mkPointNZ, x, y, z);
							mkPointNZ = x + 1;
						}
					}
					if (prevBlock != currentBlock) {
						// PY
						if (pyVisible) {
							addPY(prevBlock, mkPointPY, x, y, z);
							mkPointPY = x;
						}
						else {
							addPY(prevBlock, mkPointPY, x, y, z);
							mkPointPY = x+1;
						}

						// PZ
						if (pzVisible) {
							addPZ(prevBlock, mkPointPZ, x, y, z);
							mkPointPZ = x;
						}
						else {
							addPZ(prevBlock, mkPointPZ, x, y, z);
							mkPointPZ = x + 1;
						}

						// NZ
						if (nzVisible) {
							addNZ(prevBlock, mkPointNZ, x, y, z);
							mkPointNZ = x;
						}
						else {
							addNZ(prevBlock, mkPointNZ, x, y, z);
							mkPointNZ = x + 1;
						}
					}
					prevBlock = currentBlock;
				}

				// add traing faces
				addPY(prevBlock, mkPointPY, x, y, z);
				addPZ(prevBlock, mkPointPZ, x, y, z);
				addNZ(prevBlock, mkPointNZ, x, y, z);
			}

			// X faces
			for (int x = 0; x < CHUNK_SIZE; x++) {
				Block currentBlock = blockStore.getBlock({ x, y, z }, false);
				if (isVisiblePX(currentBlock, x, y, z)) {

					glm::vec3 faceMin(x, y - 1, z);
					glm::vec3 faceMax(x, y, z + 1);
					faceMin.x += 1.f;
					faceMax.x += 1.f;

					GreedyData data;
					data._normal = glm::vec3(1, 0, 0);
					data._materialIdx = (unsigned int)currentBlock;
					data._corner0 = faceMin;
					data._corner1 = { faceMin.x, faceMin.y, faceMax.z };
					data._corner2 = { faceMin.x, faceMax.y, faceMin.z };
					data._corner3 = faceMax;
					greedyBufferData.push_back(data);
				}
				
				if (isVisibleNX(currentBlock, x, y, z)) {
					glm::vec3 faceMin(x, y - 1, z);
					glm::vec3 faceMax(x, y, z + 1);

					GreedyData data;
					data._normal = glm::vec3(-1, 0, 0);
					data._materialIdx = (unsigned int)currentBlock;
					data._corner0 = faceMin;
					data._corner1 = { faceMin.x, faceMax.y, faceMin.z };
					data._corner2 = { faceMin.x, faceMin.y, faceMax.z };
					data._corner3 = faceMax;
					greedyBufferData.push_back(data);
				}
			}
		}
	}
}

void ChunkColumn::setUpGreedyBuffer()
{
	greedyBuffer.setUp(greedyBufferData.data(), greedyBufferData.size());
}

void ChunkColumn::generateBlockStore(BlockStore& bs)
{
	bs = BlockStore(getWorldPosition2D(), seed);
}

void ChunkColumn::createMesh(const std::unordered_map<glm::vec2, BlockStore>& neighbours, const BlockStore& blockStore)
{
	greedyMesh(neighbours, blockStore);
}
#endif // ALWAYS_USE_GREEDY_MESH

void ChunkColumn::reallocBuffer()
{
	buffer_.realloc(bufferData.data(), bufferData.size());
}

#ifdef ALWAYS_USE_NOISE_MESH
void ChunkColumn::generateNoiseBuffer()
{
	Timer timer("Generate from noise");
	timer.start();
	std::vector<float> heightsPadded = std::move(world_generation::getRawHeightsPadded(getWorldPosition2D(), seed));

	auto index = [](unsigned int x, unsigned int z) { return x + z * CHUNK_SIZE; };
	auto indexPadded = [](unsigned int x, unsigned int z) { return x + z * CHUNK_SIZE_PADDED; };
	auto getHeight = [&heightsPadded, &indexPadded](unsigned int x, unsigned int z) -> unsigned char { return heightsPadded[indexPadded(x, z)]; };

	timer.mark("Heights Generated");
		
	// contqains the list of differances in height along axis (should equate to how many faces are visible
	std::vector<unsigned char> pxDelta(CHUNK_AREA);
	std::vector<unsigned char> nxDelta(CHUNK_AREA);
	std::vector<unsigned char> pzDelta(CHUNK_AREA);
	std::vector<unsigned char> nzDelta(CHUNK_AREA);

	bufferData.reserve(CHUNK_AREA);
	for (unsigned int i = 1; i < CHUNK_SIZE_PADDED - 1; i++) {
		for (unsigned int j = 1; j < CHUNK_SIZE_PADDED - 1; j++) {
			unsigned int x = i - 1;
			unsigned int z = j - 1;
			unsigned int currentIndex = index(x, z);
			unsigned char height = getHeight(i, j);

			// deltas
			unsigned char px = fmaxf(0, height - getHeight(i + 1, j));
			unsigned char nx = fmaxf(0, height - getHeight(i - 1, j));

			unsigned char pz = fmaxf(0, height - getHeight(i, j + 1));
			unsigned char nz = fmaxf(0, height - getHeight(i, j - 1));

			// mesh
			const BlocksEncoded currentColumn = world_generation::createColumn(height);
			unsigned char maxDepth = fmaxf(fmaxf(px, pz), fmaxf(nx, nz));
			for (unsigned int i = 0; i < maxDepth; i++) {
				Block block = currentColumn[height - i];
				GeomData data;

				// addes the water level
				if (height < WATER_LEVEL) {
					if (i == 0) markSlot(data.cubeType_, 4); // top face
					data.textureIndex_ = (unsigned char)Block::WATER;
					data.setPos({ x, WATER_LEVEL, z });
					bufferData.push_back(data);
					data.cubeType_ = 0;
				}

				data.textureIndex_ = (unsigned char)block;
				data.setPos({ x, height - i, z });
				if (i == 0) markSlot(data.cubeType_, 4); // top face
				if (i < px) markSlot(data.cubeType_, 3);
				if (i < nx) markSlot(data.cubeType_, 2);
				if (i < pz) markSlot(data.cubeType_, 0);
				if (i < nz) markSlot(data.cubeType_, 1);

				bufferData.push_back(data);
			}
			if (maxDepth == 0) {
				GeomData data;
				if (height < WATER_LEVEL) {
					markSlot(data.cubeType_, 4); // top face
					data.textureIndex_ = (unsigned char)Block::WATER;
					data.setPos({ x, WATER_LEVEL, z });
					bufferData.push_back(data);
				}
				Block block = currentColumn[height];
				data.setPos({ x, height, z });
				data.textureIndex_ = (unsigned char)block;
				markSlot(data.cubeType_, 4); // top face
				bufferData.push_back(data);
			}
		}
	}

	timer.mark("Mesh generated");

#if !defined(GENERATE_CHUNKS_ASYNC) && !GENERATE_NEW_CHUNKS
	setUpBuffer();
	timer.mark("OpenGl");
#endif // !GENERATE_CHUNKS_ASYNC

	//timer.showDetails(1);
}
#endif

const BufferGeom& ChunkColumn::getBuffer() const
{
	return buffer_;
}

BufferGeom* ChunkColumn::getBufferPtr()
{
	return &buffer_;
}

void ChunkColumn::addBlock(const glm::vec3& worldPos, const Block block)
{
	Timer timer("Add Block");
	struct RemoveFace {
		glm::vec3 localPos;
		unsigned char face;
	};
	std::list<RemoveFace> toRemove;
	const glm::vec3 localPos = toLocal(worldPos);

	GeomData toAdd;
	toAdd.setPos(localPos);
	toAdd.cubeType_ = 63; // all faces
	toAdd.textureIndex_ = (unsigned char) block;


	for (unsigned int i = 0; i < OFFSETS_3D.size(); i++) {
		// needs to check the world map
		RemoveFace data{};
		data.localPos = localPos + OFFSETS_3D[i];
		data.face = i++;

		toRemove.push_back(data);
	}
	timer.mark("Create to Remove");


	for (auto itt1 = bufferData.begin(); itt1 != bufferData.end();) {
		GeomData& data = *itt1;
		const glm::vec3 dataLocalPos = data.getPos();
		for (auto itt2 = toRemove.begin(); itt2 != toRemove.end();) {
			const RemoveFace& remove = *itt2;
			if (dataLocalPos == remove.localPos) {
				unsigned int mask = 1u << remove.face;
				toAdd.cubeType_ ^= mask; // sets the face to add

				if (remove.face & 1) {
					mask = 1 << remove.face - 1;
				}
				else {
					mask = 1 << remove.face + 1;
				}

				mask = ~mask;
				data.cubeType_ &= mask; // will set the slot of face to 0
				itt2 = toRemove.erase(itt2);
				break;
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
	bufferData.push_back(toAdd);
	editedBlocks[worldPos] = block;
	timer.mark("Edit mesh");

	buffer_.realloc(bufferData.data(), bufferData.size());
	timer.mark("OpenGL");
	timer.showDetails(1);
}

void ChunkColumn::removeBlock(const glm::vec3& worldPos, World* world)
{
	const glm::vec3 localPos = toLocal(worldPos);
	std::vector<AddFaces> toAdd;

	for (unsigned int i = 0; i > OFFSETS_3D.size(); i++) {
		// needs to check the world map
		AddFaces data{};
		data.offset = OFFSETS_3D[i];
		data.worldPos = worldPos + data.offset;
		data.face = i++; // swaps face
		if (data.face & 1) {
			data.face--;
		}
		else {
			data.face++;
		}
		//face is the index of the face to add relative to the block to add it too

		toAdd.push_back(data);
	}

	bool removed = false;
	for (auto itt1 = bufferData.begin(); itt1 != bufferData.end();) {
		GeomData& data = *itt1;
		glm::vec3 local = data.getPos();
		const glm::vec3 dataWorldPos = toWorld(local);
		if (dataWorldPos == worldPos) {
			// the block its self
			itt1 = bufferData.erase(itt1);
			removed = true;
			continue;
		}

		for (auto itt2 = toAdd.begin(); itt2 != toAdd.end();) {
			const AddFaces& add = *itt2;
			if (dataWorldPos == add.worldPos) {
				markSlot(data.cubeType_, add.face);
				itt2 = toAdd.erase(itt2);
				break;
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
	for (const AddFaces& add : toAdd) {
		if (outOfRange(toLocal(add.worldPos))) {
			if (!neibours.size()) {
				neibours = world->getNeibours(position);
			}
			glm::vec2 newChunkPos = position;
			newChunkPos.x += add.offset.x;
			newChunkPos.y += add.offset.z;
			for (ChunkColumn* chunk : neibours) {
				if (chunk->getPosition2D() == newChunkPos) {
					chunk->addFace(add, true);
					break;
				}
			}
		}
		else {
			// the reson why more faces are added then needed is beacuse it doesnt check to see if that blokcs had being broken before DONT THINK THIS IS THE CASE ANYMORE
			const Block b = getBlock(add.worldPos);
			if (b == Block::AIR) {
				continue;
			}
			GeomData data;
			data.setPos(toLocal(add.worldPos));
			data.textureIndex_ = (unsigned char) b;
			markSlot(data.cubeType_, add.face);
			bufferData.push_back(data);
		}
	}

	if (editedBlocks.contains(worldPos)) {
		editedBlocks.erase(worldPos);
	}
	else {
		editedBlocks[worldPos] = Block::AIR;
	}

	buffer_.realloc(bufferData.data(), bufferData.size());
}

const glm::vec2& ChunkColumn::getPosition2D() const
{
	return position;
}

const glm::vec3 ChunkColumn::getPosition3D() const
{
	return glm::vec3(position.x, 0, position.y);
}

const glm::vec2 ChunkColumn::getWorldPosition2D() const
{
	return position * CHUNK_SIZE_F;
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
	std::vector<unsigned char> buffer_(std::istreambuf_iterator<char>(fs), {});
	unsigned int index = 0;
	seed = *(reinterpret_cast<unsigned int*>(&buffer_[index]));
	index += 4;
	unsigned int bufferDataSize = *(reinterpret_cast<unsigned int*>(&buffer_[index]));
	index += 4;

	bufferData.reserve(bufferDataSize);

	for (unsigned int i = index; i < buffer_.size(); i += sizeof(GeomData)) {
		GeomData* d = reinterpret_cast<GeomData*>(&buffer_[i]);
		bufferData.push_back(*d);
	}

	this->buffer_.setUp(bufferData.data(), bufferData.size());
}

unsigned int ChunkColumn::getHeight(const float x, const float z) const
{
	glm::vec2 worldPos(x, z);
	worldPos += getWorldPosition2D();
	return world_generation::heightOfColumn(worldPos, seed);
}

const std::vector<GeomData>& ChunkColumn::getMeshData() const
{
	return bufferData;
}

std::array<BlockDetails, CHUNK_AREA * WORLD_HEIGHT> ChunkColumn::getBlocksGrid()
{
	auto index = [](int x, int y, int z) -> int {
		return x + y * CHUNK_SIZE + z * CHUNK_SIZE * WORLD_HEIGHT;
	};

	std::array<BlockDetails, CHUNK_AREA * WORLD_HEIGHT> res;
	res.fill(getDetails(Block::AIR));

	for (const GeomData& data : bufferData) {
		const glm::vec3 p = data.getPos();
		int idx = index(p.x, p.y, p.z);
		if ((Block)data.textureIndex_ == Block::WATER) {
			int _ = 0;
		}
		res[idx] = getDetails((Block) data.textureIndex_);
	}
	for (auto& [p, block] : editedBlocks) {
		int idx = index(p.x, p.y, p.z);
		res[idx] = getDetails(block);
	}

	return res;
}

const Block ChunkColumn::getBlock_BlockStore(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const
{
	if (worldPos) {
		return blockStore.getBlock(toLocal(pos));
	}
	return blockStore.getBlock(pos);
}

const Block ChunkColumn::getBlock(const glm::vec3& worldPos) {
	if (editedBlocks.size() > 0 && editedBlocks.find(worldPos) != editedBlocks.end()) {
		return editedBlocks.at(worldPos);
	}
	//return Block::ERROR;
	const BlocksEncoded column = world_generation::getColumn({ worldPos.x, worldPos.z }, seed);
	return column[worldPos.y];
}

DrawData ChunkColumn::getDrawData() const
{
	DrawData res;
	res.type_ = DrawData::CHUNK;
	res.buffer_ = const_cast<BufferGeom*>(&buffer_);
	res.drawOrigin_ = getWorldPosition3D();

	return res;
}

//const Block ChunkColumn::getBlock_BlockStore(const glm::vec3& worldPos, const std::vector<ChunkColumn*>& neibours, const BlockStore& bs)
//{
//	if (outOfRange(worldPos)) {
//		const glm::vec3 local = toLocal(worldPos);
//		const glm::vec3 delta = sign(local - (CHUNK_SIZE_F - 1.f));
//		const glm::vec2 newChunkPos = position + glm::vec2(delta.x, delta.z);
//		for (ChunkColumn* chunk : neibours) {
//			if (chunk->getPosition2D() == newChunkPos) {
//				return chunk->getBlock(worldPos);
//			}
//		}
//	}
//	else {
//		return getBlock_BlockStore(worldPos, true, bs);
//	}
//	return Block::AIR;
//}

void ChunkColumn::addFace(const AddFaces& add, bool realoc) {
	const glm::vec3 localPos = toLocal(add.worldPos);
	for (GeomData& data : bufferData) {
		if (data.getPos() == localPos) {
			markSlot(data.cubeType_, add.face);
			if(realoc)
				buffer_.realloc(bufferData.data(), bufferData.size());
			return;
		}
	}
	// will reach here if the block doesnt exist in the mesh
	GeomData data;
	data.setPos(localPos);
	markSlot(data.cubeType_, add.face);
	data.textureIndex_ = (unsigned char) getBlock(add.worldPos);
	if (data.textureIndex_ == (unsigned char) Block::AIR || data.textureIndex_ == (unsigned char) Block::ERROR) {
		return;
	}
	bufferData.push_back(data);
	if(realoc)
		buffer_.realloc(bufferData.data(), bufferData.size());
}

bool ChunkColumn::outOfRange(const glm::vec3& localPos)
{
	return glm::any(glm::greaterThanEqual(localPos, glm::vec3(CHUNK_SIZE_F, WORLD_HEIGHT, CHUNK_SIZE_F)) || glm::lessThan(localPos, glm::vec3(0)));
}

const glm::vec3 ChunkColumn::getWorldPosition3D() const
{
	return glm::vec3(position.x, 0, position.y) * CHUNK_SIZE_F;
}

glm::vec3 ChunkColumn::toLocal(const glm::vec3& p) const
{
	return p - getWorldPosition3D();
}

glm::vec3 ChunkColumn::toWorld(const glm::vec3& p) const
{
	return p + getWorldPosition3D();
}

// getters
const Block ChunkColumn::getBlock_WorldMap(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const {
	glm::vec3 relativePostion = worldPos ? toLocal(pos) : pos;

	const BlockStore& blockStore = worldMap[position];

	if (glm::all(glm::greaterThanEqual(relativePostion, { 0, 0, 0 }))) {
		if (glm::all(glm::lessThan(relativePostion, { CHUNK_SIZE_F, WORLD_HEIGHT, CHUNK_SIZE_F }))) {
			return getBlock_BlockStore(pos, worldPos, blockStore);
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