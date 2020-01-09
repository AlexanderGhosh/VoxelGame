#include "Chunk.h"
Chunk::Chunk(glm::vec3 pos) {
	position = pos;
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
						blockMesh.addFace(FACES::LEFT, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
					}
				}
				else {
					blockMesh.addFace(FACES::LEFT, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
				}
				if (x < CHUNK_SIZE - 1) {
					pos = { x + 1, y, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::RIGHT, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
					}
				}
				else {
					blockMesh.addFace(FACES::RIGHT, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
				}

				if (y > 0) {
					pos = { x, y - 1, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::BOTTOM, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
					}
				}
				else {
					blockMesh.addFace(FACES::BOTTOM, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
				}
				if (y < CHUNK_SIZE - 1) {
					pos = { x, y + 1, z };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::TOP, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
					}
				}
				else {
					blockMesh.addFace(FACES::TOP, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
				}

				if (z > 0) {
					pos = { x, y, z - 1 };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::BACK, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
					}
				}
				else {
					blockMesh.addFace(FACES::BACK, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
				}
				if (z < CHUNK_SIZE - 1) {
					pos = { x, y, z - 1 };
					num = std::to_string(pos.x) + std::to_string(pos.y) + std::to_string(pos.z);
					temp = std::stringstream(num);
					temp >> numb;
					if (blocks[numb] == 0) {
						blockMesh.addFace(FACES::FRONT, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
					}
				}
				else {

					blockMesh.addFace(FACES::FRONT, true, { x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, z + position.z * CHUNK_SIZE });
				}

				mesh.faces.insert(mesh.faces.end(), blockMesh.faces.begin(), blockMesh.faces.end());
			}
		}
	}
}
