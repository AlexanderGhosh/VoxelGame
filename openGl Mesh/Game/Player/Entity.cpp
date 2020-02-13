#include "Entity.h"
Entity::Entity(GLboolean init) {
	pos = { 0, 0, 0 };
	vel = { 0, 0, 0 };
	acc = { 0, 0, 0 };
	forward = { 0, 0, -1 };
	right = { 1, 0, 0 };
	movementSpeed = 1; // m/s
	jumpForce = 5;
	grounded = 0;
	hasBody = GL_FALSE;
	if (init) {
		renderer = Render::ChunkMeshRender(true, "");
	}
	else {
		renderer = Render::ChunkMeshRender(false, "");
	}
}
glm::vec3& Entity::getPosition() {
	return pos;
}
glm::vec3& Entity::getVelocity() {
	return vel;
}
glm::vec3& Entity::getAcceleration() {
	return acc;
}

void Entity::setVelocity(const glm::vec3& vel) {
	this->vel = vel;
}
void Entity::setVelocity(const GLfloat& vel) {
	this->vel = glm::vec3(vel);
}
void Entity::setAcceleration(const glm::vec3& acc) {
	this->acc = acc;
}
void Entity::setMovementSpeed(const GLfloat& speed) {
	movementSpeed = speed;
}

void Entity::addVelocity(const glm::vec3& vel) {
	this->vel += vel;
}
void Entity::addAcceleration(const glm::vec3& acc) {
	this->acc += acc;
}

void Entity::updatePosition(GLfloat deltaTime, std::vector<Face*> ground) {
	vel += acc * deltaTime;
	findGrounded(ground, vel * deltaTime);
	if (!grounded) vel.y -= GRAVITY * deltaTime;
	else if (vel.y < 0) vel.y = 0;
	pos += vel * deltaTime;
	renderer.setPosition(pos);
}

void Entity::move(Move_Dir dir) {
	switch (dir)
	{
	case Move_Dir::FORWARD:
		vel += forward * movementSpeed;
		break;
	case Move_Dir::BACKWARD:
		vel += -forward * movementSpeed;
		break;
	case Move_Dir::LEFT:
		vel += -right * movementSpeed;
		break;
	case Move_Dir::RIGHT:
		vel += right * movementSpeed;
		break;
	case Move_Dir::UP:
		if (!grounded) break;
		vel.y += jumpForce;
		break;
	case Move_Dir::DOWN:
		break;
	}
}

void Entity::render(glm::mat4 projection, Camera* cam) {
	if (!hasBody) return;
	renderer.render(*cam, projection);
}
GLboolean Entity::determinCollision(std::array<glm::vec3, 4> plane, glm::vec3 deltaV) {
	GLfloat a, b, c, eq, u, v;
	glm::vec3 norm;
	// normal calculating
	glm::vec3 pq = plane[1] - plane[0];
	glm::vec3 pr = plane[2] - plane[0];
	norm = glm::cross(pq, pr);

	// plane equation
	a = norm.x;
	b = norm.y;
	c = norm.z;
	eq = a * plane[0].x + b * plane[0].y + c * plane[0].z;

	v = eq - a * deltaV.x - b * deltaV.y - c * deltaV.z;
	u = a + b + c;
	// check intersection
	if (u != 0 || v == 0) {
		// is colliding
		for (int i = 0; i < 3; i ++) {
			plane[i] += plane[3];
		}
		glm::vec3 center = (plane[0] + plane[1] + plane[2]) / 3.0f;
		auto c = deltaV + getCenter();
		auto dist = c.y - center.y;
		if (dist < 1.015f) return 1;
	}
	return 0;
}
GLboolean Entity::determinCollision(std::array<glm::vec3, 4> plane) {
	return determinCollision(plane, pos);
}

void Entity::findGrounded(std::vector<Face*> ground, glm::vec3 deltaV) {
	// if (deltaV == glm::vec3(0)) return;
	for (auto& face : ground) {
		Buffer* b = std::get<0>(*face);
		std::vector<glm::vec3> vertices = b->getVertices();
		std::array<glm::vec3, 4> plane = { vertices[0], vertices[1], vertices[2] , std::get<2>(*face) };
		grounded = determinCollision(plane, deltaV);
		if (grounded) return;
	}
}
glm::vec3 Entity::getCenter() {
	return pos + glm::vec3(0.5f, 0.5, -0.5f);
}