#include "Game.h"
#include <gtx/string_cast.hpp>
#include <gtc/random.hpp>
#include <iostream>
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"
#include "../Mangers/EntityManager.h"
#include "../Mangers/ModelManager.h"
#include "../Mangers/GizmoManager.h"
#include "../Mangers/PhysicsManager.h"
#include "../IndexedBuffer.h"
#include "../Helpers/ModelLoaders/ModelLoader.h"
#include "../Helpers/ModelLoaders/Model.h"
#include "../Helpers/ShadowBox.h"
#include "../Helpers/Functions.h"
#include "../Gizmos/Composite/Grid2D.h"
#include "../Material.h"
#include "../Helpers/Timers/Timer.h"
#include "../EntityComponent/Entities/Player.h"
#include "../EntityComponent/Entities/TestObject.h"
#include "../EntityComponent/Components/Transform.h"
#include "../EntityComponent/Components/RigidBody.h"
#include "../EntityComponent/Components/RenderMesh.h"
// envents stuff
#include "../Mangers/EventsManager.h"
#include "../EventsSystem/EventCallback.h"
#include "../EventsSystem/EventDetails/ClickEventInfo.h"
#include "../EventsSystem/EventDetails/KeyEventInfo.h"
// New GUI
#include "../GUI/Containers/StackContainer.h"
#include "../GUI/Containers/BasicContainer.h"
#include "../GUI/Utils/Text/GlyphRendering.h"

#pragma region GameConfig
bool GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.5, 0.5, 0.5 };
unsigned int GameConfig::FPSlock = 0;
#pragma endregion


glm::vec3 Game::mouseData;
glm::vec2 Game::mouseOffset;
std::array<bool, 1024> Game::keys;

Game::Game() : window(), deltaTime(), frameRate(), gameRunning(false), lastFrameTime(-1), quadVAO(), quadVBO(), multiPurposeFB(), guiWindow(), _world(),
	skyVAO(0), skyVBO(0), windowDim(), oitFrameBuffer1(), gBuffer(), camreraBuffer(), materialsBuffer(), _player(), _fpsCounter(), _playerPosition(), _viewDirection(), _numChunks() {
	mouseData = { 0, 0, -90 };
	GameConfig::setup();

	setUpScreenQuad();
	manager = &EntityManager::getInstance(); 
	gizmoManager = &GizmoManager::getInstance();

	camreraBuffer.allocate(sizeof(glm::mat4) * 2, 0);
	camreraBuffer.fill(0, sizeof(glm::mat4) * 2, nullptr);

	materialsBuffer.allocate(sizeof(Material) * MATERIALS.size(), 1);
	materialsBuffer.fill(0, sizeof(Material) * MATERIALS.size(), MATERIALS.data());
}

