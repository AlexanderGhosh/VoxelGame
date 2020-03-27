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
	prevChunkPos = glm::vec2(0);
	getNewChunkPositions(!terrain);
}
void World::getNewChunkPositions(GLboolean flat) {
	std::vector<glm::vec2> chunkPositions = getNewChunkPositions(glm::vec2(0));

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
	auto start = std::chrono::high_resolution_clock::now();
	for (glm::vec2& pos : chunkPositions) {
		chunks2.push_back({ pos });
	}
	for (auto& chunk : chunks2) {
		chunk.createMesh_flat(&chunks2);
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Chunk Creation Duration: " << duration.count() << " microsecconds\n";
}
void World::generateTerrain(std::vector<glm::vec2> chunkPositions) {
	std::cout << "Started\n";

	world_generation wg(seed, 3, 1, { {1.5f, 1.5f}, {0.75f, 0.75f}, {0.325f, 0.325f} });
	std::vector<glm::vec2> victims;

	for (glm::vec2& pos : chunkPositions) {
		std::string name = "chunk" + std::to_string((int)pos.x) + "," + std::to_string((int)pos.y);
		if (FILE* file = fopen(("Chunks/" + name + ".dat").c_str(), "r")) {
			fclose(file);
			chunks2.push_back({ name });
			victims.push_back(pos);
		}
		else {
			HeightMap heightMap = wg.createHeightMap(pos, 0);
			chunks2.push_back({ pos, heightMap });
		}
	}
	
	for (auto& victim : victims) {
		auto found = std::find(chunkPositions.begin(), chunkPositions.end(), victim);
		if (found == chunkPositions.end()) continue;
		chunkPositions.erase(found);
	}

	int time_ = 0;
	int counter = 0;
	for (ChunkColumn& chunk : chunks2) {
		if (std::find(chunkPositions.begin(), chunkPositions.end(), chunk.getPosition()) != chunkPositions.end()) {
			Timer t;
			t.start();
			chunk.createMesh(&chunks2); // NEEDS SPEEDING UP  4 secconds
			t.stop();
			t.showTime("Chunk Creation");
			chunk.save(seed); // neglagble


			// 4 per
		}
	}
 	genWorldMesh();
	drawable.setUp(worldMesh);


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
GLboolean first = 1;
void World::renderChunksDynamic(Camera& c, glm::mat4 projection) {
	if (reDraw && !first) {
  		auto start = std::chrono::high_resolution_clock::now();

		std::vector<glm::vec2> chunkPositions_active = centeredPositions(prevChunkPos, std::vector<glm::vec2>(), RENDER_DISTANCE);
		std::vector<glm::vec2> chunkPositions_lazy = centeredPositions(prevChunkPos, chunkPositions_active, RENDER_DISTANCE + 1);

		std::vector<glm::vec2> chunkPositions = chunkPositions_active;
		chunkPositions.insert(chunkPositions.begin(), chunkPositions_lazy.begin(), chunkPositions_lazy.end());

		std::vector<ChunkColumn> newChunks = createChunks(chunkPositions, chunkPositions_active, chunkPositions_lazy, 1, 1, &chunks2); // most costly
		auto end = std::chrono::high_resolution_clock::now();

		this->chunks2.insert(this->chunks2.begin(), newChunks.begin(), newChunks.end());
		// re-draw the world
		genWorldMesh();
		drawable.setUp(worldMesh);

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout << "redraw time: " << duration.count() << " microsecconds\n";
	}
	drawable.render(c, projection);
	reDraw = 0;
	first = 0;
}

void World::genWorldMesh() {
	worldMesh.clear();
	for(auto & chunk : chunks2) {
		std::unordered_map<GLuint, FaceB_p>& mesh = chunk.getMesh();
		for (auto& m : mesh) {
			const GLuint key = m.first;
			FaceB_p& faces = m.second;
			try {
				FaceB_p& faces2 = worldMesh.at(key);
				std::vector<glm::mat4>& models = std::get<2>(faces);
				std::get<2>(faces2).insert(std::get<2>(faces2).end(), models.begin(), models.end());
			}
			catch (std::exception e) {
				worldMesh.insert({ key, faces });
			}
		}
	}
}

void World::breakBlock(glm::vec3 pos, glm::vec3 front) {
	auto start = std::chrono::high_resolution_clock::now();

	Ray ray = Ray(pos, front, PLAYER_REACH);
	ChunkColumn* chunkOcc = getChunkOccupied(pos);
	glm::vec2 in_chunkPos = glm::round(glm::vec2(pos.x, pos.z)) - chunkOcc->getPosition();
	
	std::tuple<glm::vec3, FACES_NAMES> intersect = getIntersected(chunkOcc, in_chunkPos, ray);
	glm::vec3 p = std::get<0>(intersect);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "p1: " << duration.count() << "\n"; // 2379

	chunkOcc->editBlock(p, 1, Blocks::AIR, &chunks2);
	genWorldMesh();
	start = std::chrono::high_resolution_clock::now();
	drawable.setUp(worldMesh);
	end = std::chrono::high_resolution_clock::now();

	duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "p2: " << duration.count() << "\n"; // 3687
}
void World::placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block) {
	
	Ray ray = Ray(pos, front, PLAYER_REACH);
	ChunkColumn* chunkOcc = getChunkOccupied(pos);
	glm::vec2 in_chunkPos = glm::round(glm::vec2(pos.x, pos.z)) - chunkOcc->getPosition();

	std::tuple<glm::vec3, FACES_NAMES> intersect = getIntersected(chunkOcc, in_chunkPos, ray);
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

	chunkOcc->editBlock(p, 1, Blocks::STONE, &chunks2);

	genWorldMesh();
	drawable.setUp(worldMesh);
}

void World::updatePlayerPos(glm::vec3 pos) {
	glm::vec2 position = getChunkOccupied(pos)->getPosition();
	if (position != prevChunkPos) {
		reDraw = 1;
		prevChunkPos = position;
	}
}

std::tuple<glm::vec3, FACES_NAMES> World::getIntersected(ChunkColumn*& chunkOcc, glm::vec2 in_chunkPos, Ray ray) {
	/*std::vector<glm::vec2> positions;
	positions.push_back(chunkOcc->getPosition());
	/*glm::bvec2 close = glm::lessThan(in_chunkPos - glm::vec2(PLAYER_REACH), { 0, 0 });
	glm::bvec2 far = glm::greaterThan(in_chunkPos + glm::vec2(PLAYER_REACH), { CHUNK_SIZE - 1, CHUNK_SIZE - 1 });

	glm::vec2 delta(-CHUNK_SIZE);
	if (close.x) {
		positions.push_back(chunkOcc->getPosition() + delta * glm::vec2(1, 0));
	}
	if (close.y) {
		positions.push_back(chunkOcc->getPosition() + delta * glm::vec2(0, 1));
	}
	if (glm::all(close)) {
		positions.push_back(chunkOcc->getPosition() + delta * (glm::vec2)close);
	}

	delta *= -1;
	if (far.x) {
		positions.push_back(chunkOcc->getPosition() + delta * glm::vec2(1, 0));
	}
	if (far.y) {
		positions.push_back(chunkOcc->getPosition() + delta * glm::vec2(0, 1));
	}
	if (glm::all(far)) {
		positions.push_back(chunkOcc->getPosition() + delta * (glm::vec2)far);
	}

	if (close.x && far.y) positions.push_back(chunkOcc->getPosition() + glm::vec2(-CHUNK_SIZE, CHUNK_SIZE));
	if (close.y && far.x) positions.push_back(chunkOcc->getPosition() + glm::vec2(CHUNK_SIZE, -CHUNK_SIZE));*/

	
	std::vector<ChunkColumn*> chunks;
	std::vector<std::tuple<ChunkColumn*, std::unordered_map<GLuint, FaceB_p>>> meshes;
	for (auto& chunk : chunks2) {
		meshes.push_back({ &chunk, chunk.getMesh() });
	}
	/*for (glm::vec2 pos : positions) {
		std::vector <ChunkColumn>::iterator found = std::find(this->chunks2.begin(), this->chunks2.end(), pos);
		if (found == this->chunks2.end()) continue;
		auto& mes = (*found).getMesh();
		for (auto& m : mes) {
			const GLuint key = m.first;
			meshes[key] = 
		}
	}*/

	glm::vec3 p(0);
	FACES_NAMES face = NULL_;
	GLfloat shortestDistance = 1000;
	std::cout << "size: " << meshes.size() << std::endl;
	for (auto& mesh : meshes) {
		std::unordered_map<GLuint, FaceB_p>& faces = std::get<1>(mesh);
		for (auto& faces_ : faces) {
			FACES_NAMES& face_ = std::get<0>(faces_.second)->type;
			for (auto& model : std::get<2>(faces_.second)) {
				glm::vec3 pos = getTranslation(model);
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
	// return { {1, 232, 3}, FRONT };
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

	return res;
}

std::vector<ChunkColumn> World::createChunks(std::vector<glm::vec2> positions, std::vector<glm::vec2> activeChunks,
	std::vector<glm::vec2> lazyChunks, GLboolean rectifyForExisting, GLboolean checkLazy, std::vector<ChunkColumn>* adjacentChunks)
{
	std::vector<ChunkColumn> res;
	for (glm::vec2& chunkPos : positions) {
		HeightMap heightMap = world_generation::createHeightMap(chunkPos, 0);
		res.push_back({ chunkPos, heightMap });
	} // generation of blocks
	
	if (rectifyForExisting) {
		std::vector<glm::vec2> hitList;
		for (ChunkColumn& chunk : chunks2) {
			std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), chunk);
			if (found != res.end()) res.erase(found);
			else {
				hitList.push_back(chunk.getPosition());
			}
		}
		for (glm::vec2& pos : hitList) {
			std::vector<ChunkColumn>::iterator found = std::find(chunks2.begin(), chunks2.end(), pos);
			if (found == chunks2.end()) continue;
			chunks2.erase(found);
		}
	}
	std::vector<ChunkColumn> fromFile;
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

			std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), pos);
			if (found != res.end()) {
				res.erase(found);
				fromFile.push_back({ "chunk" + fileName });
				// fromFile.back().createMesh({}, 0);
			}
		}
	}
		
	for(glm::vec2 pos : lazyChunks) {
		std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), pos);
		if (found == res.end()) continue;
		ChunkColumn& chunk = *found;
		chunk.createMesh(adjacentChunks);
		chunk.save(seed);
		res.erase(found);
	}
	for (glm::vec2 pos : activeChunks) {
		std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), pos);
		if (found == res.end()) continue;
		ChunkColumn& chunk = *found;
		chunk.createMesh(adjacentChunks);
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

void World::save() {
	for (auto& chunk : chunks2) {
		chunk.save(seed);
	}
}