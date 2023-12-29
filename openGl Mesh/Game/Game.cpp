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
#include "../GUIRendering/GUIBuffer.h"
#include "../GUI/Containers/BasicContainer.h"
#include "../GUI/Elements/TextBox.h"

#pragma region GameConfig
bool GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.5, 0.5, 0.5 };
unsigned int GameConfig::FPSlock = 0;
#pragma endregion


glm::vec3 Game::mouseData;
glm::vec2 Game::mouseOffset;
std::array<bool, 1024> Game::keys;
World Game::world;
UI_Renderer Game::uiRenderer; 

Game::Game() : window(), deltaTime(), frameRate(), gameRunning(false), lastFrameTime(-1), guiFrameBuffer(), quadVAO(), quadVBO(), multiPurposeFB(), guiDrawable(),
	SBVAO(0), LSVAO(), Letters(), windowDim(), LSVBO(), oitFrameBuffer1(), gBuffer(), camreraBuffer(), materialsBuffer(), _player(), guiWindow() {
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
	createGUI();
	setUpFreeType();


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

		// TRANSPARENT BUFFER

		// GUI BUFFER
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

void Game::generateWorld() {
	unsigned int seed = 32;
	srand(seed);
	world = World(WORLD_ORIGIN, seed);
#if  !defined(GENERATE_CHUNKS_ASYNC) && !GENERATE_NEW_CHUNKS
	world.generateTerrain();
	world.setUpDrawable();
#endif // GENERATE_CHUNKS_ASYNC
}

void Game::doLoop(const glm::mat4& projection) {
	gameRunning = true;
	setupEventCB(window);
	cameraProjection = projection;

	camreraBuffer.fill(0, sizeof(glm::mat4), &projection);
	
	// VOXEL MODELS
	ModelManager& modelManager = ModelManager::getInstance();
	Timer voxelLoad("Load Voxel Model");
	 VoxelModel_Static& woman = modelManager.loadVoxel("C:\\Users\\AGWDW\\Desktop\\woman.ply", B_WATER);
	 woman.setPosition(0, 37, 0);
	 woman.addToDrawable(world.geomDrawable);
#ifdef ALWAYS_USE_GREEDY_MESH
	woman.addToDrawable(world.greedyDrawable);
#endif
	voxelLoad.showTime(0);

	// LOAD MODELS
	// auto& enterprise = modelManager.load("C:\\Users\\AGWDW\\Desktop\\ncc1701d_voxel.obj");
	// auto& cloud = modelManager.load("C:\\Users\\AGWDW\\Desktop\\cloud_voxel.obj");
	// std::cout << "Models Loaded" << std::endl;
	// auto mesh = ModelLoader::Load("C:\\Users\\AGWDW\\Desktop\\cube.obj");

#ifdef SSAO
	setUpSSAO();
#endif // SSAO
	
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


	GUI::GUI_Window guiWindow;
	GUI::BasicContainer container;
	GUI::TextBox tb;
	tb.setDimentions({ 1, 1 });
	tb.setBackgroundColour({ 1, 0, 0 });
	tb.setCornerRadius(0);

	container.push(&tb);
	guiWindow.setRoot(&container);

	GUIBuffer guiBuffer;
	auto bufferData = guiWindow.getDrawData();
	guiBuffer.setUp(bufferData.data(), bufferData.size());
	guiDrawable.add(guiBuffer);



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
		
		std::list<ChunkColumn*> neibours = world.getNeibours(c, true);

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
		

		world.tryStartGenerateChunks(c, _player->getFront());

		timer.mark("Start Chunk Gen");
		// std::cout << "Generated" << std::endl;

		manager->updateEvent(deltaTime);
		timer.mark("Entity Update");

		if (dtAccumulator >= FIXED_DELTA_TIME) {
			dtAccumulator -= FIXED_DELTA_TIME;

			bool found = false;
			const ChunkColumn& chunk = *world.getChunk(c, found);
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
			world.update();
			cellularAccum = 0;
		}
		timer.mark("Cellular Automota");

		glClearColor(GameConfig::backgroundCol.x, GameConfig::backgroundCol.y, GameConfig::backgroundCol.z, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		showStuff();
		timer.mark("Rendering");

		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
		numFrames++;
		mouseOffset = glm::vec2(0);

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

void Game::showFPS() {
	if (GameConfig::showFPS) {
		showText("FPS: " + std::to_string(frameRate), { 5, 875 }, 0.5f);
	}
}

void Game::showStuff() {
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
	world.render(&shadows);
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
	
	world.render(&gbufferS);
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

	world.render(&transparent);
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
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex); // random-ness
	
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
	showSkybox();

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(reactphysics3d::Vector3) + sizeof(reactphysics3d::uint32), (void*)0); // vert world pos
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
	
	
	// 6. render the GUI
	guiFrameBuffer.bind(); // use the GUI framebuffer

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	showGUI();
	showFPS();
	
	std::string m;
	m = "Position: " + glm::to_string(_player->getPosition());
	showText(m, { 5, 850 }, 0.5f);
	
	m = "View Direction: " + glm::to_string(_player->getViewDirection());
	showText(m, { 5, 825 }, 0.5f);
	
	m = "Chunk Pos: " + glm::to_string(_player->getChunkPosition());
	showText(m, { 5, 800 }, 0.5f);

	m = "Num of Chunks: " + std::to_string(world.getChunkCount());
	showText(m, { 5, 775 }, 0.5f);

	m = "Num of Drawed chunks: " + std::to_string(world.getDrawCount());
	showText(m, { 5, 750 }, 0.5f);


	// 7. render the screen quad
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

	guiDrawable.render();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Shader* greedyShader = &SHADERS[GREEDY];
	// greedyShader->bind();
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LESS);
	// glDepthMask(true); // enable depth writes so glClear won't ignore clearing the depth buffer
	// glDisable(GL_CULL_FACE);
	// world.renderGreedy(greedyShader);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

		world.placeBlock(z, invPV, mainCamera->GetFront());
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

std::string num = "0";

void Game::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	EventsManager::getInstance().mouseScroll.fire();
	/*int i = std::stoi(num);
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
	uiRenderer.appendElement(e);*/
}

void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetMouseButtonCallback(window, Game::clickCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
	glfwSetScrollCallback(window, Game::scrollCallBack);
}

void Game::cleanUp() {
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

void Game::placeBlock()
{
	world.placeBlock(_player->getPosition(), _player->getFront(), _player->getChunkPosition());
}

void Game::breakBlock()
{
	world.breakBlock(_player->getPosition(), _player->getFront(), _player->getChunkPosition());
}

void Game::explode() {
	glm::vec3 lookPos = world.lookingAt(_player->getPosition(), _player->getFront());
	world.explode(lookPos, 5);
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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer'transparent content
	SHADERS[SKYBOX].bind();
	// Camera& camera = player->getCamera();

	glBindVertexArray(SBVAO);
	TEXTURES3D[(int)Texture_Names::SKYBOX].bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	TEXTURES3D[(int)Texture_Names::SKYBOX].unBind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void Game::createGUI() {
	uiRenderer = UI_Renderer(&SHADERS[SHADER_NAMES::GUI_S]);
	UI_Element crosshair = UI_Element({ 0, 0 }, { 0.5, 0.5 }, &TEXTURES2D[(unsigned int)Texture_Names_2D::GUI_S], "crosshair");

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

	glGenTextures(1, &ssaoNoiseTex);
	glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SSAO_SCALE, SSAO_SCALE, 0, GL_RGB, GL_FLOAT, &noiseData[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
#endif // SSAO