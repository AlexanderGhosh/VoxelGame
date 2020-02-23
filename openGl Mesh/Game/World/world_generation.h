#pragma once
#include "constants.h"
#include "gtc/noise.hpp"
using ChunkHeightMap = std::array<std::array<GLubyte, CHUNK_SIZE>, CHUNK_SIZE>;

using BlockType = GLubyte; // 1 - physical 0 - air
using Height = GLushort;
using Column = std::vector<std::pair<BlockType, Height>>;
using HeightMap = std::array<std::array<Column, CHUNK_SIZE>, CHUNK_SIZE>;
class world_generation
{
public:
	static GLuint seed;
	static GLubyte octaves;
	static std::vector<glm::vec2> frequencies;
	static GLfloat redistribution;
	world_generation();
	world_generation(GLuint seed, GLubyte octaves, GLfloat redist, std::vector<glm::vec2> freqs = { glm::vec2(0.5f) });
	static GLubyte heightAtPositon(GLfloat x, GLfloat y);
	static GLubyte heightAtPositon(const glm::vec2 pos);
	static ChunkHeightMap genHeightMap(glm::vec3 chunkPos);
	static HeightMap createHeightMap(glm::vec3 chunkPos, GLboolean hasCaves = 0);
private:
	static GLfloat noise(glm::vec2 pos);
	static GLfloat noise(GLfloat x, GLfloat y);
};

