#pragma once
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

#include "../../Helpers/BlocksEncoded.h"

class Texture;
class Shader;
class ChunkColumn;
class BlockStore;
class BlockDetails;

constexpr unsigned int RENDER_DISTANCE = 5;
constexpr unsigned int PLAYER_REACH = 5;
constexpr unsigned int WORLD_HEIGHT = 256;
constexpr unsigned int WANDER_RANGE = 15;
constexpr unsigned int PARTS_PER_CHUNK = 16;

constexpr unsigned int CHUNK_SIZE = 16;

constexpr float GRAVITY = 10;
constexpr float PLAYER_SPEED = 5;
constexpr unsigned int SEED = 32;

constexpr float VOXEL_SZIE = 1;

extern glm::vec3 LIGHTPOSITION;
extern glm::vec3 LIGHTPOSITIONOrigin;

enum class Block : uint8_t;

using Chunks = std::vector<ChunkColumn>;
using WorldMap = std::unordered_map<glm::vec2, BlockStore>;
using Item = unsigned int;


// new
using HeightMap = std::array<BlocksEncoded, CHUNK_SIZE * CHUNK_SIZE>;


enum class Texture_Names {
	GRASS,
	PLAYER_BOTTOM,
	PLAYER_TOP,
	SKYBOX,
	STONE,
	DIRT,
	WATER,
	ERROR,
	LOG,
	LEAF,
	VAMPIRE_BOTTOM,
	VAMPIRE_TOP,
	SAND
};
enum class Texture_Names_2D {
	GUI,
	BOARDER_NORM,
	BOARDER_SELECTED,
	LIVE_HEART,
	DEAD_HEART
};
enum SHADER_NAMES {
	BLOCK2,
	BLOCK3,
	SKYBOX,
	GUI,
	GLYPH,
	RAY,
	DEPTH,
	GEOMBLOCKS,

	OIT_OPAQUE,
	OIT_TRANSPARENT,
	OIT_COMPOSITE,
	SCREEN_QUAD
};
enum class Move_Dir {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	Null
};
enum class Block : unsigned char {
	AIR,
	ERROR,
	GRASS,
	DIRT,
	STONE,
	WATER,
	LOG,
	LEAF,
	SAND,
	SIZE
};

enum FACES_NAMES {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	NULL_
};

extern std::vector<Texture> TEXTURES;
extern std::vector<Texture> TEXTURES2D;
extern std::vector<Shader> SHADERS;
extern std::vector<BlockDetails> BLOCK_DETAILS;



inline unsigned int columnIndex(unsigned int x, unsigned int z) {
	return z * CHUNK_SIZE + x;
}

void reduceToMultiple(glm::ivec3& victim, unsigned int multiple, const char* overload);
void reduceToMultiple(glm::ivec2& victim, unsigned int multiple, const char* overload);
glm::ivec3 reduceToMultiple(glm::ivec3 victim, unsigned int multiple);
int reduceToMultiple(float victim, unsigned int multiple);
const Texture_Names getTexture(const Block block);
const Texture& getBlockTexture(const Block block);

unsigned int toIndex(Texture_Names tex);
unsigned int toIndex(Block block);

const Block toBlock(const unsigned int number);
const std::string getName(Block block);
glm::vec3 getTranslation(glm::mat4 matrix);
glm::mat4 translate(glm::mat4 mat, glm::vec3 vec);
void translate(glm::mat4& mat, glm::vec3 vec);
const BlockDetails& getDetails(Block block);
const BlockDetails& getDetails(Texture* tex);

Block itemToBlock(Item item);
Block toBlock(std::string name);


glm::vec3 operator+(glm::vec3 p1, glm::vec2 p2);
glm::vec3 operator+(glm::vec2 p1, glm::vec3 p2);
glm::vec3 operator-(glm::vec3 p1, glm::vec2 p2);
glm::vec3 operator-(glm::vec2 p1, glm::vec3 p2);

