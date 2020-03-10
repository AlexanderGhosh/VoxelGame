#include "World.h"
#include "gtx/string_cast.hpp"
World::World() {
	chunks = std::vector<chunk_column>();
}
World::World(GLboolean gen, GLboolean terrain, GLboolean isDynamic, GLuint seed) {
	this->seed = seed;
	chunks = std::vector<chunk_column>();
	this->isDynamic = isDynamic;
	if (!gen) return;
	prevChunkPos = ChunkPosition(0);
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

		chunks.push_back(chunk);
	}

	for (auto& chunk : chunks) {
		std::vector<std::pair<Face*, GLboolean>> changed;
		chunk.createMesh(getSubChunks());
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto start = std::chrono::high_resolution_clock::now();

	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << "): " << duration.count() << " microseconds" << std::endl;
}
void World::generateTerrain(std::vector<glm::vec2> chunkPositions) {
	std::cout << "Started\n";
	GLubyte i = 0;
	world_generation wg(seed, 3, 1, { {1.5f, 1.5f}, {0.75f, 0.75f}, {0.325f, 0.325f} });
	for (GLuint i = 0; i < chunkPositions.size(); i++) {
		auto& pos = chunkPositions[i];
		chunk_column chunk(pos);
		chunks.push_back(chunk);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto& chunk : chunks) {
		std::vector<std::pair<Face*, GLboolean>> changed;
		chunk.createMesh(getSubChunks());
	}
	chunks.erase(chunks.begin());
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
		ChunkPosition& origin = prevChunkPos;
		std::vector<ChunkPosition> positions = getNewChunkPositions(origin, RENDER_DISTANCE);
		std::vector<ChunkPosition> positionsMissing = positions;
		std::vector<GLubyte> toRemove;
		for (GLubyte i = 0; i < chunks.size(); i++) {
			ChunkPosition& chunkPos = chunks[i].getPosition();
			auto found = std::find(positions.begin(), positions.end(), chunkPos);
			if (found == positions.end()) { // if not found
				toRemove.push_back(i);
				continue;
			}
			else {
				positionsMissing.erase(std::find(positionsMissing.begin(), positionsMissing.end(), chunkPos));
			}
		}
		for (GLubyte i = 0; i < toRemove.size(); i++) {
			GLubyte index = toRemove[i] - i;
			chunks.erase(chunks.begin() + index);
		}
		for (ChunkPosition& pos : positionsMissing) {
			createChunk(pos, 0);
		}

		auto subs = getSubChunks();
		auto start = std::chrono::high_resolution_clock::now();
		for (GLubyte i = chunks.size() - positionsMissing.size() - 1; i < chunks.size(); i++) {
			chunks[i].createMesh(subs);
		}

		auto end = std::chrono::high_resolution_clock::now();
		genWorldMesh();
		drawable.setUp(worldMesh);

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout << "redraw time: " << duration.count() << " microsecconds\n";
	}
	drawable.render(c, projection);
	reDraw = 0;
}
std::vector<chunk_column*> World::getChunks() {
	std::vector<chunk_column*> res;
	res.reserve(chunks.size());
	for (auto& chunk : chunks) {
		res.push_back(&chunk);
	}
	return res;
}
void World::genWorldMesh(GLboolean gen) {
	worldMesh.clear();
	for(auto & chunk : chunks) {
		auto mesh = chunk.getMesh(gen);
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
void World::updatePlayerPos(glm::vec3 pos) {
	ChunkPosition& position = getChunkOccupied(pos)->getPosition();
	if (position != prevChunkPos) {
		reDraw = 1;
		prevChunkPos = position;
	}
}

void World::createChunk(ChunkPosition position, GLboolean updateMesh) {
	/*for (auto& chunk : chunks) {
		if (position == chunk.getPosition()) return;
	}
	glm::vec3 pos(position.x, -CHUNK_SIZE, position.y);
	auto start = std::chrono::high_resolution_clock::now();
	ChunkHeightMap hm = world_generation::genHeightMap(pos);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	// std::cout << "chunkmap: " << duration.count() << " microsecconds\n";
	chunk_column chunk(position, hm);
	chunks.push_back(chunk);
	if (!updateMesh) return;
	chunks.back().createMesh(getChunks());*/
	chunk_column chunk("chunk");
	chunks.push_back(chunk);
	if (!updateMesh) return;
	chunks.back().createMesh({}, 0);
	genWorldMesh();
	drawable.setUp(worldMesh);
}
void World::removeChunk(ChunkPosition position) {
	for (GLubyte i = 0; i < chunks.size(); i++) {
		if (position == chunks[i].getPosition()) {
			chunks.erase(chunks.begin() + i);
			genWorldMesh();
			drawable.setUp(worldMesh);
			break;
		}
	}
}

void World::breakBlock(glm::vec3 pos, glm::vec3 front) {
	// std::cout << "__________________________________________________________________" << std::endl;
	// auto start1 = std::chrono::high_resolution_clock::now();
	GLboolean changed = 0;
	chunk_column* chunk = nullptr;
	glm::vec3 lookPos = pos; 
	Blocks* block = nullptr;
	for (GLubyte i = 0; i < PLAYER_REACH; i++) {
		lookPos += front;
		glm::ivec3 blockPos = glm::round(lookPos);
		// std::cout << "looking at: " << glm::to_string(blockPos) << std::endl;
		block = &getBlock(blockPos, chunk);
		// std::cout << "block: " << (int)block << std::endl;
		if (*block != Blocks::AIR) {
			changed = 1;
			break;
		}  
	}
	if (changed) {
		auto all = getSubChunks();
		auto chunks = getSubChunk(glm::round(lookPos));
		chunks->editBlock(glm::round(lookPos), Blocks::AIR, all);
		*block = Blocks::AIR;
		genWorldMesh(1);
		drawable.setUp(worldMesh);
	}
	// auto end = std::chrono::high_resolution_clock::now();
	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start1);
	// std::cout << "visual time: " << duration.count() << std::endl;
}
Blocks& World::getBlock(glm::ivec3 blockPos, chunk_column*& chunk_) {
	glm::vec3 subChunkPos = reduceToMultiple(blockPos, CHUNK_SIZE);
	subChunkPos.y = 0;
	glm::vec2 chunkPos(subChunkPos.x, subChunkPos.z);
	for (auto& chunk : chunks) {
		if (chunk.getPosition() == chunkPos) {
			chunk_ = &chunk;
			auto& block = chunk.getBlock(blockPos - (glm::ivec3)subChunkPos);
			return block;
		}
	}

}
chunk_column* World::getChunkOccupied(glm::vec3 position) {
	position = (glm::ivec3)position;
	position = reduceToMultiple(position, CHUNK_SIZE);
	glm::vec2 chunkPos(position.x, position.z);
	for (auto& chunk : chunks) {
		if (chunk.getPosition() == chunkPos) {
			return &chunk;
		}
	}
}
void World::placeBlock(glm::vec3 pos, glm::vec3 front) {
	std::array<glm::ivec3, 6> dirs = {
		glm::ivec3(1, 0, 0), glm::ivec3(-1, 0, 0),
		glm::ivec3(0, 1, 0), glm::ivec3(0, -1, 0),
		glm::ivec3(0, 0, 1), glm::ivec3(0, 0, -1)
	};
	// std::cout << "__________________________________________________________________ " << std::endl;
	// pos.y += 0.5f;
	GLboolean changed = 0;
	chunk_column* chunk = nullptr;
	glm::vec3 lookPos = pos + front * (GLfloat)(PLAYER_REACH - 1);
	glm::vec3 blockPos(0);
	for (GLubyte i = 0; i < PLAYER_REACH; i++) {
		lookPos -= front;
		blockPos = glm::round(lookPos);
		// std::cout << "looking at: " << glm::to_string(blockPos) << std::endl;
		Blocks& block = getBlock(blockPos, chunk);
		// std::cout << "block: " << (int)block << std::endl;
		if (block == Blocks::AIR) {
			for (auto& dir : dirs) {
				glm::vec3 bp = blockPos + (glm::vec3)dir;
				auto& newBlock = getBlock(bp, chunk);
				if (newBlock != Blocks::AIR) {
					// block = Blocks::WATER;
					changed = 1;
					break;
				}
			}
			if (changed)break;
		}
	}
	if (changed) {
		auto all = getSubChunks();
		auto chunks = getSubChunk(blockPos);
		chunks->editBlock(blockPos, Blocks::WATER, all);
		genWorldMesh(1);
		drawable.setUp(worldMesh);
	}
}

Chunk* World::getSubChunk(glm::ivec3 pos) {
	glm::ivec2 chunkPos(pos.x, pos.z);
	reduceToMultiple(chunkPos, (GLfloat)CHUNK_SIZE, "");
	// chunkPos.y += 2;
	chunk_column* chunkOcc = nullptr;
	for (auto& chunk : chunks) {
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
		for (auto& subChunk : chunk.chunks) {
			subChunks.push_back(&subChunk);
			
		}
	}
	return subChunks;
}

std::vector<chunk_column> World::createChunks(std::vector<ChunkPosition> positions, GLboolean lazyLoading, GLboolean rectifyExisting) {

}