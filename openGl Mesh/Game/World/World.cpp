#include "World.h"
World::World() {
	playerPosition = { 0, 0, 0 };
	getNewChunkPositions();
}
World::World(Player& player) {
	playerPosition = player.getPosition();
}
void World::getNewChunkPositions() {
	std::vector<glm::vec3> chunkPositions;
	for (GLint x = -RENDER_DISTANCE/2; x < RENDER_DISTANCE/2; x++) {
		for (GLint z = -RENDER_DISTANCE / 2; z < RENDER_DISTANCE/2; z++) {
			chunkPositions.push_back({ x * CHUNK_SIZE , -CHUNK_SIZE, z * CHUNK_SIZE });
		}
	}
	
	std::thread obj(&World::generateFlatChunks, this, chunkPositions);
	obj.detach();
}
void World::generateFlatChunks(std::vector<glm::vec3> chunkPositions) {
	for (auto& pos : chunkPositions) {
		Chunk chunk(pos);
		Render::ChunkMeshRender cmr("block2");
		chunk.create();
		cmr.loadMeshes(chunk.getMeshes());
		rs.push_back(cmr);

		chunks.push_back({ chunk, GL_TRUE });

		std::cout << "Chunk created" << std::endl;
	}
}
void World::renderChunksStatic(Camera c, glm::mat4 projection) {
	for (GLuint i = 0; i < chunks.size(); i++) {
		if (std::get<1>(chunks[i])) {
			rs[i].render(c, projection);
		}
	}
}

Chunk* World::getChunkOccupied(glm::vec3 position) {
	for (auto& chunkP : chunks) {
		Chunk& chunk = std::get<0>(chunkP);
		if (position.x < chunk.position.x + CHUNK_SIZE && position.x > chunk.position.x) {
			if (position.z < chunk.position.z + CHUNK_SIZE && position.z > chunk.position.z) {
				return &chunk;
			}
		}
	}
	return nullptr;
}