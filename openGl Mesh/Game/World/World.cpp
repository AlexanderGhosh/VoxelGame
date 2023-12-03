#include "World.h"
#include <iostream>
#include <algorithm>
#include <gtx/string_cast.hpp>
#include <gtc/random.hpp>
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Timers/Timer.h"
#include "../../Helpers/Functions.h"
#include "../../Shaders/Shader.h"

World::World() : chunks(), geomDrawable(), seed(), positionsBeingGenerated(), pool(), generated(false) {
}

World::World(const glm::vec3 worldOrigin, unsigned int seed) : World() {
	this->seed = seed;
}

void World::getNewChunkPositions(const glm::vec3 worldOrigin) {
	std::unordered_set<glm::vec2> chunkPositions = centeredPositions({ worldOrigin.x, worldOrigin.z }, RENDER_DISTANCE);

	generateTerrain(chunkPositions);
}

void World::generateTerrain(const std::unordered_set<glm::vec2>& chunkPositions) {
	std::cout << "Started\n";

	WorldMap worldMap;
	worldMap.reserve(chunkPositions.size());
	chunks.reserve(chunkPositions.size());

	Timer timer("Inital Chunk Loading");

	for (const glm::vec2& pos : chunkPositions) {
		ChunkColumn chunk(pos, seed, worldMap);
		chunks.emplace(pos, chunk);
	}
	timer.mark("Constructors");
	for (auto& chunk : chunks) {
		chunk.second.populateBuffer(worldMap);
	}
	timer.mark("Buffers");

	timer.showDetails(chunks.size());

	worldMap.clear();
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
		if ((!toGenerate.contains(pos) || dist > maxDist) && !positionsBeingGenerated.contains(pos))
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
		if (chunks.contains(pos) || positionsBeingGenerated.contains(pos) || dist > maxDist) {
			itt = toGenerate.erase(itt);
		}
		else {
			itt++;
		}
	}

	positionsBeingGenerated.insert(toGenerate.begin(), toGenerate.end());

	// compute set difference
	launchAsyncs(toGenerate, 4);
}

void World::tryFinishGenerateChunk()
{
	auto allDone = pool.getAllDone(true);
	for (auto& generated : allDone) {
		for (auto itt = generated.begin(); itt != generated.end(); itt++) {
			ChunkColumn* chunk = getValue(chunks, *itt);
			if (!chunk) continue;
			
			positionsBeingGenerated.erase(*itt);
			// ChunkColumn& chunk = chunks.at(chunkPos);
			chunk->setUpBuffer();
			geomDrawable.add(*chunk);

			const std::list<ChunkColumn*>& neighbours = getNeibours(*itt);
			for (ChunkColumn* chunk : neighbours) {
				chunk->reallocBuffer();
			}
		}
	}
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



std::unordered_set<glm::vec2> World::generateNewChunks(const std::unordered_set<glm::vec2>& positions)
{
	// can throw error if the chunk is removed from chunks while its being generated
	Timer t("Chunk Mesh");
	for (const glm::vec2& chunkPos : positions) {
		chunks[chunkPos] = ChunkColumn();
		const std::list<ChunkColumn*>& neighbours = getNeibours(chunkPos);
		chunks[chunkPos].generateChunkData(chunkPos, seed, neighbours);
	}
	t.showDetails(positions.size());
	return positions;
}

const std::list<ChunkColumn*> World::getNeibours(const glm::vec2& chunkPos, bool includeSelf)
{
	std::list<glm::vec2> offsets(OFFSETS_2D.begin(), OFFSETS_2D.end());
	if (includeSelf) {
		offsets.push_front(glm::vec2(0));
	}
	std::list<ChunkColumn*> res;
	for (auto& [pos, chunk] : chunks) {
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
	}

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
	geomDrawable.render(shader);
}

void World::setUpDrawable()
{
	geomDrawable.setUp(chunks);
}



void World::placeBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunkPos)
{
	if (!chunks.contains(occupiedChunkPos)) {
		return;
	}
	glm::vec3 placePos(0);
	float minD = FLT_MAX;
	auto chunk_s = getNeibours(occupiedChunkPos, true);
	for (auto chunk : chunk_s) {
		for (const GeomData& data : chunk->getMeshData()) {
			glm::vec3 worldBlockPos = data.getPos();

			worldBlockPos.x += chunk->getWorldPosition2D().x;
			worldBlockPos.z += chunk->getWorldPosition2D().y;
			if (rayCubeIntersection(ro, rd, worldBlockPos - HALF_VOXEL_SIZE, worldBlockPos + HALF_VOXEL_SIZE)) {
				const float d = glm::distance(ro, worldBlockPos);
				if (d < minD) {
					minD = d;
					placePos = worldBlockPos - rd * VOXEL_SIZE;
				}
			}
		}
	}
	placePos = round(placePos);
	glm::vec3 chunkPos3 = reduceToMultiple(placePos);
	glm::vec2 chunkPos(chunkPos3.x, chunkPos3.z);
	chunkPos *= CHUNK_SIZE_INV;

	if (chunks.find(chunkPos) != chunks.end()) {
		ChunkColumn& chunk = chunks.at(chunkPos);
		chunk.addBlock(placePos, Block::SAND);
	}
}

void World::breakBlock(const glm::vec3& ro, const glm::vec3& rd, const glm::vec2& occupiedChunkPos)
{
	if (!chunks.contains(occupiedChunkPos)) {
		return;
	}
	glm::vec3 breakPos(0);
	float minD = FLT_MAX;
	auto chunk_s = getNeibours(occupiedChunkPos, true);
	for (auto chunk : chunk_s) {
		for (const GeomData& data : chunk->getMeshData()) {
			glm::vec3 worldBlockPos = data.getPos();

			worldBlockPos.x += chunk->getWorldPosition2D().x;
			worldBlockPos.z += chunk->getWorldPosition2D().y;
			if (rayCubeIntersection(ro, rd, worldBlockPos - HALF_VOXEL_SIZE, worldBlockPos + HALF_VOXEL_SIZE)) {
				const float d = glm::distance(ro, worldBlockPos);
				if (d < minD) {
					minD = d;
					breakPos = worldBlockPos;
				}
			}
		}
	}
	breakPos = round(breakPos);
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
