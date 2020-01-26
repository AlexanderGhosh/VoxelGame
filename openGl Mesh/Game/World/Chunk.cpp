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
	blocks.fill(1);
	for (GLuint x = 0; x < CHUNK_SIZE; x++) {
		for (GLuint y = 0; y < CHUNK_SIZE; y++) {
			for (GLuint z = 0; z < CHUNK_SIZE; z++) {
				BlockMesh blockMesh;
				glm::vec3 pos = glm::vec3(x, y, z) + position;
				if (x > 0) {
					if (blocks[getBlockIndex({ x - 1, y, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[LEFT], TEXTURES[GRASS]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[LEFT], TEXTURES[GRASS]), pos);
				}
				if (x < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x + 1, y, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[RIGHT], TEXTURES[GRASS]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[RIGHT], TEXTURES[GRASS]), pos);
				}

				if (y > 0) {
					if (blocks[getBlockIndex({ x, y - 1, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[BOTTOM], TEXTURES[GRASS]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[BOTTOM], TEXTURES[GRASS]), pos);
				}
				if (y < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x, y + 1, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[TOP], TEXTURES[GRASS]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[TOP], TEXTURES[GRASS]), pos);
				}

				if (z > 0) {
					if (blocks[getBlockIndex({ x, y, z + 1 })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[BACK], TEXTURES[GRASS]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[BACK], TEXTURES[GRASS]), pos);
				}
				if (z < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x, y, z - 1 })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[FRONT], TEXTURES[GRASS]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[FRONT], TEXTURES[GRASS]), pos);
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
	/*std::vector<Mesh::FaceMesh*> res;
	for (auto& face : meshes) {
		res.push_back(&face);
	}*/
	return meshes;
}