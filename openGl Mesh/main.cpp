#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtc/type_ptr.hpp>

#include "Game/Game.h"

glm::ivec2 DIM(1600, 900);

GLFWwindow* createWindow();

int main() {
	GLFWwindow* window = createWindow();
	int c = 0;
	for (auto& face : FACES) {
		face->createBuffers();
		face->type = (FACES_NAMES)(FRONT + c++);
	}
	for (auto& tex : TEXTURES) {
		tex->load3D(tex->getName());
	}
	for (auto& shader : SHADERS) {
		shader->setUp();
	}

	Game game(1, 1);
	// GameConfig::showFPS = true;
	game.setWindow(window);
	game.generateWorld();				 // angle, screen ratio,                    near, far
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)DIM.x / (GLfloat)DIM.y, 0.1f, 1000.0f);
	game.doLoop(projection);

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}

GLFWwindow* createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(DIM.x, DIM.y, "Alex's Game", nullptr, nullptr);

	GLint w, h;
	glfwGetFramebufferSize(window, &w, &h);
	DIM = glm::ivec2(w, h);
	if (nullptr == window) {
		std::cout << "Falild to create window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE); // stops program
	}
	glfwMakeContextCurrent(window);
	if (GLEW_OK != glewInit()) {
		std::cout << "Falid to initialise GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disabel cursor visabilaty

	glewExperimental = GL_TRUE; // uses the modern way of stuff

	glViewport(0, 0, DIM.x, DIM.y);

	glEnable(GL_DEPTH_TEST);// dont render thing behind other things

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// aplpha suport

	 //glEnable(GL_CULL_FACE); // face culling
	 //glFrontFace(GL_CCW);

	return window;
}
