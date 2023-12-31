#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <map>
#include <array>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "World/World.h"
#include "../FrameBuffer.h"
#include "../UniformBuffer.h"
#include "../Helpers/ModelLoaders/VoxelModel_Base.h"

#include "../GUI/GUI_Window.h"
#include "../GUI/Elements/TextBox.h"

class Model;
class EntityManager;
class GizmoManager;
class Camera;
struct ClickEventInfo;

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
	~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;

	void setUpGame(const glm::mat4& projection);
	void doGameLoop();
	void generateWorld();

	void setWindow(GLFWwindow* window);

	void setPlayer(Entities::Player* player);

private:
	void setUpScreenQuad();

	static glm::vec3 mouseData;
	static glm::vec2 mouseOffset;
	static std::array<bool, 1024> keys;
	World _world;

	void placeBlock();
	void breakBlock();
	void explode();

	GUI::GUI_Window guiWindow;
	UniformBuffer camreraBuffer, materialsBuffer;
	EntityManager* manager;
	GizmoManager* gizmoManager;

	unsigned int quadVBO, quadVAO;
	unsigned int skyVAO, skyVBO;

	Entities::Player* _player;
	FrameBuffer oitFrameBuffer1, shadowFramebuffer, gBuffer;
	// Colour slot 0 is writen too
	FrameBuffer multiPurposeFB;
	GLFWwindow* window;
	float deltaTime;
	unsigned int frameRate;
	bool gameRunning;
	float lastFrameTime;
	glm::ivec2 windowDim;

	// is updated every frame with the current camera's view matrix
	glm::mat4 cameraView, cameraProjection;


	GUI::TextBox _fpsCounter;
	GUI::TextBox _playerPosition;
	GUI::TextBox _viewDirection;
	GUI::TextBox _numChunks;

	void updateGUIText();

	void calcTimes();
	void setupEventCB(GLFWwindow* window);
	void renderScene();
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouseCallBack(GLFWwindow* window, double xPos, double yPos);
	static void clickCallBack(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset);

	void makeSkybox(const std::string& skybox);
	void drawSkybox();


	std::array<glm::vec3, SSAO_NUM_SAMPLES> ssaoSamples;
	unsigned int _ssaoNoiseTex;
	void setUpSSAO();

};