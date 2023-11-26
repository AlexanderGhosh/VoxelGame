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
class Material;

#define GENERATE_NEW_CHUNKS false

#define RRC(x) (float(x) / 255.f)

// #define DEBUG_GRID_LINES


#define SSAO
#ifdef SSAO
constexpr unsigned int SSAO_NUM_SAMPLES = 64;
constexpr float SSAO_SCALE = 4;
constexpr float SSAO_RADIUS = 0.5;
constexpr float SSAO_BIAS = 0.025;
#endif // SSAO


constexpr bool VSYNC = true;
constexpr float PI = 3.14;
constexpr float HEIGHT = 720;
constexpr float WIDTH = 1280;
constexpr float ASPECT_RATIO = WIDTH / HEIGHT;
constexpr float FOV = 45;
constexpr float NEAR_PLANE = 0.1;
constexpr float FAR_PLANE = 200;

constexpr float SHADOW_MAP_SIZE = 4098;

constexpr unsigned int RENDER_DISTANCE = 4;
constexpr unsigned int WORLD_HEIGHT = 256;
constexpr unsigned unsigned int WATER_LEVEL = 20;
constexpr unsigned unsigned int SNOW_LEVEL = 38;

constexpr unsigned int CHUNK_SIZE = 16;
constexpr float CHUNK_SIZE_F = CHUNK_SIZE;
constexpr float CHUNK_SIZE_INV = 1.f / CHUNK_SIZE_F;
constexpr unsigned int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;


constexpr float PLAYER_SPEED = 5;

constexpr float VOXEL_SZIE = 0.5;
constexpr float HALF_VOXEL_SZIE = VOXEL_SZIE * .5f;
constexpr glm::vec3 WORLD_ORIGIN(0, (VOXEL_SZIE - 1) * -30.f / VOXEL_SZIE, 0);

// used to convert from world space to 'noise space' (noise space is the space used in the noise generator)
constexpr float NOISE_FACTOR = CHUNK_SIZE_INV * VOXEL_SZIE;

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

constexpr glm::vec3 LIGHT_POSITION(0, 1000, -1000);

enum class Block : unsigned char;

using Chunks = std::unordered_map<glm::vec2, ChunkColumn>;
using WorldMap = std::unordered_map<glm::vec2, BlockStore>;
using HeightMap = std::array<BlocksEncoded, CHUNK_AREA>;


extern std::vector<Texture> TEXTURES3D;
extern std::vector<Texture> TEXTURES2D;
extern std::vector<Shader> SHADERS;
extern std::vector<BlockDetails> BLOCK_DETAILS;
extern std::vector<Material> MATERIALS;

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
	ERROR,
	GRASS,
	DIRT,
	STONE,
	SAND,
	WATER,
	SNOW,

	AIR,
	SIZE
};