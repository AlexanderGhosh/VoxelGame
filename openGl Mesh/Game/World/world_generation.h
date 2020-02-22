#pragma once
#include "constants.h"
#include "gtc/noise.hpp"
using ChunkHeighMap = std::array<std::array<GLubyte, CHUNK_SIZE>, CHUNK_SIZE>;
class world_generation
{
public:
	GLuint seed;
	GLubyte octaves;
	std::vector<glm::vec2> frequencies;
	GLfloat redistribution;
	world_generation();
	world_generation(GLuint seed, GLubyte octaves, GLfloat redist, std::vector<glm::vec2> freqs = { glm::vec2(0.5f) });
	GLubyte heightAtPositon(GLfloat x, GLfloat y);
	GLubyte heightAtPositon(const glm::vec2 pos);
	ChunkHeighMap genHeightMap(glm::vec3 chunkPos);
private:
	GLfloat noise(glm::vec2 pos);
	GLfloat noise(GLfloat x, GLfloat y);
};