Game::Game(glm::ivec2 windowDim) : Game() {
	gameRunning = false;
	Game::mouseData = { 0, 0, -90 };
	lastFrameTime = -1.0f;
	this->windowDim = windowDim;

	makeSkybox("skybox");


	{
		// DEFERED RENERING G-BUFFER
		FrameBufferInit gBufferDetails; // may need to make this depth texture the one used by the transparancy buffer
		gBufferDetails.hasDepth = true;
		gBufferDetails.depthTexture = true;

		ColourBufferInit albedo;
		albedo.format = GL_FLOAT;
		albedo.internalFormat = GL_RGB16F;
		albedo.type = GL_RGB;

		ColourBufferInit fragPos;
		fragPos.format = GL_FLOAT;
		fragPos.internalFormat = GL_RGB32F;
		fragPos.type = GL_RGB;

		ColourBufferInit normal;
		normal.format = GL_FLOAT;
		normal.internalFormat = GL_RGBA16F;
		normal.type = GL_RGBA;

		ColourBufferInit accumOIT;
		accumOIT.format = GL_FLOAT;
		accumOIT.internalFormat = GL_RGBA16F;
		accumOIT.type = GL_RGBA;

		ColourBufferInit revealOIT;
		revealOIT.format = GL_FLOAT;
		revealOIT.internalFormat = GL_R8;
		revealOIT.type = GL_RED;

		gBufferDetails.colourBuffers = { albedo, fragPos, normal, accumOIT, revealOIT };

		gBuffer = FrameBuffer(windowDim);
		gBuffer.setUp(gBufferDetails);

		// OPAQUE OIT BUFFER (and used for SSAO)
		FrameBufferInit detailsOIT;
		detailsOIT.hasDepth = true;
		detailsOIT.depthTexture = true;

		ColourBufferInit opaqueOIT;
		opaqueOIT.format = GL_HALF_FLOAT;
		opaqueOIT.internalFormat = GL_RGBA16F;
		opaqueOIT.type = GL_RGBA;

		detailsOIT.colourBuffers = { opaqueOIT };
		detailsOIT.depthBuffer = gBuffer.getDepth();

		oitFrameBuffer1 = FrameBuffer(windowDim);
		oitFrameBuffer1.setUp(detailsOIT);

		// SHADOW MAP BUFFER
		FrameBufferInit detailsShadows;
		detailsShadows.hasDepth = true;
		detailsShadows.depthTexture = true;

		shadowFramebuffer = FrameBuffer({ SHADOW_MAP_SIZE , SHADOW_MAP_SIZE });
		shadowFramebuffer.setUp(detailsShadows);

		// BLUR BUFFER
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
}

Game::~Game()
{
	// screen quad
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
	quadVAO = quadVBO = 0;
	// skybox
	glDeleteBuffers(1, &skyVBO);
	glDeleteVertexArrays(1, &skyVAO);
	skyVAO = skyVBO = 0;

	glDeleteTextures(1, &_ssaoNoiseTex);
}


void Game::generateWorld() {
	unsigned int seed = 32;
	srand(seed);
	_world.setSeed(seed);
#if  !defined(GENERATE_CHUNKS_ASYNC) && !GENERATE_NEW_CHUNKS
	_world.generateTerrain();
	_world.setUpDrawable();
#endif // GENERATE_CHUNKS_ASYNC
}

void Game::setUpGame(const glm::mat4& projection)
{
	generateWorld();

	cameraProjection = projection;
	camreraBuffer.fill(0, sizeof(glm::mat4), &cameraProjection);
	setupEventCB(window);
}

void Game::doGameLoop() {
	gameRunning = true;

	
	// VOXEL MODELS
	ModelManager& modelManager = ModelManager::getInstance();
	Timer voxelLoad("Load Voxel Model");
	// VoxelModel_Static& woman = modelManager.loadVoxel("C:\\Users\\AGWDW\\Desktop\\woman.ply", B_GLASS);
	// woman.setPosition(0, 37, 0);
	// woman.addToDrawable(_world.geomDrawable);
#ifdef ALWAYS_USE_GREEDY_MESH
	woman.addToDrawable(_world.greedyDrawable);
#endif
	voxelLoad.showTime(0);

	// LOAD MODELS
	// auto& enterprise = modelManager.load("C:\\Users\\AGWDW\\Desktop\\ncc1701d_voxel.obj");
	// auto& cloud = modelManager.load("C:\\Users\\AGWDW\\Desktop\\cloud_voxel.obj");
	// std::cout << "Models Loaded" << std::endl;
	// auto mesh = ModelLoader::Load("C:\\Users\\AGWDW\\Desktop\\cube.obj");

	setUpSSAO();
	
	EventsManager& events = EventsManager::getInstance();
	// EventCallback<Game> leftReleaseCB(this, &Game::breakBlock);


	// events.click += leftReleaseCB;
	// EventCallback<Game> rightReleaseCB(this, &Game::placeBlock);
	// events.click += rightReleaseCB;
	// EventCallback<Game> middleReleaseCB(this, &Game::explode);
	// events.click += middleReleaseCB;

	unsigned int numFrames = 0;
	GizmoManager& gizmoManager = GizmoManager::getInstance();

	PhysicsManager& physManager = PhysicsManager::getInstance();

	// Entities::TestObject to1;
	// Components::Transform to1Transform;
	// Components::RenderMesh to1RenderMesh;
	// to1RenderMesh.setModel(&cloud);
	// to1.addComponent(to1Transform);
	// to1.addComponent(to1RenderMesh);

	manager->awakeEvent();

	manager->startEvent();

	_player->setPosition({ CHUNK_SIZE_F * HALF_VOXEL_SIZE, 40, CHUNK_SIZE_F * HALF_VOXEL_SIZE });
	// to1.setPosition({ 0, 100, 0 });
	/*reactphysics3d::Material& mat = _player->getRigidBody()->getMaterial();
	mat.setBounciness(0);
	mat.setFrictionCoefficient(1);*/

	float dtAccumulator = 0;
	float cellularAccum = 0;
	int prevMatSize = 0;

	glm::mat4 guiOrtho = glm::ortho<float>(0.0f, windowDim.x, 0.0f, windowDim.y);
	SHADERS[GLYPH].bind();
	SHADERS[GLYPH].setValue("projection", guiOrtho);
	SHADERS[NEW_GUI].bind();
	SHADERS[NEW_GUI].setValue("projection", guiOrtho);

	GUI::Utils::Text::GlyphRendering& instance = GUI::Utils::Text::GlyphRendering::getInstance();
	instance.setShader(SHADERS[GLYPH].getId());
	instance.loadFont("arial", "C://Windows/Fonts/arial.ttf");

	guiWindow.windowDimentions.x = windowDim.x;
	guiWindow.windowDimentions.y = windowDim.y;
	guiWindow.elementShader = SHADERS[NEW_GUI].getId();


	GUI::StackContainer container;
	container.setSpacing(5);

	_fpsCounter.setText("FPS: N/A");
	_fpsCounter.setLayoutType(GUI::TextBox::VERTICAL);
	_fpsCounter.setTextColour({ 1, 1, 1, 1 });

	_playerPosition.setText("Position: N/A");
	_playerPosition.setLayoutType(GUI::TextBox::VERTICAL);
	_playerPosition.setTextColour({ 1, 1, 1, 1 });

	_viewDirection.setText("View Direction: N/A");
	_viewDirection.setLayoutType(GUI::TextBox::VERTICAL);
	_viewDirection.setTextColour({ 1, 1, 1, 1 });

	_numChunks.setText("Num Chunks: N/A");
	_numChunks.setLayoutType(GUI::TextBox::VERTICAL);
	_numChunks.setTextColour({ 1, 1, 1, 1 });

	container.push(&_fpsCounter);
	container.push(&_playerPosition);
	container.push(&_viewDirection);
	container.push(&_numChunks);

	guiWindow.setRoot(&container);
	Timer timer("Game Loop");
	while (gameRunning) {
		if (prevMatSize != MATERIALS.size()) {
			materialsBuffer.allocate(sizeof(Material) * MATERIALS.size(), 1);
			materialsBuffer.fill(0, sizeof(Material) * MATERIALS.size(), MATERIALS.data());
			prevMatSize = MATERIALS.size();
		}
		timer.start();
		calcTimes();
		glfwPollEvents();
		_player->processMouse(mouseOffset);
		timer.mark("Player Mouse");
		glm::ivec2 c = _player->getChunkPosition();
		
		std::list<ChunkColumn*> neibours = _world.getNeibours(c, true);

		timer.mark("Player Keys");

		cameraView = _player->getViewMatrix();
		camreraBuffer.fill(sizeof(glm::mat4), sizeof(glm::mat4), &cameraView);
		timer.mark("View Buffer");


#ifdef DEBUG_GRID_LINES
		unsigned int i = 0;
		for (Gizmo::IShape* grid : gizmoManager.allGizmos) {
			grid->setPosition(glm::vec3(c.x * CHUNK_SIZE_F, 0, c.y * CHUNK_SIZE_F) + GRID_LINE_POSITIONS[i++]);
		}
#endif // DEBUG_GRID_LINES

		// glm::vec3 f = glm::normalize(glm::vec3(mainCamera.GetFront().x, 0, mainCamera.GetFront().z));
		// glm::vec3 frustrumCenter = mainCamera.GetPosition() + f * FAR_PLANE * .5f;
		float frustrumRadius = FAR_PLANE;
		glm::vec3 frustrumCenter = _player->getPosition();
		

		_world.tryStartGenerateChunks(c, _player->getFront());

		timer.mark("Start Chunk Gen");
		// std::cout << "Generated" << std::endl;

		manager->updateEvent(deltaTime);
		timer.mark("Entity Update");

		if (dtAccumulator >= FIXED_DELTA_TIME) {
			dtAccumulator -= FIXED_DELTA_TIME;

			bool found = false;
			const ChunkColumn& chunk = *_world.getChunk(c, found);
			if (found) {
				physManager.setTerrain(chunk);
			}

			manager->preFixedUpdateEvent();
			physManager.step();
			manager->postFixedUpdateEvent();
		}
		timer.mark("Physics Manger");
		dtAccumulator += deltaTime;
		cellularAccum += deltaTime;

		// occures every 1 seccond
		if (cellularAccum > 1) {
			_world.update();
			cellularAccum = 0;
		}
		timer.mark("Cellular Automota");

		updateGUIText();

		renderScene();
		timer.mark("Rendering");


		numFrames++;
		mouseOffset = glm::vec2(0);
		if (glfwWindowShouldClose(window)) gameRunning = false;
	}
	timer.showDetails(numFrames);

	manager->destroyEvent();
}

void Game::calcTimes() {
	float frame = glfwGetTime();
	deltaTime = frame - lastFrameTime;
	if (lastFrameTime == -1) deltaTime = 1.0f / 60.0f;
	lastFrameTime = frame;
	frameRate = 1 / deltaTime;
}

void Game::renderScene() {
	const glm::mat4 LSM = ShadowBox::getLSM(cameraView, LIGHT_POSITION);
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
	_world.render(&shadows);
	// castle->render(shadows);
	shadows.unBind();
	shadowFramebuffer.unBind();
	// 2. render for OIT
	// 2.1 Opaque
	
	// 2.1 Populate G-Buffer Opaque
	gBuffer.bind();
	glDisable(GL_BLEND);
	glFrontFace(GL_CW);
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	Shader& gbufferS = SHADERS[GBUFFER];
	gbufferS.bind();
	
	_world.render(&gbufferS);
	manager->renderEvent();
	// castle->render(gbufferS);


	// 2.2 Populate G-Buffer Opaque
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(3, GL_ONE, GL_ONE);
	glBlendFunci(4, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	glm::vec4 black(0);
	glm::vec4 white(1);
	glClearBufferfv(GL_COLOR, 3, &black[0]);
	glClearBufferfv(GL_COLOR, 4, &white[0]);

	Shader& transparent = SHADERS[OIT_TRANSPARENT];
	transparent.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, multiPurposeFB.getColourTex(0)); // ao
	transparent.setValue("ao", 0);
	transparent.setValue("viewDir", _player->getViewDirection());
	transparent.setValue("lightPos", LIGHT_POSITION);

	_world.render(&transparent);
	manager->renderEvent();
	// castle->render(transparent);

	
	// 3.1 Ambiant Occlusion (render to the oit opaque buffer)
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	oitFrameBuffer1.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Shader& ssao = SHADERS[AO];
	ssao.bind();
	ssao.setValue("fragPosTex", 0);
	ssao.setValue("normalRnd", 1);
	ssao.setValue("ssaoNoise", 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(1)); // fragPos
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(2)); // normal
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex); // random-ness
	
	glUniform3fv(glGetUniformLocation(ssao.getId(), "ssaoSamples"), ssaoSamples.size(), &ssaoSamples[0][0]);
	glm::vec2 noiseScale = ((glm::vec2)windowDim) / SSAO_SCALE;
	ssao.setValue("ssaoNoiseScale", noiseScale);
	ssao.setValue("ssaoRadius", SSAO_RADIUS);
	ssao.setValue("ssaoBias", SSAO_BIAS);
	ssao.setValue("voxelSize", VOXEL_SIZE); 
	
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// 3.2 Blurs Ambiant Occlusion (renders to the multi purpose buffer)
	multiPurposeFB.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	Shader& blur = SHADERS[BLUR];
	blur.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer1.getColourTex(0));
	blur.setValue("img", 0);
	
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// 4. Deffered Lighting
	oitFrameBuffer1.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Shader& deffered = SHADERS[DEFFERED];
	deffered.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(0)); // albedo
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(1)); // fragPos
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(2)); // normal
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, multiPurposeFB.getColourTex(0)); // blured ao
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.getDepth()); // shadow map
	
	deffered.setValue("_albedo", 0);
	deffered.setValue("_fragPos", 1);
	deffered.setValue("_normal", 2);
	deffered.setValue("ao", 3);
	deffered.setValue("shadowMap", 4);
	
	deffered.setValue("viewDir", _player->getViewDirection());
	deffered.setValue("view_inv", glm::inverse(cameraView));
	deffered.setValue("lightMatrix", LSM);
	deffered.setValue("lightPos", LIGHT_POSITION);
	
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// renderModels();
	drawSkybox();

	// 5. Composite
	glDisable(GL_CULL_FACE);
	oitFrameBuffer1.bind(); // render to the OIT framebuffer
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader& composite = SHADERS[OIT_COMPOSITE];
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(3));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.getColourTex(4));
	
	composite.bind();
	composite.setValue("accum", 0);
	composite.setValue("reveal", 1);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	
	gizmoManager->render(cameraProjection * cameraView);


