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
				if (block == Blocks::AIR) return;
				// addBlock(block, chunks, pos, { x, y, z });
				GLuint tex_index = (GLuint)getTexture(block);
				if (getBlock_safe({ x - 1, y, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[LEFT], TEXTURES[tex_index], pos });
				}
				if (getBlock_safe({ x + 1, y, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[RIGHT], TEXTURES[tex_index], pos });
				}

				if (getBlock_safe({ x, y - 1, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[BOTTOM], TEXTURES[tex_index], pos });
				}
				if (getBlock_safe({ x, y + 1, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[TOP], TEXTURES[tex_index], pos });
				}

				if (getBlock_safe({ x, y, z - 1 }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[BACK], TEXTURES[tex_index], pos });
				}
				if (getBlock_safe({ x, y, z + 1 }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[FRONT], TEXTURES[tex_index], pos });
				}
			}
		}
	}
}
void Chunk::createMesh(std::map<GLuint, Chunk*> chunks) {
	meshes.clear();
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				glm::vec3 pos = glm::vec3(x, y, z) + position;
				Blocks block = getBlock_safe({ x, y, z }, chunks);
				if (block == Blocks::AIR) continue;
				GLuint tex_index = (GLuint)getTexture(block);
				if (getBlock_safe({ x - 1, y, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[LEFT], TEXTURES[tex_index], pos });
				}
				if (getBlock_safe({ x + 1, y, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[RIGHT], TEXTURES[tex_index], pos });
				}

				if (getBlock_safe({ x, y - 1, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[BOTTOM], TEXTURES[tex_index], pos });
				}
				if (getBlock_safe({ x, y + 1, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[TOP], TEXTURES[tex_index], pos });
				}

				if (getBlock_safe({ x, y, z - 1 }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[BACK], TEXTURES[tex_index], pos });
				}
				if (getBlock_safe({ x, y, z + 1 }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[FRONT], TEXTURES[tex_index], pos });
				}
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
Blocks Chunk::getBlock_unsafe(glm::ivec3 pos) {
	auto p = pos;
	while (pos.x%CHUNK_SIZE != 0 && !(std::abs(pos.x) > CHUNK_SIZE || std::abs(pos.x) < CHUNK_SIZE)) {
		pos -= glm::ivec3(CHUNK_SIZE);
	}
	try {
		return blocks.at(getBlockIndex(pos));
	}
	catch (std::exception e) {
		return Blocks::AIR;
	}
}
Blocks Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks) {
	if (glm::all(glm::greaterThanEqual(inChunkPosition, glm::vec3(0)))) {
		if (glm::all(glm::lessThan(inChunkPosition, glm::vec3(CHUNK_SIZE)))) {
			return blocks[getBlockIndex(inChunkPosition)];
		}
	}
	glm::vec3 toLookAt = inChunkPosition;
	glm::vec3 chunkPositionToLookAt = position;

	glm::vec3 delta = inChunkPosition + glm::vec3(CHUNK_SIZE);
	glm::bvec3 comp = glm::lessThan(inChunkPosition, glm::vec3(0));
	delta *= comp;
	
	for (GLuint i = 0; i < 3; i++) {
		if (delta[i] != 0) {
			toLookAt[i] = delta[i];
		}
	}

	chunkPositionToLookAt -= glm::vec3(CHUNK_SIZE) * (glm::vec3)comp;

	auto t = glm::greaterThan(inChunkPosition, glm::vec3(CHUNK_SIZE - 1));
	comp = glm::not_(comp) && t;
	delta = glm::vec3(CHUNK_SIZE);
	toLookAt -= delta * (glm::vec3)comp;
	chunkPositionToLookAt += delta * (glm::vec3)comp;
		
	int index = getBlockIndex(toLookAt); 

	if (index < 0) return Blocks::AIR;
	for (auto& chunk : chunks) {
		if (chunk->position == chunkPositionToLookAt) {
			return chunk->blocks[index];
		}
	}
	return Blocks::AIR;
}
GLboolean Chunk::isNull() {
	return null;
}

Blocks Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::map<GLuint, Chunk*> chunks) {
	if (glm::all(glm::greaterThanEqual(inChunkPosition, glm::vec3(0)))) {
		if (glm::all(glm::lessThan(inChunkPosition, glm::vec3(CHUNK_SIZE)))) {
			return blocks[getBlockIndex(inChunkPosition)];
		}
	}
	glm::vec3 toLookAt = inChunkPosition;
	glm::vec3 chunkPositionToLookAt = position;

	glm::vec3 delta = inChunkPosition + glm::vec3(CHUNK_SIZE);
	glm::bvec3 comp = glm::lessThan(inChunkPosition, glm::vec3(0));
	delta *= comp;

	for (GLuint i = 0; i < 3; i++) {
		if (delta[i] != 0) {
			toLookAt[i] = delta[i];
		}
	}

	chunkPositionToLookAt -= glm::vec3(CHUNK_SIZE) * (glm::vec3)comp;

	auto t = glm::greaterThan(inChunkPosition, glm::vec3(CHUNK_SIZE - 1));
	comp = glm::not_(comp) && t;
	delta = glm::vec3(CHUNK_SIZE);
	toLookAt -= delta * (glm::vec3)comp;
	chunkPositionToLookAt += delta * (glm::vec3)comp;

	int index = getBlockIndex(toLookAt);

	if (index < 0) return Blocks::AIR;
	try {
		return chunks.at(hash(chunkPositionToLookAt))->blocks.at(index);
	}
	catch (std::exception e) {

	}
	/*for (auto& chunk : chunks) {
		if (chunk->position == chunkPositionToLookAt) {
			return chunk->blocks[index];
		}
	}*/
	return Blocks::AIR;
}

void Chunk::addBlock(Blocks block, std::vector<Chunk*>& chunks, glm::vec3& absPos, glm::vec3 xyz) {
	auto& pos = absPos;
	auto& x = xyz.x;
	auto& y = xyz.y;
	auto& z = xyz.z;
	GLuint tex_index = (GLuint)getTexture(block);
	if (getBlock_safe({ x - 1, y, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[LEFT], TEXTURES[tex_index], pos });
	}
	if (getBlock_safe({ x + 1, y, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[RIGHT], TEXTURES[tex_index], pos });
	}

	if (getBlock_safe({ x, y - 1, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[BOTTOM], TEXTURES[tex_index], pos });
	}
	if (getBlock_safe({ x, y + 1, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[TOP], TEXTURES[tex_index], pos });
	}

	if (getBlock_safe({ x, y, z - 1 }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[BACK], TEXTURES[tex_index], pos });
	}
	if (getBlock_safe({ x, y, z + 1 }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[FRONT], TEXTURES[tex_index], pos });
	}
}

void Chunk::editBlock(glm::vec3 pos, Blocks block, std::vector<Chunk*> chunks) {
	if (block == Blocks::AIR) {
		// break block
		std::vector<Face*> victims;
		for (GLuint i = 0; i < meshes.size(); i++) {
			auto& face = meshes[i];
			auto& p = std::get<2>(face);
			if (p == pos) {
				victims.push_back(&face);
			}
		}
		for (auto& victim : victims) {
			auto found = std::find(meshes.begin(), meshes.end(), *victim);
			meshes.erase(found);
		}

		// addBlock(block, chunks, pos - position, pos);
	}
	else {
		// place block

	}
}