#include "World.h"
#include "gtx/string_cast.hpp"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
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
	for (glm::vec2 pos : chunkPositions) {
		std::string name = "chunk" + std::to_string((int)pos.x) + "," + std::to_string((int)pos.y);
		if (FILE* file = fopen(("Chunks/" + name + ".dat").c_str(), "r")) {
			fclose(file);
			chunks.push_back({ name });
		}
		else {
			chunks.push_back({ pos });
		}
	}
	

	auto start = std::chrono::high_resolution_clock::now();
	std::vector<Chunk*> subChunks = getSubChunks();
	for (auto& chunk : chunks) {
		if (chunk.getPosition() == glm::vec2(0)) {
			chunk.createMesh({}, 0);
		}
		chunk.createMesh(subChunks);
	}
 	genWorldMesh(0);
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
GLboolean first = 0;
void World::renderChunksDynamic(Camera& c, glm::mat4 projection) {
	if (reDraw && !first) {
  		auto start = std::chrono::high_resolution_clock::now();

		std::vector<glm::vec2> chunkPositions_active = centeredPositions(prevChunkPos, std::vector<glm::vec2>(), RENDER_DISTANCE);
		std::vector<glm::vec2> chunkPositions_lazy = centeredPositions(prevChunkPos, chunkPositions_active, RENDER_DISTANCE + 1);

		std::vector<glm::vec2> chunkPositions = chunkPositions_active;
		chunkPositions.insert(chunkPositions.begin(), chunkPositions_lazy.begin(), chunkPositions_lazy.end());

		std::vector<glm::vec2> subPos = centeredPositions(prevChunkPos, std::vector<glm::vec2>(), RENDER_DISTANCE + 3);
		std::vector<chunk_column> chunks;
		std::vector<Chunk*> subChunks;
		for (glm::vec2& pos : subPos) {
			chunks.push_back({ pos });
		}
		for (chunk_column& chunk : chunks) {
			std::vector<Chunk*> subs = chunk.getSubChunkPointers();
			subChunks.insert(subChunks.begin(), subs.begin(), subs.end());
		}

		std::vector<chunk_column> newChunks = createChunks(chunkPositions, chunkPositions_active, chunkPositions_lazy, 1, 1, subChunks); // most costly
		auto end = std::chrono::high_resolution_clock::now();
	
		this->chunks.insert(this->chunks.begin(), newChunks.begin(), newChunks.end());
		// re-draw the world
		genWorldMesh(1);
		drawable.setUp(worldMesh);

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout << "redraw time: " << duration.count() << " microsecconds\n";
	}
	drawable.render(c, projection);
	reDraw = 0;
	first = 0;
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
		chunk_column* chunkOcc = nullptr;
		auto chunks = getSubChunkOccupied(glm::round(lookPos), chunkOcc);
		chunks->editBlock(glm::round(lookPos), Blocks::AIR, all);
		*block = Blocks::AIR;
		if (chunkOcc) {
			chunkOcc->createMesh({}, 0);
			chunkOcc->save("chunk" + std::to_string((int)chunkOcc->getPosition().x) + "," + std::to_string((int)chunkOcc->getPosition().y), seed);
		}
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
void World::placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block) {
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
					changed = 1;
					break;
				}
			}
			if (changed)break;
		}
	}
	if (changed) {
		auto all = getSubChunks();
		chunk_column* chunkOcc = nullptr;
		auto chunks = getSubChunkOccupied(blockPos, chunkOcc);
		chunks->editBlock(blockPos, block, all);
		if (chunkOcc) {
			chunkOcc->createMesh({}, 0);
			chunkOcc->save("chunk" + std::to_string((int)chunkOcc->getPosition().x) + "," + std::to_string((int)chunkOcc->getPosition().y), seed);
		}
		genWorldMesh(1);
		drawable.setUp(worldMesh);
	}
}

