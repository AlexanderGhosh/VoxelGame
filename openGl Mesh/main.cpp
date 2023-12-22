#include <iostream>
#include <gtx/string_cast.hpp>

#include <glad/glad.h>
#include <glfw3.h>
#include <gtc/type_ptr.hpp>

#include "Game/Game.h"

#include "Helpers/Constants.h"
#include "Helpers/Functions.h"
#include "Textures/Texture.h"
#include "Shaders/Shader.h"

#include "EntityComponent/Entity.h"
#include "EntityComponent/Entities/Player.h"
#include "EntityComponent/Components/Transform.h"
#include "EntityComponent/Components/Camera.h"
#include "EntityComponent/Components/CreativeComponent.h"
#include "EntityComponent/Components/RigidBody.h"

#include "Mangers/EntityManager.h"
#include "Mangers/ComponentManager.h"
#include "Mangers/ModelManager.h"
#include "Mangers/GizmoManager.h"
#include "Mangers/PhysicsManager.h"


#include "Gizmos/Composite/Grid2D.h"
#include "Game/World/world_generation.h"
#include "Helpers/Timers/Timer.h"

#include "Material.h"
#include "Block.h"
#include "JsonParser.h"
#include <random>


// #include "Helpers/ModelLoaders/VoxelModel.h"
glm::ivec2 DIM(WIDTH, HEIGHT);

GLFWwindow* createWindow();

// #define TESTING
#ifdef TESTING
#endif

int main() {
#ifdef TESTING

	return 0;
#endif // TESTING
	JsonParser materialParser(".\\Materials.json");
	MATERIALS = std::move(materialParser.getAllMaterials());
	JsonParser blockParser(".\\Blocks.json");
	BLOCKS = std::move(blockParser.getAllBlocks());

	GLFWwindow* window = createWindow();
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
	Components::CreativeComponent playerControler;
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
	game.generateWorld();

	glm::mat4 projection = glm::perspective(FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	game.doLoop(projection);

	game.cleanUp();

	glfwDestroyWindow(window);
	glfwTerminate();

	ComponentManager::getInstance().destroy();
	EntityManager::getInstance().destroy();
	ModelManager::getInstance().destroy();
	GizmoManager::getInstance().destroy();
	PhysicsManager::getInstance().destroy();
	return 0;
}

GLFWwindow* createWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(DIM.x, DIM.y, "Voxel Game", nullptr, nullptr);

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	DIM = glm::ivec2(w, h);
	if (nullptr == window) {
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

	glViewport(0, 0, DIM.x, DIM.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// aplpha suport

	// glEnable(GL_MULTISAMPLE); // MSAA

	return window;
}
