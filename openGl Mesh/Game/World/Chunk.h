#pragma once
#include <sstream>
#include "../Player/Camera.h"
#include "constants.h"
#include "Physics.h"
#include "../../Mesh.h"
#include <map>
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
	std::vector<Mesh::FaceMesh>& getMeshes();
	void cleanUp();
	std::vector<std::pair<GLuint, GLuint>>& getCompressBlocks();
	std::vector<std::pair<Mesh::FaceMesh, GLuint>>& getCompressMesh();
private:
	GLboolean isNull;
	std::vector<Mesh::FaceMesh> meshes;
	Physics::Object object;
	std::vector<std::pair<GLuint, GLuint>> compressedBlocks;
	std::vector<std::pair<Mesh::FaceMesh, GLuint>> compressedMesh;

	std::vector<std::pair<GLuint, GLuint>>& compressBlocks();
	std::vector<std::pair<Mesh::FaceMesh, GLuint>>& compressMesh();
	void sortMesh();
};

