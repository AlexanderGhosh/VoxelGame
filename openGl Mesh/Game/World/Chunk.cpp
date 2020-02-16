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
	createBlocks();
	createMesh();
}
void Chunk::createBlocks() {
	for (auto& b : blocks) {
		GLfloat num = std::rand() / GLfloat(RAND_MAX);
		if (num > 0.6f) {
			b = 1;
		}
		else {
			b = 0;
		}
	}
	//if (position.y >= -CHUNK_SIZE) {
	//	for (GLushort x = 0; x < CHUNK_SIZE; x++) {
	//		for (GLushort z = 0; z < CHUNK_SIZE; z++) {		
	//			for (GLushort y = 8; y < 16; y++) {
	//				blocks[getBlockIndex({ x, y, z })] = 1;
	//			}
	//			/*GLushort noise = std::rand() % CHUNK_SIZE;
	//			if (noise > CHUNK_SIZE) noise = CHUNK_SIZE;
	//			for (GLushort i = 0; i < noise; i++) {
	//				blocks[getBlockIndex({ x, i, z })] = 1;
	//			}*/
	//		}
	//		
	//	}
	//}
	//else {
	//	blocks.fill(1);
	//}
}
void Chunk::createMesh(std::vector<Chunk*> chunks) {
	auto addblock = [](FACES_NAMES face, GLshort blockType, glm::vec3 position, std::vector<Face>& meshes) {
		meshes.push_back({ FACES[face], TEXTURES[blockType], position });
	};
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				glm::vec3 pos = glm::vec3(x, y, z) + position;
				if (getBlock_safe({ x, y, z }, chunks) == 0)
					continue;
				if (getBlock_safe({ x - 1, y, z }, chunks) == 0) {
					meshes.push_back({ FACES[LEFT], TEXTURES[GRASS], pos });
				}
				if (getBlock_safe({ x + 1, y, z }, chunks) == 0) {
					meshes.push_back({ FACES[RIGHT], TEXTURES[GRASS], pos });
				}

				if (getBlock_safe({ x, y - 1, z }, chunks) == 0) {
					meshes.push_back({ FACES[BOTTOM], TEXTURES[GRASS], pos });
				}
				if (getBlock_safe({ x, y + 1, z }, chunks) == 0) {
					meshes.push_back({ FACES[TOP], TEXTURES[GRASS], pos });
				}

				if (getBlock_safe({ x, y, z - 1}, chunks) == 0) {
					meshes.push_back({ FACES[BACK], TEXTURES[GRASS], pos });
				}
				if (getBlock_safe({ x, y, z + 1}, chunks) == 0) {
					meshes.push_back({ FACES[FRONT], TEXTURES[GRASS], pos });
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
