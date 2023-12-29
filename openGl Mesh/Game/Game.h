#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <map>
#include <array>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Renders/UI Stuff/UI_Renderer.h"
#include "World/World.h"
#include "../FrameBuffer.h"
#include "../UniformBuffer.h"
#include "../Helpers/ModelLoaders/VoxelModel_Base.h"

#include "../GUIRendering/DrawableGUI.h"
#include "../GUI/GUI_Window.h"

class Model;
class EntityManager;
class GizmoManager;
class Camera;
struct ClickEventInfo;

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

struct GameConfig {
	static bool showFPS;
	static glm::vec3 backgroundCol;
	static unsigned int FPSlock;
	static void setup(bool showFPS = false, glm::vec3 bgCol = { 0.5, 0.5, 0.5 }, unsigned int FPS = 0) {
		GameConfig::showFPS = showFPS;
		GameConfig::backgroundCol = bgCol;
		GameConfig::FPSlock = FPS;
	}
};

namespace Entities {
	class Player;
}
class Game
{
public:
	Game();
	Game(glm::ivec2 windowDim);
	void doLoop(const glm::mat4& projection);
	void generateWorld();

	void setWindow(GLFWwindow* window);
	void cleanUp();

	void setPlayer(Entities::Player* player);

	DrawableGUI guiDrawable;

private:
	void setUpScreenQuad();

	static glm::vec3 mouseData;
	static glm::vec2 mouseOffset;
	static std::array<bool, 1024> keys;
	static World world;
	static UI_Renderer uiRenderer;

	void placeBlock();
	void breakBlock();
	void explode();

	GUI::GUI_Window guiWindow;
	UniformBuffer camreraBuffer, materialsBuffer;
	EntityManager* manager;
	GizmoManager* gizmoManager;

	unsigned int quadVBO, quadVAO;

	Entities::Player* _player;
	FrameBuffer oitFrameBuffer1, guiFrameBuffer, shadowFramebuffer, gBuffer;
	// Colour slot 0 is writen too
	FrameBuffer multiPurposeFB;
	GLFWwindow* window;
	float deltaTime;
	unsigned int frameRate;
	bool gameRunning;
	float lastFrameTime;
	unsigned int SBVAO,LSVAO, LSVBO;
	std::map<char, Character> Letters;
	glm::ivec2 windowDim;

	// is updated every frame with the current camera's view matrix
	glm::mat4 cameraView, cameraProjection;



	void showFPS();
	void calcTimes();
	void setupEventCB(GLFWwindow* window);
	void showStuff();
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouseCallBack(GLFWwindow* window, double xPos, double yPos);
	static void clickCallBack(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset);

	void makeSkybox(const std::string& skybox);
	void showSkybox();
	void setUpFreeType();
	void showText(const std::string& text, const glm::vec2& position, float scale = 1.0f, const glm::vec3 colour = glm::vec3(1));
	void createGUI();
	void showGUI();


	// SSAO
#ifdef SSAO
	std::array<glm::vec3, SSAO_NUM_SAMPLES> ssaoSamples;
	unsigned int ssaoNoiseTex;
	void setUpSSAO();
#endif // SSAO

};