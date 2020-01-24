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
						blockMesh.addFace(new FaceMesh(FACES[LEFT]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[LEFT]), pos);
				}
				if (x < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x + 1, y, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[RIGHT]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[RIGHT]), pos);
				}

				if (y > 0) {
					if (blocks[getBlockIndex({ x, y - 1, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[BOTTOM]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[BOTTOM]), pos);
				}
				if (y < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x, y + 1, z })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[TOP]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[TOP]), pos);
				}

				if (z > 0) {
					if (blocks[getBlockIndex({ x, y, z + 1 })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[BACK]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[BACK]), pos);
				}
				if (z < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x, y, z - 1 })] == 0) {
						blockMesh.addFace(new FaceMesh(FACES[FRONT]), pos);
					}
				}
				else {
					blockMesh.addFace(new FaceMesh(FACES[FRONT]), pos);
				}

				if (blockMesh.faces.size() < 1) continue;
				Mesh::FaceMesh mesh((GLboolean)false);
				if (blockMesh.faces.size() > 1) {
					// std::cout << "hi\n";
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
	// compressMesh();
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
	std::vector<std::vector<Mesh::FaceMesh>> sorted; 
	sorted.resize(CHUNK_SIZE);
	for (int i = 0; i < meshes.size(); i++) {
		auto& mesh = meshes[i];
		int index = (int)std::abs(mesh.position.y) - 1;
		sorted[index].push_back(mesh);
	}
	std::vector<Mesh::FaceMesh> res;
	for (auto& sort : sorted) {
		res.insert(res.end(), sort.begin(), sort.end());
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