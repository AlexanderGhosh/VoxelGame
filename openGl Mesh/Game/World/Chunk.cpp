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
	}
}
void Chunk::create() {
	blocks.fill(1);
	for (GLuint x = 0; x < CHUNK_SIZE; x++) {
		for (GLuint y = 0; y < CHUNK_SIZE; y++) {
			for (GLuint z = 0; z < CHUNK_SIZE; z++) {
				BlockMesh blockMesh;

				if (x > 0) {
					if (blocks[getBlockIndex({ x - 1, y, z })] == 0) {
						blockMesh.addFace(FaceMesh(FACES[LEFT]), glm::vec3(x, y, z) + position);
					}
				}
				else {
					blockMesh.addFace(FaceMesh(FACES[LEFT]), glm::vec3(x, y, z) + position);
				}
				if (x < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x + 1, y, z })] == 0) {
						blockMesh.addFace(FaceMesh(FACES[RIGHT]), glm::vec3(x, y, z) + position);
					}
				}
				else {
					blockMesh.addFace(FaceMesh(FACES[RIGHT]), glm::vec3(x, y, z) + position);
				}

				if (y > 0) {
					if (blocks[getBlockIndex({ x, y - 1, z })] == 0) {
						blockMesh.addFace(FaceMesh(FACES[BOTTOM]), glm::vec3(x, y, z) + position);
					}
				}
				else {
					blockMesh.addFace(FaceMesh(FACES[BOTTOM]), glm::vec3(x, y, z) + position);
				}
				if (y < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x, y + 1, z })] == 0) {
						blockMesh.addFace(FaceMesh(FACES[TOP]), glm::vec3(x, y, z) + position);
					}
				}
				else {
					blockMesh.addFace(FaceMesh(FACES[TOP]), glm::vec3(x, y, z) + position);
				}

				if (z > 0) {
					if (blocks[getBlockIndex({ x, y, z - 1 })] == 0) {
						blockMesh.addFace(FaceMesh(FACES[BACK]), glm::vec3(x, y, z) + position);
					}
				}
				else {
					blockMesh.addFace(FaceMesh(FACES[BACK]), glm::vec3(x, y, z) + position);
				}
				if (z < CHUNK_SIZE - 1) {
					if (blocks[getBlockIndex({ x, y, z - 1 })] == 0) {
						blockMesh.addFace(FaceMesh(FACES[FRONT]), glm::vec3(x, y, z) + position);
					}
				}
				else {
					blockMesh.addFace(FaceMesh(FACES[FRONT]), glm::vec3(x, y, z) + position);
				}

				if (blockMesh.faces.size() < 1) continue;
				for (auto& face : blockMesh.faces) {
					// face.setTexture("grass");
					meshes.push_back(face);
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
GLuint getBlockIndex(glm::vec3 position) {
	GLuint index;
	std::stringstream(std::to_string(position.x) + std::to_string(position.y) + std::to_string(position.z)) >> index;
	return index;
}