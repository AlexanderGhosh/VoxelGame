#pragma once
#include <string>
#include <sstream>
#include "../../Shaders/Shader.h"
#include "../../Mesh.h"

#define RENDER_DISTANCE 4

#define INITALL_VIEW 16
#define INITALL_VIEW_AREA INITALL_VIEW * INITALL_VIEW
#define INITALL_VIEW_VOLUME INITALL_VIEW * INITALL_VIEW * INITALL_VIEW
#define CHUNK_SIZE 16
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

#define GRAVITY 9.81
#define AIR_RESISTANCE 2.0
#define PLAYER_SPEED 2.0

using namespace Mesh;
enum TEXTURE_NAMES {
	GRASS,
	PLAYER_BOTTOM,
	PLAYER_TOP,
	SKYBOX_T,
	STONE,
	DIRT,
	WATER
};
enum SHADER_NAMES {
	BLOCK2,
	BLOCK3,
	SKYBOX
};
enum class Move_Dir
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	Null
};
extern const std::array<Buffer*, 6> FACES;
extern const std::vector<Texture*> TEXTURES;
extern const std::vector<Shader*> SHADERS;

GLint getBlockIndex(glm::vec3 position);
//GLint getChunkIndex(glm::vec3 position, GLboolean absalute = GL_FALSE, GLboolean reduced = GL_FALSE);
//GLuint getChunkIndex(glm::vec3 pos, GLboolean rectify, GLboolean raw, const char* overload);
void reduceToMultiple(glm::ivec3& victim, GLuint multiple, const char* overload);
glm::ivec3 reduceToMultiple(glm::ivec3 victim, GLuint multiple);
