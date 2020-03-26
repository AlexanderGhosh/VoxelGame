#include "constants.h"
const std::array<Buffer*, 6> FACES = {
	FaceMesh({ glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5) },   // FRONT
	{ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) },
	{ glm::vec3(-1, -1, 1), glm::vec3(-1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5) },  // BACK
	{ glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1) },
	{ glm::vec3(-1, 1, -1), glm::vec3(-1), glm::vec3(1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5), glm::vec3(-0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5, 0.5, 0.5) },  // LEFT
	{ glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0) },
	{ glm::vec3(1, -1, -1), glm::vec3(1, -1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1) }).getBuffer(),

	FaceMesh({ glm::vec3(0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5) },     // RIGHT
	{ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) },
	{ glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5) },	  // TOP
	{ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) },
	{ glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1), glm::vec3(-1, -1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5),glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5) },   // BOTTOM
	{ glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0) },
	{ glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1), glm::vec3(1), glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1) }).getBuffer()
}; 
const std::vector<Texture*>TEXTURES = {
	new Texture("grass/hd", ""),
	new Texture("player/bottom", ""),
	new Texture("player/top", ""),
	new Texture("skybox", ""),
	new Texture("stone", ""),
	new Texture("dirt", ""),
	new Texture("water", ""),
	new Texture("error", "")
};
const std::vector<Shader*>SHADERS = {
	new Shader("block2"),
	new Shader("block3"),
	new Shader("skybox"),
	new Shader("crosshair"),
	new Shader("glyph"),
	new Shader("ray")
};
std::map<Blocks, BlockDet> BLOCKS = {
	{ Blocks::GRASS, {} },
	{ Blocks::DIRT, {} },
	{ Blocks::STONE, {} },
	{ Blocks::WATER, {} }
};
const std::vector<Blocks> AllBlocks = {
	Blocks::AIR,
	Blocks::GRASS,
	Blocks::DIRT,
	Blocks::STONE,
	Blocks::WATER
};
GLint getBlockIndex(glm::vec3 position) {
	int index = position.x + position.z * CHUNK_SIZE + position.y * CHUNK_AREA;
	if (index >= CHUNK_VOLUME) index *= -1;
	return index;
}

void reduceToMultiple(glm::ivec3& victim, GLuint multiple, const char* overload) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
}
void reduceToMultiple(glm::ivec2& victim, GLuint multiple, const char* overload) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0) {
		victim.y -= 1;
	}
}
glm::ivec3 reduceToMultiple(glm::ivec3 victim, GLuint multiple) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
	return victim;
}
GLint reduceToMultiple(GLfloat victim, GLuint multiple) {
	victim = std::round(victim);
	while ((GLint)victim % multiple != 0) {
		victim--;
	}
	return victim;
}
Texture_Names getTexture(Blocks block) {
	Texture_Names res = Texture_Names::ERROR;
	switch (block)
	{
	case Blocks::AIR:
		break;
	case Blocks::GRASS:
		res = Texture_Names::GRASS;
		break;
	case Blocks::DIRT:
		res = Texture_Names::DIRT;
		break;
	case Blocks::STONE:
		res = Texture_Names::STONE;
		break;
	case Blocks::WATER:
		res = Texture_Names::WATER;
		break;
	}
	return res;
}
Texture* getBlockTexture(Blocks block) {
	return BLOCKS[block].Tex;
}
GLubyte toIndex(Texture_Names tex) {
	return GLubyte(tex);
}
GLubyte toIndex(Blocks block) {
	return GLubyte(block);
}
Blocks toBlock(GLubyte number) {
	return Blocks(number);
}
std::string getName(Blocks block) {
	std::string name("null");
	switch (block)
	{
	case Blocks::AIR:
		name = "air";
		break;
	case Blocks::GRASS:
		name = "grass";
		break;
	case Blocks::DIRT:
		name = "dirt";
		break;
	case Blocks::STONE:
		name = "stone";
		break;
	case Blocks::WATER:
		name = "water";
		break;
	}
	return name;
}
glm::vec3 getTranslation(glm::mat4 matrix) {
	return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

glm::vec3 operator+(glm::vec3 p1, glm::vec2 p2)
{
	return p1 + glm::vec3(p2.x, 0, p2.y);
}

glm::vec3 operator+(glm::vec2 p1, glm::vec3 p2)
{
	return p2 + glm::vec3(p1.x, 0, p1.y);
}

glm::vec3 operator-(glm::vec3 p1, glm::vec2 p2)
{
	return p1 - glm::vec3(p2.x, 0, p2.y);
}
glm::vec3 operator-(glm::vec2 p1, glm::vec3 p2)
{
	return glm::vec3(p1.x, 0, p1.y) - p2;
}
