#include "Game.h"
#include <gtx/string_cast.hpp>
#include <gtc/random.hpp>
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"
#include "../Mangers/EntityManager.h"
#include "../Mangers/ModelManager.h"
#include "../Mangers/GizmoManager.h"
#include "../IndexedBuffer.h"
#include "../Helpers/ModelLoaders/ModelLoader.h"
#include "../Helpers/ModelLoaders/Model.h"
#include "../Helpers/ShadowBox.h"
#include "../Helpers/Functions.h"
#include "../Gizmos/Composite/Grid2D.h"
#include "../Material.h"

#pragma region GameConfig
bool GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.5, 0.5, 0.5 };
unsigned int GameConfig::FPSlock = 0;
#pragma endregion


Camera Game::mainCamera;
glm::vec3 Game::mouseData;
std::array<bool, 1024> Game::keys;
World Game::world;
UI_Renderer Game::uiRenderer;

const glm::vec3 lightPos(100, 30, 100);

Game::Game() : window(), deltaTime(), frameRate(), gameRunning(false), lastFrameTime(-1), guiFrameBuffer(), quadVAO(), quadVBO(), multiPurposeFB(),
SBVAO(0), LSVAO(), Letters(), windowDim(), LSVBO(), oitFrameBuffer1(), oitFrameBuffer2(), modelRenderer(), gBuffer(), materialsBuffer() {
	mainCamera = Camera({ 0, 2, 0 });
	mouseData = { 0, 0, -90 };
	GameConfig::setup();
	setUpScreenQuad();
	manager = &EntityManager::getInstance(); 
	gizmoManager = &GizmoManager::getInstance();

	materialsBuffer.allocate(sizeof(Material) * MATERIALS.size(), 0);
	materialsBuffer.fill(0, sizeof(Material) * MATERIALS.size(), MATERIALS.data());
}

Game::Game(glm::ivec2 windowDim) : Game() {
	gameRunning = false;
	Game::mouseData = { 0, 0, -90 };
	lastFrameTime = -1.0f;
	this->windowDim = windowDim;

	makeSkybox("skybox");
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


	// DEFERED RENERING
	FrameBufferInit gBufferDetails; // may need to make this depth texture the one used by the transparancy buffer
	gBufferDetails.hasDepth = true;
	gBufferDetails.depthTexture = true;

	ColourBufferInit albedoPos;
	albedoPos.format = GL_FLOAT;
	albedoPos.internalFormat = GL_RGBA32F;
	albedoPos.type = GL_RGBA;

	ColourBufferInit normalRnd;
	normalRnd.format = GL_FLOAT;
	normalRnd.internalFormat = GL_RGBA16F;
	normalRnd.type = GL_RGBA;

	ColourBufferInit worldPos;
	worldPos.format = GL_FLOAT;
	worldPos.internalFormat = GL_RGBA;
	worldPos.type = GL_RGBA;

	gBufferDetails.colourBuffers = { albedoPos, normalRnd, worldPos };

	gBuffer = FrameBuffer(windowDim);
	gBuffer.setUp(gBufferDetails);

	// TRANSPARENT BUFFER
	ColourBufferInit accumOIT;
	accumOIT.format = GL_FLOAT;
	accumOIT.internalFormat = GL_RGBA;
	accumOIT.type = GL_RGBA;

	ColourBufferInit revealOIT;
	revealOIT.format = GL_FLOAT;
	revealOIT.internalFormat = GL_R8;
	revealOIT.type = GL_RED;

	detailsOIT.colourBuffers = { accumOIT, revealOIT };
	detailsOIT.depthBuffer = gBuffer.getDepth();

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

	shadowFramebuffer = FrameBuffer({ SHADOW_MAP_SIZE , SHADOW_MAP_SIZE });
	shadowFramebuffer.setUp(detailsShadows);

	FrameBufferInit multiPurposeFBDetails;
	multiPurposeFBDetails.hasDepth = false;
	multiPurposeFBDetails.depthTexture = false;

	ColourBufferInit mpInput;
	mpInput.format = GL_FLOAT;
	mpInput.internalFormat = GL_RGBA;
	mpInput.type = GL_RGBA;

	ColourBufferInit mpOutput;
	mpOutput.format = GL_FLOAT;
	mpOutput.internalFormat = GL_RGBA;
	mpOutput.type = GL_RGBA;


	multiPurposeFBDetails.colourBuffers = { mpOutput, mpInput };
	multiPurposeFB = FrameBuffer(windowDim);
	multiPurposeFB.setUp(multiPurposeFBDetails);
}

