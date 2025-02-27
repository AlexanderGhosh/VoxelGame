#include "Constants.h"
#include "../Textures/Texture.h"
#include "../Shaders/Shader.h"
#include "../Material.h"

std::vector<Texture> TEXTURES3D = {
	std::move(Texture("skybox", "")),
};

std::vector<Texture> TEXTURES2D = {
	std::move(Texture("crosshair", "")),
	std::move(Texture("boarders/normal", "")),
	std::move(Texture("boarders/selected", "")),
	std::move(Texture("hearts/live_heart", "")),
	std::move(Texture("hearts/dead_heart", ""))
};
std::vector<Shader> SHADERS = {
	Shader("skybox"),
	Shader("crosshair"),
	Shader("glyph"),
#ifdef ALWAYS_USE_GREEDY_MESH
	Shader("Greedy/greedy", "Greedy/greedy", "OIT/Transparent/transparent"),
#else
	Shader("OIT/Transparent/transparent", "OIT/Transparent/transparent", "OIT/Transparent/transparent"),
#endif
	Shader("OIT/Composite/composite"),
	Shader("ScreenQuad/screenQuad"),
	Shader("Shadow/shadow", true),
	Shader("Model/model"),
#ifdef ALWAYS_USE_GREEDY_MESH
	Shader("Greedy/greedy", "Greedy/greedy", "GBuffer/gbuffer"),
#else
	Shader("OIT/Transparent/transparent", "OIT/Transparent/transparent", "GBuffer/gbuffer"),
#endif
	Shader("Deffered/deffered"),
	Shader("SSAO/ssao"),
	Shader("Blur/blur"),
	Shader("Gizmos/PointShader/pointShader"),
	Shader("Gizmos/LineShader/lineShader", true),
	Shader("Gizmos/CircleShader/circleShader", true),
	Shader("PhysicsDebug/physicsDebug"),
	Shader("NoiseRender/noiseRender", true),
	Shader("NewGUI/newGUI")
};
std::vector<Block> BLOCKS = {};
std::vector<Material> MATERIALS = {};