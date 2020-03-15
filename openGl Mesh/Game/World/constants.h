#pragma once
#include <string>
#include <sstream>
#include "../../Shaders/Shader.h"
#include "../../Mesh.h"

#define RENDER_DISTANCE 3
#define PLAYER_REACH 5

#define INITALL_VIEW 4
#define INITALL_VIEW_AREA INITALL_VIEW * INITALL_VIEW
#define INITALL_VIEW_VOLUME INITALL_VIEW * INITALL_VIEW * INITALL_VIEW
#define CHUNK_SIZE 16
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

#define GRAVITY 10
#define PLAYER_SPEED 2.0

using namespace Mesh; 
struct BlockDet {
	std::string Name;
	Texture* Tex;
	Texture ItemTex;
};
enum class Texture_Names {
	GRASS,
	PLAYER_BOTTOM,
	PLAYER_TOP,
	SKYBOX,
	STONE,
	DIRT,
	WATER
};
enum SHADER_NAMES {
	BLOCK2,
	BLOCK3,
	SKYBOX,
	CROSSHAIR,
	GLYPH
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
	WATER	
};
extern const std::array<Buffer*, 6> FACES;
extern const std::vector<Texture*> TEXTURES;
extern const std::vector<Shader*> SHADERS;
extern std::map<Blocks, BlockDet> BLOCKS;
extern const std::vector<Blocks> AllBlocks;

GLint getBlockIndex(glm::vec3 position);
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
