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

glm::ivec2 DIM(WIDTH, HEIGHT);

GLFWwindow* createWindow();
void createBlockDetails();

int main() {
	GLFWwindow* window = createWindow();

	for (auto& tex : TEXTURES) {
		tex.loadCube(tex.getName());
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
	return 0;
}

void createBlockDetails() {
	BLOCK_DETAILS.resize((size_t) Block::SIZE);
	for (unsigned int i = 0; i < BLOCK_DETAILS.size(); i++) {
		Block block = (Block) i;
		BlockDetails& dets = BLOCK_DETAILS[i];
		dets.Name = getName(block);
		dets.Tex = &TEXTURES[(unsigned int) getTexture(block)];
		if(dets.Name != "air")
			dets.ItemTex = Texture("Items/" + dets.Name);
		dets.isTransparant = false;
		if (dets.Name == "air" || dets.Name == "leaf" || dets.Name == "water") {
			dets.isTransparant = true;
		}
	}
}

GLFWwindow* createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(DIM.x, DIM.y, "Alex's Game", nullptr, nullptr);

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	DIM = glm::ivec2(w, h);
	if (nullptr == window) {
		std::cout << "Falild to create window" << std::endl;
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

	//glewExperimental = GL_TRUE; // uses the modern way of stuff

	glViewport(0, 0, DIM.x, DIM.y);

	glEnable(GL_DEPTH_TEST);// dont render thing behind other things

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// aplpha suport

	glEnable(GL_MULTISAMPLE); // MSAA

	return window;
}
