#include "Chunk.h"
Chunk::Chunk() {
	null = GL_TRUE;
}
Chunk::Chunk(glm::vec3 pos, GLboolean create) {
	position = pos;
	null = GL_FALSE;
	blocks.fill(Blocks::AIR);
	if (create) {
		this->create();
	}
}
void Chunk::create() {
	createBlocksFlat(Blocks::AIR);
	createMesh(std::vector<Chunk*>());
}
void Chunk::createBlocksFlat(Blocks block) {
	if (position.y > -CHUNK_SIZE) block = Blocks::AIR;
	blocks.fill(block);

	if (position == glm::vec3(0, -2, 0) || position == glm::vec3(-2, -2, -2)) {
		//blocks[getBlockIndex({ 0, 0, 0 })] = 0;
		blocks[getBlockIndex({ 0, -1, 0 })] = Blocks::AIR;
		blocks[getBlockIndex({ 1, -1, 0 })] = Blocks::AIR;
		blocks[getBlockIndex({ 1, -1, 1 })] = Blocks::AIR;
		blocks[getBlockIndex({ 0, -1, 1 })] = Blocks::AIR;
	}
}
void Chunk::createBlocks(HeightMapChunk heightMap) {
	blocks.fill(Blocks::AIR);
	for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
		for (GLubyte z = 0; z < CHUNK_SIZE; z++) {
			glm::vec3 position = glm::abs(this->position);
			GLubyte height = heightMap[x][z];
			if (height < 2) {
				blocks[getBlockIndex({ x, 0, z })] = Blocks::WATER;
				continue;
			}
			for (GLubyte y = 0; y < height; y++) {
				Blocks block = Blocks::GRASS; // grass
				if (y < height - 3) {
					block = Blocks::STONE; // stone
				}
				else if (y < height - 1) {
					block = Blocks::DIRT; // dirt
				}
				blocks[getBlockIndex({ x, y, z })] = block;
			}
		}
	}
}
void Chunk::createMesh(std::vector<Chunk*> chunks) {
	meshes.clear();
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				glm::vec3 pos = glm::vec3(x, y, z) + position;
				Blocks block = getBlock_safe({ x, y, z }, chunks);
				if (block == Blocks::AIR) continue;
				addBlock(block, chunks, pos, 1);
			}
		}
	}
}
void Chunk::cleanUp() {
	for (auto& face : meshes) {
		face = {};
	}
}

std::vector<Face*> Chunk::getMeshes() {
	std::vector<Face*> res;
	for (auto& mesh : meshes) {
		res.push_back(&mesh);
	}
	return res;
}
Blocks& Chunk::getBlock_unsafe(glm::ivec3 pos, GLboolean& error) {
	auto air = Blocks::AIR;
	/*while (pos.x%CHUNK_SIZE != 0 && !(std::abs(pos.x) > CHUNK_SIZE || std::abs(pos.x) < CHUNK_SIZE)) {
		pos -= glm::ivec3(CHUNK_SIZE);
	}*/
	try {
		return blocks.at(getBlockIndex(pos));
	}
	catch (std::exception e) {
		error = 1;
		return air;
	}
}
Blocks& Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks) {
	Chunk* pointer;
	return getBlock_safe(inChunkPosition, chunks, pointer);
}
GLboolean Chunk::isNull() {
	return null;
}

void Chunk::addBlock(Blocks block, std::vector<Chunk*>& chunks, glm::vec3 absPos, GLboolean skip) {
	if (skip && block == Blocks::AIR) return;
	auto relativePos = absPos - position;
	auto& x = relativePos.x;
	auto& y = relativePos.y;
	auto& z = relativePos.z;
	GLuint tex_index = (GLuint)getTexture(block);
	if (getBlock_safe({ x - 1, y, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[LEFT], TEXTURES[tex_index], absPos });
	}
	if (getBlock_safe({ x + 1, y, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[RIGHT], TEXTURES[tex_index], absPos });
	}

	if (getBlock_safe({ x, y - 1, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[BOTTOM], TEXTURES[tex_index], absPos });
	}
	if (getBlock_safe({ x, y + 1, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[TOP], TEXTURES[tex_index], absPos });
	}

	if (getBlock_safe({ x, y, z - 1 }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[BACK], TEXTURES[tex_index], absPos });
	}
	if (getBlock_safe({ x, y, z + 1 }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[FRONT], TEXTURES[tex_index], absPos });
	}
}

