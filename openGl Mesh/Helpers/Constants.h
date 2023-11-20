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

#define GENERATE_NEW_CHUNKS false

#define DEBUG_GRID_LINES


#define SSAO
#ifdef SSAO
constexpr unsigned int SSAO_NUM_SAMPLES = 64;
constexpr float SSAO_SCALE = 4;
constexpr float SSAO_RADIUS = 0.5;
constexpr float SSAO_BIAS = 0.025;
#endif // SSAO


constexpr bool VSYNC = false;
constexpr float PI = 3.14;
constexpr float HEIGHT = 720;
constexpr float WIDTH = 1280;
constexpr float ASPECT_RATIO = WIDTH / HEIGHT;
constexpr float FOV = 45;
constexpr float NEAR_PLANE = 0.1;
constexpr float FAR_PLANE = 200;

constexpr float SHADOW_MAP_SIZE = 4098;

constexpr unsigned int CHUNK_SAMPLES = 4;
constexpr unsigned int RENDER_DISTANCE = 20;
constexpr unsigned int PLAYER_REACH = 5;
constexpr unsigned int WORLD_HEIGHT = 256;
constexpr unsigned int PARTS_PER_CHUNK = 16;
constexpr unsigned unsigned int WATER_LEVEL = 20;

constexpr unsigned int CHUNK_SIZE = 16;
constexpr float CHUNK_SIZE_F = CHUNK_SIZE;
constexpr float CHUNK_SIZE_INV = 1.f / CHUNK_SIZE_F;

constexpr float PLAYER_SPEED = 5;
constexpr unsigned int SEED = 32;

constexpr float VOXEL_SZIE = 0.5;
constexpr float HALF_VOXEL_SZIE = VOXEL_SZIE * .5f;
constexpr glm::vec3 WORLD_ORIGIN(0, (VOXEL_SZIE - 1) * -30.f / VOXEL_SZIE, 0);

constexpr std::array<glm::vec3, 6> OFFSETS_3D = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0)
};
#ifdef DEBUG_GRID_LINES
constexpr std::array<glm::vec3, 4> GRID_LINE_POSITIONS{
	glm::vec3(-HALF_VOXEL_SZIE),
	glm::vec3(VOXEL_SZIE * (CHUNK_SIZE - .5f), -HALF_VOXEL_SZIE, -HALF_VOXEL_SZIE),
	glm::vec3(-HALF_VOXEL_SZIE),
	glm::vec3(-HALF_VOXEL_SZIE, -HALF_VOXEL_SZIE, VOXEL_SZIE * (CHUNK_SIZE - .5f))
};
#endif // DEBUG_GRID_LINES

constexpr glm::vec3 LIGHT_POSITION(100.f);

enum class Block : unsigned char;

using Chunks = std::unordered_map<glm::vec2, ChunkColumn>;
using WorldMap = std::unordered_map<glm::vec2, BlockStore>;
using HeightMap = std::array<BlocksEncoded, CHUNK_SIZE* CHUNK_SIZE>;


extern std::vector<Texture> TEXTURES3D;
extern std::vector<Texture> TEXTURES2D;
extern std::vector<Shader> SHADERS;
extern std::vector<BlockDetails> BLOCK_DETAILS;

//enum FACES_NAMES : unsigned char {
//	FRONT,
//	BACK,
//	LEFT,
//	RIGHT,
//	TOP,
//	BOTTOM,
//	NULL_
//};

enum class Texture_Names : unsigned char {
	SKYBOX
};

enum class Texture_Names_2D : unsigned char {
	GUI,
	BOARDER_NORM,
	BOARDER_SELECTED,
	LIVE_HEART,
	DEAD_HEART
};

enum SHADER_NAMES : unsigned char {
	SKYBOX,
	GUI,
	GLYPH,

	OIT_TRANSPARENT,
	OIT_COMPOSITE,
	SCREEN_QUAD,
	SHADOW,
	MODEL,
	GBUFFER,
	DEFFERED,
	AO,
	BLUR,
	// GIZMOS
	POINT_GIZMO,
	LINE_GIZMO,
	CIRCLE_GIZMO,
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