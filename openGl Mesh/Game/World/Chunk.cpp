#include "Chunk.h"
Chunk::Chunk() {
	null = GL_TRUE;
}
Chunk::Chunk(glm::vec3 pos, GLboolean create) : object((GLfloat)100.0f, { position, (GLfloat)1 }) {
	position = pos;
	object.setKinematic(GL_TRUE);
	object.setPhysical(GL_TRUE);
	object.setPosition(position);
	Physics::Collider collider(position, 1);
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
	blocks.fill(1);
}
void Chunk::createMesh(std::vector<Chunk*> chunks) {
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				glm::vec3 pos = glm::vec3(x, y, z) + position;
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
	// sortMesh();
	// compressBlocks();
	// compressMesh();
}
void Chunk::cleanUp() {
	for (auto& face : meshes) {
		face = {};
	}
}

std::vector<std::pair<GLuint, GLuint>>& Chunk::compressBlocks() {
	unsigned int prevBlock = blocks.front();
	unsigned int count = 1;
	for (auto& block : blocks) {
		if (prevBlock == block) {
			count++;
		}
		else {
			compressedBlocks.push_back({ block, count });
			prevBlock = block;
		}
	}
	compressedBlocks.push_back({ prevBlock, --count });
	return compressedBlocks;
}
std::vector<std::pair<Face, GLuint>>& Chunk::compressMesh() {
	Face prevMesh = meshes.front();
	unsigned int count = 1;
	for (auto& mesh : meshes) {
		if (std::get<0>(prevMesh) == std::get<0>(mesh)) {
			count++;
		}
		else {
			compressedMesh.push_back({ mesh, count });
			prevMesh = mesh;
		}
	}
	compressedMesh.push_back({ prevMesh, --count });
	return compressedMesh;
}
std::vector<std::pair<GLuint, GLuint>>& Chunk::getCompressBlocks() {
	return compressedBlocks;
}
std::vector<std::pair<Face, GLuint>>& Chunk::getCompressMesh() {
	return compressedMesh;
}
/*void Chunk::sortMesh() {
	std::map<int, std::vector<FaceMesh>> seperated;
	for (auto& mesh : meshes) {
		int location = 0;
		for (auto& buffer : mesh.comboOf) {
			location += (int)buffer;
		}
		if (!location) {
			location = (int)mesh.getBuffer();
		}
		try {
			seperated[location].push_back(mesh);
		}
		catch (std::exception e) {
			seperated.insert({ location, { mesh } });
		}
	}
	std::vector<FaceMesh> res;
	for (auto& sep : seperated) {
		res.insert(res.end(), sep.second.begin(), sep.second.end());
	}
	meshes = res;
}*/

std::vector<Face*> Chunk::getMeshes() {
	std::vector<Face*> res;
	for (auto& mesh : meshes) {
		res.push_back(&mesh);
	}
	return res;
}
GLuint Chunk::getBlock_unsafe(glm::ivec3 pos) {
	while (pos.x%CHUNK_SIZE != 0) {
		pos -= glm::ivec3(CHUNK_SIZE);
	}
	try {
		return blocks[getBlockIndex(pos)];
	}
	catch (std::exception e) {
		return 0;
	}
}
GLuint Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks) {
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
GLboolean Chunk::checkCollision(Physics::Update& update) {
	for (auto& chunkFace : this->meshes) {
		std::array<glm::vec3, 2> cubeCorners = {
				glm::vec3(0) + std::get<2>(chunkFace), glm::vec3(1, 1, -1) + std::get<2>(chunkFace)
		};
		for (auto& playerMesh : update.Extra) {
			auto& meshPos = update.Data;
			std::array<glm::vec3, 8> playerCorners = {
				glm::vec3(0, 0, -1) + meshPos, glm::vec3(1, 0, -1) + meshPos,
				glm::vec3(0, 1, -1) + meshPos, glm::vec3(1, 1, -1) + meshPos,

				glm::vec3(0, 1, 0) + meshPos, glm::vec3(1, 1, 0) + meshPos,
				glm::vec3(0, 0, 0) + meshPos, glm::vec3(1, 0, 0) + meshPos
			};
			for (auto& meshVertex : playerCorners) {
				if (meshVertex.x <= cubeCorners[1].x && meshVertex.x >= cubeCorners[0].x) {
					if (meshVertex.y <= cubeCorners[1].y && meshVertex.y >= cubeCorners[0].y) {
						if (meshVertex.z >= cubeCorners[1].z && meshVertex.z <= cubeCorners[0].z) {
							return GL_TRUE;
						}
					}
				}
			}
		}
	}
	return GL_FALSE;
}