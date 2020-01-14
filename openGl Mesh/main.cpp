#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Renders/meshRender.h"
#include "Game/Player/Player.h"
#include "Renders/chunkRender.h"
//#include "Game/World/constants.h"
//#include "Game/World/Chunk.h"
//#include "Game/Player/Camera.h"
#include "Game/World/PhysicsEngine.h"
// #include "Game/World/World.h"

Player p1;
glm::vec3 m_pos(1);
glm::ivec2 DIM(1600, 900);
glm::vec3 BACKGROUND(0.5);
GLfloat prev_t;

bool keys[1024];
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = DIM.x / 2, lastY = DIM.y / 2;

void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallBack(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

GLuint getFrameRate();

GLFWwindow* createWindow();
//Camera c(glm::vec3(0, 2, 0));
Player p;
int main() {
	GLFWwindow* window = createWindow();

	Render::FaceMeshRender fmr("block2");
	Mesh::FaceMesh front = FACES::FRONT;
	front.setPosition({ 0, 2, -3 });
	front.setTexture("grass");
	fmr.loadMesh(front);

	World world;
	p.create();

	glm::mat4 projection = glm::perspective(p.cam.GetZoom(), (GLfloat)DIM.x / (GLfloat)DIM.y, 0.1f, 100.0f);
	while (!glfwWindowShouldClose(window))
	{
		GLfloat frame = glfwGetTime();
		deltaTime = frame - lastFrame;
		lastFrame = frame;

		glfwPollEvents();
		DoMovement();

		glClearColor(BACKGROUND.r, BACKGROUND.g, BACKGROUND.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		fmr.render(p.cam, projection);
		world.renderChunksStatic(p.cam, projection);
		p.render(projection);
		


		Physics::Engine::doUpdates(world);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	//cmr.destroy();
	return EXIT_SUCCESS;
}

GLFWwindow* createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(DIM.x, DIM.y, "Temp", nullptr, nullptr);

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
	}// setup events
	glfwSetKeyCallback(window, KeyCallBack);
	glfwSetCursorPosCallback(window, MouseCallBack);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disabel cursor visabilaty

	glewExperimental = GL_TRUE; // uses the modern way of stuff

	glViewport(0, 0, DIM.x, DIM.y);

	glEnable(GL_DEPTH_TEST);// enable depth

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// aplpha suport

	return window;
}
GLuint getFrameRate() {
	GLfloat curr_t = glfwGetTime();
	GLfloat delta_t = curr_t - prev_t;
	prev_t = curr_t;
	return (GLuint)(1 / delta_t);
}

void DoMovement() {
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
		Physics::Update up = p.processMovement(FORWARD, deltaTime);
		Physics::Engine::addUpdate(up); 
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
		Physics::Update up = p.processMovement(BACKWARD, deltaTime);
		Physics::Engine::addUpdate(up);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		Physics::Update up = p.processMovement(RIGHT, deltaTime);
		Physics::Engine::addUpdate(up);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
		Physics::Update up = p.processMovement(LEFT, deltaTime);
		Physics::Engine::addUpdate(up);
	}
}
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS || action == GLFW_RELEASE) {
			keys[key] ^= true;
		}
	}
	//glm::vec3 pos = c.GetPostion();
	//std::cout << pos.x << "," << pos.y << "," << pos.z << std::endl;
}
void MouseCallBack(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	p.processMouse(xOffset, yOffset);
}