#pragma once
#include <string>
#include <sstream>
#include "../../Shaders/Shader.h"
#include "../../Mesh.h"

#define RENDER_DISTANCE 8
#define RENDERED_AREA RENDER_DISTANCE * RENDER_DISTANCE
#define RENDERED_VOLUME RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE
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
	SKYBOX_T
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
GLint getChunkIndex(glm::vec3 position, GLboolean reduced = GL_TRUE);
