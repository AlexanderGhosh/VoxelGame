#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <array>
#include <chrono>
#include <thread>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <time.h>

#include "Player/Player.h"
#include "World/World.h"
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};
struct GameConfig {
	static GLboolean showFPS;
	static glm::vec3 backgroundCol;
	static GLuint FPSlock;
	static void setup(GLboolean showFPS = false, glm::vec3 bgCol = { 0.5, 0.5, 0.5 }, GLuint FPS = 0) {
		GameConfig::showFPS = showFPS;
		GameConfig::backgroundCol = bgCol;
		GameConfig::FPSlock = FPS;
	}
};
class Game
{
public:
	Game();
	Game(GLboolean hasPlayer, GLboolean hasSkybox);
	void generateWorld();
	void setupPlayer();
	void doLoop(glm::mat4 projection);

	void setWindow(GLFWwindow* window);
	void cleanUp();

	static Camera* mainCamera;
	static glm::vec3 mouseData;
	static std::array<GLboolean, 1024> keys;
	Ray ray;
private:
	GLFWwindow* window;
	GLfloat deltaTime;
	GLuint frameRate;
	GLboolean gameRunning;
	static World world;
	static GLboolean hasPlayer;
	static Player player;
	GLboolean hasSkybox;
	GLfloat lastFrameTime;
	glm::mat4 projection;
	GLuint SBVAO, CHVAO, LSVAO, LSVBO;
	Texture texCH, texBN, texBS;
	std::map<GLchar, Character> Letters;
	// blockupdates
	void showFPS();
	void calcTimes();
	void proccesEvents();
	void setupEventCB(GLFWwindow* window);
	void lockFPS();
	void showStuff();
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouseCallBack(GLFWwindow* window, double xPos, double yPos);
	static void clickCallBack(GLFWwindow* window, int button, int action, int mods);
	void processKeys();
	void makeSkybox(std::string skybox);
	void showSkybox();
	void createCrossHair();
	void showCrossHair();
	void setUpFreeType();
	void showText(std::string text, glm::vec2 position, GLfloat scale = 1.0f, glm::vec3 colour = glm::vec3(0));
	void createGUI();
	void showGUI();
};