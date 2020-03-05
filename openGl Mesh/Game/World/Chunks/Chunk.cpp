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
	createMesh({});
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
				if (block == Blocks::AIR)
					continue;
				Texture_Names tex = getTexture(block);
				if (getBlock_safe({ x - 1, y, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[LEFT], TEXTURES[(int)tex], pos });
				}
				if (getBlock_safe({ x + 1, y, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[RIGHT], TEXTURES[(int)tex], pos });
				}

				if (getBlock_safe({ x, y - 1, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[BOTTOM], TEXTURES[(int)tex], pos });
				}
				if (getBlock_safe({ x, y + 1, z }, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[TOP], TEXTURES[(int)tex], pos });
				}

				if (getBlock_safe({ x, y, z - 1}, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[BACK], TEXTURES[(int)tex], pos });
				}
				if (getBlock_safe({ x, y, z + 1}, chunks) == Blocks::AIR) {
					meshes.push_back({ FACES[FRONT], TEXTURES[(int)tex], pos });
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
