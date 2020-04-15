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
Entity* Game::player = new Entity();
World Game::world = World(0);
EntityHander Game::entityHander = EntityHander();
UI_Renderer Game::uiRenderer = UI_Renderer();
#pragma endregion


Game::Game() {
	// hasPlayer = false;
	hasSkybox = false;
	gameRunning = false;
	lastFrameTime = -1.0f;
	Game::mainCamera = new Camera({ 0, 2, 0 });
	Game::mouseData = { 0, 0, -90 };
	GameConfig::setup();
}
Game::Game(GLboolean hasPlayer, GLboolean hasSkybox, glm::ivec2 windowDim) {
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
}

void Game::generateWorld() {
	world = World(1, 1, 0);
}
void Game::doLoop(glm::mat4 projection) {
	gameRunning = true;
	setupEventCB(window);
	this->projection = projection;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
	mainCamera->setPosition({ 8, 65, 8 });
	// temp vampire
	Entity vampire(1);
	vampire.getCollider().setDimentions({ 0.85, 0.85, 0.85 }, { 0.85, 2.55, 0.85 });
	vampire.setPosition({ 5, 80, 0 });
	vampire.setTextues(Texture_Names::VAMPIRE_BOTTOM, Texture_Names::VAMPIRE_TOP);
	
	setupDepthFBO();

	// entityHander.addEntity(vampire);

	while (gameRunning) {
		calcTimes();
		proccesEvents();
		processKeys();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world.advanceGeneration();


		std::vector<std::vector<ChunkColumn*>> adjacentChunkss;
		std::vector<ChunkColumn*> occuped;
		for (auto& e : entityHander.getEntitys()) {
			glm::vec3& p = e.getPosition();
			adjacentChunkss.push_back(world.getAdjacentChunks(p));
			occuped.push_back(world.getChunkOccupied(p));
		}
		player = &entityHander.getEntitys()[0];
		if (world.created) {
			entityHander.update(projection, player->getCamera(), adjacentChunkss, occuped, deltaTime);
		}

		showStuff();
		showFPS();


		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
	}
}
void Game::calcTimes() {
	GLfloat frame = glfwGetTime();
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
const GLuint SHADOW_WIDTH = 3072, SHADOW_HEIGHT = 3072;
void Game::showStuff() {
	Camera& cam = player->getCamera();
	Shader* s = SHADERS[DEPTH];
	s->bind();
	// light orhto projection
	float near_plane = 0.1f, far_plane = 100.0f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(LIGHTPOSITION, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 LSM = lightProjection * lightView;
	s->setValue("lightSpaceMatrix", LSM);
	s->setValue("lightPos", LIGHTPOSITION);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	// world.render(lightProjection, lightView);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	s->unBind();

	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, windowDim.x, windowDim.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	if (hasSkybox) {
		showSkybox();
	}

	world.render(cam, projection, LSM, depthMap);
	
	showGUI();

	m = "Position: " + glm::to_string(player->getPosition());
	showText(m, { 5, 850 }, 0.5f);

	m = "Controlling: Player";
	showText(m, { 5, 825 }, 0.5f);

	glm::vec2 p(0);
	auto e = world.getChunkOccupied(player->getPosition());
	if (e)  p = e->getPosition();
	m = "Chunk Pos: " + glm::to_string(p);
	showText(m, { 5, 800 }, 0.5f);

	m = "Health: " + std::to_string(player->getHealth());
	showText(m, { 5, 775 }, 0.5f);

	ray.render(cam, projection);
}
void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}
void Game::setupPlayer() {
	Entity p = Entity({ 0, 1.25f, 0 }, 1, 0);
	p.setPosition({ 8, 26, 8 });
	p.setTextues(Texture_Names::PLAYER_BOTTOM, Texture_Names::PLAYER_TOP);
	PlayerInv& inv = p.getInventory();
	// p.setInvincable(1);
	entityHander.addEntity(p, 0);
}
void Game::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		world.save();
		glfwSetWindowShouldClose(window, true);
	}
	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
		Camera& cam = player->getCamera();
		Ray ray = Ray(cam.GetPosition(), cam.GetFront(), PLAYER_REACH);
		auto e = world.getIntersectedEntity(entityHander, ray);
		if (!e) {
			std::cout << "No Entity found" << std::endl;
		}
		else {
			std::cout << "Entity found at: " << glm::to_string(e->getPosition()) << std::endl;
			e->takeDamage(10);
		}

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

	GLfloat xOffset = xPos - Game::mouseData.x;
	GLfloat yOffset = Game::mouseData.y - yPos;

	Game::mouseData.x = xPos;
	Game::mouseData.y = yPos;
	// Game::mainCamera->ProcessMouseMovement(xOffset, yOffset);

	Game::player->updateCamera(xOffset, yOffset);
}
void Game::clickCallBack(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Camera& cam = player->getCamera();
		Game::world.breakBlock(cam.GetPosition(), cam.GetFront());
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Camera& cam = player->getCamera();
		Game::world.placeBlock(cam.GetPosition(), cam.GetFront(), player->getInventory().getBlockHotbar(player->getInventory().getHotbarSelected()));
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		Camera& cam = player->getCamera();
		entityHander.setTarget(cam.GetPosition());
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
	UI_Element e = UI_Element(element.getPos(), element.getSize(), TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_SELECTED], "slot_selected");
	uiRenderer.appendElement(e);
	player->getInventory().setHotbarSelected(i);
}
void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetMouseButtonCallback(window, Game::clickCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
	glfwSetScrollCallback(window, Game::scrollCallBack);
}
void Game::processKeys() {
	auto& k = Game::keys;
	GLfloat speed = 9.0f;
	if (k[GLFW_KEY_LEFT_CONTROL]) {
		speed = 12.0f;
		player->setMovementSpeed(PLAYER_SPEED * 2.5f);
	} // running
	else {
		speed = 2.0f;
		player->setMovementSpeed(PLAYER_SPEED);
	} // walking
	player->setVelocity({ 0, player->getFlying() ? 0 : player->getVelocity().y, 0 });

	if (k[GLFW_KEY_W]) {
		player->move(Move_Dir::FORWARD);
	}
	if (k[GLFW_KEY_S]) {
		player->move(Move_Dir::BACKWARD);
	}
	if (k[GLFW_KEY_A]) {
		player->move(Move_Dir::LEFT);
	}
	if (k[GLFW_KEY_D]) {
		player->move(Move_Dir::RIGHT);
	}
	if (k[GLFW_KEY_SPACE]) {
		player->move(Move_Dir::UP);
	}
	if (k[GLFW_KEY_LEFT_SHIFT]) {
		player->move(Move_Dir::DOWN);
	}
	/*else {
		if (k[GLFW_KEY_W]) {
			Game::mainCamera->GetPosition() += Game::mainCamera->GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_S]) {
			Game::mainCamera->GetPosition() -= Game::mainCamera->GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_A]) {
			Game::mainCamera->GetPosition() -= Game::mainCamera->GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_D]) {
			Game::mainCamera->GetPosition() += Game::mainCamera->GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_SPACE]) {
			Game::mainCamera->GetPosition() += glm::vec3(0, 1, 0) * speed * deltaTime;
		}
		if (k[GLFW_KEY_LEFT_SHIFT]) {
			Game::mainCamera->GetPosition() += glm::vec3(0, -1, 0) * speed * deltaTime;
		}
	}*/
	Camera& cam = player->getCamera();
	world.updatePlayerPos(player->getPosition());
	if (k[GLFW_KEY_0]) {
		ray = Ray(cam.GetPosition(), cam.GetFront(), PLAYER_REACH, 1);
	}

	if (k[GLFW_KEY_1]) {
		num = "0";
		// player.setInvSlot(0);
	}
	if (k[GLFW_KEY_2]) {
		num = "1";
		// player.setInvSlot(1);
	}
	if (k[GLFW_KEY_3]) {
		num = "2";
		// player.setInvSlot(2);
	}
	if (k[GLFW_KEY_4]) {
		num = "3";
		// player.setInvSlot(3);
	}
	if (k[GLFW_KEY_5]) {
		num = "4";
		// player.setInvSlot(4);
	}
	if (k[GLFW_KEY_6]) {
		num = "5";
		// player.setInvSlot(5);
	}
	if (k[GLFW_KEY_7]) {
		num = "6";
		// player.setInvSlot(6);
	}
	if (k[GLFW_KEY_8]) {
		num = "7";
		// player.setInvSlot(7);
	}
	if (k[GLFW_KEY_9]) {
		num = "8";
		// player.setInvSlot(8);
	}
	if (num == "") return;
	uiRenderer.popWhere("slot_selected");
	UI_Element& element = uiRenderer.getWhere("slot" + num);
	UI_Element e = UI_Element(element.getPos(), element.getSize(), TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_SELECTED], "slot_selected");
	uiRenderer.appendElement(e);
	player->getInventory().setHotbarSelected(std::stoi(num));
}
void Game::cleanUp() {
}

