#include "Game.h"
#include <gtx/string_cast.hpp>
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"

#pragma region GameConfig
bool GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.5, 0.5, 0.5 };
unsigned int GameConfig::FPSlock = 0;
#pragma endregion

#pragma region Static Members
Camera* Game::mainCamera = new Camera({ 0, 2, 0 });
glm::vec3 Game::mouseData(0);
std::array<bool, 1024> Game::keys = std::array<bool, 1024>();
// Entity* Game::player = new Entity();
World Game::world = World(0);
// EntityHander Game::entityHander = EntityHander();
UI_Renderer Game::uiRenderer = UI_Renderer();
#pragma endregion


/*

	Ray ray;
private:
	GLFWwindow* window;
	float deltaTime;
	unsigned int frameRate;
	bool gameRunning;
	bool hasSkybox;
	float lastFrameTime;
	glm::mat4 projection, lightProjection;
	unsigned int SBVAO,LSVAO, LSVBO, depthFBO;
	std::map<GLchar, Character> Letters;
	unsigned int depthMap;
	glm::ivec2 windowDim;
*/

Game::Game() : window(), deltaTime(), frameRate(), gameRunning(false), hasSkybox(false), lastFrameTime(-1), guiFrameBuffer(), quadVAO(), quadVBO(),
projection(1), lightProjection(0), SBVAO(0), LSVAO(), Letters(), depthMap(), windowDim(), LSVBO(), oitFrameBuffer1(), oitFrameBuffer2() {
	Game::mainCamera = new Camera({ 0, 2, 0 });
	Game::mouseData = { 0, 0, -90 };
	GameConfig::setup();
	setUpScreenQuad();
}
Game::Game(bool hasPlayer, bool hasSkybox, glm::ivec2 windowDim) : Game() {
	// this->hasPlayer = hasPlayer;
	this->hasSkybox = hasSkybox;
	setupPlayer();
	gameRunning = false;
	Game::mouseData = { 0, 0, -90 };
	lastFrameTime = -1.0f;
	this->windowDim = windowDim;
	GameConfig::setup();
	if (hasSkybox) {
		makeSkybox("skybox");
	}
	createGUI();
	setUpFreeType();

	FrameBufferInit detailsOIT;
	detailsOIT.hasDepth = true;
	detailsOIT.depthTexture = true;

	ColourBufferInit opaqueOIT;
	opaqueOIT.format = GL_HALF_FLOAT;
	opaqueOIT.internalFormat = GL_RGBA16F;
	opaqueOIT.type = GL_RGBA;

	detailsOIT.colourBuffers = { opaqueOIT };

	oitFrameBuffer1 = FrameBuffer(windowDim);
	oitFrameBuffer1.setUp(detailsOIT);

	ColourBufferInit accumOIT;
	accumOIT.format = GL_HALF_FLOAT;
	accumOIT.internalFormat = GL_RGBA;
	accumOIT.type = GL_RGBA;

	ColourBufferInit revealOIT;
	revealOIT.format = GL_HALF_FLOAT;
	revealOIT.internalFormat = GL_R8;
	revealOIT.type = GL_RED;

	detailsOIT.colourBuffers = { accumOIT, revealOIT };
	detailsOIT.depthBuffer = oitFrameBuffer1.getDepth();

	oitFrameBuffer2 = FrameBuffer(windowDim);
	oitFrameBuffer2.setUp(detailsOIT);


	FrameBufferInit detailsGUI;

	ColourBufferInit colourGUI;
	colourGUI.format = GL_FLOAT;
	colourGUI.internalFormat = GL_RGBA32F;
	colourGUI.type = GL_RGBA;
	
	detailsGUI.hasDepth = false;
	detailsGUI.depthTexture = false;


	detailsGUI.colourBuffers = { colourGUI };
	guiFrameBuffer = FrameBuffer(windowDim);
	guiFrameBuffer.setUp(detailsGUI);

	FrameBufferInit detailsShadows;
	detailsShadows.hasDepth = true;
	detailsShadows.depthTexture = true;

	shadowFramebuffer = FrameBuffer(windowDim);
	shadowFramebuffer.setUp(detailsShadows);
}

