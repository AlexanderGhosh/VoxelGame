#include "World.h"
#include "Chunks/ChunkColumn.h"

World::World() : chunks(), geomDrawable(), seed(), worldMap() {
}
World::World(bool gen, bool terrain, unsigned int seed) : World() {
	this->seed = seed;
	if (!gen) return;
	worldMap.reserve(RENDER_DISTANCE + 2);
	getNewChunkPositions(!terrain);

}


void World::getNewChunkPositions(bool flat) {
	std::vector<glm::vec2> chunkPositions = centeredPositions(glm::vec2(0), {});

	generateTerrain(chunkPositions);
}

void World::generateTerrain(const std::vector<glm::vec2>& chunkPositions) {
	std::cout << "Started\n";
	// files and blocks
	for (const glm::vec2& pos : chunkPositions) {
		chunks.emplace_back(pos, seed);
		chunks.back().addTrees();
	}

	for (auto& chunk : chunks) {
		worldMap[chunk.getPosition()] = &chunk.getBlockStore();
	}

	for (auto& chunk : chunks) {
		chunk.createMeshNew(&worldMap);
	}
}

void World::render(Camera& c, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap) const {
	geomDrawable.render(c, projection, lightMatrix, depthMap);
}

void World::setUpDrawable()
{
	geomDrawable.setUp(chunks);
}

const std::vector<glm::vec2> World::centeredPositions(glm::vec2 origin, const std::vector<glm::vec2>& exclude, int renderDist) const {
	origin.x = reduceToMultiple(origin.x, CHUNK_SIZE);
	origin.y = reduceToMultiple(origin.y, CHUNK_SIZE);
	if (renderDist % 2 == 0)  renderDist++;
	GLubyte radius = (renderDist - 1) / 2;
	std::vector<glm::vec2> res;
	for (GLbyte x = -radius; x < radius + 1; x++) {
		for (GLbyte y = -radius; y < radius + 1; y++) {
			glm::vec2 pos(x, y);
			pos *= CHUNK_SIZE;
			pos += origin;
			if (exclude.size() > 0 && std::find(exclude.begin(), exclude.end(), pos) != exclude.end()) continue;
			res.push_back(pos);
		}
	}
	return res;
}
