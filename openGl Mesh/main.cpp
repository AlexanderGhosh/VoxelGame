#include <random>
#include "Game/Game.h"

#include "Helpers/Constants.h"
#include "Textures/Texture.h"
#include "Shaders/Shader.h"

#include "EntityComponent/Entity.h"
#include "EntityComponent/Entities/Player.h"
#include "EntityComponent/Components/Transform.h"
#include "EntityComponent/Components/Camera.h"
#include "EntityComponent/Components/FlightControls.h"
#include "EntityComponent/Components/CreativeComponent.h"
#include "EntityComponent/Components/RigidBody.h"

#include "Mangers/EntityManager.h"
#include "Mangers/ComponentManager.h"
#include "Mangers/ModelManager.h"
#include "Mangers/GizmoManager.h"
#include "Mangers/PhysicsManager.h"
#include "Mangers/EventsManager.h"


#include "Game/World/world_generation.h"

#include "Material.h"
#include "Block.h"
#include "JsonParser.h"


GLFWwindow* createWindow(glm::ivec2& dimentions);

// #define TESTING
#ifdef TESTING
#include "GUI/GUI_Window.h"
#include "GUI/Containers/BasicContainer.h"
#include "GUI//Elements/TextBox.h"
#endif

int main() {
#ifdef TESTING
	GUI::GUI_Window gui;
	GUI::BasicContainer container;
	GUI::TextBox tb;
	tb.setDimentions({ 1, 1 });
	tb.setBackgroundColour({ 1, 0, 0 });
	tb.setCornerRadius(0);

	container.push(&tb);
	gui.setRoot(&container);

	auto t = gui.getDrawData();
	return 0;
#endif // TESTING
	glm::ivec2 DIM(WIDTH, HEIGHT);
	JsonParser materialParser(".\\Materials.json");
	MATERIALS = std::move(materialParser.getAllMaterials());
	JsonParser blockParser(".\\Blocks.json");
	BLOCKS = std::move(blockParser.getAllBlocks());

	GLFWwindow* window = createWindow(DIM);
	EventsManager::getInstance().setWindow(window);

	reactphysics3d::PhysicsCommon physCommon;
	PhysicsManager::getInstance().setPhysCommon(&physCommon);

	for (auto& tex : TEXTURES3D) {
		tex.load3D(tex.getName());
	}
	for (auto& tex : TEXTURES2D) {
		tex.load2D(tex.getName());
	}
	for (auto& shader : SHADERS) {
		shader.setUp();
	}
	world_generation::setUp();

	// ENTITY COMPONENT SYSTEMS
	ComponentManager& componentManager = ComponentManager::getInstance();
	EntityManager& entityManager = EntityManager::getInstance();

	Entities::Player player(false);
	Components::Transform playerTransform;
	Components::Camera playerCamera;
	Components::CreativeComponent playerControler(true);
	Components::RigidBody playerRB;

	player.addComponent(playerTransform);
	player.addComponent(playerCamera);
	player.addComponent(playerControler);
	player.addComponent(playerRB);

	GizmoManager& gizmoManager = GizmoManager::getInstance();

#ifdef DEBUG_GRID_LINES
	glm::ivec2 gridDims(CHUNK_SIZE, WORLD_HEIGHT);
	glm::vec3 colour(0.4);

	Gizmo::Grid2D grid1(glm::vec3(0), gridDims, VOXEL_SIZE, true, false);
	grid1.setColour(colour);

	Gizmo::Grid2D grid2(glm::vec3(0), gridDims, VOXEL_SIZE, true, false);
	grid2.setColour(colour);

	Gizmo::Grid2D grid3(glm::vec3(0), gridDims, VOXEL_SIZE, false, false);
	grid3.setColour(colour);

	Gizmo::Grid2D grid4(glm::vec3(0), gridDims, VOXEL_SIZE, false, false);
	grid4.setColour(colour);

	gizmoManager.addGizmo(grid1);
	gizmoManager.addGizmo(grid2);
	gizmoManager.addGizmo(grid3);
	gizmoManager.addGizmo(grid4);
#endif // DEBUG_GRID_LINES


	Game game = Game(DIM);
	GameConfig::showFPS = true;

	game.setPlayer(&player);


	game.setWindow(window);

	glm::mat4 projection = glm::perspective(FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	game.setUpGame(projection);
	game.doGameLoop();

	glfwDestroyWindow(window);
	glfwTerminate();

	ComponentManager::getInstance().destroy();
	EntityManager::getInstance().destroy();
	ModelManager::getInstance().destroy();
	GizmoManager::getInstance().destroy();
	PhysicsManager::getInstance().destroy();
	return 0;
}

GLFWwindow* createWindow(glm::ivec2& dimentions) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(dimentions.x, dimentions.y, "Voxel Game", nullptr, nullptr);

	glfwGetFramebufferSize(window, &dimentions.x, &dimentions.y);
	if (!window) {
		std::cout << "Failed to create window" << std::endl;
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
	if (!VSYNC) glfwSwapInterval(0);

	glViewport(0, 0, dimentions.x, dimentions.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// aplpha suport

	return window;
}