void Game::generateWorld() {
	world = World(true, false, 32);
	world.setUpDrawable();
}

void Game::doLoop(const glm::mat4& projection) {
	gameRunning = true;
	setupEventCB(window);
	this->projection = projection;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
	mainCamera->setPosition({ 8, 25, 15 });
	// temp vampire
	// Entity vampire(1);
	// vampire.getCollider().setDimentions({ 0.85, 0.85, 0.85 }, { 0.85, 2.55, 0.85 });
	// vampire.setPosition({ 5, 80, 0 });
	// vampire.setTextues(Texture_Names::VAMPIRE_BOTTOM, Texture_Names::VAMPIRE_TOP);
	
	//setupDepthFBO();

	// entityHander.addEntity(vampire);

	// world.advanceGeneration();
	while (gameRunning) {
		calcTimes();
		proccesEvents();
		processKeys();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		/*std::vector<std::vector<ChunkColumn*>> adjacentChunkss;
		std::vector<ChunkColumn*> occuped;
		for (auto& e : entityHander.getEntitys()) {
			glm::vec3& p = e.getPosition();
			adjacentChunkss.push_back(world.getAdjacentChunks(p));
			occuped.push_back(world.getChunkOccupied(p));
		}
		player = &entityHander.getEntitys()[0];

		entityHander.update(projection, player->getCamera(), adjacentChunkss, occuped, deltaTime);*/

		showStuff();


		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
	}
}

void Game::calcTimes() {
	float frame = glfwGetTime();
	deltaTime = frame - lastFrameTime;
	if (lastFrameTime == -1) deltaTime = 1.0f / 60.0f;
	lastFrameTime = frame;
	frameRate = 1 / deltaTime;
}

void Game::showFPS() {
	if (GameConfig::showFPS) {
		showText("FPS: " + std::to_string(frameRate), { 5, 875 }, 0.5f);
	}
}

std::string m;

void Game::proccesEvents() {
	glfwPollEvents();
}

const unsigned int SHADOW_WIDTH = 3072, SHADOW_HEIGHT = 3072;

