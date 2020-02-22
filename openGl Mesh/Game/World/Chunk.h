#pragma once
#include <chrono>
#include <gtc/noise.hpp>
#include "../Player/Camera.h"
#include "constants.h"

#include <map>
using namespace Mesh;
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
using HeightMapChunk = std::array<std::array<GLubyte, CHUNK_SIZE>, CHUNK_SIZE>;
using HeightMapWorld = std::array<std::pair<HeightMapChunk, GLuint>, INITALL_VIEW_AREA>;
class Chunk
{
public:
	glm::vec3 position;
	std::array<GLushort, CHUNK_VOLUME> blocks; // 0 - air, 1 - grass
	Chunk();
	Chunk(glm::vec3 pos, GLboolean create = false);
	void create();
	std::vector<Face*> getMeshes();
	void cleanUp();
	void createMesh(std::vector<Chunk*> chunks = std::vector<Chunk*>());
	void createBlocks(GLboolean isFlat, GLubyte block);
	void createBlocks(HeightMapChunk hightMap);
	GLushort getBlock_unsafe(glm::ivec3 pos);
	GLushort getBlock_safe(const glm::vec3 pos, std::vector<Chunk*> chunks);
	GLboolean isNull();
private:
	GLboolean null;
	std::vector<Face> meshes;
};