void Game::generateWorld() {
	srand(time(0));
	world = World(WORLD_ORIGIN, 32);
	world.setUpDrawable();
}

void Game::doLoop(const glm::mat4& projection) {
	gameRunning = true;
	setupEventCB(window);
	
	manager->awakeEvent();

	manager->startEvent();
	
	mainCamera.setPosition({ 8, 25, 15 });

	// LOAD MODELS
	//ModelManager& modelManager = ModelManager::getInstance();
	//auto buffer = modelManager.load("C:\\Users\\AGWDW\\Desktop\\ncc1701d.obj");
	//std::cout << "Model Loaded" << std::endl;
	//addModel(buffer);
	// auto mesh = ModelLoader::Load("C:\\Users\\AGWDW\\Desktop\\cube.obj");

#ifdef SSAO
	setUpSSAO();
#endif // SSAO

	while (gameRunning) {
		calcTimes();
		glfwPollEvents();
		processKeys();

		glm::vec3 p = floor(mainCamera.GetPosition());
		glm::ivec2 c(p.x, p.z);
		reduceToMultiple(c, CHUNK_SIZE * VOXEL_SZIE);

		GizmoManager& gizmoManager = GizmoManager::getInstance();
#ifdef DEBUG_GRID_LINES
		unsigned int i = 0;
		for (Gizmo::IShape* grid : gizmoManager.allGizmos) {
			grid->setPosition(glm::vec3(c.x, -(int)WORLD_HEIGHT, c.y) + GRID_LINE_POSITIONS[i++]);
		}
#endif // DEBUG_GRID_LINES

		

		c /= CHUNK_SIZE * VOXEL_SZIE;
		world.tryStartGenerateChunks(c);
		// std::cout << "Generated" << std::endl;

		manager->updateEvent();

		manager->fixedUpdateEvent();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		showStuff(projection);

		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
	}

	manager->destroyEvent();
}

