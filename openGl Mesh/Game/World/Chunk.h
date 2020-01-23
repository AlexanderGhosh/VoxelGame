#pragma once
#include <sstream>
#include "../Player/Camera.h"
#include "constants.h"
#include "Physics.h"
#include "../../Mesh.h"
using namespace Mesh;
class Chunk
{
public:
	glm::vec3 position;
	std::array<GLuint, CHUNK_VOLUME> blocks; // 0 - air, 1 - grass
	Chunk();
	Chunk(glm::vec3 pos, GLboolean create = false);
	void create();
	GLboolean checkCollision(Physics::Object& object);
	std::vector<Mesh::FaceMesh>& getMeshes() {
		return meshes;
	}
private:
	GLboolean isNull;
	std::vector<Mesh::FaceMesh> meshes;
	Physics::Object object;
};
GLuint getBlockIndex(glm::vec3 position);

