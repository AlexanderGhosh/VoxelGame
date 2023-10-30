#include <iostream>
// #define GLEW_STATIC
#include <glad/glad.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>

#include "Game/Game.h"

#include "Game/World/constants.h"
#include "Textures/Texture.h"
#include "Shaders/Shader.h"

#include "Helpers/BlockDetails.h"

glm::ivec2 DIM(1280, 720);

GLFWwindow* createWindow();
void createBlocks();
// namespace fs = std::experimental::filesystem;
int main() {
	GLFWwindow* window = createWindow();
	int c = 0;
	/*for (auto& face : FACES) {
		face->createBuffers();
		face->type = (FACES_NAMES)(FRONT + c++);
	}*/
	for (auto& tex : TEXTURES) {
		tex.load3D(tex.getName());
	}
	for (auto& tex : TEXTURES2D) {
		tex.load2D(tex.getName());
	}
	for (auto& shader : SHADERS) {
		shader.setUp();
	}
	createBlocks();

	Game game = Game(true, true, DIM);
	GameConfig::showFPS = 1;
	game.setWindow(window);
	game.generateWorld();				 // angle, screen ratio,                    near, far
	glm::mat4 projection = glm::perspective(45.0f, (float)DIM.x / (float)DIM.y, 0.01f, 1000.0f);
	game.doLoop(projection);

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}

void createBlocks() {
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
