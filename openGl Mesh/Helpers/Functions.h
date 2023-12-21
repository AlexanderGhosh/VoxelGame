#pragma once
#include <glm.hpp>
#include <string>
#include <unordered_map>
#include <list>
#include "BlockDetails.h"

class Texture;

enum class Texture_Names : unsigned char;
enum class Block : unsigned char;

inline unsigned int getMaterialIndex(unsigned int block) {
	return block -= 2;
}

inline unsigned int getMaterialIndex(Block block) {
	return getMaterialIndex((unsigned int)block);
}

bool rayCubeIntersection(const glm::vec3& ro, const glm::vec3& rd, const glm::vec3& lp, const glm::vec3& up);

float radiusOfCube(const glm::vec3& scale, const glm::vec3& dir);

float randRange(float lower, float upper);
int randInt(int lower, int upper);

float bilinearInterpolation(const float xLen, const float yLen, const float& c00, const float& c01, const float& c10, const float& c11);


void markSlot(unsigned char& val, const unsigned int slot);

// returns a vec3 with a one in the arg max slot and 0 in the rest
glm::vec3 argmax(const glm::vec3& a);
// returns a vec3 with a one in the arg max slot and 0 in the rest
// abs(a) prior to argmax
glm::vec3 argmax_abs(const glm::vec3& a);

const float radians(const float d);
const float degrees(const float r);

const unsigned int columnIndex(const unsigned int x, const unsigned int z);

void reduceToMultiple(glm::ivec3& victim, const unsigned int multiple);
void reduceToMultiple(glm::ivec2& victim, const unsigned int multiple);

/// <summary>
/// Reduces each element of victim to a multiple of 16
/// </summary>
/// <returns></returns>
glm::vec3 reduceToMultiple(const glm::vec3& victim);


unsigned int toIndex(const Texture_Names tex);
unsigned int toIndex(const Block block);

const Block toBlock(const unsigned char number);
const BlockDetails& getDetails(const Block block);

// returns str split at the delimeter
std::vector<std::string> split(const std::string& str, const std::string& delim);

// true if a contains b otherwise false
bool contains(const std::string& a, const std::string& b);

// replaces all occorances of from with to in str
std::string replaceAll(std::string str, std::string from, std::string to);

glm::vec3 operator+(const glm::vec3& p1, const glm::vec2& p2);
//glm::vec3 operator+(const glm::vec2& p1, const glm::vec3& p2);
glm::vec3 operator-(const glm::vec3& p1, const glm::vec2& p2);
//glm::vec3 operator-(const glm::vec2& p1, const glm::vec3& p2);

/// <summary>
/// Will return the value associated with the key if avaliable WILL NOT CREATE NEW KEYS
/// null if not found
/// </summary>
template<typename Key, typename Value>
inline Value* getValue(std::unordered_map<Key, Value>& map, const Key& key)
{
	const size_t size = map.size();
	Value& val = map[key];
	if (map.size() > size) {
		map.erase(key);
		return nullptr;
	}
	return &val;
}