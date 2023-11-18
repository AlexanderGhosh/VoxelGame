#include "Constants.h"
#include "../Textures/Texture.h"
#include "../Shaders/Shader.h"

std::vector<Texture> TEXTURES3D = {
	Texture("skybox", ""),
};
std::vector<Texture> TEXTURES2D = {
	Texture("crosshair", ""),
	Texture("boarders/normal", ""),
	Texture("boarders/selected", ""),
	Texture("hearts/live_heart", ""),
	Texture("hearts/dead_heart", "")
};
std::vector<Shader> SHADERS = {
	Shader("skybox"),
	Shader("crosshair"),
	Shader("glyph"),
	Shader("OIT/Transparent/transparent", true),
	Shader("OIT/Composite/composite"),
	Shader("ScreenQuad/screenQuad"),
	Shader("Shadow/shadow", true),
	Shader("Model/model"),
	Shader("GBuffer/gbuffer", true),
	Shader("Deffered/deffered"),
	Shader("SSAO/ssao"),
	Shader("Blur/blur"),
};
std::vector<BlockDetails> BLOCK_DETAILS = {};
glm::vec3 LIGHTPOSITION = glm::vec3(-8, 80, -8);
glm::vec3 LIGHTPOSITIONOrigin = glm::vec3(-8, 80, -8);