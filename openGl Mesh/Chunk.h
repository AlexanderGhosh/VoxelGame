#pragma once
#define CHUNK_SIZE 32
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE
#include "Mesh.h"
#include "Camera.h"
#include <sstream>
class Chunk
{
public:
	glm::vec3 position;
	std::array<GLuint, CHUNK_VOLUME> blocks; // 0 - air, 1 - grass
	Chunk(glm::vec3 pos);
	void create();
	void render(Camera p1, glm::mat4 projection);
	ChunkMesh getMesh() {
		return mesh;
	}
private:
	ChunkMesh mesh;
};

