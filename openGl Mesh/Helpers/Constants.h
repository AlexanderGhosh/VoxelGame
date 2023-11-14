#pragma once
#include <glm.hpp>
#include <gtx/hash.hpp>
#include <vector>
#include <array>
#include <unordered_map>
#include "BlocksEncoded.h"
#include "BlockDetails.h"

class Texture;
class Shader;
class BlockDetails;
class ChunkColumn;
class BlockStore;

constexpr float PI = 3.14;
constexpr float HEIGHT = 720;
constexpr float WIDTH = 1280;
constexpr float ASPECT_RATIO = WIDTH / HEIGHT;
constexpr float FOV = 45;
constexpr float NEAR_PLANE = 0.1;
constexpr float FAR_PLANE = 500;
constexpr float SHADOW_DISTANCE = 1000;
constexpr float SHADOW_MAP_SIZE = 4098;

constexpr unsigned int RENDER_DISTANCE = 4;
constexpr unsigned int PLAYER_REACH = 5;
constexpr unsigned int WORLD_HEIGHT = 256;
constexpr unsigned int WANDER_RANGE = 15;
constexpr unsigned int PARTS_PER_CHUNK = 16;
constexpr unsigned unsigned int WATER_LEVEL = 20;

constexpr unsigned int CHUNK_SIZE = 16;
constexpr float CHUNK_SIZE_F = CHUNK_SIZE;
constexpr float CHUNK_SIZE_INV = 1.f / CHUNK_SIZE_F;

constexpr float GRAVITY = 10;
constexpr float PLAYER_SPEED = 5;
constexpr unsigned int SEED = 32;

constexpr float VOXEL_SZIE = 1;

constexpr std::array<glm::vec3, 6> OFFSETS_3D = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0)
};

extern glm::vec3 LIGHTPOSITION;
extern glm::vec3 LIGHTPOSITIONOrigin;

enum class Block : unsigned char;

using Chunks = std::unordered_map<glm::vec2, ChunkColumn>;
using WorldMap = std::unordered_map<glm::vec2, BlockStore>;
using Item = unsigned char;
using HeightMap = std::array<BlocksEncoded, CHUNK_SIZE* CHUNK_SIZE>;


extern std::vector<Texture> TEXTURES;
extern std::vector<Texture> TEXTURES2D;
extern std::vector<Shader> SHADERS;
extern std::vector<BlockDetails> BLOCK_DETAILS;

enum class Texture_Names : unsigned char {
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

enum class Texture_Names_2D : unsigned char {
	GUI,
	BOARDER_NORM,
	BOARDER_SELECTED,
	LIVE_HEART,
	DEAD_HEART
};

enum SHADER_NAMES : unsigned char {
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
	SCREEN_QUAD,
	SHADOW,
	MODEL,
	GBUFFER,
	DEFFERED
};

enum class Move_Dir : unsigned char {
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

enum FACES_NAMES : unsigned char {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	NULL_
};