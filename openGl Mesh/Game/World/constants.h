#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#include "../../Shaders/Shader.h"
#include "../../Textures/Texture.h"

class ChunkColumn;
class BlockStore;
class Buffer;

#define RENDER_DISTANCE 3
#define PLAYER_REACH 5
#define WORLD_HEIGHT 256
#define WANDER_RANGE 15
#define PARTS_PER_CHUNK 16

#define CHUNK_SIZE 16

#define GRAVITY 10
#define PLAYER_SPEED 5.0
constexpr unsigned int SEED = 32;

extern glm::vec3 LIGHTPOSITION;
extern glm::vec3 LIGHTPOSITIONOrigin;

enum class Blocks : uint8_t;

using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
using Faces = std::vector<Face>;
using Faces_P = std::vector<Face*>;
using Block_Count = std::pair<Blocks, unsigned int>;
using HeightMap = std::array<std::array<std::vector<Block_Count>, CHUNK_SIZE>, CHUNK_SIZE>;
using Chunks = std::vector<ChunkColumn>;
using FaceB = std::tuple<Buffer, Texture*, unsigned int>;
using FaceB_p = std::tuple<Buffer*, Texture*, std::vector<glm::mat4>>;
using AdjacentMap = std::unordered_map<glm::vec2, ChunkColumn>;
using WorldMap = std::unordered_map<glm::vec2, const BlockStore*>;
using Item = unsigned int;

using AdjacentMap_p = std::unordered_map<glm::vec2, ChunkColumn*>;

struct BlockDet {
	std::string Name;
	Texture* Tex;
	Texture ItemTex;
	bool isTransparant;
};
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
	CROSSHAIR,
	BOARDER_NORM,
	BOARDER_SELECTED,
	LIVE_HEART,
	DEAD_HEART
};
enum SHADER_NAMES {
	BLOCK2,
	BLOCK3,
	SKYBOX,
	CROSSHAIR,
	GLYPH,
	RAY,
	DEPTH,
	GEOMBLOCKS
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
enum class Blocks : uint8_t {
	AIR,
	ERROR,
	GRASS,
	DIRT,
	STONE,
	WATER,
	LOG,
	LEAF,
	SAND
};
extern const std::array<Buffer*, 6> FACES;
extern const std::vector<Texture*> TEXTURES;
extern const std::vector<Texture*> TEXTURES2D;
extern const std::vector<Shader*> SHADERS;
extern std::map<Blocks, BlockDet> BLOCKS;
extern const std::vector<Material> MATERIALS;

void reduceToMultiple(glm::ivec3& victim, unsigned int multiple, const char* overload);
void reduceToMultiple(glm::ivec2& victim, unsigned int multiple, const char* overload);
glm::ivec3 reduceToMultiple(glm::ivec3 victim, unsigned int multiple);
int reduceToMultiple(float victim, unsigned int multiple);
Texture_Names getTexture(Blocks block);
Texture* getBlockTexture(Blocks block);
GLubyte toIndex(Texture_Names tex);
GLubyte toIndex(Blocks block);
Blocks toBlock(GLubyte number);
std::string getName(Blocks block);
glm::vec3 getTranslation(glm::mat4 matrix);
glm::mat4 translate(glm::mat4 mat, glm::vec3 vec);
void translate(glm::mat4& mat, glm::vec3 vec);
BlockDet& getDets(Blocks block);
BlockDet& getDets(Texture* tex);
Material getMaterial(const Texture_Names& tex);
Material getMaterial(const std::string& texName);

Blocks itemToBlock(Item item);
Blocks toBlock(std::string name);

std::vector<Face> toFaces(FaceB_p face);

glm::vec3 operator+(glm::vec3 p1, glm::vec2 p2);
glm::vec3 operator+(glm::vec2 p1, glm::vec3 p2);
glm::vec3 operator-(glm::vec3 p1, glm::vec2 p2);
glm::vec3 operator-(glm::vec2 p1, glm::vec3 p2);

class Timer {
private:
	std::chrono::time_point<std::chrono::steady_clock> start_;
	std::chrono::time_point<std::chrono::steady_clock> stop_;
	std::chrono::microseconds duration;
public:
	void start();
	void stop();
	void end();
	GLulong getTime();
	void showTime(std::string name = "", bool inFrames = 0);
};