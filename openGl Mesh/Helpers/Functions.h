#pragma once
#include <glm.hpp>
#include <string>
#include "BlockDetails.h"

class Texture;

enum class Texture_Names : unsigned char;
enum class Block : unsigned char;


const float radians(const float d);
const float degrees(const float r);

const unsigned int columnIndex(const unsigned int x, const unsigned int z);

void reduceToMultiple(glm::ivec3& victim, const unsigned int multiple);
void reduceToMultiple(glm::ivec2& victim, const unsigned int multiple);
int reduceToMultiple(float victim, const unsigned int multiple);
const Texture_Names getTexture(const Block block);
const Texture& getBlockTexture(const Block block);

unsigned int toIndex(const Texture_Names tex);
unsigned int toIndex(const Block block);

const Block toBlock(const unsigned char number);
const std::string getName(const Block block);
glm::vec3 getTranslation(const glm::mat4& matrix);
glm::mat4 translate(const glm::mat4& mat, glm::vec3 vec);
const BlockDetails& getDetails(const Block block);
const BlockDetails& getDetails(const Texture* tex);

const Block toBlock(const std::string& name);


glm::vec3 operator+(const glm::vec3& p1, const glm::vec2& p2);
glm::vec3 operator+(const glm::vec2& p1, const glm::vec3& p2);
glm::vec3 operator-(const glm::vec3& p1, const glm::vec2& p2);
glm::vec3 operator-(const glm::vec2& p1, const glm::vec3& p2);