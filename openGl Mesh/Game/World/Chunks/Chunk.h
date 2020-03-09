#pragma once
#include <chrono>
#include <gtc/noise.hpp>
#include "../../Player/Camera.h"
#include "../constants.h"

#include <map>
using namespace Mesh;
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
using HeightMapChunk = std::array<std::array<GLubyte, CHUNK_SIZE>, CHUNK_SIZE>;
using HeightMapWorld = std::array<std::pair<HeightMapChunk, GLuint>, INITALL_VIEW_AREA>;
class Chunk
{
public:
	glm::vec3 position;
	std::array<Blocks, CHUNK_VOLUME> blocks; // 0 - air, 1 - grass
	Chunk();
	Chunk(glm::vec3 pos, GLboolean create = false);
	void create();
	std::vector<Face*> getMeshes();
	void cleanUp(); 
	void createMesh(std::vector<Chunk*> chunks);
	void createBlocksFlat(Blocks block);
	void createBlocks(HeightMapChunk hightMap);
	Blocks getBlock_unsafe(glm::ivec3 pos);
	Blocks& getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks);
	GLboolean isNull();
	void editBlock(glm::vec3 pos, Blocks block, std::vector<Chunk*> chunks);
	std::vector<Face*> getPointerMesh();
private:
	GLboolean null;
	std::vector<Face> meshes;
	void addBlock(Blocks block, std::vector<Chunk*>& chunks, glm::vec3 absPos, GLboolean skip = 0);
	Blocks& getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks, Chunk*& chunk_);
};

