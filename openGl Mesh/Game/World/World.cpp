#include "World.h"
World::World() {
	playerPosition = { 0, 0, 0 };
	chunks = std::vector<std::tuple<Chunk, GLboolean>>();
	// getNewChunkPositions();
}
World::World(GLboolean gen) {
	playerPosition = { 0, 0, 0 };
	chunks = std::vector<std::tuple<Chunk, GLboolean>>();
	getNewChunkPositions();
}
World::World(Player& player) {
	playerPosition = player.getPosition();
	chunks = std::vector<std::tuple<Chunk, GLboolean>>();
}
void World::getNewChunkPositions() {
	std::vector<glm::vec3> chunkPositions;
	for (GLint x = -RENDER_DISTANCE / 2; x < RENDER_DISTANCE/2; x++) {
		for (GLint z = -RENDER_DISTANCE / 2; z < RENDER_DISTANCE/2; z++) {
			chunkPositions.push_back({ x * CHUNK_SIZE , -CHUNK_SIZE, z * CHUNK_SIZE });
		}
	}
	
	generateFlatChunks(chunkPositions);
	/*
	std::thread obj(&World::generateFlatChunks, this, chunkPositions);
	obj.detach();*/
}
void World::generateFlatChunks(std::vector<glm::vec3> chunkPositions) {
	for (auto& pos : chunkPositions) {
		Chunk chunk(pos, false);
		chunk.createBlocks();

		chunks.push_back({ chunk, GL_TRUE });
	}

	for (auto& tup : chunks) {
		Chunk& chunk = std::get<0>(tup);
		chunk.createMesh(getChunks());
		Render::ChunkMeshRender cmr(SHADERS[BLOCK2]);
		cmr.loadMeshes(chunk.getMeshes());
		rs.push_back(cmr);

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
void World::cleanUp() {
	for (auto& chunk : chunks) {
		std::get<0>(chunk).cleanUp();
	}
	for (auto& renderer : rs) {
		renderer.cleanUp();
	}
	render.cleanUp();
}
std::vector<Chunk> World::getChunks() {
	std::vector<Chunk> res;
	for (auto& chunk : chunks) {
		res.push_back(std::get<0>(chunk));
	}
	return res;
}
/*Chunk* World::getChunkOccupied(glm::vec3 position) {
	/*for (auto& chunkP : chunks) {
		Chunk chunk  = std::get<0>(chunkP);
		if (position.x < chunk.position.x + CHUNK_SIZE && position.x > chunk.position.x) {
			if (position.z < chunk.position.z + CHUNK_SIZE && position.z > chunk.position.z) {
				return &chunk;
			}
		}
	}
	return nullptr;
}*/