#include "Constants.h"
#include "../Textures/Texture.h"
#include "../Shaders/Shader.h"
#include "../Material.h"

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
	Shader("Gizmos/PointShader/pointShader"),
	Shader("Gizmos/LineShader/lineShader", true),
	Shader("Gizmos/CircleShader/circleShader", true),
};
std::vector<BlockDetails> BLOCK_DETAILS = {};
std::vector<Material> MATERIALS = {
	Material(0, 0, 0, 1), // error
	Material(0, .4, .1, 1), // grass
	Material(.5, .25, .25, 1), // dirt
	Material(.5, .5, .5, 1), // stone
	Material(.4, .1, 0, 1), // sand
	Material(0, 0.6, 1, .65), // water
	Material(1, 1, 1, 1), // show
};