void Chunk::editBlock(glm::vec3 pos, Blocks block, std::vector<Chunk*> chunks) {
	std::vector<Face> victims;
	if (!this) return; 
	GLboolean error = 0;
	Blocks& b = getBlock_unsafe(pos - position, error);
	if (error) return;
	if (block == Blocks::AIR) {
		// break block
		for (GLuint i = 0; i < meshes.size(); i++) {
			auto& face = meshes[i];
			auto& p = std::get<2>(face);
			if (p == pos) {
				victims.push_back(face);
			}
		}
		for (auto& victim : victims) {
			auto found = std::find(meshes.begin(), meshes.end(), victim);
			meshes.erase(found);
		}
		glm::vec3 deltas[] = {
			glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
			glm::vec3( 1, 0, 0), glm::vec3(-1, 0, 0),
			glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)
		};
		
		for (GLubyte i = 0; i < 6; i++) {
			auto& delta = deltas[i];
			auto p = pos + delta;
			Chunk* chunk = this;
			GLboolean error = 0;
			Blocks block_ = getBlock_unsafe(p - position, error);
			if (error || block_ == Blocks::AIR) continue;
			GLubyte tex = (GLuint)getTexture(block_);
			Face face = { FACES[i], TEXTURES[tex], p };
			chunk->meshes.push_back(face);
		}
	}
	else {
		std::vector<Chunk*> chunks_;
		glm::vec3 deltas[] = {
			glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0),
			glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)
		}; 

		for (GLubyte i = 0; i < 6; i++) {
			glm::vec3 delta = deltas[i];
			Chunk* chunk = this;
			GLboolean error = 0;
			Blocks blk = getBlock_unsafe((pos + delta) - position, error);
			if (!chunk || error || blk == Blocks::AIR) continue;
			std::vector<Face>& meshs = chunk->meshes;
			for (auto& face : meshs) {
				GLubyte b = std::get<0>(face)->type;
				glm::vec3& p = std::get<2>(face);
				if (p == delta + pos && b == FACES[i]->type) {
					victims.push_back(face);
					chunks_.push_back(chunk);
					break;
				}
			}
		}
		for (GLubyte i = 0; i < victims.size(); i++) {
			Face& victim = victims[i];
			std::vector<Face>& meshs = chunks_[i]->meshes;
			auto found = std::find(meshs.begin(), meshs.end(), victim);
			meshs.erase(found);
		}
		addBlock(block, chunks, pos);
	}
	b = block;
}
std::vector<Face*> Chunk::getPointerMesh() {
	std::vector<Face*> res;
	for (auto& face : meshes) {
		res.push_back(&face);
	}
	return res;
}

Blocks& Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks, Chunk*& chunk_) {
	/*auto air = Blocks::AIR;
	if (glm::all(glm::greaterThanEqual(inChunkPosition, glm::vec3(0)))) {
		if (glm::all(glm::lessThan(inChunkPosition, glm::vec3(CHUNK_SIZE)))) {
			auto x = getBlockIndex(inChunkPosition);
			return blocks[x];
		}
	}

	glm::vec3 chunkPositionToLookAt = position;
	glm::vec3 toLookAt = inChunkPosition;
	// maps toLookAt to the correct range
	toLookAt = glm::abs(toLookAt) - 16.0f * glm::floor(glm::abs(toLookAt) / 16.0f);
	
	// if acceeds add 16 
	if (inChunkPosition.x >= CHUNK_SIZE) {
		chunkPositionToLookAt.x += CHUNK_SIZE;
	}
	if (inChunkPosition.y >= CHUNK_SIZE) {
		chunkPositionToLookAt.y += CHUNK_SIZE;
	}
	if (inChunkPosition.z >= CHUNK_SIZE) {
		chunkPositionToLookAt.z += CHUNK_SIZE;
	}
	// if bellow subtract 16
	if (inChunkPosition.x < 0) {
		chunkPositionToLookAt.x -= CHUNK_SIZE;
	}					  
	if (inChunkPosition.y < 0) {
		chunkPositionToLookAt.y -= CHUNK_SIZE;
	}					  
	if (inChunkPosition.z < 0) {
		chunkPositionToLookAt.z -= CHUNK_SIZE;
	}
	

	int index = getBlockIndex(toLookAt);

	if (index < 0) return air;
	for (auto& chunk : chunks) {
		if (chunk->position == chunkPositionToLookAt) {
			chunk_ = chunk;
			return chunk->blocks[index];
		}
	}
	return air;*/
	Blocks air = Blocks::AIR;
	if (inChunkPosition.x >= 0 && inChunkPosition.y >= 0 && inChunkPosition.z >= 0) {
		if (inChunkPosition.x < CHUNK_SIZE && inChunkPosition.y < CHUNK_SIZE && inChunkPosition.z < CHUNK_SIZE) {
			return blocks[getBlockIndex(inChunkPosition)];
		}
	}
	glm::vec3 toLookAt = inChunkPosition;
	glm::vec3 chunkPositionToLookAt = position;
	if (inChunkPosition.x < 0) {
		toLookAt.x = CHUNK_SIZE + inChunkPosition.x;
		chunkPositionToLookAt.x -= CHUNK_SIZE;
	}
	else if (inChunkPosition.x > CHUNK_SIZE - 1) {
		toLookAt.x -= CHUNK_SIZE;
		chunkPositionToLookAt.x += CHUNK_SIZE;
	}

	if (inChunkPosition.y < 0) {
		toLookAt.y = CHUNK_SIZE + inChunkPosition.y;
		chunkPositionToLookAt.y -= CHUNK_SIZE;
	}
	else if (inChunkPosition.y > CHUNK_SIZE - 1) {
		toLookAt.y -= CHUNK_SIZE;
		chunkPositionToLookAt.y += CHUNK_SIZE;
	}

	if (inChunkPosition.z < 0) {
		toLookAt.z = CHUNK_SIZE + inChunkPosition.z;
		chunkPositionToLookAt.z -= CHUNK_SIZE;
	}
	else if (inChunkPosition.z > CHUNK_SIZE - 1) {
		toLookAt.z -= CHUNK_SIZE;
		chunkPositionToLookAt.z += CHUNK_SIZE;
	}

	int index = getBlockIndex(toLookAt);

	if (index < 0) return air;
	for (auto& chunk : chunks) {
		if (chunk->position == chunkPositionToLookAt) {
			return chunk->blocks[index];
		}
	}
	return air;
}