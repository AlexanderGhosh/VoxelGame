#include "World.h"
#include <iostream>
#include <algorithm>
#include <gtx/string_cast.hpp>
#include <gtc/random.hpp>
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Timers/Timer.h"
#include "../../Helpers/Functions.h"
#include "../../Shaders/Shader.h"

World::World() : chunks(), geomDrawable(), seed(), positionsBeingGenerated(), pool() {
}
World::World(const glm::vec3 worldOrigin, unsigned int seed) : World() {
	this->seed = seed;
	//getNewChunkPositions(worldOrigin);
}

void World::getNewChunkPositions(const glm::vec3 worldOrigin) {
	std::unordered_set<glm::vec2> chunkPositions = centeredPositions({ worldOrigin.x, worldOrigin .z }, RENDER_DISTANCE);

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
	if (!GENERATE_NEW_CHUNKS) {
		return;
	}
	auto toGenerate = centeredPositions(center, RENDER_DISTANCE);

	float maxDist = sqrtf(2.f * CHUNK_AREA * VOXEL_SZIE) + frustrumRadius;
	for (auto itt = chunks.cbegin(); itt != chunks.cend();)
	{
		auto& [pos, _] = *itt;
		float dist = glm::distance((pos + .5f) * CHUNK_SIZE_F * VOXEL_SZIE, { frustrumCenter.x, frustrumCenter.z });
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
		const glm::vec2& pos = *itt;
		float dist = glm::distance((pos + .5f) * CHUNK_SIZE_F * VOXEL_SZIE, { frustrumCenter.x, frustrumCenter.z });
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
	if (!GENERATE_NEW_CHUNKS) return;

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
	if (chunks.contains(chunkPos)) {
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
	for (const glm::vec2& chunkPos : positions) {
		chunks[chunkPos] = ChunkColumn();
		const std::list<ChunkColumn*>& neighbours = getNeibours(chunkPos);
		chunks[chunkPos].generateChunkData(chunkPos, seed, neighbours);
	}
	return positions;
}

const std::list<ChunkColumn*> World::getNeibours(const glm::vec2& chunkPos, bool includeSelf)
{
	std::list<glm::vec2> offsets = {
		glm::vec2(0, 1),
		glm::vec2(-1, 0),
		glm::vec2(1, 0),
		glm::vec2(0, -1)
	};
	if (includeSelf) {
		offsets.push_back(glm::vec2(0));
	}
	std::list<ChunkColumn*> res;
	for (auto& [pos, chunk] : chunks) {
		for (auto itt = offsets.begin(); itt != offsets.end();) {
			const glm::vec2& delta = *itt;
			const glm::vec2 cp = chunkPos + delta;
			if (cp == pos) {
				itt = offsets.erase(itt);
				res.push_back(&chunk);
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


const std::list<glm::vec3> offsets = {
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, -1),

	glm::vec3(-1, 0, 0),
	glm::vec3(1, 0, 0),

	glm::vec3(0, 1, 0),
	glm::vec3(0, -1, 0)
};

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

			worldBlockPos.x += chunk->getWorldPosition().x;
			worldBlockPos.z += chunk->getWorldPosition().y;
			if (rayCubeIntersection(ro, rd, worldBlockPos - HALF_VOXEL_SZIE, worldBlockPos + HALF_VOXEL_SZIE)) {
				const float d = glm::distance(ro, worldBlockPos);
				if (d < minD) {
					minD = d;
					placePos = worldBlockPos - rd * VOXEL_SZIE;
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

void World::breakBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front)
{
	glm::vec4 screenPos(0, 0, zCoord, 1); // the center of screen screen coords
	screenPos = invPV * screenPos;
	glm::vec3 hitPos = glm::vec3(screenPos) / screenPos.w;

	hitPos = floor((hitPos * 2.f) + 0.5f) * .5f;

	glm::vec3 breakPos(0);

	glm::vec3 norm(0);
	float dist = 0;
	for (const glm::vec3& normal : offsets) {
		float d = glm::dot(normal, front);
		if (d < dist) {
			dist = d;
			norm = normal;
		}
	}
	breakPos = floor(hitPos - norm * .5f);

	std::cout << glm::to_string(breakPos) << std::endl;

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
