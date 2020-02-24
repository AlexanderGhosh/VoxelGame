#include "World.h"
World::World() {
	chunks = std::vector<std::pair<chunk_column, GLboolean>>();
}
World::World(GLboolean gen, GLboolean flat, GLboolean isDynamic) {
	chunks = std::vector<std::pair<chunk_column, GLboolean>>();
	this->isDynamic = isDynamic;
	if (!gen) return;
	getNewChunkPositions(flat);
}
void World::getNewChunkPositions(GLboolean flat) {
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<glm::vec2> chunkPositions = getNewChunkPositions(glm::vec2(0));
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration  = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "position duration: " << duration.count() << " microsecconds\n";
	if (flat) {
		generateFlatChunks(chunkPositions);
	}
	else {
		generateTerrain(chunkPositions);
	}
}
std::vector<glm::vec2> World::getNewChunkPositions(glm::vec2 origin, GLint renderDist) {
	std::vector<glm::vec2> chunkPositions;
	for (GLint x = -renderDist / 2; x < renderDist / 2; x++) {
		for (GLint z = -renderDist / 2; z < renderDist / 2; z++) {
			glm::vec2 pos(x, z);
			pos *= CHUNK_SIZE;
			pos += origin;
			chunkPositions.push_back(pos);
		}
	}
	return chunkPositions;
}
void World::generateFlatChunks(std::vector<glm::vec2> chunkPositions) {
	std::cout << "Started\n";
	for (auto& pos : chunkPositions) {
		chunk_column chunk(pos, 1);

		chunks.push_back({ chunk, GL_TRUE });
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto& pair : chunks) {
		auto& chunk = pair.first;
		chunk.createMesh(getChunks());
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << "): " << duration.count() << " microseconds" << std::endl;
}
HeightMapWorld createHeightMap(std::vector<glm::vec3> chunkPositions) {
	HeightMapWorld res;
	GLushort i = 0;
	for (GLubyte j = 0; j < res.size(); j++) {
		auto& chunk = res[j];
		glm::vec3 p = chunkPositions[i++];
		if (p.y != -CHUNK_SIZE) {
			j--;
			continue;
		}
		p += glm::vec3((CHUNK_SIZE * INITALL_VIEW) / 2, 0, (CHUNK_SIZE * INITALL_VIEW) / 2);
		for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
			for (GLubyte z = 0; z < CHUNK_SIZE; z++) {
				GLfloat sum = 0;
				glm::vec2 n((GLfloat)(p.x + x) / (GLfloat)CHUNK_SIZE - 0.5, (GLfloat)(p.z + z) / (GLfloat)CHUNK_SIZE - 0.5);
				sum = glm::perlin(n) + 0.5 * glm::perlin(0.5f * n) + 0.25 * glm::perlin(0.5f * n);
				sum *= 10;
				sum = std::abs(sum);
				if (sum > CHUNK_SIZE) sum = CHUNK_SIZE - 1;
				sum = std::lround(sum);
				chunk.first[x][z] = sum;
			}
		}
		chunk.second = i - 1;
	}
	return res;
}
void World::generateTerrain(std::vector<glm::vec2> chunkPositions) {
	std::cout << "Started\n";
	GLubyte i = 0;
	world_generation wg(CHUNK_SIZE, 3, 1, { {1.5f, 1.5f}, {0.75f, 0.75f}, {0.325f, 0.325f} });
	for (GLuint i = 0; i < chunkPositions.size(); i++) {
		auto& pos = chunkPositions[i];
		chunk_column chunk(pos);
		chunks.push_back({ chunk, GL_TRUE });
	}

	for (auto& pair : chunks) {
		chunk_column& chunk = pair.first;
		chunk.createMesh(getChunks());
	}
	genWorldMesh();
	drawable.setUp(worldMesh);

	auto start = std::chrono::high_resolution_clock::now();
	int i1 = std::pow(2, 1600 * 6400);
	auto stop = std::chrono::high_resolution_clock::now();
	i1 = 0;
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << " size: " << CHUNK_SIZE * std::pow(chunks.size(), 1.0f / 3.0f) << "): " << duration.count() << " microseconds!" << std::to_string(i1) << std::endl;
}