void Game::showStuff() {
	// light orhto projection
	float near_plane = 0.1f, far_plane = 100.0f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(LIGHTPOSITION, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 LSM = lightProjection * lightView;

	// 1. render from the lights perspective for the shadow map
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	shadowFramebuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	Shader& shadows = SHADERS[SHADOW];
	shadows.bind();
	shadows.setValue("lightMatrix", LSM);
	shadows.setValue("lightPos", LIGHTPOSITION);
	world.render(&shadows);
	shadows.unBind();

	// 2. render for OIT
	glm::mat4 viewMatrix = mainCamera->GetViewMatrix();
	oitFrameBuffer1.bind(); // render to the OIT framebuffer1
	// 2.1 Opaque
	glDisable(GL_BLEND);
	glClearColor(0, 0, 0, 0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Shader& opaue = SHADERS[OIT_OPAQUE];
	opaue.bind();
	bool b1 = opaue.setValue("view", viewMatrix);
	bool b2 = opaue.setValue("projection", projection);
	world.render(&opaue);
	opaue.unBind();

	// 2.2 Transparent
	oitFrameBuffer2.bind(); // render to the OIT framebuffer2
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);
	
	glm::vec3 black(0);
	glm::vec3 white(1);
	glClearBufferfv(GL_COLOR, 0, &black[0]);
	glClearBufferfv(GL_COLOR, 1, &white[0]);
	
	Shader& transparent = SHADERS[OIT_TRANSPARENT];
	transparent.bind();
	bool b3 = transparent.setValue("view", viewMatrix);
	bool b4 = transparent.setValue("projection", projection);
	world.render(&transparent);
	transparent.unBind();

	// 2.3 Composite
	oitFrameBuffer1.bind(); // render to the OIT framebuffer
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader& composite = SHADERS[OIT_COMPOSITE];
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer2.getColourTex(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer2.getColourTex(1));
	
	composite.bind();
	composite.setValue("accum", 0);
	composite.setValue("reveal", 1);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	composite.unBind();

	// 3. render the GUI
	guiFrameBuffer.bind(); // use the GUI framebuffer
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	showGUI();
	showFPS();


	m = "Position: " + glm::to_string(mainCamera->GetPosition());
	showText(m, { 5, 850 }, 0.5f);
	
	m = "View Direction: " + glm::to_string(mainCamera->GetFront());
	showText(m, { 5, 825 }, 0.5f);
	// 
	// m = "Controlling: Player";
	// showText(m, { 5, 80 }, 0.5f);

	//glm::vec2 p(0);
	// auto e = world.getChunkOccupied(player->getPosition());
	// if (e)  p = e->getPosition();
	// m = "Chunk Pos: " + glm::to_string(p);
	// showText(m, { 5, 775 }, 0.5f);

	// m = "Health: " + std::to_string(player->getHealth());
	// showText(m, { 5, 750 }, 0.5f);

	//ray.render(cam, projection);
	//guiFrameBuffer.unBind();

	// 4. render the screen quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // use the default frambuffer

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
	glDisable(GL_BLEND);
	
	glViewport(0, 0, windowDim.x, windowDim.y);
	
	const Shader& screenQuad = SHADERS[SCREEN_QUAD];
	screenQuad.bind();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer1.getColourTex(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, guiFrameBuffer.getColourTex(0));
	
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	
	screenQuad.unBind();

	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// opaue.bind();
	// world.render(*mainCamera, projection, LSM, depthMap, &opaue);
	// opaue.unBind();
}

void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}

void Game::setupPlayer() {
	// Entity p = Entity({ 0, 1.25f, 0 }, 1, 0);
	// GOD MODE VVV
	//Entity p = Entity({ 0, 0, 0 }, 0, 1);
	//p.setPosition({ 8, 26, 8 });
	//p.setTextues(Texture_Names::PLAYER_BOTTOM, Texture_Names::PLAYER_TOP);
	//PlayerInv& inv = p.getInventory();
	//// p.setInvincable(1);
	//entityHander.addEntity(p, 0);
}

void Game::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
		//Camera& cam = player->getCamera();
		//Ray ray = Ray(cam.GetPosition(), cam.GetFront(), PLAYER_REACH);
		// auto e = world.getIntersectedEntity(entityHander, ray);
		// if (!e) {
		// 	std::cout << "No Entity found" << std::endl;
		// }
		// else {
		// 	std::cout << "Entity found at: " << glm::to_string(e->getPosition()) << std::endl;
		// 	e->takeDamage(10);
		// }

	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS || action == GLFW_RELEASE) {
			Game::keys[key] ^= true;
		}
	}
	if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
		GameConfig::showFPS = !GameConfig::showFPS;
	}
}

void Game::mouseCallBack(GLFWwindow* window, double xPos, double yPos) {
	if ((int)Game::mouseData.z) {
		Game::mouseData.x = xPos;
		Game::mouseData.y = yPos;
		Game::mouseData.z = false;
	}

	float xOffset = xPos - Game::mouseData.x;
	float yOffset = Game::mouseData.y - yPos;

	Game::mouseData.x = xPos;
	Game::mouseData.y = yPos;
	Game::mainCamera->ProcessMouseMovement(xOffset, yOffset);

	//Game::player->updateCamera(xOffset, yOffset);
}

void Game::clickCallBack(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Camera& cam = player->getCamera();
		//Game::world.breakBlock(cam.GetPosition(), cam.GetFront());
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		// Camera& cam = player->getCamera();
		//Game::world.placeBlock(cam.GetPosition(), cam.GetFront(), player->getInventory().getBlockHotbar(player->getInventory().getHotbarSelected()));
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		// Camera& cam = player->getCamera();
		// entityHander.setTarget(cam.GetPosition());
	}
}