#ifdef PHYSICS_DEBUG_RENDERER
	glEnable(GL_DEPTH_TEST);
	glDepthMask(false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	const Shader& physDebug = SHADERS[PHYS_DEBUG];
	physDebug.bind();
	
	// creates VAO, VBO populates with lines then draws repeat for triangles then deltes buffers (every frame
	static PhysicsManager& physManger = PhysicsManager::getInstance();
	auto debugger = physManger.getDebugRenderer();
	auto numLines = debugger->getNbLines();
	auto numTriangles = debugger->getNbTriangles();
	
	
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(reactphysics3d::Vector3) + sizeof(reactphysics3d::uint32), (void*)0); // vert _world pos
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(reactphysics3d::Vector3) + sizeof(reactphysics3d::uint32), (void*)sizeof(reactphysics3d::Vector3)); // vert colour
	
	if (numLines > 0) {
		auto lines = debugger->getLinesArray();
		physDebug.setValue("colour", glm::vec3(1, 0, 1));
		glBufferData(GL_ARRAY_BUFFER, numLines * sizeof(reactphysics3d::DebugRenderer::DebugLine), lines, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, numLines*2); // draw all lines
	}
	if (numTriangles > 0) {
		auto triangles = debugger->getTrianglesArray();
		physDebug.setValue("colour", glm::vec3(0, 1, 0));
		// refill data with triangle data
		glBufferData(GL_ARRAY_BUFFER, numTriangles * sizeof(reactphysics3d::DebugRenderer::DebugTriangle), triangles, GL_STATIC_DRAW);
	
		glDrawArrays(GL_TRIANGLES, 0, numTriangles*3); // draw triangles
	}
	
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	
	composite.unBind();

	// 6. render the screen quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // use the default frambuffer
	glViewport(0, 0, windowDim.x, windowDim.y);

	glClearColor(GameConfig::backgroundCol.x, GameConfig::backgroundCol.y, GameConfig::backgroundCol.z, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
	glDisable(GL_BLEND);
	
	
	const Shader& screenQuad = SHADERS[SCREEN_QUAD];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, oitFrameBuffer1.getColourTex(0));
	
	screenQuad.bind();
	
	screenQuad.setValue("screen", 0);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	// 7. GUI
	guiWindow.render();

	glfwSwapBuffers(window);
}

