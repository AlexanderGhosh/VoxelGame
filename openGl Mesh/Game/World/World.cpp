#include "World.h"
#include <iostream>
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Timer.h"

World::World() : chunks(), geomDrawable(), seed() {
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

	for (const glm::vec2& pos : chunkPositions) {
		chunks.emplace_back(pos, seed, worldMap);
	}

	for (auto& chunk : chunks) {
		chunk.populateBuffer(worldMap);
	}

	worldMap.clear();
}

void World::render(Shader* shader) const {
	geomDrawable.render(shader);
}

void World::setUpDrawable()
{
	geomDrawable.setUp(chunks);
}

void World::placeBlock()
{
	ChunkColumn& chunk = chunks[0];
	const glm::vec2& chunkPos = chunk.getPosition();
	glm::vec3 placePos(-36, 24, 2);
	chunk.addBlock(placePos, Block::SAND);
}

void World::breakBlock()
{
	ChunkColumn& chunk = chunks[0];
	const glm::vec2& chunkPos = chunk.getPosition();
	glm::vec3 breakPos(-36, 23, 2);
	chunk.removeBlock(breakPos);
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
