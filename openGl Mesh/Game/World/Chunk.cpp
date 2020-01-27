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
		this->sortMesh();
	}
}
void Chunk::create() {
	createBlocks();
	createMesh();
}
void Chunk::createBlocks() {
	blocks.fill(1);
}
void Chunk::createMesh(std::vector<Chunk> chunks) {
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				BlockMesh blockMesh;
				glm::vec3 pos = glm::vec3(x, y, z) + position;

				if (getBlock_safe({ x - 1, y, z }, chunks) == 0) {
					blockMesh.addFace(new FaceMesh(FACES[LEFT], TEXTURES[GRASS]), pos);
				}
				if (getBlock_safe({ x + 1, y, z }, chunks) == 0) {
					blockMesh.addFace(new FaceMesh(FACES[RIGHT], TEXTURES[GRASS]), pos);
				}

				if (getBlock_safe({ x, y - 1, z }, chunks) == 0) {
					blockMesh.addFace(new FaceMesh(FACES[BOTTOM], TEXTURES[GRASS]), pos);
				}
				if (getBlock_safe({ x, y + 1, z }, chunks) == 0) {
					blockMesh.addFace(new FaceMesh(FACES[TOP], TEXTURES[GRASS]), pos);
				}

				if (getBlock_safe({ x, y, z + 1}, chunks) == 0) {
					blockMesh.addFace(new FaceMesh(FACES[FRONT], TEXTURES[GRASS]), pos);
				}
				if (getBlock_safe({ x, y, z - 1}, chunks) == 0) {
					blockMesh.addFace(new FaceMesh(FACES[BACK], TEXTURES[GRASS]), pos);
				}

				if (blockMesh.faces.size() < 1) continue;
				Mesh::FaceMesh mesh((GLboolean)false);
				if (blockMesh.faces.size() > 1) {
					for (auto& m : blockMesh.faces) {
						mesh += *m;
					}
					meshes.push_back(mesh);
					}
				else {
					meshes.push_back(*blockMesh.faces[0]);
				}
			}
		}
	}
	sortMesh();
	compressBlocks();
	compressMesh();
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
		face.cleanUp();
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
std::vector<std::pair<Mesh::FaceMesh, GLuint>>& Chunk::compressMesh() {
	Mesh::FaceMesh prevMesh = meshes.front();
	unsigned int count = 1;
	for (auto& mesh : meshes) {
		if (prevMesh.getBuffer() == mesh.getBuffer()) {
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
std::vector<std::pair<Mesh::FaceMesh, GLuint>>& Chunk::getCompressMesh() {
	return compressedMesh;
}
void Chunk::sortMesh() {
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
}

std::vector<Mesh::FaceMesh>& Chunk::getMeshes() {
	return meshes;
}
GLuint Chunk::getBlock_unsafe(const glm::vec3 pos) {
	return blocks[getBlockIndex(pos)];
}
GLuint Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk> chunks) {
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
		if (chunk.position == chunkPositionToLookAt) {
			return chunk.blocks[index];
		}
	}
	return 0;
}