#include "Game.h"

#pragma region GameConfig
GLboolean GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.5, 0.5, 0.5 };
GLuint GameConfig::FPSlock = 0;
#pragma endregion

#pragma region Static Members
Camera* Game::mainCamera = new Camera({ 0, 2, 0 });
glm::vec3 Game::mouseData(0);
std::array<GLboolean, 1024> Game::keys = std::array<GLboolean, 1024>();
Player Game::player = Player(false);
GLboolean Game::hasPlayer = GL_FALSE;
Physics::Engine Game::physicsEng = Physics::Engine();
#pragma endregion
Game::Game() {
	hasPlayer = false;
	gameRunning = false;
	Game::mainCamera = new Camera({ 0, 2, 0 });
	Game::mouseData = { 0, 0, -90 };
	GameConfig::setup();
}
Game::Game(GLboolean hasPlayer) {
	this -> hasPlayer = hasPlayer;
	setupPlayer();
	gameRunning = false;
	Game::mouseData = { 0, 0, -90 };
	GameConfig::setup();
}
void Game::generateWorld() {
	world = World(true);
}
void Game::update() {

}
void Game::doLoop(glm::mat4 projection) {
	gameRunning = true;
	setupEventCB(window);
	this->projection = projection;
	while (gameRunning) {
		calcTimes();
		lockFPS();
		showFPS();
		proccesEvents();
		doMovement();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		showStuff();
		Game::physicsEng.doUpdates(Game::world);
		// rend.render(*Game::mainCamera, projection);


		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
	}
}
void Game::calcTimes() {
	GLfloat frame = glfwGetTime();
	deltaTime = frame - lastFrameTime;
	lastFrameTime = frame;
	frameRate = 1 / deltaTime;
}
void Game::showFPS() {
	if (GameConfig::showFPS) {
		std::cout << "FPS: " << frameRate << std::endl;
	}
}
void Game::proccesEvents() {
	glfwPollEvents();
}
void Game::lockFPS() {
	if (GameConfig::FPSlock > 0) {
		if (frameRate > GameConfig::FPSlock) {
			GLfloat toEllaps = (GLfloat)1 / (frameRate - GameConfig::FPSlock);
			while (toEllaps < 0) toEllaps *= 10;
			std::this_thread::sleep_for(std::chrono::milliseconds((GLuint)toEllaps));
		}
	}
}
void Game::showStuff(GLboolean showStatic) {
	Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
	if (hasPlayer) {
		player.render(projection);
	}
	if (showStatic) {
		world.renderChunksStatic(cam, projection);
	}
	else {
		// world.renderChunksDynamic();
	}
}
void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}
void Game::setupPlayer() {
	//{ 0, 1, -1 }
	player = Player({ 0, 0.01, 0 }, { 0, 5, -1 });
	player.create();
}
void Game::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS || action == GLFW_RELEASE) {
			Game::keys[key] ^= true;
		}
	}
}
void Game::mouseCallBack(GLFWwindow* window, double xPos, double yPos) {
	if ((int)Game::mouseData.z) {
		Game::mouseData.x = xPos;
		Game::mouseData.y = yPos;
		Game::mouseData.z = false;
	}

	GLfloat xOffset = xPos - Game::mouseData.x;
	GLfloat yOffset = Game::mouseData.y - yPos;

	Game::mouseData.x = xPos;
	Game::mouseData.y = yPos;
	if (Game::hasPlayer) {
		Game::player.processMouse(xOffset, yOffset, mouseData.x);
		return;
	}
	Game::mainCamera->ProcessMouseMovement(xOffset, yOffset);
}
void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
}
void Game::doMovement() {
	std::vector<Physics::Update> updates;
	if (Game::keys[GLFW_KEY_W] || Game::keys[GLFW_KEY_UP]) {
		if (Game::hasPlayer) {
			updates.push_back(Game::player.processMovement(FORWARD, deltaTime));
		}
		else {
			Game::mainCamera->ProcessMovement(FORWARD, deltaTime);
		}
	}
	if (Game::keys[GLFW_KEY_S] || Game::keys[GLFW_KEY_DOWN]) {
		if (Game::hasPlayer) {
			updates.push_back(Game::player.processMovement(BACKWARD, deltaTime));
		}
		else {
			Game::mainCamera->ProcessMovement(BACKWARD, deltaTime);
		}
	}
	if (Game::keys[GLFW_KEY_D] || Game::keys[GLFW_KEY_RIGHT]) {
		if (Game::hasPlayer) {
			updates.push_back(Game::player.processMovement(RIGHT_C, deltaTime));
		}
		else {
			Game::mainCamera->ProcessMovement(RIGHT_C, deltaTime);
		}
	}
	if (Game::keys[GLFW_KEY_A] || Game::keys[GLFW_KEY_LEFT]) {
		if (Game::hasPlayer) {
			updates.push_back(Game::player.processMovement(LEFT_C, deltaTime));
		}
		else {
			Game::mainCamera->ProcessMovement(LEFT_C, deltaTime);
		}
	}
	if (Game::keys[GLFW_KEY_SPACE]) {
		if (Game::hasPlayer) {
			updates.push_back(Game::player.processMovement(UP_C, deltaTime));
		}
		else {
			Game::mainCamera->ProcessMovement(UP_C, deltaTime);
		}
	}
	if (Game::keys[GLFW_KEY_LEFT_SHIFT]) {
		if (Game::hasPlayer) {
			updates.push_back(Game::player.processMovement(DOWN_C, deltaTime));
		}
		else {
			Game::mainCamera->ProcessMovement(DOWN_C, deltaTime);
		}
	}
	//std::cout << updates.size() << "\n";
	Game::physicsEng.addUpdates(updates);
}
void Game::cleanUp() {
	world.cleanUp();
}