void Game::addModel(Model& model) {
	modelRenderer.add(model);
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

void Game::renderModels(const glm::mat4& projection) {
	Shader* modelShader = &SHADERS[MODEL];
	modelShader->bind();
	modelShader->setValue("view", mainCamera.GetViewMatrix());
	modelShader->setValue("projection", projection);
	modelRenderer.render(modelShader);
	modelShader->unBind();
}

void Game::showStuff(const glm::mat4& projection) {
	const glm::mat4 LSM = ShadowBox::getLSM(mainCamera, projection, lightPos);
	const glm::mat4 viewMatrix = mainCamera.GetViewMatrix();
	// 1. render from the lights perspective for the shadow map
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glFrontFace(GL_CCW); // this is what stops the water from rendering

	shadowFramebuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	Shader& shadows = SHADERS[SHADOW];
	shadows.bind();
	shadows.setValue("lightMatrix", LSM);
	world.render(&shadows);
	shadows.unBind();
	shadowFramebuffer.unBind();
	// 2. render for OIT
	// 2.1 Opaque
	
	// 2.1.1 Populate G-Buffer
	gBuffer.bind();
	glDisable(GL_BLEND);
	glFrontFace(GL_CW);
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	Shader& gbufferS = SHADERS[GBUFFER];
	gbufferS.bind();
	gbufferS.setValue("view", viewMatrix);
	gbufferS.setValue("projection", projection);
	gbufferS.setValue("numBlocks", 8.f);

	world.render(&gbufferS);
	gizmoManager->render(projection * viewMatrix);
	//renderModels(projection);
	manager->renderEvent();

	// 2.1.2 Ambiant Occlusion (render to the oit opaque buffer)
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	oitFrameBuffer1.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Shader& ssao = SHADERS[AO];
	ssao.bind();
	ssao.setValue("numBlocks", (float) MATERIALS.size());
	ssao.setValue("albedoPos", 0);
	ssao.setValue("normalRnd", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(1));
#ifdef SSAO
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);
	ssao.setValue("ssaoNoise", 2);
	glUniform3fv(glGetUniformLocation(ssao.getId(), "ssaoSamples"), ssaoSamples.size(), &ssaoSamples[0][0]);
	glm::vec2 noiseScale = ((glm::vec2)windowDim) / SSAO_SCALE;
	ssao.setValue("ssaoNoiseScale", noiseScale);
	ssao.setValue("ssaoRadius", SSAO_RADIUS);
	ssao.setValue("ssaoBias", SSAO_BIAS);
	ssao.setValue("projection", projection);
	ssao.setValue("view", viewMatrix);
#endif // SSAO

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// 2.1.3 Blurs Ambiant Occlusion (renders to the multi purpose buffer)
	multiPurposeFB.bind();
	Shader& blur = SHADERS[BLUR];
	blur.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer1.getColourTex(0));
	blur.setValue("img", 0);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// 2.1.4 OIT Opaque (renders into the oit1 buffer and apply AO)
	oitFrameBuffer1.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Shader& deffered = SHADERS[DEFFERED];
	deffered.bind();
	deffered.setValue("numBlocks", (float)MATERIALS.size());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(1));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(2));

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, multiPurposeFB.getColourTex(0));

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.getDepth());

	deffered.setValue("albedoPos", 0);
	deffered.setValue("normalRnd", 1);
	deffered.setValue("worldPos", 2);
	deffered.setValue("ao", 3);
	deffered.setValue("shadowMap", 4);
	deffered.setValue("viewDir", mainCamera.GetFront());
	deffered.setValue("view_inv", glm::inverse(viewMatrix));
	deffered.setValue("lightMatrix", LSM);
	materialsBuffer.bind();
	deffered.setValueUBO("Mats", 0);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	renderModels(projection);

	// 2.2 Transparent
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	oitFrameBuffer2.bind(); // render to the OIT framebuffer2
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);
	
	glm::vec4 black(0);
	glm::vec4 white(1);
	glClearBufferfv(GL_COLOR, 0, &black[0]);
	glClearBufferfv(GL_COLOR, 1, &white[0]);
	
	Shader& transparent = SHADERS[OIT_TRANSPARENT];
	transparent.bind();
	transparent.setValue("view", viewMatrix);
	transparent.setValue("projection", projection);

	world.render(&transparent);

	transparent.unBind();

	// 2.3 Composite
	glDisable(GL_CULL_FACE);
	oitFrameBuffer1.bind(); // render to the OIT framebuffer
	// showSkybox(projection);
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

	std::string m;
	m = "Position: " + glm::to_string(mainCamera.GetPosition());
	showText(m, { 5, 850 }, 0.5f);
	
	m = "View Direction: " + glm::to_string(mainCamera.GetFront());
	showText(m, { 5, 825 }, 0.5f);
	glm::vec3 p = floor(mainCamera.GetPosition());
	glm::ivec2 c(p.x, p.z);
	reduceToMultiple(c, CHUNK_SIZE);
	c /= CHUNK_SIZE;

	m = "Chunk Pos: " + glm::to_string(c);
	showText(m, { 5, 800 }, 0.5f);

	// 4. render the screen quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // use the default frambuffer

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
	glDisable(GL_BLEND);
	
	glViewport(0, 0, windowDim.x, windowDim.y);
	
	const Shader& screenQuad = SHADERS[SCREEN_QUAD];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer1.getColourTex(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, guiFrameBuffer.getColourTex(0));

	screenQuad.bind();
	
	screenQuad.setValue("screen", 0);
	screenQuad.setValue("gui", 1);
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
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS || action == GLFW_RELEASE) {
			Game::keys[key] ^= true;
		}
	}
	if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
		GameConfig::showFPS = !GameConfig::showFPS;
	}
	/*glBindFramebuffer(GL_FRAMEBUFFER, 1);
	if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
		float zcoord = 0;
		glReadPixels(WIDTH * .5f, HEIGHT * .5f, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zcoord);
		glm::mat4 invPV = glm::inverse(projection * mainCamera->GetViewMatrix());
		float z = (zcoord - 0.5f) * 2.0f;

		world.breakBlock(z, invPV, mainCamera->GetFront());
	}
	if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
		float zcoord = 0;
		glReadPixels(WIDTH * .5f, HEIGHT * .5f, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zcoord);
		glm::mat4 invPV = glm::inverse(projection * mainCamera->GetViewMatrix());
		float z = (zcoord - 0.5f) * 2.0f;

		world.placeBlock(z, invPV, mainCamera->GetFront());
	}*/
	if (key == GLFW_KEY_H && action == GLFW_RELEASE) {
		world.save();
		std::cout << "Saved" << std::endl;
	}
	if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
		glm::vec3 p = floor(mainCamera.GetPosition());
		glm::ivec2 c(p.x, p.z);
		reduceToMultiple(c, CHUNK_SIZE);
		c /= CHUNK_SIZE;
		world.tryStartGenerateChunks(c);
		std::cout << "Generated" << std::endl;
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
	Game::mainCamera.ProcessMouseMovement(xOffset, yOffset);
}

