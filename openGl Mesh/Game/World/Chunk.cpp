#include "Chunk.h"
Chunk::Chunk() {
	isNull = GL_TRUE;
}
Chunk::Chunk(glm::vec3 pos) : object((GLfloat)100.0f, { position, (GLfloat)1 }) {
	position = pos;
	object.setKinematic(GL_TRUE);
	object.setPhysical(GL_TRUE);
	object.setPosition(position);
	Physics::BoxCollider collider(position, 1);
	isNull = GL_FALSE;
}
void Chunk::create() {
	blocks.fill(1);
	for (GLuint x = 0; x < CHUNK_SIZE; x++) {
		for (GLuint y = 0; y < CHUNK_SIZE; y++) {
			for (GLuint z = 0; z < CHUNK_SIZE; z++) {
				BlockMesh blockMesh;
				glm::vec3 pos;

				std::string num;
				std::stringstream temp;
				int numb = 0;
				if (x > 0) {
					pos = { x - 1, y, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::LEFT, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
					}
				}
				else {
					blockMesh.addFace(FACES::LEFT, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
				}
				if (x < CHUNK_SIZE - 1) {
					pos = { x + 1, y, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::RIGHT, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
					}
				}
				else {
					blockMesh.addFace(FACES::RIGHT, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
				}

				if (y > 0) {
					pos = { x, y - 1, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::BOTTOM, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
					}
				}
				else {
					blockMesh.addFace(FACES::BOTTOM, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
				}
				if (y < CHUNK_SIZE - 1) {
					pos = { x, y + 1, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::TOP, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
					}
				}
				else {
					blockMesh.addFace(FACES::TOP, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
				}

				if (z > 0) {
					pos = { x, y, z - 1 };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::BACK, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
					}
				}
				else {
					blockMesh.addFace(FACES::BACK, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
				}
				if (z < CHUNK_SIZE - 1) {
					pos = { x, y, z - 1 };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::FRONT, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
					}
				}
				else {

					blockMesh.addFace(FACES::FRONT, { x + position.x * 1, y + position.y * 1, z + position.z * 1 });
				}

				if (blockMesh.faces.size() < 1) continue;
				for (auto& face : blockMesh.faces) {
					face.setTexture("grass");
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
