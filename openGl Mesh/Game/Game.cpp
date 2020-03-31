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
Player Game::player = Player();
GLboolean Game::hasPlayer = GL_FALSE;
World Game::world = World(0);
#pragma endregion


Game::Game() {
	hasPlayer = false;
	hasSkybox = false;
	gameRunning = false;
	lastFrameTime = -1.0f;
	Game::mainCamera = new Camera({ 0, 2, 0 });
	Game::mouseData = { 0, 0, -90 };
	GameConfig::setup();
}
Game::Game(GLboolean hasPlayer, GLboolean hasSkybox) {
	this->hasPlayer = hasPlayer;
	this->hasSkybox = hasSkybox;
	setupPlayer();
	gameRunning = false;
	Game::mouseData = { 0, 0, -90 };
	lastFrameTime = -1.0f;
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
	mainCamera->setPosition({ 8, 65, 8 });
	while (gameRunning) {
		calcTimes();
		lockFPS();
		proccesEvents();
		processKeys();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world.advanceGeneration();

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
GLboolean alt = 0;
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
void Game::showStuff() {
	Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
	if (hasPlayer) {
		player.render(projection);
	}
	world.render(cam, projection);
	if (hasSkybox) {
		showSkybox();
	}
	showGUI();
	m = "Position: ";
	if (hasPlayer) {
		m += glm::to_string(player.getPosition());
	}
	else {
		m += glm::to_string(cam.GetPosition());
	}
	showText(m, { 5, 850 }, 0.5f);
	m = "Controlling: ";
	if (alt) {
		m += "Player";
	}
	else {
		m += "Camera";
	}
	showText(m, { 5, 825 }, 0.5f);
	glm::vec2 p(0);
	auto e = world.getChunkOccupied(hasPlayer ? player.getPosition() : mainCamera->GetPosition());
	if (e)  p = e->getPosition();
	m = "Chunk Pos: " + glm::to_string(p);
	showText(m, { 5, 800 }, 0.5f);
	ray.render(cam, projection);
}
void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}
void Game::setupPlayer() {
	player = Player({ 1.0f, 260.0f, 1.0f }, { 0.0f, 1.25f, 0.0f }, 1);
	player.create();
}
void Game::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		world.save();
		glfwSetWindowShouldClose(window, true);
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
	if (Game::hasPlayer) {
		Game::player.updateCamera(xOffset, yOffset);
		// Game::player.processMouse(xOffset, yOffset, mouseData.x);
		return;
	}
	Game::mainCamera->ProcessMouseMovement(xOffset, yOffset);
}
void Game::clickCallBack(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
		Game::world.breakBlock(cam.GetPosition(), cam.GetFront());
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
		Game::world.placeBlock(cam.GetPosition(), cam.GetFront(), player.getInvBar()[player.getSlot()]);
	}
}
void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetMouseButtonCallback(window, Game::clickCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
}
void Game::processKeys() {
	auto& k = Game::keys;
	GLfloat speed = 9.0f;
	if (k[GLFW_KEY_LEFT_CONTROL]) {
		speed = 12.0f;
		player.setMovementSpeed(PLAYER_SPEED * 2.5f);
	}
	else {
		speed = 2.0f;
		player.setMovementSpeed(PLAYER_SPEED);
	}
	if (Game::hasPlayer) {
		player.setVelocity({ 0, player.getVelocity().y, 0 });
		if (k[GLFW_KEY_LEFT_ALT] && (GLuint)time(NULL) % 2 == 0) {
			alt = !alt;
		}
		if (alt || 0) {
			if (k[GLFW_KEY_W]) {
				player.move(Move_Dir::FORWARD);
			}
			if (k[GLFW_KEY_S]) {
				player.move(Move_Dir::BACKWARD);
			}
			if (k[GLFW_KEY_A]) {
				player.move(Move_Dir::LEFT);
			}
			if (k[GLFW_KEY_D]) {
				player.move(Move_Dir::RIGHT);
			}
			if (k[GLFW_KEY_SPACE]) {
				player.move(Move_Dir::UP);
			}
		}
		else {
			auto& c = player.getCamera();
			if (k[GLFW_KEY_W]) {
				c.GetPosition() += c.GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_S]) {
				c.GetPosition() -= c.GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_A]) {
				c.GetPosition() -= c.GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_D]) {
				c.GetPosition() += c.GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_SPACE]) {
				c.GetPosition() += glm::vec3(0, 1, 0) * speed * deltaTime;
			}
			if (k[GLFW_KEY_LEFT_SHIFT]) {
				c.GetPosition() += glm::vec3(0, -1, 0) * speed * deltaTime;
			}
		}

		m = "Collison: " + player.updatePosition(Game::deltaTime, world);
	}
	else {
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
	}
	Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
	world.updatePlayerPos(cam.GetPosition());
	if (k[GLFW_KEY_0]) {
		ray = Ray(cam.GetPosition(), cam.GetFront(), PLAYER_REACH, 1);
	}

	if (k[GLFW_KEY_1]) {
		player.setInvSlot(0);
	}
	if (k[GLFW_KEY_2]) {
		player.setInvSlot(1);
	}
	if (k[GLFW_KEY_3]) {
		player.setInvSlot(2);
	}
	if (k[GLFW_KEY_4]) {
		player.setInvSlot(3);
	}
	if (k[GLFW_KEY_5]) {
		player.setInvSlot(4);
	}
	if (k[GLFW_KEY_6]) {
		player.setInvSlot(5);
	}
	if (k[GLFW_KEY_7]) {
		player.setInvSlot(6);
	}
	if (k[GLFW_KEY_8]) {
		player.setInvSlot(7);
	}
	if (k[GLFW_KEY_9]) {
		player.setInvSlot(8);
	}
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
	Camera& camera = hasPlayer ? Game::player.getCamera() : *Game::mainCamera;
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	SHADERS[SKYBOX]->setValue("view", view);
	SHADERS[SKYBOX]->setValue("projection", projection);

	glBindVertexArray(SBVAO);
	TEXTURES[(int)Texture_Names::SKYBOX]->bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDepthFunc(GL_LESS); // set depth function back to default
}

