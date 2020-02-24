#pragma once
#include "Chunk.h"
#include "../world_generation.h"
using ChunkPosition = glm::vec2;
using Faces = std::vector<Face*>;
class chunk_column
{
public:
	std::vector<Chunk> chunks; // move to private


	chunk_column();
	chunk_column(GLboolean gen);
	chunk_column(ChunkPosition pos, GLboolean isFlat = 0);
	chunk_column(ChunkPosition pos, ChunkHeightMap hm);
	Faces getMesh();
	void addChunk(Chunk chunk, GLboolean sort = 0);
	void addChunks(std::vector<Chunk> chunks, GLboolean sort = 0);
	void createMesh(std::vector<chunk_column*> columns);
	ChunkPosition& getPosition();
	GLubyte& getBlock(glm::ivec3 blockPos);
private:
	ChunkPosition pos;
	ChunkHeightMap heightMap;
	Faces faces;
	GLboolean hasCaves;
	void genHeightMap(GLboolean isFlat);
	void applyHeightMap();
	void sortChunks();
	glm::vec3 fromIndex(GLushort index);
	std::pair<GLushort, GLushort> getHeightRange();
	void createChunks();
};

