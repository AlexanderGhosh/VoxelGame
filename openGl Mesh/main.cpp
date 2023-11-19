#include <iostream>
#include <gtx/string_cast.hpp>

#include <glad/glad.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>

#include "Game/Game.h"

#include "Helpers/Constants.h"
#include "Helpers/Functions.h"
#include "Textures/Texture.h"
#include "Shaders/Shader.h"

#include "Helpers/BlockDetails.h"

#include "EntityComponent/Entity.h"
#include "Mangers/EntityManager.h"
#include "Mangers/ComponentManager.h"
#include "Mangers/ModelManager.h"
#include "Mangers/GizmoManager.h"
#include "Gizmos/Primatives/Circle.h"

glm::ivec2 DIM(1280, 720);

GLFWwindow* createWindow();
void createBlockDetails();

int main() {
	GLFWwindow* window = createWindow();

	for (auto& tex : TEXTURES3D) {
		tex.load3D(tex.getName());
	}
	for (auto& tex : TEXTURES2D) {
		tex.load2D(tex.getName());
	}
	for (auto& shader : SHADERS) {
		shader.setUp();
	}
	createBlockDetails();

	// ENTITY COMPONENT SYSTEMS
	ComponentManager& componentManager = ComponentManager::getInstance();
	EntityManager& entityManager = EntityManager::getInstance();
	Entity& player = entityManager.createEntity();
	Transform playerTransform;
	player.addComponent(playerTransform);

	GizmoManager& gizmoManager = GizmoManager::getInstance();
	Gizmo::Circle pointA({ 0, 0, 0 }, { 1, 0 ,0 });
	//pointA.setThickness(10);
	gizmoManager.addGizmo(pointA);

	Game game = Game(DIM);
	GameConfig::showFPS = true;


	game.setWindow(window);
	game.generateWorld();

	glm::mat4 projection = glm::perspective(FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	game.doLoop(projection);

	game.cleanUp();

	glfwDestroyWindow(window);
	glfwTerminate();

	ComponentManager::getInstance().destroy();
	EntityManager::getInstance().destroy();
	ModelManager::getInstance().destroy();
	GizmoManager::getInstance().destroy();
	return 0;
}

void createBlockDetails() {
	BLOCK_DETAILS.resize((size_t) Block::SIZE);
	for (unsigned int i = 0; i < BLOCK_DETAILS.size(); i++) {
		Block block = (Block) i;
		BlockDetails& dets = BLOCK_DETAILS[i];
		dets.isTransparant = block == Block::AIR || block == Block::LEAF || block == Block::WATER;
	}
}

GLFWwindow* createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(DIM.x, DIM.y, "Voxel Game", nullptr, nullptr);

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	DIM = glm::ivec2(w, h);
	if (nullptr == window) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE); // stops program
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE); // stops program
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disabel cursor visabilaty
	if(!VSYNC) glfwSwapInterval(0);

	glViewport(0, 0, DIM.x, DIM.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// aplpha suport

	// glEnable(GL_MULTISAMPLE); // MSAA

	return window;
}
