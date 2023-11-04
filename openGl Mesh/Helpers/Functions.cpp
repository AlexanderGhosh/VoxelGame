#include "Functions.h"
#include "Constants.h"


void markSlot(unsigned int& val, const unsigned int slot)
{
	val |= 1u << slot;
}

const float radians(const float d) {
	return (d * PI) / 180.f;
}

const float degrees(const float r) {
	return (180.f * r) / PI;
}

const unsigned int columnIndex(const unsigned int x, const unsigned int z) {
	return x + CHUNK_SIZE * z;
}

void reduceToMultiple(glm::ivec3& victim, const unsigned int multiple) {
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

void reduceToMultiple(glm::ivec2& victim, const unsigned int multiple) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0) {
		victim.y -= 1;
	}
}

int reduceToMultiple(float victim, const unsigned int multiple) {
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
	return BLOCK_DETAILS[(unsigned int)block].Tex;
}

unsigned int toIndex(Texture_Names tex) {
	return (unsigned int)tex;
}

unsigned int toIndex(Block block) {
	return (unsigned int)block;
}

const Block toBlock(const unsigned char number) {
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

glm::vec3 getTranslation(const glm::mat4& matrix) {
	return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

glm::mat4 translate(glm::mat4 mat, glm::vec3 vec) {
	for (unsigned char i = 0; i < 3; i++) {
		mat[3][i] = vec[i];
	}
	return mat;
}

const BlockDetails& getDetails(Block block) {
	return BLOCK_DETAILS[(unsigned int)block];
}

const BlockDetails& getDetails(Texture* tex) {
	for (auto& b : BLOCK_DETAILS) {
		if (tex->getName().find(b.Name) != std::string::npos) {
			return b;
		}
	}
	return BLOCK_DETAILS[(unsigned int)Block::AIR];
}

const Block toBlock(const std::string& name) {
	for (unsigned int i = 0; i < BLOCK_DETAILS.size(); i++) {
		if (name == BLOCK_DETAILS[i].Name) {
			return (Block)i;
		}
	}
	return Block::ERROR;
}

glm::vec3 operator+(const glm::vec3& p1, const glm::vec2& p2)
{
	return p1 + glm::vec3(p2.x, 0, p2.y);
}

glm::vec3 operator+(const glm::vec2& p1, const glm::vec3& p2)
{
	return p2 + glm::vec3(p1.x, 0, p1.y);
}

glm::vec3 operator-(const glm::vec3& p1, const glm::vec2& p2)
{
	return p1 - glm::vec3(p2.x, 0, p2.y);
}

glm::vec3 operator-(const glm::vec2& p1, const glm::vec3& p2)
{
	return glm::vec3(p1.x, 0, p1.y) - p2;
}
