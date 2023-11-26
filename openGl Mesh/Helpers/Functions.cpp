#include "Functions.h"
#include "Constants.h"


float randRange(float lower, float upper) {
	float v = rand();
	v /= RAND_MAX;
	float range = upper - lower;
	v *= range;
	v += lower;
	return v;
}

int randInt(int lower, int upper) {
	int v = randRange(lower, upper);
	return v;
}

float bilinearInterpolation(const float x, const float y, const float& c00, const float& c01, const float& c10, const float& c11)
{
	/*float  a = c00 * (1 - xLen) + c10 * xLen;
	float  b = c01 * (1 - xLen) + c11 * xLen;
	return a - yLen + b * yLen;*/
	float dx = 1.f - x;
	float dy = 1.f - y;
	float w00 = dx * dy;
	float w01 = dx * y;
	float w10 = x * dy;
	float w11 = x * y;

	return w00 * c00 + w10 * c10 + w01 * c01 + w11 * c11;
}

std::list<std::string> splitString(std::string str, const std::string& delimiter)
{
	size_t pos = 0;
	std::string token;
	std::list<std::string> res;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		res.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	res.push_back(str);
	return res;
}

void markSlot(unsigned char& val, const unsigned int slot)
{
	val |= 1u << slot;
}

glm::vec3 argmax(const glm::vec3& a)
{
	glm::vec3 res(0);
	float max = fmaxf(fmaxf(a.x, a.y), a.z);
	for (unsigned char i = 0; i < 3; i++) {
		res[i] = max == a[i] ? 1 : 0;
	}
	return res;
}

glm::vec3 argmax_abs(const glm::vec3& a)
{
	return argmax(glm::abs(a));
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

glm::vec3 reduceToMultiple(const glm::vec3& victim)
{
	glm::ivec3 res = victim;
	reduceToMultiple(res, CHUNK_SIZE);
	return res;
}

glm::vec2 reduceToMultiple(const glm::vec2& victim)
{
	glm::ivec2 res = victim;
	reduceToMultiple(res, CHUNK_SIZE);
	return res;
}

int reduceToMultiple(float victim, const unsigned int multiple) {
	victim = std::round(victim);
	while ((int)victim % multiple != 0) {
		victim--;
	}
	return victim;
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
	case Block::SAND:
		name = "sand";
		break;
	}
	return name;
}

glm::vec3 getTranslation(const glm::mat4& matrix) {
	return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

const BlockDetails& getDetails(Block block) {
	return BLOCK_DETAILS[(unsigned int)block];
}

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

bool contains(const std::string& a, const std::string& b) {
	const short s = b.size();
	for (short i = 0; i < a.size() - b.size(); i++) {
		const std::string& sub = a.substr(i, s);
		if (sub == b) {
			return true;
		}
	}
	return false;
}

std::string replaceAll(std::string str, std::string from, std::string to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

glm::vec3 operator+(const glm::vec3& p1, const glm::vec2& p2)
{
	return p1 + glm::vec3(p2.x, 0, p2.y);
}

glm::vec3 operator-(const glm::vec3& p1, const glm::vec2& p2)
{
	return p1 - glm::vec3(p2.x, 0, p2.y);
}