void Game::makeSkybox(std::string skybox) {
	GLfloat skyboxVertices[] = {
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
	GLuint skyboxVBO;
	glGenVertexArrays(1, &SBVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(SBVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	auto& shader = SHADERS[SKYBOX];
	shader->bind();
	shader->setValue("skybox", 0);

}
void Game::showSkybox() {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	SHADERS[SKYBOX]->bind();
	Camera& camera = player->getCamera();
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	SHADERS[SKYBOX]->setValue("view", view);
	SHADERS[SKYBOX]->setValue("projection", projection);

	glBindVertexArray(SBVAO);
	TEXTURES[(int)Texture_Names::SKYBOX]->bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	TEXTURES[(int)Texture_Names::SKYBOX]->unBind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void Game::createGUI() {
	uiRenderer = UI_Renderer(SHADERS[SHADER_NAMES::CROSSHAIR]);
	UI_Element crosshair = UI_Element({ 0, 0 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::CROSSHAIR], "crosshair");

	UI_Element slot0 = UI_Element({ -0.24, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot0");
	UI_Element slot1 = UI_Element({ -0.18, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot1");
	UI_Element slot2 = UI_Element({ -0.12, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot2");
	UI_Element slot3 = UI_Element({ -0.06, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot3");
	UI_Element slot4 = UI_Element({ 0, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot4"); // mid
	UI_Element slot5 = UI_Element({ 0.06, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot5");
	UI_Element slot6 = UI_Element({ 0.12, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot6");
	UI_Element slot7 = UI_Element({ 0.18, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot7");
	UI_Element slot8 = UI_Element({ 0.24, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_NORM], "slot8");

	UI_Element slotSelected = UI_Element({ -0.24, -0.945 }, { 0.5, 0.5 }, TEXTURES2D[(GLuint)Texture_Names_2D::BOARDER_SELECTED], "slot_selected");

	UI_Element heart0 = UI_Element({ -0.26, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart0");
	UI_Element heart1 = UI_Element({ -0.22, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart1");
	UI_Element heart2 = UI_Element({ -0.18, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart2");
	UI_Element heart3 = UI_Element({ -0.14, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart3");
	UI_Element heart4 = UI_Element({ -0.10, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart4");
	UI_Element heart5 = UI_Element({ -0.06, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart5");
	UI_Element heart6 = UI_Element({ -0.02, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart6");
	UI_Element heart7 = UI_Element({ 0.02, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart7");
	UI_Element heart8 = UI_Element({ 0.06, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart8");
	UI_Element heart9 = UI_Element({ 0.10, -0.83 }, { 0.3, 0.3 }, TEXTURES2D[(GLuint)Texture_Names_2D::LIVE_HEART], "heart9");

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
GLint prevPlayerHealth = 100;
void Game::showGUI() {
	uiRenderer.render();

	auto hotBar = player->getInventory().getHotBarTextures();
	if (hotBar != prevHotBar) {
		prevHotBar = hotBar;
		GLuint i = 0;
		for (Texture* tex : hotBar) {
			if (tex) {
				UI_Element& slot = uiRenderer.getWhere("slot" + std::to_string(i++));
				uiRenderer.popWhere(slot.getName() + "_block");
				UI_Element item = UI_Element(slot.getPos(), slot.getSize(), tex, slot.getName() + "_block");
				uiRenderer.prependElement(item);
			}
		}
	}
	GLint playerHealth = player->getHealth();
	if (playerHealth != prevPlayerHealth) {
		GLint diff = 100 - playerHealth;
		diff = reduceToMultiple(diff, 10);
		diff /= 10;
		if (diff == 10) {
			for (GLubyte i = 0; i < 10; i++) {
				UI_Element& heartPos = uiRenderer.getWhere("heart" + std::to_string(i));
				uiRenderer.popWhere(heartPos.getName() + "_dead");
				UI_Element heart = UI_Element(heartPos.getPos(), heartPos.getSize(), TEXTURES2D[(GLuint)Texture_Names_2D::DEAD_HEART], heartPos.getName() + "_dead");
				uiRenderer.appendElement(heart);
			}
		}
		else {
			for (GLubyte i = 9; i > 9 - diff; i--) {
				UI_Element& heartPos = uiRenderer.getWhere("heart" + std::to_string(i));
				uiRenderer.popWhere(heartPos.getName() + "_dead");
				UI_Element heart = UI_Element(heartPos.getPos(), heartPos.getSize(), TEXTURES2D[(GLuint)Texture_Names_2D::DEAD_HEART], heartPos.getName() + "_dead");
				uiRenderer.appendElement(heart);
			}
		}
	}

	if (player->getSubmerged()) {
		UI_Element submerged = UI_Element({ 0, 0 }, { 16, 9 }, &getDets(Blocks::WATER).ItemTex, "submerged");
		uiRenderer.popWhere(submerged.getName());
		uiRenderer.prependElement(submerged);
	}
	else {
		uiRenderer.popWhere("submerged");
	}
}

void Game::setupDepthFBO()
{
	glGenFramebuffers(1, &depthFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "hhhhhhhhh\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		GLuint texture;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f);
	SHADERS[GLYPH]->bind();
	SHADERS[GLYPH]->setValue("projection", projection);
}
void Game::showText(std::string text, glm::vec2 position, GLfloat scale, glm::vec3 colour) {
	GLfloat& x = position.x;
	GLfloat& y = position.y;

	auto& s = SHADERS[GLYPH];
	s->bind();
	s->setValue("textColor", colour);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(LSVAO);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character& ch = Letters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
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