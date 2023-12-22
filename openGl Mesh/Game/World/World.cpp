#include "World.h"
#include <iostream>
#include <algorithm>
#include <gtx/string_cast.hpp>
#include <gtc/random.hpp>
#include <glad/glad.h>
#include "../../Block.h"
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Timers/Timer.h"
#include "../../Helpers/Functions.h"
#include "../../Shaders/Shader.h"

World::World() : chunks(), geomDrawable(), seed(), positionsBeingGenerated(), pool(), generated(false)
{
}

World::World(const glm::vec3 worldOrigin, unsigned int seed) : World() {
	this->seed = seed;
}

void World::getNewChunkPositions(const glm::vec3 worldOrigin) {
	generateTerrain();
}

void World::generateTerrain() {
	generated = true;
	std::cout << "Started\n";

	Timer timer("Inital Chunk Loading");

	std::unordered_set<glm::vec2> chunkPositions = centeredPositions({ 0, 0 }, RENDER_DISTANCE);
	chunks.reserve(chunkPositions.size());

	std::unordered_map<glm::vec2, BlockStore> blockData;
	for (const glm::vec2& pos : chunkPositions) {
		ChunkColumn chunk(pos, seed);
#ifdef ALWAYS_USE_SLOW_MESH
		auto neighbours = getNeibours(pos, true);
		chunk.generateChunkData(pos, seed, neighbours);
		chunk.setUpBuffer();
#elif defined(ALWAYS_USE_NOISE_MESH)
		chunk.generateNoiseBuffer();
#elif defined(ALWAYS_USE_GREEDY_MESH)
		auto neighbours = getNeibours(pos, true);

		chunk.generateBlockStore(blockData[pos]);

		for (ChunkColumn* chunk : neighbours) {
			if (!blockData.contains(chunk->getPosition2D())) {
				chunk->generateBlockStore(blockData[chunk->getPosition2D()]);
			}
		}
		chunk.createMesh(blockData, blockData[pos]);
		chunk.setUpGreedyBuffer();
#endif // ALWAYS_USE_SLOW_MESH

		chunks.emplace(pos, std::move(chunk));
	}

	timer.showDetails(chunks.size());
}

void World::tryStartGenerateChunks(const glm::vec2& center, const glm::vec3& frustrumCenter, const float frustrumRadius)
{
	// distance calculations performed in scaled world space
	if (generated && !GENERATE_NEW_CHUNKS) {
		return;
	}
	generated = true;
	// local unscaled
	auto toGenerate = centeredPositions(center, RENDER_DISTANCE);

	float maxDist = sqrtf(2.f * CHUNK_AREA * VOXEL_SIZE) + frustrumRadius;
	for (auto itt = chunks.cbegin(); itt != chunks.cend();)
	{
		// pos unscaled local chunk pos
		auto& [pos, _] = *itt;
		float dist = glm::distance((pos + .5f) * CHUNK_SIZE_SCALED, { frustrumCenter.x, frustrumCenter.z });
		if ((dist > maxDist || !toGenerate.contains(pos)) && !positionsBeingGenerated.contains(pos))
		{
			geomDrawable.remove(pos);
			itt = chunks.erase(itt);
		}
		else
		{
			++itt;
		}
	}

	for (auto itt = toGenerate.begin(); itt != toGenerate.end();) {
		// local unscaled
		const glm::vec2& pos = *itt;
		float dist = glm::distance((pos + .5f) * CHUNK_SIZE_SCALED, { frustrumCenter.x, frustrumCenter.z });
		if (dist > maxDist || positionsBeingGenerated.contains(pos) || chunks.contains(pos)) {
			itt = toGenerate.erase(itt);
		}
		else {
			itt++;
		}
	}

	positionsBeingGenerated.insert(toGenerate.begin(), toGenerate.end());

	// compute set difference
	launchAsyncs(toGenerate, 1);
}

void World::tryFinishGenerateChunk()
{
	Timer timer("Finish Async");
	auto allDone = pool.getAllDone(true);
	for (auto& generated : allDone) {
		for (auto itt = generated.begin(); itt != generated.end(); itt++) {
			ChunkColumn* chunk = getValue(chunks, *itt);
			if (!chunk) continue;
			
			positionsBeingGenerated.erase(*itt);
			// ChunkColumn& chunk = chunks.at(chunkPos);
#ifndef ALWAYS_USE_GREEDY_MESH
			chunk->setUpBuffer();
			geomDrawable.add(chunk);
#else
			chunk->setUpGreedyBuffer();
			greedyDrawable.add(*chunk);
#endif
		}
	}
	// if (allDone.size() == 0) return;
	// timer.showDetails(allDone.front().size());
}