std::string num = "0";

void Game::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	int i = std::stoi(num);
	i -= yoffset;
	if (i > 8) {
		i = 9 - i;
	}
	else if (i < 0) {
		i = 9 + i;
	}
	num = std::to_string(i);
	uiRenderer.popWhere("slot_selected");
	UI_Element& element = uiRenderer.getWhere("slot" + num);
	UI_Element e = UI_Element(element.getPos(), element.getSize(), &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_SELECTED], "slot_selected");
	uiRenderer.appendElement(e);
	//player->getInventory().setHotbarSelected(i);
}

void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetMouseButtonCallback(window, Game::clickCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
	glfwSetScrollCallback(window, Game::scrollCallBack);
}

void Game::processKeys() {
	auto& k = Game::keys;
	float speed = 9.0f;
	if (k[GLFW_KEY_LEFT_CONTROL]) {
		speed = 20.0f;
		//player->setMovementSpeed(PLAYER_SPEED * 2.5f);
	} // running
	else {
		speed = 12.0f;
		//player->setMovementSpeed(PLAYER_SPEED);
	} // walking
	// prevents floaty behavour
	//player->setVelocity({ 0, player->getFlying() ? 0 : player->getVelocity().y, 0 });

	// if (k[GLFW_KEY_W]) {
	// 	player->move(Move_Dir::FORWARD);
	// }
	// if (k[GLFW_KEY_S]) {
	// 	player->move(Move_Dir::BACKWARD);
	// }
	// if (k[GLFW_KEY_A]) {
	// 	player->move(Move_Dir::LEFT);
	// }
	// if (k[GLFW_KEY_D]) {
	// 	player->move(Move_Dir::RIGHT);
	// }
	// if (k[GLFW_KEY_SPACE]) {
	// 	player->move(Move_Dir::UP);
	// }
	// if (k[GLFW_KEY_LEFT_SHIFT]) {
	// 	player->move(Move_Dir::DOWN);
	// }
	{
		if (k[GLFW_KEY_W]) {
			Game::mainCamera->GetPosition() += glm::normalize(Game::mainCamera->GetFront() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_S]) {
			Game::mainCamera->GetPosition() -= glm::normalize(Game::mainCamera->GetFront() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_A]) {
			Game::mainCamera->GetPosition() -= glm::normalize(Game::mainCamera->GetRight() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_D]) {
			Game::mainCamera->GetPosition() += glm::normalize(Game::mainCamera->GetRight() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_SPACE]) {
			Game::mainCamera->GetPosition() += glm::vec3(0, 1, 0) * speed * deltaTime;
		}
		if (k[GLFW_KEY_LEFT_SHIFT]) {
			Game::mainCamera->GetPosition() += glm::vec3(0, -1, 0) * speed * deltaTime;
		}
	}
	// Camera& cam = player->getCamera();
	// if (k[GLFW_KEY_0]) {
	// 	ray = Ray(cam.GetPosition(), cam.GetFront(), PLAYER_REACH, 1);
	// }

	//if (k[GLFW_KEY_1]) {
	//	num = "0";
	//	// player.setInvSlot(0);
	//}
	//if (k[GLFW_KEY_2]) {
	//	num = "1";
	//	// player.setInvSlot(1);
	//}
	//if (k[GLFW_KEY_3]) {
	//	num = "2";
	//	// player.setInvSlot(2);
	//}
	//if (k[GLFW_KEY_4]) {
	//	num = "3";
	//	// player.setInvSlot(3);
	//}
	//if (k[GLFW_KEY_5]) {
	//	num = "4";
	//	// player.setInvSlot(4);
	//}
	//if (k[GLFW_KEY_6]) {
	//	num = "5";
	//	// player.setInvSlot(5);
	//}
	//if (k[GLFW_KEY_7]) {
	//	num = "6";
	//	// player.setInvSlot(6);
	//}
	//if (k[GLFW_KEY_8]) {
	//	num = "7";
	//	// player.setInvSlot(7);
	//}
	//if (k[GLFW_KEY_9]) {
	//	num = "8";
	//	// player.setInvSlot(8);
	//}
	//if (num == "") return;
	//uiRenderer.popWhere("slot_selected");
	//UI_Element& element = uiRenderer.getWhere("slot" + num);
	//UI_Element e = UI_Element(element.getPos(), element.getSize(), TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_SELECTED], "slot_selected");
	//uiRenderer.appendElement(e);
	//player->getInventory().setHotbarSelected(std::stoi(num));
}

