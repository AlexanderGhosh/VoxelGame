#include "World.h"
World::World() {
	playerPosition = { 0, 0, 0 };
	chunks = std::vector<std::pair<Chunk, GLboolean>>();
	// getNewChunkPositions();
}
World::World(GLboolean gen) {
	playerPosition = { 0, 0, 0 };
	chunks = std::vector<std::pair<Chunk, GLboolean>>();
	getNewChunkPositions();
}
World::World(Player& player) {
	playerPosition = player.getPosition();
	chunks = std::vector<std::pair<Chunk, GLboolean>>();
}
void World::getNewChunkPositions() {
	std::vector<glm::vec3> chunkPositions;
	for (GLint x = -RENDER_DISTANCE / 2; x < RENDER_DISTANCE/2; x++) {
		for (GLint z = -RENDER_DISTANCE / 2; z < RENDER_DISTANCE/2; z++) {
			chunkPositions.push_back({ x * CHUNK_SIZE , -CHUNK_SIZE, z * CHUNK_SIZE });
		}
	}
	
	generateFlatChunks(chunkPositions);
	
	/*std::thread obj(&World::generateFlatChunks, this, chunkPositions);
	obj.detach();*/
}
void World::generateFlatChunks(std::vector<glm::vec3> chunkPositions) {
	std::cout << "Started\n";
	for (auto& pos : chunkPositions) {
		Chunk chunk(pos, false);
		chunk.createBlocks();

		chunks.push_back({ chunk, GL_TRUE });
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto& pair : chunks) {
		Chunk& chunk = pair.first;
		chunk.createMesh(getChunks());
		// std::cout << "Chunk created" << std::endl;
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << " size: " << CHUNK_SIZE*std::pow(chunks.size(), 0.5f) << "): " << duration.count() << " microseconds" << std::endl;
}
void World::renderChunksStatic(Camera c, glm::mat4 projection) {
	/*for (auto& chunk : chunks) {
		render.loadMeshes(&chunk.first.getMeshes());
		render.render(c, projection);
	}*/

	/*render.loadMeshes(&worldMesh);
	render.render(c, projection);*/

	drawable.render(c, projection);
}
void World::cleanUp() {
	for (auto& chunk : chunks) {
		chunk.first.cleanUp();
	}
	render.cleanUp();
}
std::vector<Chunk*> World::getChunks() {
	std::vector<Chunk*> res;
	for (auto& chunk : chunks) {
		res.push_back(&chunk.first);
	}
	return res;
}
void World::genWorldMesh() {
	std::map<Buffer*, std::vector<Face*>> seperated;
	std::vector<Face*> sorted;
	for (auto& chunk : chunks) {
		for (auto& mesh : chunk.first.getMeshes()) {
			try {
				seperated[std::get<0>(*mesh)].push_back(mesh);
			}
			catch (std::exception e) {
				seperated.insert({ std::get<0>(*mesh), { mesh } });
			}
		}
	}
	for (auto& pair : seperated) {
		sorted.insert(sorted.end(), pair.second.begin(), pair.second.end());
	}
	worldMesh = sorted;
	/*std::map<int, std::vector<Mesh::FaceMesh>> seperated;
	std::vector<Mesh::FaceMesh> sorted;
	int location = 0;
	for (auto& chunk : chunks) {
		for (auto& mesh : chunk.first.getMeshes()) {
			for (auto& buffer : mesh.comboOf) {
				location += (int)buffer;
			}
			try {
				seperated[location].push_back(mesh);
			}
			catch (std::exception e) {
				seperated.insert({ location, { mesh } });
			}
			location = 0;
		}
	}
<<<<<<< HEAD
}
Chunk& World::getOccupiedChunk(glm::ivec3 occPosition) {
	reduceToMultiple(occPosition, CHUNK_SIZE);
	int index = getChunkIndex(occPosition, false);
	if (index < 0) {
		Chunk chunk = Chunk();
		return chunk;
=======
	for (auto& pair : seperated) {
		sorted.insert(sorted.end(), pair.second.begin(), pair.second.end());
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
	}
	worldMesh = sorted;*/
}