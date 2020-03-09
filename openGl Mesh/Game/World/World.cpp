#include "World.h"
#include "gtx/string_cast.hpp"
World::World() {
	chunks = std::vector<std::pair<chunk_column, GLboolean>>();
}
World::World(GLboolean gen, GLboolean terrain, GLboolean isDynamic) {
	chunks = std::vector<std::pair<chunk_column, GLboolean>>();
	this->isDynamic = isDynamic;
	if (!gen) return;
	getNewChunkPositions(!terrain);
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

	for (auto& pair : chunks) {
		auto& chunk = pair.first;
		std::vector<std::pair<Face*, GLboolean>> changed;
		chunk.createMesh(getChunks(),0 , changed);
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto start = std::chrono::high_resolution_clock::now();

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
	world_generation wg(CHUNK_SIZE*2, 3, 1, { {1.5f, 1.5f}, {0.75f, 0.75f}, {0.325f, 0.325f} });
	for (GLuint i = 0; i < chunkPositions.size(); i++) {
		auto& pos = chunkPositions[i];
		chunk_column chunk(pos);
		chunks.push_back({ chunk, GL_TRUE });
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto& pair : chunks) {
		chunk_column& chunk = pair.first;
		std::vector<std::pair<Face*, GLboolean>> changed;
		chunk.createMesh(getChunks(),0 ,changed);
	}
	genWorldMesh();
	drawable.setUp(worldMesh);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << " size: " << CHUNK_SIZE * std::pow(chunks.size(), 1.0f / 3.0f) << "): " << duration.count() << " microseconds" << std::endl;
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
	/*std::map<Buffer*, std::vector<Face*>> seperated;
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
	worldMesh = sorted;*/
	worldMesh.clear();
	for(auto & chunk : chunks) {
		auto mesh = chunk.first.getMesh();
		worldMesh.insert(worldMesh.end(), mesh.begin(), mesh.end());
	}
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
	std::vector<std::pair<Face*, GLboolean>> changed;
	chunk.createMesh(getChunks(), 0, changed);
	chunks.push_back({ chunk, 1 });
	genWorldMesh();
	drawable.setUp(worldMesh);
}
void World::breakBlock(glm::vec3 pos, glm::vec3 front) {
	std::cout << "__________________________________________________________________" << std::endl;
	// pos.y += 0.5f;
	GLboolean changed = 0;
	chunk_column* chunk = nullptr;
	glm::vec3 lookPos = pos;
	for (GLubyte i = 0; i < PLAYER_REACH; i++) {
		lookPos += front;
		glm::ivec3 blockPos = glm::round(lookPos);
		std::cout << "looking at: " << glm::to_string(blockPos) << std::endl;
		Blocks& block = getBlock(blockPos, chunk);
		std::cout << "block: " << (int)block << std::endl;
		if (block != Blocks::AIR) {
			// block = Blocks::AIR;
			changed = 1;
			break;
		}  
	}
	if (changed) {
		auto all = getSubChunks();
		auto chunks = getSubChunk(glm::round(lookPos));

		auto start2 = std::chrono::high_resolution_clock::now();
		/*for (GLuint i = 0; i < chunks.size(); i++)
		{
			if (i == 0) {

			}
			else {

			}
		}*/
		chunks->editBlock(glm::round(lookPos), Blocks::AIR, all);
	
		auto end = std::chrono::high_resolution_clock::now();

		genWorldMesh();
		drawable.setUp(worldMesh);
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start2);
		std::cout << "genWorldMesh time: " << duration.count() << std::endl;

	}
	auto end = std::chrono::high_resolution_clock::now();
	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start1);
	// std::cout << "visual time: " << duration.count() << std::endl;
}
Blocks& World::getBlock(glm::ivec3 blockPos, chunk_column*& chunk_) {
	glm::vec3 subChunkPos = reduceToMultiple(blockPos, CHUNK_SIZE);
	subChunkPos.y = 0;
	glm::vec2 chunkPos(subChunkPos.x, subChunkPos.z);
	for (auto& chunk : chunks) {
		if (chunk.first.getPosition() == chunkPos) {
			chunk_ = &chunk.first;
			auto& block = chunk.first.getBlock(blockPos - (glm::ivec3)subChunkPos);
			return block;
		}
	}

}
void World::placeBlock(glm::vec3 pos, glm::vec3 front) {
	std::array<glm::ivec3, 6> dirs = {
		glm::ivec3(1, 0, 0), glm::ivec3(-1, 0, 0),
		glm::ivec3(0, 1, 0), glm::ivec3(0, -1, 0),
		glm::ivec3(0, 0, 1), glm::ivec3(0, 0, -1)
	};
	std::cout << "__________________________________________________________________ " << std::endl;
	// pos.y += 0.5f;
	GLboolean changed = 0;
	chunk_column* chunk = nullptr;
	glm::vec3 lookPos = pos + front * (GLfloat)(PLAYER_REACH - 1);
	for (GLubyte i = 0; i < PLAYER_REACH; i++) {
		lookPos -= front;
		glm::ivec3 blockPos = glm::round(lookPos);
		std::cout << "looking at: " << glm::to_string(blockPos) << std::endl;
		Blocks& block = getBlock(blockPos, chunk);
		std::cout << "block: " << (int)block << std::endl;
		if (block == Blocks::AIR) {
			for (auto& dir : dirs) {
				auto bp = blockPos + dir;
				auto& newBlock = getBlock(bp, chunk);
				if (newBlock != Blocks::AIR) {
					block = Blocks::WATER;
					changed = 1;
					break;
				}
			}
			if (changed)break;
		}
	}
	if (changed) {
		auto chunks_ = getChunks();
		std::vector<std::pair<Face*, GLboolean>> changed;
		chunk->createMesh(chunks_, 1, changed);
		worldMesh.clear();
		genWorldMesh();
		drawable.setUp(worldMesh);
	}
}

