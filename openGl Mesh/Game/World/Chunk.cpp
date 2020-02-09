#include "Chunk.h"
Chunk::Chunk() {
	isNull = GL_TRUE;
}
Chunk::Chunk(glm::vec3 pos, GLboolean create) : object((GLfloat)100.0f, { position, (GLfloat)1 }) {
	position = pos;
	object.setKinematic(GL_TRUE);
	object.setPhysical(GL_TRUE);
	object.setPosition(position);
	Physics::BoxCollider collider(position, 1);
	isNull = GL_FALSE;
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

				if (getBlock_safe({ x, y, z - 1 }, chunks) == 0) {
					meshes.push_back({ FACES[BACK], TEXTURES[GRASS], pos });
				}
				if (getBlock_safe({ x, y, z + 1 }, chunks) == 0) {
					meshes.push_back({ FACES[FRONT], TEXTURES[GRASS], pos });
				}
			}
		}
	}
}
GLboolean Chunk::checkCollision(Physics::Object& object) {
	/*if (!object.getPhysical()) return GL_FALSE;
	for (auto& mesh : meshes) {
		Physics::BoxCollider collider = mesh.getCollider();
		if (collider.checkCollision(&object)) {
			return GL_TRUE;
		}
	}*/
	return GL_FALSE;
	/*try {
		Physics::BoxCollider collider = this->object.getCollider();
		return collider.checkCollision(&object);
	}
	catch ( std::exception e)
	{

	}*/
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

std::vector<Face*> Chunk::getMeshes() {
	std::vector<Face*> res;
	for (auto& mesh : meshes) {
		res.push_back(&mesh);
	}
	return res;
}
GLuint Chunk::getBlock_unsafe(const glm::vec3 pos) {
	return blocks[getBlockIndex(pos)];
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
<<<<<<< HEAD
}
GLboolean Chunk::isNull() {
	return null;
}
GLboolean Chunk::checkCollision(Physics::Update& update) {
	std::vector<glm::vec3> diag = {
				glm::vec3(0), glm::vec3(1, 1, -1)
	};
	std::vector<glm::vec3> full = {
				glm::vec3(0, 0, -1), glm::vec3(1, 0, -1),
				glm::vec3(0, 1, -1), glm::vec3(1, 1, -1),
														
				glm::vec3(0, 1, 0), glm::vec3(1, 1, 0),
				glm::vec3(0, 0, 0), glm::vec3(1, 0, 0) 
	};
	auto translate = [](std::vector<glm::vec3> vertices, glm::vec3 translation) {
		for (auto& vertex : vertices) {
			vertex += translation;
		}
		return vertices;
	};
	for (auto& chunkFace : this->meshes) {
		std::vector<glm::vec3> cubeCorners = translate(diag, std::get<2>(chunkFace));
		for (auto& playerMesh : update.Vertices) {
			std::vector<glm::vec3> playerCorners = translate(full, update.PrevPosition);
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
=======
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
}