#include "World.h"
#include <iostream>
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Timer.h"
#include <gtx/string_cast.hpp>
#include "../../Helpers/Functions.h"

World::World() : chunks(), geomDrawable(), seed(), blockStoreCreated(), chunkCreationInprogress(false) {
}
World::World(bool gen, bool terrain, unsigned int seed) : World() {
	this->seed = seed;
	if (!gen) return;
	getNewChunkPositions(!terrain);
}


void World::getNewChunkPositions(bool flat) {
	std::vector<glm::vec2> chunkPositions = centeredPositions(glm::vec2(0), RENDER_DISTANCE);

	generateTerrain(chunkPositions);
}

void World::generateTerrain(const std::vector<glm::vec2>& chunkPositions) {
	std::cout << "Started\n";

	WorldMap worldMap;
	worldMap.reserve(chunkPositions.size());
	chunks.reserve(chunkPositions.size());

	// for (const glm::vec2& pos : chunkPositions) {
	// 	chunks.emplace(pos, ChunkColumn());
	// 	chunks.at(pos).load(pos);
	// }
	// worldMap.clear();
	// return;


	for (const glm::vec2& pos : chunkPositions) {
		ChunkColumn chunk(pos, seed, worldMap);
		chunks.emplace(pos, chunk);
	}

	for (auto& chunk : chunks) {
		chunk.second.populateBuffer(worldMap);
	}

	worldMap.clear();
}

void World::startGenerateChunk(const glm::vec2& chunkPos)
{
	chunks[chunkPos] = ChunkColumn();
	chunkCreationInprogress = true;
	blockStoreCreated = std::async(&ChunkColumn::buildBlockStore, &chunks[chunkPos], chunkPos, seed);
}

void World::tryFinishGenerateChunk(const glm::vec2& chunkPos)
{
	if (chunkCreationInprogress && blockStoreCreated.valid()) {
		chunkCreationInprogress = false;
		BlockStore bs = blockStoreCreated.get();
		const auto neibours = getNeibours(chunkPos);
		std::list<ChunkColumn*> n(neibours.begin(), neibours.end());
		chunks.at(chunkPos).populateBuffer(n, bs);
		geomDrawable.add(chunks.at(chunkPos));
	}
}

const std::vector<ChunkColumn*> World::getNeibours(const glm::vec2& chunkPos)
{
	std::vector<glm::vec2> offsets = {
		glm::vec2(0, 1),
		glm::vec2(-1, 0),
		glm::vec2(1, 0),
		glm::vec2(0, -1)
	};
	std::vector<ChunkColumn*> res;
	res.reserve(4);
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
	if (chunkCreationInprogress) {
		tryFinishGenerateChunk(glm::vec2(4, 0));
	}
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
	glm::vec3(0, -1, 0),

};

void World::placeBlock(const float zCoord, const glm::mat4& invPV, const glm::vec3& front)
{
	glm::vec4 screenPos(0, 0, zCoord, 1); // the center of screen screen coords
	screenPos = invPV * screenPos;
	glm::vec3 hitPos = glm::vec3(screenPos) / screenPos.w;
	
	hitPos = floor((hitPos * 2.f) + 0.5f) * .5f;

	glm::vec3 placePos(0);

	glm::vec3 norm(0);
	float dist = 0;
	for (const glm::vec3& normal : offsets) {
		float d = glm::dot(normal, front);
		if (d < dist) {
			dist = d;
			norm = normal;
		}
	}
	placePos = floor(hitPos + norm * .5f);

	// std::cout << glm::to_string(placePos) << std::endl;

	glm::vec3 chunkPos3 = reduceToMultiple(placePos);
	glm::vec2 chunkPos(chunkPos3.x, chunkPos3.z);
	chunkPos *= CHUNK_SIZE_INV;

	std::cout << glm::to_string(chunkPos) << std::endl;
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

const std::vector<glm::vec2> World::centeredPositions(const glm::vec2& origin, int renderDist) const {

	std::vector<glm::vec2> res;
	for (int x = -renderDist; x < renderDist + 1; x++) {
		int Y = pow(renderDist * renderDist - x * x, 0.5); // bound for y given x
		for (int y = -Y; y < Y + 1; y++) {
			glm::vec2 pos(x, y);
			pos += origin;
			res.push_back(pos);
		}
	}
	return res;
}
