#include "Chunk.h"
Chunk::Chunk() {
	null = GL_TRUE;
}
Chunk::Chunk(glm::vec3 pos, GLboolean create) {
	position = pos;
	null = GL_FALSE;
	if (create) {
		this->create();
		// this->sortMesh();
	}
}
void Chunk::create() {
	createBlocks(true);
	createMesh();
}
void Chunk::createBlocks(GLboolean isFlat) {
	
	if (isFlat) {
		if (position.y < -CHUNK_SIZE) {
			blocks.fill(1);
		}
		else {
			blocks.fill(1);
		}
		if (position == glm::vec3(0, -2, 0) || position == glm::vec3(-2, -2, - 2)) {
			blocks[getBlockIndex({ 0, -1, 0 })] = 0;
			blocks[getBlockIndex({ 1, -1, 0 })] = 0;
			blocks[getBlockIndex({ 1, -1, 1 })] = 0;
			blocks[getBlockIndex({ 0, -1, 1 })] = 0;
		}

		return;
	}

	// random blocks
	/*for (int x = 0; x < blocks.size(); x++) {
		if ((float)rand()/(float)RAND_MAX > 0.5f) {
			blocks[x] = 1;
		}
		else {
			blocks[x] = 5;
		}
	}*/

	GLfloat xFactor = 1 / (CHUNK_SIZE - 1);
	GLfloat yFactor = 1 / (CHUNK_SIZE - 1);
	GLfloat freq(0.25);
	if (position.y >= -CHUNK_SIZE) {
		blocks.fill(0);
		for (GLushort x = 0; x < CHUNK_SIZE; x++) {
			for (GLushort z = 0; z < CHUNK_SIZE; z++) {
				GLfloat sum = 0;
				glm::vec2 n((GLfloat)x / (GLfloat)CHUNK_SIZE - 0.5, (GLfloat)z / (GLfloat)CHUNK_SIZE - 0.5);
				n += (position.x + position.y + position.z) / 3.0f;
				sum = glm::perlin(n) + 0.5 * glm::perlin(2.0f * n) + 0.25 * glm::perlin(4.0f * n);
				GLushort height = std::abs(sum) * CHUNK_SIZE;
				height = height > CHUNK_SIZE ? CHUNK_SIZE : height;
				height = height == 0 ? 1 : height;
				for (GLushort y = 0; y < height; y++) {
					GLushort block = 1; 
					/*if ((float)rand() / (float)RAND_MAX > 0.5f) {
						blocks[getBlockIndex({ x, y, z })] = 1;
					}
					else {
						blocks[getBlockIndex({ x, y, z })] = 5;
					}*/
					if (y < height - 2) {
						block = 5; // stone
					}
					else if (y < height - 1) {
						block = 6; // dirt
					}
					blocks[getBlockIndex({ x, y, z })] = block;
				}
			}
		}
	}
	else {
		blocks.fill(5);
	}
}
void Chunk::createMesh(std::vector<Chunk*> chunks) {
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				glm::vec3 pos = glm::vec3(x, y, z) + position;
				GLuint block = getBlock_safe({ x, y, z }, chunks);
				if (block == 0)
					continue;
				block--;
				
				if (getBlock_safe({ x - 1, y, z }, chunks) == 0) {
					meshes.push_back({ FACES[LEFT], TEXTURES[block], pos });
				}
				if (getBlock_safe({ x + 1, y, z }, chunks) == 0) {
					meshes.push_back({ FACES[RIGHT], TEXTURES[block], pos });
				}

				if (getBlock_safe({ x, y - 1, z }, chunks) == 0) {
					meshes.push_back({ FACES[BOTTOM], TEXTURES[block], pos });
				}
				if (getBlock_safe({ x, y + 1, z }, chunks) == 0) {
					meshes.push_back({ FACES[TOP], TEXTURES[block], pos });
				}

				if (getBlock_safe({ x, y, z - 1}, chunks) == 0) {
					meshes.push_back({ FACES[BACK], TEXTURES[block], pos });
				}
				if (getBlock_safe({ x, y, z + 1}, chunks) == 0) {
					meshes.push_back({ FACES[FRONT], TEXTURES[block], pos });
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
GLushort Chunk::getBlock_unsafe(glm::ivec3 pos) {
	auto p = pos;
	while (pos.x%CHUNK_SIZE != 0 && !(std::abs(pos.x) > CHUNK_SIZE || std::abs(pos.x) < CHUNK_SIZE)) {
		pos -= glm::ivec3(CHUNK_SIZE);
	}
	try {
		return blocks.at(getBlockIndex(pos));
	}
	catch (std::exception e) {
		return 0;
	}
}
GLushort Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks) {
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

	if (index < 0) return 0;
	for (auto& chunk : chunks) {
		if (chunk->position == chunkPositionToLookAt) {
			return chunk->blocks[index];
		}
	}
	return 0;
}
GLboolean Chunk::isNull() {
	return null;
}
