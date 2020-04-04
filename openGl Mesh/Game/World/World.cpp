#include "World.h"
#include "gtx/string_cast.hpp"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
World::World() {
	chunks2 = std::vector<ChunkColumn>();
}
World::World(GLboolean gen, GLboolean terrain, GLboolean isDynamic, GLuint seed) {
	this->seed = seed;
	chunks2 = std::vector<ChunkColumn>();
	this->isDynamic = isDynamic;
	if (!gen) return;
	chunkOccupiedPosition = glm::vec2(0);
	getNewChunkPositions(!terrain);
}


void World::getNewChunkPositions(GLboolean flat) {
	std::vector<glm::vec2> chunkPositions = centeredPositions(glm::vec2(0), {});

	if (flat) {
		generateFlatChunks(chunkPositions);
	}
	else {
		generateTerrain(chunkPositions);
	}
}


void World::generateFlatChunks(std::vector<glm::vec2> chunkPositions) {
	std::cout << "Started\n";
	auto start = std::chrono::high_resolution_clock::now();
	for (glm::vec2& pos : chunkPositions) {
		chunks2.push_back({ pos });
	}
	auto t = getAdjacentMap({ 0, 0, 0 }, RENDER_DISTANCE + 2);
	for (auto& chunk : chunks2) {
		chunk.createMesh_flat(t);
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Chunk Creation Duration: " << duration.count() << " microsecconds\n";
}

void World::generateTerrain(std::vector<glm::vec2>& chunkPositions) {
	std::cout << "Started\n";

	world_generation wg(seed, 1, 0.5, {  { 1, 1 } });

	std::vector<glm::vec2> victims;
	// files and blocks
	for (glm::vec2& pos : chunkPositions) {
		std::string name = "chunk" + std::to_string((int)pos.x) + "," + std::to_string((int)pos.y);
		if (FILE* file = fopen(("Chunks/" + name + ".dat").c_str(), "r")) {
			fclose(file);
			chunks2.push_back({ name });
			if (chunks2.back().getMesh().size() > 0) victims.push_back(pos);
			else chunks2.back().stage = 0;
		}
		else {
			chunks2.push_back({ pos });
			chunks2.back().addTrees();

			generationStack.push_back(chunks2.size() - 1);
		}
	}
	std::vector<glm::vec2> ring = centeredPositions({ 0, 0 }, chunks2, RENDER_DISTANCE + 2);
	for (auto& chunk : chunks2) {
		worldMap[chunk.getPosition()] = *chunk.getBlockStore();
		chunk.setBlockStore(&worldMap[chunk.getPosition()]);
	}
	for (auto& pos : ring) {
		worldMap[pos] = BlockStore(pos);
	}
	
	genWorldMesh();
	drawable.setUp(worldMesh);
}

void World::render(Camera& c, glm::mat4 projection) {
	drawable.render(c, projection);
}

void World::genWorldMesh() {
	worldMesh.clear();
	for(auto & chunk : chunks2) {
		std::unordered_map<GLuint, FaceB_p>& mesh = chunk.getMesh();
		for (auto& m : mesh) {
			const GLuint key = m.first;
			FaceB_p& chunkFaces = m.second;
			GLuint initSize = worldMesh.size();
			auto& faces = worldMesh[key];
			if (worldMesh.size() > initSize) {
				faces = chunkFaces;
			}
			else {
				auto& models = std::get<2>(chunkFaces);
				std::get<2>(faces).insert(std::get<2>(faces).end(), models.begin(), models.end());
			}

			/*
			try {
				FaceB_p& faces2 = worldMesh.at(key);
				std::vector<glm::mat4>& models = std::get<2>(chunkFaces);
				std::get<2>(faces2).insert(std::get<2>(faces2).end(), models.begin(), models.end());
			}
			catch (std::exception e) {
				worldMesh.insert({ key, chunkFaces });
			}*/
		}
	}
}

void World::breakBlock(glm::vec3 pos, glm::vec3 front) {
	Ray ray = Ray(pos, front, PLAYER_REACH);
	ChunkColumn* chunkOcc = getChunkOccupied(pos);
	
	std::tuple<glm::vec3, FACES_NAMES> intersect = getIntersectedBlock(chunkOcc, ray);
	glm::vec3 p = std::get<0>(intersect);

	auto t = getAdjacentMapPointers(pos, RENDER_DISTANCE + 2);
	chunkOcc->editBlock(p, 1, Blocks::AIR, t);

	genWorldMesh();
	drawable.setUp(worldMesh);
}
void World::placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block) {
	
	Ray ray = Ray(pos, front, PLAYER_REACH);
	ChunkColumn* chunkOcc = getChunkOccupied(pos);

	std::tuple<glm::vec3, FACES_NAMES> intersect = getIntersectedBlock(chunkOcc, ray);
	auto p = std::get<0>(intersect);
	auto face = std::get<1>(intersect);

	switch (face)
	{
	case FRONT:
		p.z++;
		break;
	case BACK:
		p.z--;
		break;
	case LEFT:
		p.x--;
		break;
	case RIGHT:
		p.x++;
		break;
	case TOP:
		p.y++;
		break;
	case BOTTOM:
		p.y--;
		break;
	case NULL_:
		return;
	}
	// p is the world pos

	auto t = getAdjacentMapPointers(pos);
	chunkOcc->editBlock(p, 1, block, t);

	genWorldMesh();
	drawable.setUp(worldMesh);
}