void World::render(Camera& c, glm::mat4 projection) {
	if (isDynamic) {
		renderChunksDynamic(c, projection);
	}
	else {
		renderChunksStatic(c, projection);
	}
}
void World::renderChunksStatic(Camera& c, glm::mat4 projection) {
	drawable.render(c, projection);
}
void World::renderChunksDynamic(Camera& c, glm::mat4 projection) {
	if (reDraw) {
		glm::vec3 p = c.GetPosition();
		p += c.GetFront() * (GLfloat)CHUNK_SIZE;
		p = reduceToMultiple(p, CHUNK_SIZE);
		
		auto positions = getNewChunkPositions(p, RENDER_DISTANCE);
		for (auto& pos : positions) {
			pos.y += CHUNK_SIZE;
		}
		std::vector<Chunk*> chunks;
		for (auto& chunk : this->chunks) {
			/*if (std::find(positions.begin(), positions.end(), chunk.first.position) != positions.end()){
				chunks.push_back(&chunk.first);
			}*/
		}
		auto t = genWorldMesh(chunks);
		drawable.setUp(t, "");
	}
	drawable.render(c, projection);
	reDraw = 0;
}
std::vector<chunk_column*> World::getChunks() {
	std::vector<chunk_column*> res;
	res.reserve(chunks.size());
	for (auto& chunk : chunks) {
		res.push_back(&chunk.first);
	}
	return res;
}
void World::genWorldMesh() {
	std::map<Buffer*, std::vector<Face*>> seperated;
	std::vector<Face*> sorted;
	for (auto& chunk : chunks) {
		if (!chunk.second) continue;
		for (auto& mesh : chunk.first.getMesh()) {
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
}
std::vector<Face*> World::genWorldMesh(std::vector<Chunk*> chunks) {
	std::map<Buffer*, std::vector<Face*>> seperated;
	std::vector<Face*> sorted;
	for (auto& chunk : chunks) {
		for (auto& mesh : chunk->getMeshes()) {
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
	return sorted;
}
std::vector<Face*>& World::getWorldMesh() {
	return worldMesh;
}
void World::updatePlayerPos(glm::vec3* pos) {
	playerPos = pos;
	reDraw = 1;
}

void World::createChunk(ChunkPosition position) {
	glm::vec3 pos(position.x, -CHUNK_SIZE, position.y);
	ChunkHeightMap hm = world_generation::genHeightMap(pos);
	chunk_column chunk(position, hm);
	chunk.createMesh(getChunks());
	chunks.push_back({ chunk, 1 });
	genWorldMesh();
	drawable.setUp(worldMesh);
}
void World::breakBlock(glm::vec3 pos, glm::vec3 front) {
	auto endPoint = glm::round(pos);
	GLboolean changed = 0;
	chunk_column* chunk = nullptr;
	for (GLubyte i = 0; i < PLAYER_REACH; i++) {
		auto toLookAt = endPoint + front * (GLfloat)(i + 1);
		auto blockPos = glm::round(toLookAt); // absolute position in world space
		auto& block = getBlock(blockPos, chunk);
		if (block != 0) {
			block = 0;
			changed = 1;
			break;
		}
	}
	if (changed) {
		chunk->createMesh(getChunks());
		worldMesh.clear();
		genWorldMesh();
		drawable.setUp(worldMesh);
	}
}
GLubyte& World::getBlock(glm::ivec3 blockPos, chunk_column*& chunk_) {
	glm::vec3 subChunkPos = reduceToMultiple(blockPos, CHUNK_SIZE);
	subChunkPos.y = 0;
	glm::vec2 chunkPos(subChunkPos.x, subChunkPos.z);
	GLubyte b = 0;
	for (auto& chunk : chunks) {
		if (chunk.first.getPosition() == chunkPos) {
			chunk_ = &chunk.first;
			auto& block = chunk.first.getBlock(blockPos - (glm::ivec3)subChunkPos);
			if (block < 0) return b;
			return block;
		}
	}

}