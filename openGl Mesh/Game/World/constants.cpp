#include "constants.h"
#include "../../Helpers/BlockDetails.h"
#include "../../Textures/Texture.h"
#include "../../Shaders/Shader.h"

std::vector<Texture> TEXTURES = {
	Texture("grass", ""),
	Texture("player/bottom", ""),
	Texture("player/top", ""),
	Texture("skybox", ""),
	Texture("stone", ""),
	Texture("dirt", ""),
	Texture("water", ""),
	Texture("error", ""),
	Texture("vampire/bottom", ""), // logs
	Texture("leaf", ""),
	Texture("vampire/bottom", ""),
	Texture("vampire/top", ""),
	Texture("sand", "")
};
std::vector<Texture> TEXTURES2D = {
	Texture("crosshair", ""),
	Texture("boarders/normal", ""),
	Texture("boarders/selected", ""),
	Texture("hearts/live_heart", ""),
	Texture("hearts/dead_heart", "")
};
std::vector<Shader> SHADERS = {
	Shader("block2"),
	Shader("block3"),
	Shader("skybox"),
	Shader("crosshair"),
	Shader("glyph"),
	Shader("ray"),
	Shader("depth"),
	Shader("GeomBlocks/geom", true),
	Shader("OIT/Opaque/opaque", true),
	Shader("OIT/Transparent/transparent", true),
	Shader("OIT/Composite/composite"),
	Shader("ScreenQuad/screenQuad"),
};
std::vector<BlockDetails> BLOCK_DETAILS = {};

const std::vector<Material> MATERIALS = {
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(0), 0 }, // grass
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // player bottom
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // player top
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // skybox
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(1), 32 }, // stone
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(0), 0 }, // dirt
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(1), 64 }, // water
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // error
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(0), 0 }, // log
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(1), 64 }, // leaf
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // vampire bottom
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // vampire top
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(0), 0 }, // sand
};
glm::vec3 LIGHTPOSITION = glm::vec3(-8, 80, -8);
glm::vec3 LIGHTPOSITIONOrigin = glm::vec3(-8, 80, -8);

void reduceToMultiple(glm::ivec3& victim, unsigned int multiple, const char* overload) {
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
void reduceToMultiple(glm::ivec2& victim, unsigned int multiple, const char* overload) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0) {
		victim.y -= 1;
	}
}
glm::ivec3 reduceToMultiple(glm::ivec3 victim, unsigned int multiple) {
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
int reduceToMultiple(float victim, unsigned int multiple) {
	victim = std::round(victim);
	while ((int)victim % multiple != 0) {
		victim--;
	}
	return victim;
}
const Texture_Names getTexture(Block block) {
	Texture_Names res = Texture_Names::ERROR;
	switch (block)
	{
	case Block::AIR:
		break;
	case Block::GRASS:
		res = Texture_Names::GRASS;
		break;
	case Block::DIRT:
		res = Texture_Names::DIRT;
		break;
	case Block::STONE:
		res = Texture_Names::STONE;
		break;
	case Block::WATER:
		res = Texture_Names::WATER;
		break;
	case Block::LOG:
		res = Texture_Names::LOG;
		break;
	case Block::LEAF:
		res = Texture_Names::LEAF;
		break;
	case Block::SAND:
		res = Texture_Names::SAND;
		break;
	}
	if (res == Texture_Names::ERROR) {
		// std::cout << "Error Texture created\n";
	}
	return res;
}
const Texture& getBlockTexture(Block block) {
	return BLOCK_DETAILS[(unsigned int) block].Tex;
}
unsigned int toIndex(Texture_Names tex) {
	return (unsigned int) tex;
}
unsigned int toIndex(Block block) {
	return (unsigned int) block;
}
Block toBlock(unsigned char number) {
	return Block(number);
}

const std::string getName(Block block) {
	std::string name("null");
	switch (block)
	{
	case Block::AIR:
		name = "air";
		break;
	case Block::GRASS:
		name = "grass";
		break;
	case Block::DIRT:
		name = "dirt";
		break;
	case Block::STONE:
		name = "stone";
		break;
	case Block::WATER:
		name = "water";
		break;
	case Block::LOG:
		name = "log";
		break;
	case Block::LEAF:
		name = "leaf";
		break;
	case Block::SAND:
		name = "sand";
		break;
	}
	return name;
}

glm::vec3 getTranslation(glm::mat4 matrix) {
	return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

const BlockDetails& getDetails(Block block) {
	return BLOCK_DETAILS[(unsigned int) block];
}

const BlockDetails& getDetails(Texture* tex) {
	for (auto& b : BLOCK_DETAILS) {
		if (tex->getName().find(b.Name) != std::string::npos) {
			return b;
		}
	}
	return BLOCK_DETAILS[(unsigned int) Block::AIR];
}

glm::mat4 translate(glm::mat4 mat, glm::vec3 vec) {
	for (unsigned char i = 0; i < 3; i++) {
		mat[3][i] = vec[i];
	}
	return mat;
}
void translate(glm::mat4& mat, glm::vec3 vec) {
	for (unsigned char i = 0; i < 3; i++) {
		mat[3][i] = vec[i];
	}
}

Block itemToBlock(Item item)
{
	switch (item)
	{
	case 0:
		return Block::AIR;
	case 1:
		return Block::GRASS;
	case 2:
		return Block::DIRT;
	case 3:
		return Block::STONE; 
	case 4:
		return Block::WATER;
	case 5:
		return Block::ERROR;
	case 6:
		return Block::LOG;
	case 7:
		return Block::LEAF;
	case 8:
		return Block::SAND;
	}
}
Block toBlock(std::string name) {
	for (unsigned int i = 0; i < BLOCK_DETAILS.size(); i++) {
		if (name == BLOCK_DETAILS[i].Name) {
			return (Block) i;
		}
	}
	return Block::ERROR;
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