std::array<std::vector<glm::vec2>, 2> getNewOld(glm::vec2 oldChunkPos, glm::vec2 newChunkPos) {
	std::array<std::vector<glm::vec2>, 2> res{};
	glm::vec2 delta = glm::normalize(newChunkPos - oldChunkPos);
	delta *= RENDER_DISTANCE%2 ? RENDER_DISTANCE/2 : (RENDER_DISTANCE+1)/2;
	// delta += oldChunkPos;
	int i = 0;
	if (!delta.y) {
		i = 1;
	}
	delta *= CHUNK_SIZE;
	GLint vary = (RENDER_DISTANCE / 2) * CHUNK_SIZE;
	GLint max = delta[i] + vary;
	for (GLint t = delta[i] - vary; t < max + CHUNK_SIZE; t+=CHUNK_SIZE) {
		glm::vec2 pos = delta + newChunkPos;
		pos[i] += t;
		res[0].push_back(pos);
		pos = oldChunkPos - delta;
		pos[i] += t;
		res[1].push_back(pos);
	}
	return res;
}

void World::updatePlayerPos(glm::vec3 pos) {
	Timer t;
	t.start();

	ChunkColumn* chunk = getChunkOccupied(pos);
	if (!chunk) return;
	glm::vec2 position = chunk->getPosition();
	if (position == chunkOccupiedPosition || !isDynamic) return;

	Timer t1;
	t1.start();

	std::array<std::vector<glm::vec2>, 2> addVic = getNewOld(chunkOccupiedPosition, position);

	chunkOccupiedPosition = position;

	t1.end();
	t1.showTime("getOldNew func", 1);

	t1.start();

	const std::vector<glm::vec2> newPos = addVic[0];
	const std::vector<glm::vec2> victims = addVic[1];
	
	for (auto& vic : victims) {
		Chunks::iterator found = std::find(chunks2.begin(), chunks2.end(), vic);
		if (found == chunks2.end()) continue;
		chunks2.erase(found);
		worldMap.erase(vic);
	}
	t1.end();
	t1.showTime("erase", 1);

	t.start();
	for (auto& pos : newPos) {
		std::string name = ChunkColumn(pos).getFileName();
		if (FILE* file = fopen(("Chunks/" + name + ".dat").c_str(), "r")) {
			chunks2.push_back({ name });
		}
		else {
			chunks2.push_back({ pos, &worldMap });
		}
		if (chunks2.back().getMesh().size() > 0) continue;
		// chunks2.back().addTrees();
		generationStack.push_back(chunks2.size() - 1);
	}

	t1.end();
	t1.showTime("add to chunks2", 1);

	
	t1.start();

	// updating worldMap
	glm::vec2 deltas[] = {
		{1, 0}, {0, 1}, {1, 1}, {0, 0}
	};
	GLuint size = worldMap.size();
	for (auto& pos : newPos) {
		for (auto delta : deltas) {
			delta *= CHUNK_SIZE;
			glm::vec2 p = pos + delta;
			auto& bs = worldMap[p];
			if (worldMap.size() > size) {
				bs = BlockStore(pos);
				size++;
			}
		}
	}

	t1.end();
	t1.showTime("updating worldMap", 1);

	t.end();
	t.showTime("all", 1);
	std::cout << "_________________________________________________________________________\n";
}

