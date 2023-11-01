#include "Constants.h"
#include "../Textures/Texture.h"
#include "../Shaders/Shader.h"

std::vector<Texture> TEXTURES = {
	Texture("grass", ""),
	Texture("player/bottom", ""),
	Texture("player/top", ""),
	Texture("skybox", ""),
	Texture("stone", ""),
	Texture("dirt", ""),
	Texture("water", ""),
	Texture("error", ""),
	Texture("vampire/bottom", ""), // logs
	Texture("leaf", ""),
	Texture("vampire/bottom", ""),
	Texture("vampire/top", ""),
	Texture("sand", "")
};
std::vector<Texture> TEXTURES2D = {
	Texture("crosshair", ""),
	Texture("boarders/normal", ""),
	Texture("boarders/selected", ""),
	Texture("hearts/live_heart", ""),
	Texture("hearts/dead_heart", "")
};
std::vector<Shader> SHADERS = {
	Shader("block2"),
	Shader("block3"),
	Shader("skybox"),
	Shader("crosshair"),
	Shader("glyph"),
	Shader("ray"),
	Shader("depth"),
	Shader("GeomBlocks/geom", true),
	Shader("OIT/Opaque/opaque", true),
	Shader("OIT/Transparent/transparent", true),
	Shader("OIT/Composite/composite"),
	Shader("ScreenQuad/screenQuad"),
	Shader("Shadow/shadow", true),
};
std::vector<BlockDetails> BLOCK_DETAILS = {};

const std::vector<Material> MATERIALS = {
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(0), 0 }, // grass
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // player bottom
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // player top
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // skybox
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(1), 32 }, // stone
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(0), 0 }, // dirt
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(1), 64 }, // water
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // error
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(0), 0 }, // log
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(1), 64 }, // leaf
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // vampire bottom
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // vampire top
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(0), 0 }, // sand
};

glm::vec3 LIGHTPOSITION = glm::vec3(-8, 80, -8);
glm::vec3 LIGHTPOSITIONOrigin = glm::vec3(-8, 80, -8);