void Game::updateGUIText()
{
	_fpsCounter.setText("FPS: " + std::to_string(frameRate));
	_playerPosition.setText("Position: " + glm::to_string(_player->getPosition()));
	_viewDirection.setText("View Direction: " + glm::to_string(_player->getFront()));
	_numChunks.setText("Num Chunks: " + std::to_string(_world.getChunkCount()));
}

void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}

void Game::setPlayer(Entities::Player* player)
{
	_player = player;
}

void Game::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mod) {

	EventsManager& events = EventsManager::getInstance();
	KeyEventInfo info;
	info._key = key;
	info._action = action;
	info._mod = mod;
	events.keyPress.fire(info);

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

		_world.placeBlock(z, invPV, mainCamera->GetFront());
	}
	if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
		float zcoord = 0;
		glReadPixels(WIDTH * .5f, HEIGHT * .5f, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zcoord);
		glm::mat4 invPV = glm::inverse(projection * mainCamera->GetViewMatrix());
		float z = (zcoord - 0.5f) * 2.0f;

		_world.placeBlock(z, invPV, mainCamera->GetFront());
	}*/
	if (key == GLFW_KEY_H && action == GLFW_RELEASE) {
		// _world.save();
		std::cout << "Saved" << std::endl;
	}
}

void Game::mouseCallBack(GLFWwindow* window, double xPos, double yPos) {
	if ((int)Game::mouseData.z) {
		Game::mouseData.x = xPos;
		Game::mouseData.y = yPos;
		Game::mouseData.z = false;
	}

	Game::mouseOffset.x = xPos - Game::mouseData.x;
	Game::mouseOffset.y = Game::mouseData.y - yPos;

	Game::mouseData.x = xPos;
	Game::mouseData.y = yPos;
}