std::tuple<glm::vec3, FACES_NAMES> World::getIntersectedBlock(ChunkColumn*& chunkOcc, Ray ray) {
	std::vector<ChunkColumn*> chunks;
	std::vector<std::tuple<ChunkColumn*, std::unordered_map<GLuint, FaceB_p>>> meshes;
	for (auto& chunk : chunks2) {
		meshes.push_back({ &chunk, chunk.getMesh() });
	}
	
	auto isSameDirection = [](glm::vec3 pos, Ray ray) -> GLboolean {
		glm::vec3 diff = glm::normalize(glm::round(pos - ray.getOrigin()));
		glm::vec3 direction = ray.getDirection();
		for (GLubyte i = 0; i < 3; i++) {
			if (diff[i] >= 0 && direction[i] >= 0 || diff[i] <= 0 && direction[i] <= 0) {
			}
			else {
				return 0;
			}
		}
		return 1;
	};
	glm::vec3 p(0);
	FACES_NAMES face = NULL_;
	GLfloat shortestDistance = 1000;
	for (auto& mesh : meshes) {
		std::unordered_map<GLuint, FaceB_p>& faces = std::get<1>(mesh);
		for (auto& faces_ : faces) {
			FACES_NAMES& face_ = std::get<0>(faces_.second)->type;
			for (auto& model : std::get<2>(faces_.second)) {
				glm::vec3 pos = getTranslation(model);
				if (!isSameDirection(pos, ray)) continue;
				auto dist = ray.checkIntercesction_Block(pos, face_);
				if (dist == -1) {
					continue;
				}
				if (dist < shortestDistance) {
					shortestDistance = dist;
					p = pos;
					face = face_;
					chunkOcc = std::get<0>(mesh);
				}
			}
		}
	}
	return { p, face };
}
Entity* World::getIntersectedEntity(EntityHander& handler, Ray ray)
{
	std::vector<Entity>& entitys = handler.getEntitys();
	auto isSameDirection = [](glm::vec3 pos, Ray ray) -> GLboolean {
		glm::vec3 diff = glm::normalize(glm::round(pos - ray.getOrigin()));
		glm::vec3 direction = ray.getDirection();
		for (GLubyte i = 0; i < 3; i++) {
			if (diff[i] >= 0 && direction[i] >= 0 || diff[i] <= 0 && direction[i] <= 0) {
			}
			else {
				return 0;
			}
		}
		return 1;
	};
	Entity* entity = nullptr;
	FACES_NAMES face = NULL_;
	GLfloat shortestDistance = 1000;
	for (auto& en : entitys) {
		Entity* e = &en;
		if (!isSameDirection(e->getPosition(), ray)) continue;
		for (auto& face : en.getBody()) {
			FACES_NAMES& faceType = std::get<0>(face)->type;
			glm::vec3& pos = std::get<2>(face);
			if (!isSameDirection(pos, ray)) continue;
			auto dist = ray.checkIntercesction_Block(pos, faceType);
			if (dist == -1) {
				continue;
			}
			if (dist < shortestDistance) {
				shortestDistance = dist;
				entity = e;
			}
		}
	}
	return entity;
}
ChunkColumn* World::getChunkOccupied(glm::vec3 position) {
	position = glm::floor(position);
	position = reduceToMultiple(position, CHUNK_SIZE);
	glm::vec2 chunkPos(position.x, position.z);
	for (auto& chunk : chunks2) {
		if (chunk.getPosition() == chunkPos) {
			return &chunk;
		}
	}
	return nullptr;
}
std::unordered_map<GLuint, FaceB_p>& World::getWorldMesh()
{
	return worldMesh;
}