void Game::clickCallBack(GLFWwindow* window, int button, int action, int mods) {
	/*if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		float zcoord = 0;
		glReadPixels(WIDTH * .5f, HEIGHT * .5f, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zcoord);
		glm::mat4 invPV = glm::inverse(projection * mainCamera->GetViewMatrix());
		float z = (zcoord - 0.5f) * 2.0f;

		world.breakBlock(z, invPV, mainCamera->GetFront());
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		float zcoord = 0;
		glReadPixels(WIDTH * .5f, HEIGHT * .5f, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zcoord);
		glm::mat4 invPV = glm::inverse(projection * mainCamera->GetViewMatrix());
		float z = (zcoord - 0.5f) * 2.0f;

		world.placeBlock(z, invPV, mainCamera->GetFront());
	}*/
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
	} // running
	else {
		speed = 12.0f;
	} // walking
	
	{
		if (k[GLFW_KEY_W]) {
			Game::mainCamera.GetPosition() += glm::normalize(Game::mainCamera.GetFront() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_S]) {
			Game::mainCamera.GetPosition() -= glm::normalize(Game::mainCamera.GetFront() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_A]) {
			Game::mainCamera.GetPosition() -= glm::normalize(Game::mainCamera.GetRight() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_D]) {
			Game::mainCamera.GetPosition() += glm::normalize(Game::mainCamera.GetRight() * glm::vec3(1, 0, 1)) * speed * deltaTime;
		}
		if (k[GLFW_KEY_SPACE]) {
			Game::mainCamera.GetPosition() += glm::vec3(0, 1, 0) * speed * deltaTime;
		}
		if (k[GLFW_KEY_LEFT_SHIFT]) {
			Game::mainCamera.GetPosition() += glm::vec3(0, -1, 0) * speed * deltaTime;
		}
	}
}

void Game::cleanUp() {
	oitFrameBuffer1.cleanUp();
	oitFrameBuffer2.cleanUp();
	guiFrameBuffer.cleanUp();
	shadowFramebuffer.cleanUp();
	gBuffer.cleanUp();
	multiPurposeFB.cleanUp();

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

void Game::showSkybox(const glm::mat4& projection) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer'transparent content
	SHADERS[SKYBOX].bind();
	// Camera& camera = player->getCamera();
	glm::mat4 view = glm::mat4(glm::mat3(mainCamera.GetViewMatrix())); // remove translation from the view matrix
	SHADERS[SKYBOX].setValue("view", view);
	SHADERS[SKYBOX].setValue("projection", projection);

	glBindVertexArray(SBVAO);
	TEXTURES3D[(int)Texture_Names::SKYBOX].bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	TEXTURES3D[(int)Texture_Names::SKYBOX].unBind();
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


#ifdef SSAO
void Game::setUpSSAO()
{
	auto lerp = [](float a, float b, float c) {
		return a + c * (b - a);
	};
	// THE SAMPLES
	float inv_num = 1.f / (float)SSAO_NUM_SAMPLES;
	for (unsigned int i = 0; i < SSAO_NUM_SAMPLES; i++) {
		glm::vec3 r = glm::gaussRand(glm::vec3(0, 0, .5), glm::vec3(1, 1, .5));
		r = glm::normalize(r);
		float s = i * inv_num;
		s = lerp(0.1f, 1.f, s * s);
		r *= s;
		ssaoSamples[i] = r;
	}
	// THE NOISE
	std::array<glm::vec3, (size_t)(SSAO_SCALE* SSAO_SCALE)> noiseData{};
	for (unsigned int i = 0; i < noiseData.size(); i++) {
		glm::vec3 r = glm::gaussRand(glm::vec3(0), glm::vec3(1));
		r.z = 0;
		noiseData[i] = r;
	}

	glGenTextures(1, &ssaoNoiseTex);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SSAO_SCALE, SSAO_SCALE, 0, GL_RGB, GL_FLOAT, &noiseData[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
#endif // SSAO