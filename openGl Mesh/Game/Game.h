#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <array>
#include <chrono>
#include <thread>

#include "Player/Camera.h"
// #include "Player/Player.h"
// #include "World/World.h"
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
	Game(GLboolean hasPlayer);
	void generateWorld();
	void setupPlayer();
	void update();
	void doLoop(glm::mat4 projection);

	void setWindow(GLFWwindow* window);

	static Camera* mainCamera;
	static glm::vec3 mouseData;
	static std::array<GLboolean, 1024> keys;
private:
	GLFWwindow* window;
	GLfloat deltaTime;
	GLuint frameRate;
	GLboolean gameRunning;
	// World world;
	GLboolean hasPlayer;
	// Player player;
	GLfloat lastFrameTime;
	glm::mat4 projection;
	// blockupdates
	void showFPS();
	void calcTimes();
	void proccesEvents();
	void setupEventCB(GLFWwindow* window);
	void lockFPS();
	void showStuff(GLboolean showStatic = true);
	void cleanUp();
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouseCallBack(GLFWwindow* window, double xPos, double yPos);
};