void Game::clickCallBack(GLFWwindow* window, int button, int action, int mods) {
	EventsManager& events = EventsManager::getInstance();
	if (action == GLFW_RELEASE) {
		ClickEventInfo info{};
		info._mouseButton = button;
		events.click.fire(info);
	}
}

void Game::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	EventsManager::getInstance().mouseScroll.fire();
}

void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetMouseButtonCallback(window, Game::clickCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
	glfwSetScrollCallback(window, Game::scrollCallBack);
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

void Game::placeBlock()
{
	_world.placeBlock(_player->getPosition(), _player->getFront(), _player->getChunkPosition());
}

void Game::breakBlock()
{
	_world.breakBlock(_player->getPosition(), _player->getFront(), _player->getChunkPosition());
}

void Game::explode() {
	glm::vec3 lookPos = _world.lookingAt(_player->getPosition(), _player->getFront());
	_world.explode(lookPos, 5);
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
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	auto shader = &SHADERS[SKYBOX];
	shader->bind();
	shader->setValue("skybox", 0);
}

void Game::drawSkybox() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer'transparent content
	SHADERS[SKYBOX].bind();
	// Camera& camera = player->getCamera();

	glBindVertexArray(skyVAO);
	TEXTURES3D[(int)Texture_Names::SKYBOX].bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	TEXTURES3D[(int)Texture_Names::SKYBOX].unBind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void Game::setUpSSAO()
{
	auto lerp = [](float a, float b, float c) {
		return a + c * (b - a);
	};
	// THE SAMPLES
	float inv_num = 1.f / (float)SSAO_NUM_SAMPLES;
	for (unsigned int i = 0; i < SSAO_NUM_SAMPLES; i++) {
		glm::vec3 r(0);
		r.x = randRange(-1, 1);
		r.y = randRange(-1, 1);
		r.z = randRange(0, 1);
		r = glm::normalize(r);

		float s = (float)i * inv_num;
		s = lerp(.1f, 1.f, s * s);
		r *= s;
		ssaoSamples[i] = r;
	}
	// THE NOISE
	std::array<glm::vec3, (size_t)(SSAO_SCALE* SSAO_SCALE)> noiseData{};
	for (unsigned int i = 0; i < noiseData.size(); i++) {
		glm::vec3 r(0);
		r.x = randRange(-1, 1);
		r.y = randRange(-1, 1);
		r.z = 0;
		r = glm::normalize(r);
		noiseData[i] = r;
	}

	glGenTextures(1, &_ssaoNoiseTex);
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SSAO_SCALE, SSAO_SCALE, 0, GL_RGB, GL_FLOAT, &noiseData[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}