#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include "../../Shaders/Shader.h"
#include "../../Mesh.h"

class ChunkColumn;
class BlockStore;

#define RENDER_DISTANCE 5
#define PLAYER_REACH 5
#define WORLD_HEIGHT 256
#define WANDER_RANGE 15
#define PARTS_PER_CHUNK 16

#define CHUNK_SIZE 16

#define GRAVITY 10
#define PLAYER_SPEED 2.0
extern glm::vec3 LIGHTPOSITION;
extern glm::vec3 LIGHTPOSITIONOrigin;

enum class Blocks : uint8_t;

using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
using Faces = std::vector<Face>;
using Faces_P = std::vector<Face*>;
using Block_Count = std::pair<Blocks, GLuint>;
using HeightMap = std::array<std::array<std::vector<Block_Count>, CHUNK_SIZE>, CHUNK_SIZE>;
using Chunks = std::vector<ChunkColumn>;
using FaceB = std::tuple<Buffer, Texture*, GLuint>;
using FaceB_p = std::tuple<Buffer*, Texture*, std::vector<glm::mat4>>;
using AdjacentMap = std::unordered_map<glm::vec2, ChunkColumn>;
using WorldMap = std::unordered_map<glm::vec2, BlockStore>;
using Item = GLuint;

using AdjacentMap_p = std::unordered_map<glm::vec2, ChunkColumn*>;

using namespace Mesh; 
struct BlockDet {
	std::string Name;
	Texture* Tex;
	Texture ItemTex;
	GLboolean isTransparant;
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
	DEPTH
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
	GRASS,
	DIRT,
	STONE,
	WATER,
	ERROR,
	LOG,
	LEAF
};
extern const std::array<Buffer*, 6> FACES;
extern const std::vector<Texture*> TEXTURES;
extern const std::vector<Texture*> TEXTURES2D;
extern const std::vector<Shader*> SHADERS;
extern std::map<Blocks, BlockDet> BLOCKS;
extern const std::vector<Blocks> AllBlocks;

void reduceToMultiple(glm::ivec3& victim, GLuint multiple, const char* overload);
void reduceToMultiple(glm::ivec2& victim, GLuint multiple, const char* overload);
glm::ivec3 reduceToMultiple(glm::ivec3 victim, GLuint multiple);
GLint reduceToMultiple(GLfloat victim, GLuint multiple);
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

Blocks itemToBlock(Item item);

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
	void showTime(std::string name = "", GLboolean inFrames = 0);
};