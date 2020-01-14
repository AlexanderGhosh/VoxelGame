#pragma once
#include <glm.hpp>
#include "../World/Physics.h"
#include "Camera.h"
#include "../../Renders/chunkRender.h"
class Player
{
public:
	Camera cam;
	Player();
	Player(glm::vec3 position, glm::vec3 camOff);
	glm::vec3 getPosition();
	Physics::Update processMovement(Camera_Movement movement, GLfloat deltaTime);
	void processMouse(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true);
	void create();
	void render(glm::mat4 projection);
private:
	GLfloat movementSpeed;
	Physics::Object object;
	glm::vec3 prevPos;
	glm::vec3 camera_offset;
	Render::ChunkMeshRender renderer;
};