Chunk* World::getSubChunk(glm::ivec3 pos) {
	glm::ivec2 chunkPos(pos.x, pos.z);
	reduceToMultiple(chunkPos, (GLfloat)CHUNK_SIZE, "");
	// chunkPos.y += 2;
	chunk_column* chunkOcc = nullptr;
	for (auto& chunk_ : chunks) {
		auto& chunk = std::get<0>(chunk_);
		if (chunk.getPosition() == (ChunkPosition)chunkPos) {
			chunkOcc = &chunk;
			break;
		}
	}
	if (!chunkOcc) return nullptr;
	return chunkOcc->getSubchunk_unsafe(pos.y);
}
std::vector<Chunk*> World::getSubChunk(glm::ivec3 pos, GLboolean surrounding) {
	ChunkPosition chunkPos(pos.x, pos.z);
	glm::ivec2 chunkP = chunkPos;
	reduceToMultiple(chunkP, (GLfloat)CHUNK_SIZE, "");
	chunkPos = chunkP;
	std::vector<ChunkPosition> poses = {
		chunkPos,

		chunkPos + ChunkPosition(CHUNK_SIZE, 0),	
		chunkPos + ChunkPosition(-CHUNK_SIZE, 0),

		chunkPos + ChunkPosition(0, CHUNK_SIZE),
		chunkPos + ChunkPosition(0, -CHUNK_SIZE),

		chunkPos + ChunkPosition(CHUNK_SIZE, CHUNK_SIZE),
		chunkPos + ChunkPosition(-CHUNK_SIZE, CHUNK_SIZE),

		chunkPos + ChunkPosition(-CHUNK_SIZE, -CHUNK_SIZE),
		chunkPos + ChunkPosition(CHUNK_SIZE, -CHUNK_SIZE)
	};
	std::vector<Chunk*> subChunks;
	for (auto& p : poses) {
		glm::vec3 po(p.x, pos.y, p.y);
		subChunks.push_back(getSubChunk(po));
	}
	return subChunks;
}
std::vector<Chunk*> World::getSubChunks() {
	std::vector<Chunk*> subChunks;
	for (auto& chunk : chunks) {
		for (auto& subChunk : chunk.first.chunks) {
			subChunks.push_back(&subChunk);
			
		}
	}
	return subChunks;
}