void Game::cleanUp() {
	oitFrameBuffer1.cleanUp();
	oitFrameBuffer2.cleanUp();
	guiFrameBuffer.cleanUp();
	shadowFramebuffer.cleanUp();

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	quadVAO = quadVBO = 0;
}

void Game::setUpScreenQuad()
{
	float vertices[] = {
		-1, -1,
		1, -1, 
		-1, 1, 
		1, 1
	};
	glGenVertexArrays(1, &quadVAO); // VAO
	glGenBuffers(1, &quadVBO); // VBO
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Game::makeSkybox(const std::string& skybox) {
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	unsigned int skyboxVBO;
	glGenVertexArrays(1, &SBVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(SBVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	auto shader = &SHADERS[SKYBOX];
	shader->bind();
	shader->setValue("skybox", 0);

}

void Game::showSkybox() {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer'transparent content
	SHADERS[SKYBOX].bind();
	// Camera& camera = player->getCamera();
	glm::mat4 view = glm::mat4(glm::mat3(mainCamera->GetViewMatrix())); // remove translation from the view matrix
	SHADERS[SKYBOX].setValue("view", view);
	SHADERS[SKYBOX].setValue("projection", projection);

	glBindVertexArray(SBVAO);
	TEXTURES[(int)Texture_Names::SKYBOX].bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	TEXTURES[(int)Texture_Names::SKYBOX].unBind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void Game::createGUI() {
	uiRenderer = UI_Renderer(&SHADERS[SHADER_NAMES::GUI]);
	UI_Element crosshair = UI_Element({ 0, 0 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::GUI], "crosshair");

	UI_Element slot0 = UI_Element({ -0.24, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot0");
	UI_Element slot1 = UI_Element({ -0.18, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot1");
	UI_Element slot2 = UI_Element({ -0.12, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot2");
	UI_Element slot3 = UI_Element({ -0.06, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot3");
	UI_Element slot4 = UI_Element({ 0, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot4"); // mid
	UI_Element slot5 = UI_Element({ 0.06, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot5");
	UI_Element slot6 = UI_Element({ 0.12, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot6");
	UI_Element slot7 = UI_Element({ 0.18, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot7");
	UI_Element slot8 = UI_Element({ 0.24, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_NORM], "slot8");

	UI_Element slotSelected = UI_Element({ -0.24, -0.945 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::BOARDER_SELECTED], "slot_selected");

	UI_Element heart0 = UI_Element({ -0.26, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart0");
	UI_Element heart1 = UI_Element({ -0.22, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart1");
	UI_Element heart2 = UI_Element({ -0.18, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart2");
	UI_Element heart3 = UI_Element({ -0.14, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart3");
	UI_Element heart4 = UI_Element({ -0.10, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart4");
	UI_Element heart5 = UI_Element({ -0.06, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart5");
	UI_Element heart6 = UI_Element({ -0.02, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart6");
	UI_Element heart7 = UI_Element({ 0.02, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart7");
	UI_Element heart8 = UI_Element({ 0.06, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart8");
	UI_Element heart9 = UI_Element({ 0.10, -0.83 }, { 0.3, 0.3 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::LIVE_HEART], "heart9");

	uiRenderer.appendElement(slot0);
	uiRenderer.appendElement(slot1);
	uiRenderer.appendElement(slot2);
	uiRenderer.appendElement(slot3);
	uiRenderer.appendElement(slot4);
	uiRenderer.appendElement(slot5);
	uiRenderer.appendElement(slot6);
	uiRenderer.appendElement(slot7);
	uiRenderer.appendElement(slot8);
	uiRenderer.appendElement(slotSelected);

	uiRenderer.appendElement(heart0);
	uiRenderer.appendElement(heart1);
	uiRenderer.appendElement(heart2);
	uiRenderer.appendElement(heart3);
	uiRenderer.appendElement(heart4);
	uiRenderer.appendElement(heart5);
	uiRenderer.appendElement(heart6);
	uiRenderer.appendElement(heart7);
	uiRenderer.appendElement(heart8);
	uiRenderer.appendElement(heart9);

	uiRenderer.appendElement(crosshair);
}

std::vector<Texture*> prevHotBar;
int prevPlayerHealth = 100;

void Game::showGUI() {
	uiRenderer.render();

	/*auto hotBar = player->getInventory().getHotBarTextures();
	if (hotBar != prevHotBar) {
		prevHotBar = hotBar;
		unsigned int i = 0;
		for (Texture* tex : hotBar) {
			if (tex) {
				UI_Element& slot = uiRenderer.getWhere("slot" + std::to_string(i++));
				uiRenderer.popWhere(slot.getName() + "_block");
				UI_Element item = UI_Element(slot.getPos(), slot.getSize(), tex, slot.getName() + "_block");
				uiRenderer.prependElement(item);
			}
		}
	}
	int playerHealth = player->getHealth();
	if (playerHealth != prevPlayerHealth) {
		int diff = 100 - playerHealth;
		diff = reduceToMultiple(diff, 10);
		diff /= 10;
		if (diff == 10) {
			for (GLubyte i = 0; i < 10; i++) {
				UI_Element& heartPos = uiRenderer.getWhere("heart" + std::to_string(i));
				uiRenderer.popWhere(heartPos.getName() + "_dead");
				UI_Element heart = UI_Element(heartPos.getPos(), heartPos.getSize(), TEXTURES2D[(unsigned int)Texture_Names_2D::DEAD_HEART], heartPos.getName() + "_dead");
				uiRenderer.appendElement(heart);
			}
		}
		else {
			for (GLubyte i = 9; i > 9 - diff; i--) {
				UI_Element& heartPos = uiRenderer.getWhere("heart" + std::to_string(i));
				uiRenderer.popWhere(heartPos.getName() + "_dead");
				UI_Element heart = UI_Element(heartPos.getPos(), heartPos.getSize(), TEXTURES2D[(unsigned int)Texture_Names_2D::DEAD_HEART], heartPos.getName() + "_dead");
				uiRenderer.appendElement(heart);
			}
		}
	}

	if (player->getSubmerged()) {
		UI_Element submerged = UI_Element({ 0, 0 }, { 16, 9 }, &getDetails(Block::WATER).ItemTex, "submerged");
		uiRenderer.popWhere(submerged.getName());
		uiRenderer.prependElement(submerged);
	}
	else {
		uiRenderer.popWhere("submerged");
	}*/
}

void Game::setUpFreeType() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "C://Windows/Fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48); // font size

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) // set active glyph
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Letters.insert({ c, character });
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	glGenVertexArrays(1, &LSVAO);
	glGenBuffers(1, &LSVBO);
	glBindVertexArray(LSVAO);
	glBindBuffer(GL_ARRAY_BUFFER, LSVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f);
	SHADERS[GLYPH].bind();
	SHADERS[GLYPH].setValue("projection", projection);
}

void Game::showText(const std::string& text, const glm::vec2& position, float scale, const glm::vec3 colour) {
	float x = position.x;
	const float y = position.y;

	auto s = &SHADERS[GLYPH];
	s->bind();
	s->setValue("textColor", colour);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(LSVAO);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character& ch = Letters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// Update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, LSVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}