std::vector<ChunkColumn*> World::getAdjacentChunks(glm::vec3 worldPosition)
{
	std::vector<glm::vec2> chunksToFind;
	std::vector<ChunkColumn*> res;
	res.push_back(getChunkOccupied(worldPosition));
	if (!res.back()) return { };
	glm::vec2 centerd = res.back()->getPosition();
	worldPosition.x -= centerd.x;
	worldPosition.z -= centerd.y;
	if (worldPosition.x < 3) chunksToFind.push_back(centerd + glm::vec2(-CHUNK_SIZE, 0));
	else if(worldPosition.x > CHUNK_SIZE - 4) chunksToFind.push_back(centerd + glm::vec2(CHUNK_SIZE, 0));

	if (worldPosition.z < 3) chunksToFind.push_back(centerd + glm::vec2(0, -CHUNK_SIZE));
	else if (worldPosition.z > CHUNK_SIZE - 4) chunksToFind.push_back(centerd + glm::vec2(0, CHUNK_SIZE));

	for (glm::vec2& pos : chunksToFind) {
		std::vector<ChunkColumn>::iterator found = std::find(chunks2.begin(), chunks2.end(), pos);
		if (found != chunks2.end()) {
			res.push_back(&*found);
		}
	}
	if (res.front() == nullptr) {
		res.pop_back();
	}
	return res;
}

AdjacentMap World::getAdjacentMap(glm::vec3 worldPos, GLuint range)
{
	AdjacentMap res;
	std::vector<glm::vec2> poss = centeredPositions({ worldPos.x , worldPos.z }, chunks2, range);
	for (glm::vec2& pos : poss) {
		HeightMap hm = world_generation::createHeightMap(pos, 0);
		res.insert({ pos, { pos } });
	}
	for (ChunkColumn& chunk : chunks2) {
		res.insert({ chunk.getPosition(), chunk });
	}
	return res;
}

AdjacentMap_p World::getAdjacentMapPointers(glm::vec3 worldPos, GLuint range)
{
	AdjacentMap_p res;
	std::vector<glm::vec2> poss = centeredPositions({ worldPos.x , worldPos.z }, { }, range);
	for (glm::vec2& pos : poss) {
		Chunks::iterator found = std::find(chunks2.begin(), chunks2.end(), pos);
		if (found != chunks2.end()) {
			res.insert({ pos, &*found });
		}
	}
	return res;
}

std::vector<glm::vec2> World::centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, GLint renderDist) {
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
			if (std::find(exclude.begin(), exclude.end(), pos) != exclude.end()) continue;
			res.push_back(pos);
		}
	}
	return res;
}

std::vector<glm::vec2> World::centeredPositions(glm::vec2 origin, std::vector<ChunkColumn>& exclude, GLint renderDist)
{
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
			if (std::find(exclude.begin(), exclude.end(), pos) != exclude.end()) continue;
			res.push_back(pos);
		}
	}
	return res;
}

void World::save() {
	for (auto& chunk : chunks2) {
		// chunk.save(seed);
	}
}
void World::advanceGeneration()
{
	if (generationStack.size() == 0) return;
	ChunkColumn* chunk = &chunks2[generationStack.back()];
	glm::vec2 pos = chunk->getPosition();
	
	if (chunk->getMesh().size() == 0 || chunk->stage < 100) {
		chunk->createMesh(&worldMap);
	}
	else if (chunk->fromFile) {
		chunk->stage = 100;
	}

	
	
	if (chunk->stage >= 4) {
		generationStack.pop_back();
		genWorldMesh();
		drawable.setUp(worldMesh);
	}
}
