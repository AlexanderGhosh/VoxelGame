#pragma once
#include <chrono>
#include "../Player/Camera.h"
#include "constants.h"
#include "Physics.h"

#include <map>
using namespace Mesh;
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Chunk
{
public:
	glm::vec3 position;
	std::array<GLuint, CHUNK_VOLUME> blocks; // 0 - air, 1 - grass
	Chunk();
	Chunk(glm::vec3 pos, GLboolean create = false);
	void create();
	GLboolean checkCollision(Physics::Update& update);
	std::vector<Face*> getMeshes();
	void cleanUp();
	std::vector<std::pair<GLuint, GLuint>>& getCompressBlocks();
	std::vector<std::pair<Face, GLuint>>& getCompressMesh();
	void createMesh(std::vector<Chunk*> chunks = std::vector<Chunk*>());
	void createBlocks();
	GLuint getBlock_unsafe(glm::ivec3 pos);
	GLuint getBlock_safe(const glm::vec3 pos, std::vector<Chunk*> chunks);
	GLboolean isNull();
private:
	GLboolean null;
	std::vector<Face> meshes;
	Physics::Object object;
	std::vector<std::pair<GLuint, GLuint>> compressedBlocks;
	std::vector<std::pair<Face, GLuint>> compressedMesh;

	std::vector<std::pair<GLuint, GLuint>>& compressBlocks();
	std::vector<std::pair<Face, GLuint>>& compressMesh();
	void sortMesh();
};

