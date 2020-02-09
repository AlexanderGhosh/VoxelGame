#pragma once
#include <string>
#include <sstream>
#include "../../Shaders/Shader.h"
#include "../../Mesh.h"

<<<<<<< HEAD
#define RENDER_DISTANCE 8
#define RENDERED_AREA RENDER_DISTANCE * RENDER_DISTANCE
#define RENDERED_VOLUME RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE
#define CHUNK_SIZE 4
=======
#define RENDER_DISTANCE 4
#define CHUNK_SIZE 16
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

#define GRAVITY 9.81f
#define DRAG_COEFFICIENT 2.0f

using namespace Mesh;
enum FACES_NAMES {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};
enum TEXTURE_NAMES {
	GRASS,
	PLAYER_BOTTOM,
	PLAYER_TOP
};
enum SHADER_NAMES {
	BLOCK2,
	BLOCK3
};

extern const std::array<Buffer*, 6> FACES;
extern const std::vector<Texture*> TEXTURES;
extern const std::vector<Shader*> SHADERS;

GLint getBlockIndex(glm::vec3 position);