void Game::createGUI() {
	createCrossHair();
	texBN = Texture("boarders/normal", 1);
	texBS = Texture("boarders/selected", 1);
}
void Game::showGUI() {
	showCrossHair();
	auto invBar = [](std::array<Blocks, 9> bar, glm::vec3 center, Texture& texNorm, Texture& texSele, Shader& shader, GLuint VAO) {
		GLubyte length = 9;
		shader.bind();
		glBindVertexArray(VAO); 
		glm::vec3 delta(0.5, 0, 0);
		for(GLbyte i = -length/2; i < length/2 +1; i++) {
			glm::vec3 delta1(delta.x * i, 0, 0);
			glm::vec3 pos = center + delta1;
			

			shader.setValue("modelPos", pos);

			Blocks block = bar[i + length / 2];
			if (block != Blocks::AIR) {
				BLOCKS[block].ItemTex.bind();
				glDrawArrays(GL_TRIANGLES, 0, 6);
				BLOCKS[block].ItemTex.unBind();
			}
			if (player.getSlot() == i + length / 2) {
				texSele.bind();
			}
			else {
				texNorm.bind();
			}
			glDrawArrays(GL_TRIANGLES, 0, 6);
			texNorm.unBind();
		}
		glBindVertexArray(0);
		shader.unBind();
		texNorm.unBind();
	};
	Shader& shader = *SHADERS[CROSSHAIR];
	std::array<Blocks, 9> bar = {
		Blocks::GRASS,
		Blocks::DIRT,
		Blocks::STONE,
		Blocks::WATER,
		Blocks::AIR,
		Blocks::AIR,
		Blocks::AIR,
		Blocks::AIR,
		Blocks::AIR
	};
	if (hasPlayer) bar = player.getInvBar();
	invBar(bar, { 0, -9.5, 0 }, texBN, texBS, shader, CHVAO);
}

void Game::createCrossHair() {
	texCH = Texture("crosshair", 1);
	GLfloat vertices[] = {
		0.25f,  0.45f, 0.0f,  1, 1,
		0.25f, -0.45f, 0.0f,  1, 0,
		-0.25f,  0.45f, 0.0f, 0, 1,

		0.25f, -0.45f, 0.0f,  1, 0,
		-0.25f, -0.45f, 0.0f, 0, 0,
		-0.25f,  0.45f, 0.0f, 0, 1
	};
	GLuint vbo;
	glGenVertexArrays(1, &CHVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(CHVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	auto& shader = SHADERS[CROSSHAIR];
	shader->bind();
	texCH.bind();
	auto scale = glm::vec3(0.1f);
	shader->setValue("scale", scale);
	shader->setValue("texture0", 0);
	shader->unBind();
}
void Game::showCrossHair() {
	glm::vec3 scale(0.1);
	auto& shader = SHADERS[CROSSHAIR];
	shader->bind();
	glm::vec3 pos(0, 0, 0);
	shader->setValue("modelPos", pos);
	shader->setValue("scale", scale);
	texCH.bind();
	glBindVertexArray(CHVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	shader->unBind();
	texCH.unBind();
}

void Game::setUpFreeType() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
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
		x += (ch.Advance >> 6)* scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}