Chunk* World::getSubChunkOccupied(glm::ivec3 pos, chunk_column*& chunkOcc) {
	glm::ivec2 chunkPos(pos.x, pos.z);
	reduceToMultiple(chunkPos, (GLfloat)CHUNK_SIZE, "");
	// chunkPos.y += 2;
	// chunk_column* chunkOcc = nullptr;
	for (auto& chunk : chunks) {
		if (chunk.getPosition() == (ChunkPosition)chunkPos) {
			chunkOcc = &chunk;
			break;
		}
	}
	if (!chunkOcc) return nullptr;
	return chunkOcc->getSubchunk_unsafe(pos.y);
}
std::vector<Chunk*> World::getSubChunkOccupied(glm::ivec3 pos, GLboolean surrounding) {
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
		chunk_column* chunkOcc = nullptr;
		subChunks.push_back(getSubChunkOccupied(po, chunkOcc));
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

std::vector<chunk_column> World::createChunks(std::vector<ChunkPosition> positions, std::vector<ChunkPosition> activeChunks,
	std::vector<ChunkPosition> lazyChunks, GLboolean rectifyForExisting, GLboolean checkLazy, std::vector<Chunk*> subChunks)
{
	std::vector<chunk_column> res;
	for (ChunkPosition& chunkPos : positions) {
		res.push_back(chunk_column(chunkPos));
	} // generation of blocks
	
	if (rectifyForExisting) {
		std::vector<glm::vec2> hitList;
		for (chunk_column& chunk : chunks) {
			std::vector<chunk_column>::iterator found = std::find(res.begin(), res.end(), chunk);
			if (found != res.end()) res.erase(found);
			else {
				hitList.push_back(chunk.getPosition());
			}
		}
		for (glm::vec2& pos : hitList) {
			std::vector<chunk_column>::iterator found = std::find(chunks.begin(), chunks.end(), pos);
			if (found == chunks.end()) continue;
			chunks.erase(found);
		}
	}
	std::vector<chunk_column> fromFile;
	if (checkLazy) {
		for (auto& p : std::experimental::filesystem::directory_iterator("Chunks")) {
			if (!p.path().has_extension() || p.path().extension() != ".dat") continue;
			std::string fileName = p.path().filename().string();
			for (auto& l : p.path().extension().string()) {
				fileName.pop_back();
			}
			fileName.erase(fileName.begin(), fileName.begin() + 5);
			std::stringstream ss(fileName);
			std::string token;
			glm::vec2 pos(0);
			GLubyte i = 0;
			while (getline(ss, token, ',')) {
				pos[i++] = std::stof(token);
			}

			std::vector<chunk_column>::iterator found = std::find(res.begin(), res.end(), pos);
			if (found != res.end()) {
				res.erase(found);
				fromFile.push_back(chunk_column("chunk" + fileName));
				fromFile.back().createMesh({}, 0);
			}
		}
	}
		
	for(glm::vec2 pos : lazyChunks) {
		std::vector<chunk_column>::iterator found = std::find(res.begin(), res.end(), pos);
		if (found == res.end()) continue;
		chunk_column& chunk = *found;
		chunk.createMesh(subChunks);
		chunk.save("chunk" + std::to_string((int)chunk.getPosition().x) + "," + std::to_string((int)chunk.getPosition().y), seed);
		res.erase(found);
	}
	for (glm::vec2 pos : activeChunks) {
		std::vector<chunk_column>::iterator found = std::find(res.begin(), res.end(), pos);
		if (found == res.end()) continue;
		chunk_column& chunk = *found;
		chunk.createMesh(subChunks);
	}
	res.insert(res.end(), fromFile.begin(), fromFile.end());
	return res;
}

std::vector<glm::vec2> World::centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, GLint renderDist) {
	if (renderDist % 2 == 0)  renderDist++;
	GLubyte radius = (renderDist - 1) / 2;
	std::vector<glm::vec2> res;
	for (GLbyte x = -radius; x < radius + 1; x++) {
		for (GLbyte y = -radius; y < radius + 1; y++) {
			glm::vec2 pos(x, y);
			pos *= CHUNK_SIZE;
			pos += origin;
			if (std::find(exclude.begin(), exclude.end(), pos) != exclude.end()) continue;
			res.push_back(pos);
		}
	}
	return res;
}