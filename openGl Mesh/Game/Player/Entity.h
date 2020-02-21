#pragma once
#include <gtx/string_cast.hpp>
#include "../World/World.h"
#include "../../Renders/chunkRender.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Entity
{
public:
	Entity(GLboolean init = GL_FALSE);
	glm::vec3& getPosition();
	glm::vec3& getVelocity();
	glm::vec3& getAcceleration();
	glm::vec3 getCenter();
	glm::vec3 getCenter(glm::vec3 pos);
	
	void setVelocity(const glm::vec3& vel);
	void setVelocity(const GLfloat& vel);
	void setAcceleration(const glm::vec3& acc);
	void setMovementSpeed(const GLfloat& speed);

	void addVelocity(const glm::vec3& vel);
	void addAcceleration(const glm::vec3& acc);

	void updatePosition(GLfloat deltaTime, World& world);

	void move(Move_Dir dir);

	void render(glm::mat4 projection, Camera* cam = nullptr);
	glm::vec3 determinCollision(World& world, glm::vec3 deltaV);
	std::vector<glm::vec3> getVertices();

protected:
	glm::vec3 pos, vel, acc;
	glm::vec3 forward, right;
	GLfloat movementSpeed;
	GLfloat jumpForce;
	std::vector<Face> body;
	Render::ChunkMeshRender renderer;
	GLboolean grounded;
	GLboolean hasBody;

};

