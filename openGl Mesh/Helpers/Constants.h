#pragma once
#include <ext/vector_float3.hpp>
#include <ext/vector_float2.hpp>
#include <gtx/hash.hpp>
#include <vector>
#include <array>
#include <unordered_map>
#include "BlocksEncoded.h"

class Texture;
class Shader;
class BlockDetails;
class ChunkColumn;
class BlockStore;
class Material;
class Block;

// forces the terrain to be flat
// #define FLAT_TERRAIN

// insted of manuly declaring render distance this calculates it based of a desired terrain radius in real units
// #define RENDER_DIST_FROM_DISTANCE

// #define TERRAIN_HAS_COLLIDER
// #define PHYSICS_DEBUG_RENDERER

// if defeined chunkcs are generated in circles
//#define CIRCULAR_CHUNKS
// only works on the intal generation (all chunks when running will be async)
//#define GENERATE_CHUNKS_ASYNC 
#define GENERATE_NEW_CHUNKS true
// #define CELLULAR_AUTOMOTA
// #define GENERATE_INDEX_DATA_GREEDY
#define MINIMAL_GREEDY_MESH

//#define RENDER_WIREFRAMES


// when defined chunk generation and voxel model loading will be perfored with the GreedyMeshing (at least the collider on models will use it) 40ms BAD MEMORY
//#define ALWAYS_USE_GREEDY_MESH
// when defined chunk generation will be perfored with the noise map 4ms
 
#define ALWAYS_USE_NOISE_MESH
// when defined chunk generation will be perfored with the BlockStore 40ms
 
//#define ALWAYS_USE_SLOW_MESH


// these gates prevent a clash in mesh generation methods
#ifdef ALWAYS_USE_GREEDY_MESH
#undef ALWAYS_USE_NOISE_MESH
#undef ALWAYS_USE_SLOW_MESH
#endif
#ifdef ALWAYS_USE_NOISE_MESH
#undef ALWAYS_USE_GREEDY_MESH
#undef ALWAYS_USE_SLOW_MESH
#endif
#ifdef ALWAYS_USE_SLOW_MESH
#undef ALWAYS_USE_GREEDY_MESH
#undef ALWAYS_USE_NOISE_MESH
#endif

//converts 0-256 colour to 0-1
#define RRC(x) (float(x) / 255.f)

// #define DEBUG_GRID_LINES


#define SSAO
#ifdef SSAO
constexpr unsigned int SSAO_NUM_SAMPLES = 64;
constexpr float SSAO_SCALE = 4;
constexpr float SSAO_RADIUS = 0.5;
constexpr float SSAO_BIAS = 0.025;
#endif // SSAO

constexpr float FIXED_DELTA_TIME = 1.f / 60.f;
constexpr bool VSYNC = true;
constexpr float PI = 3.14;
constexpr float HEIGHT = 720;
constexpr float WIDTH = 1280;
constexpr float ASPECT_RATIO = WIDTH / HEIGHT;
constexpr float FOV = 45;
constexpr float NEAR_PLANE = 0.1;
constexpr float FAR_PLANE = 1000;
constexpr float SHADOW_FAR_PLANE = 100;

constexpr float SHADOW_MAP_SIZE = 4098;

constexpr float VOXEL_SIZE = 1;

constexpr unsigned int CHUNK_SIZE = 16;
constexpr float CHUNK_SIZE_F = CHUNK_SIZE;

#ifdef RENDER_DIST_FROM_DISTANCE
constexpr float RADIUS_DISTANCE = 16;

constexpr unsigned int RENDER_DISTANCE = RADIUS_DISTANCE / (VOXEL_SIZE * CHUNK_SIZE_F);
#else
constexpr unsigned int RENDER_DISTANCE = 5;
#endif

constexpr unsigned int WORLD_HEIGHT = 50;
constexpr unsigned unsigned int WATER_LEVEL = 20;
constexpr unsigned unsigned int SNOW_LEVEL = 50;

constexpr float CHUNK_SIZE_INV = 1.f / CHUNK_SIZE_F;
constexpr unsigned int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr unsigned int CHUNK_VOLUME = CHUNK_AREA * WORLD_HEIGHT;

constexpr unsigned int CHUNK_SIZE_PADDED = CHUNK_SIZE + 2;
constexpr unsigned int CHUNK_AREA_PADDED = CHUNK_SIZE_PADDED * CHUNK_SIZE_PADDED;


constexpr float PLAYER_SPEED = 1;

constexpr float VOXEL_SIZE_INV = 1.f / VOXEL_SIZE;
constexpr float HALF_VOXEL_SIZE = VOXEL_SIZE * .5f;
constexpr glm::vec3 WORLD_ORIGIN(0, 0, 0);

constexpr float CHUNK_SIZE_SCALED = CHUNK_SIZE_F * VOXEL_SIZE;

// used to convert from world space to 'noise space' (noise space is the space used in the noise generator)
constexpr float NOISE_FACTOR = CHUNK_SIZE_INV * VOXEL_SIZE * 0.1;

constexpr glm::vec3 GRAVITY(0, 0, 0);

constexpr std::array<glm::vec3, 6> OFFSETS_3D = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0)
};
constexpr std::array<glm::vec2, 4> OFFSETS_2D = {
		glm::vec2(0, 1),
		glm::vec2(0, -1),

		glm::vec2(-1, 0),
		glm::vec2(1, 0)
};
#ifdef DEBUG_GRID_LINES
constexpr std::array<glm::vec3, 4> GRID_LINE_POSITIONS{
	glm::vec3(-HALF_VOXEL_SIZE),
	glm::vec3(VOXEL_SIZE * (CHUNK_SIZE - .5f), -HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE),
	glm::vec3(-HALF_VOXEL_SIZE),
	glm::vec3(-HALF_VOXEL_SIZE, -HALF_VOXEL_SIZE, VOXEL_SIZE * (CHUNK_SIZE - .5f))
};
#endif // DEBUG_GRID_LINES

constexpr glm::vec3 LIGHT_POSITION(0, 1000, -1000);

// maps local unscaled pos to the chunk
using Chunks = std::unordered_map<glm::vec2, ChunkColumn>;
// maps local unscaled pos to the respective blockstore
using WorldMap = std::unordered_map<glm::vec2, BlockStore>;
using HeightMap = std::array<BlocksEncoded, CHUNK_AREA>;


extern std::vector<Texture> TEXTURES3D;
extern std::vector<Texture> TEXTURES2D;
extern std::vector<Shader> SHADERS;

extern std::vector<Block> BLOCKS;
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
	PHYS_DEBUG,

	GREEDY
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

//enum class Block : unsigned char {
//	ERROR,
//
//	GRASS,
//	DIRT,
//	STONE,
//	SAND,
//	WATER,
//	SNOW,
//
//	GRAVEL,
//	AIR,
//};