std::unordered_set<glm::vec2> World::generateNewChunks(const std::unordered_set<glm::vec2>& positions)
{
	Timer t("Chunk Generate");
	// can throw error if the chunk is removed from chunks while its being generated
	for (const glm::vec2& chunkPos : positions) {
		chunks[chunkPos] = ChunkColumn(chunkPos, seed);
		ChunkColumn& chunk = chunks.at(chunkPos);
#ifdef ALWAYS_USE_NOISE_MESH
		chunk.generateNoiseBuffer();
#elif  defined(ALWAYS_USE_SLOW_MESH)
		auto neighbours = getNeibours(chunkPos, true);
		chunk.generateChunkData(chunkPos, seed, neighbours);
#elif defined(ALWAYS_USE_GREEDY_MESH)
		auto neighbours = getNeibours(chunkPos, true);

		std::unordered_map<glm::vec2, BlockStore> blockData;
		chunk.generateBlockStore(blockData[chunkPos]);

		for (ChunkColumn* chunk : neighbours) {
			if (!blockData.contains(chunk->getPosition2D())) {
				chunk->generateBlockStore(blockData[chunk->getPosition2D()]);
			}
		}

		chunk.createMesh(blockData, blockData[chunkPos]);
		greedyDrawable.add(chunk);
#endif
	}
	// t.showDetails(positions.size());
	return positions;
}

const ChunkColumn& World::getChunk(const glm::vec2& chunkPos, bool& success) const
{
	if (chunks.contains(chunkPos) && !positionsBeingGenerated.contains(chunkPos)) {
		success = true;
		return chunks.at(chunkPos);
	}
	success = false;
	return {};
}

ChunkColumn* World::getChunk(const glm::vec2& chunkPos, bool& success)
{
	if (chunks.contains(chunkPos) && !positionsBeingGenerated.contains(chunkPos)) {
		success = true;
		return &chunks.at(chunkPos);
	}
	success = false;
	return nullptr;
}

void World::update()
{
#ifndef CELLULAR_AUTOMOTA
	return;
#endif
	// performed in local unscaled space
	/*auto index = [](int x, int y, int z) -> int {
		return x + y * CHUNK_SIZE + z * CHUNK_SIZE * WORLD_HEIGHT;
	};
	struct MoveRequest {
		glm::ivec3 currentPos;
		glm::ivec3 nextPos;
		Block currentBlock;
		Block nextBlock;
	};

	bool found = false;
	ChunkColumn& chunk = *getChunk({ 0, 0 }, found);
	if (!found) {
		return;
	}
	const auto data = chunk.getBlocksGrid();
	std::list<MoveRequest> movementRequests;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = WORLD_HEIGHT - 1; y > 0; y--) {
				int idx = index(x, y, z);
				const BlockDetails& block = data[idx];
				if (block.isDynamic) {
					idx = index(x, y - 1, z);
					const BlockDetails& under = data[idx];
					if (under.type == B_AIR) {
						MoveRequest request{};
						request.currentBlock = block.type;
						request.nextBlock = block.type;
						request.currentPos = { x, y, z };
						request.nextPos = { x, y - 1, z };
						movementRequests.push_back(request);
					}
				}
			}
		}
	}
	for (auto& req : movementRequests) {
		chunk.removeBlock(req.currentPos, this);
		chunk.addBlock(req.nextPos, req.nextBlock);
	}*/
}

void World::explode(const glm::vec3& worldPos, float radius)
{
	std::unordered_set<glm::vec3> positoins;
	for (int i = 0; i < radius * 2; i++) {
		for (int j = 0; j < radius * 2; j++) {
			for (int k = 0; k < radius * 2; k++) {
				glm::vec3 canidate(i, j, k);
				canidate -= radius;
				float dist = glm::length(canidate);
				if (dist < radius) {
					positoins.insert(canidate + worldPos);
				}
			}
		}
	}

	for (const glm::vec3& pos : positoins) {
		glm::vec2 chunkPos(pos.x, pos.z);
		chunkPos *= VOXEL_SIZE_INV * CHUNK_SIZE_INV;
		chunkPos = floor(chunkPos);

		ChunkColumn& chunk = chunks.at(chunkPos);
		if (chunk.getBlock(pos) == B_AIR) {
			continue;
		}
		chunk.removeBlock(pos, this);
	}
}

