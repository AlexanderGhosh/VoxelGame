#pragma once
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/string_cast.hpp>
#include <tuple>
#include "../World/Physics.h"
#include "Camera.h"
#include "../../Renders/chunkRender.h"
#include "../../Drawable.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3 >;
class Player
{
public:
	Player();
	Player(glm::vec3 position, glm::vec3 camOff);
	Player(GLboolean init);
	glm::vec3 getPosition();
	Physics::Update processMovement(Camera_Movement movement, GLfloat deltaTime);
	void processMouse(GLfloat xOffset, GLfloat yOffset, GLfloat x, GLboolean constrainPitch = true);
	void create();
	void render(glm::mat4 projection);
	Camera& getCamera();
	Physics::Object& getObject();
private:
	Camera cam;
	Physics::Object object;
	GLfloat movementSpeed;
	glm::vec3 prevPos;
	glm::vec3 camera_offset;
	Render::ChunkMeshRender renderer;
	std::vector<Face> faces;
};

