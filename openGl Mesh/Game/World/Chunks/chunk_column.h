#pragma once
#include <fstream>
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
	chunk_column(std::string fileName);
	~chunk_column();
	Faces getMesh(GLboolean reGen =0);
	void addChunk(Chunk chunk, GLboolean sort = 0);
	void addChunks(std::vector<Chunk> chunks, GLboolean sort = 0);
	void createMesh(std::vector<chunk_column*> columns);
	void createMesh(std::vector<Chunk*> chunks, GLboolean create = 1);
	ChunkPosition& getPosition();
	Blocks& getBlock(glm::ivec3 blockPos);
	Chunk* getSubchunk_unsafe(GLint yPos);
	void save(std::string name, GLuint seed);
	void destroy();
	std::vector<Chunk*> getSubChunkPointers();
	void editBlock(glm::vec3 pos, GLboolean is_abs, Blocks block, std::vector<Chunk*> subs);
	bool operator ==(chunk_column chunk2);
	bool operator !=(chunk_column chunk2);
	bool operator ==(glm::vec2 pos2);
	bool operator !=(glm::vec2 pos2);
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
struct Savable {
	GLfloat x, z;
	GLuint seed;
};
struct Tuple {
	GLubyte bufferType;
	GLubyte textureMap;
	GLfloat x, y, z;
	Face toFace();
};