glm::vec3 World::lookingAt(const glm::vec3& ro, const glm::vec3& rd)
{
	glm::vec2 chunkPos(ro.x, ro.z);
	chunkPos *= VOXEL_SIZE_INV * CHUNK_SIZE_INV;
	chunkPos = floor(chunkPos);

	if (!chunks.contains(chunkPos)) {
		return glm::vec3(-FLT_MAX);
	}
	glm::vec3 hitPos(0);
	float minD = FLT_MAX;
	auto chunk_s = getNeibours(chunkPos, true);
	for (auto chunk : chunk_s) {
		for (const GeomData& data : chunk->getMeshData()) {
			glm::vec3 worldBlockPos = data.getPos();

			worldBlockPos.x += chunk->getWorldPosition2D().x;
			worldBlockPos.z += chunk->getWorldPosition2D().y;
			if (rayCubeIntersection(ro, rd, worldBlockPos - HALF_VOXEL_SIZE, worldBlockPos + HALF_VOXEL_SIZE)) {
				const float d = glm::distance(ro, worldBlockPos);
				if (d < minD) {
					minD = d;
					hitPos = worldBlockPos;
				}
			}
		}
	}
	return hitPos;
}

void World::launchAsyncs(const std::unordered_set<glm::vec2>& allChunkPoss, const unsigned int n)
{
	unsigned int size = allChunkPoss.size() / n;
	std::unordered_set<glm::vec2> accumulator; // accumulates the positions to run async is then cleared
	unsigned int i = 0;
	for (auto itt = allChunkPoss.begin(); itt != allChunkPoss.end(); itt++) {
		accumulator.insert(*itt);
		if (i == size) {
			pool.add() = std::async(&World::generateNewChunks, this, accumulator);
			i = 0;
			accumulator.clear();
			continue;
		}
		i++;
	}
	if (accumulator.size() > 0) {
		// runs the rest (happends if allChunkPoss.size() % n != 0)
		pool.add() = std::async(&World::generateNewChunks, this, accumulator);
	}
}


const std::list<ChunkColumn*> World::getNeibours(const glm::vec2& chunkPos, bool includeSelf)
{
	std::list<glm::vec2> offsets(OFFSETS_2D.begin(), OFFSETS_2D.end());
	if (includeSelf) {
		offsets.push_front(glm::vec2(0));
	}
	std::list<ChunkColumn*> res;

	for (auto& off : offsets) {
		ChunkColumn* chunk = getValue(chunks, chunkPos + off);
		if (chunk) {
			res.push_back(chunk);
		}
	}

	/*for (auto& [pos, chunk] : chunks) {
		for (auto itt = offsets.begin(); itt != offsets.end();) {
			const glm::vec2& delta = *itt;
			const glm::vec2 cp = chunkPos + delta;
			if (cp == pos) {
				itt = offsets.erase(itt);
				res.push_back(&chunk);
				break;
			}
			else {
				itt++;
			}
		}
	}*/

	return res;
}

void World::save() const
{
	for (const auto& [pos, c] : chunks) {
		c.save();
	}
}

void World::render(Shader* shader) {
	tryFinishGenerateChunk();
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyDrawable.render(shader);
#endif // ALWAYS_USE_GREEDY_MESH
	geomDrawable.render(shader);
}

void World::setUpDrawable()
{
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyDrawable.setUp(chunks);
#else
	geomDrawable.setUp(chunks);
#endif
}

void World::placeBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunkPos)
{
	Timer t("Place Block");
	if (!chunks.contains(occupiedChunkPos)) {
		return;
	}
	glm::vec3 placePos = lookingAt(ro, rd);
	placePos = round(placePos - rd * VOXEL_SIZE);
	glm::vec3 chunkPos3 = reduceToMultiple(placePos);
	glm::vec2 chunkPos(chunkPos3.x, chunkPos3.z);
	chunkPos *= CHUNK_SIZE_INV;

	if (chunks.find(chunkPos) != chunks.end()) {
		ChunkColumn& chunk = chunks.at(chunkPos);
		chunk.addBlock(placePos, B_GRAVEL);
	}
	t.showDetails(1);
}

void World::breakBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunkPos)
{
	if (!chunks.contains(occupiedChunkPos)) {
		return;
	}
	glm::vec3 breakPos = lookingAt(ro, rd);

	glm::vec3 chunkPos3 = reduceToMultiple(breakPos);
	glm::vec2 chunkPos(chunkPos3.x, chunkPos3.z);
	chunkPos *= CHUNK_SIZE_INV;

	if (chunks.find(chunkPos) != chunks.end()) {
		ChunkColumn& chunk = chunks.at(chunkPos);
		chunk.removeBlock(breakPos, this);
	}
}

const std::unordered_set<glm::vec2> World::centeredPositions(const glm::vec2& origin, int renderDist) const {

	std::unordered_set<glm::vec2> res;
	for (int x = -renderDist; x < renderDist + 1; x++) {
		int Y = pow(renderDist * renderDist - x * x, 0.5); // bound for y given x
		for (int y = -Y; y < Y + 1; y++) {
			glm::vec2 pos(x, y);
			pos += origin;
			res.insert(pos);
		}
	}
	return res;
}
