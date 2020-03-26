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
	collider = BoxCollider(glm::vec3(1.0f), this->pos);
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

void Entity::updatePosition(GLfloat deltaTime, World& world, std::string& collision) {
	deltaTime = 1.0f / 60.0f;
	grounded = 0;
	vel += acc * deltaTime;
	vel.y -= GRAVITY * deltaTime;
	glm::ivec3 collisions = determinCollision(world, vel * deltaTime);
	collision = glm::to_string(collisions);
	if (collisions.y == -1) {
		grounded = 1;
		vel.y = 0;
	}
	if (collisions.x == -1) {
		vel.x = 0;
	}
	if (collisions.x == 1) {
		vel.x = 0;
	}
	if (collisions.z == -1) {
		vel.z = 0;
	}
	if (collisions.z == 1) {
		vel.z = 0;
	}

	/*if (!grounded) vel.y -= GRAVITY * deltaTime;
	else if (vel.y < 0) vel.y = 0;*/
	pos += vel * deltaTime;
	renderer.setPosition(pos);
}

void Entity::move(Move_Dir dir) {
	prevVel = vel;
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
GLboolean isAbove(glm::vec3 above, glm::vec3 bellow, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { bellow + glm::vec3(0.5), bellow - glm::vec3(0.5) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (above.x < top.x && above.x > bottom.x) {
			if (above.z < top.z && above.z > bottom.z) {
				if (above.y > top.y) {
					return 1;
				}
			}
		}
	}
	else {
		if (above.x == bellow.x && above.z == bellow.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isBellow(glm::vec3 bellow, glm::vec3 above, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { above + glm::vec3(0.9), above - glm::vec3(0.9) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (bellow.x < top.x && bellow.x > bottom.x) {
			if (bellow.z < top.z && bellow.z > bottom.z) {
				if (bellow.y < above.y) {
					return 1;
				}
			}
		}
	}
	else {
		if (bellow.x == above.x && bellow.z == above.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isRight(glm::vec3 right, glm::vec3 left, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { left + glm::vec3(0.9), left - glm::vec3(0.9) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (right.y < top.y && right.y > bottom.y) {
			if (right.z < top.z && right.z > bottom.z) {
				if (right.x > left.x) {
					return 1;
				}
			}
		}
	}
	else {
		if (right.y == left.y && right.z == left.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isLeft(glm::vec3 left, glm::vec3 right, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { right + glm::vec3(0.9), right - glm::vec3(0.9) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (left.y < top.y && left.y > bottom.y) {
			if (left.z < top.z && left.z > bottom.z) {
				if (left.x < right.x) {
					return 1;
				}
			}
		}
	}
	else {
		if (left.y == right.y && left.z == right.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isInFront(glm::vec3 front, glm::vec3 behind, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { behind + glm::vec3(0.9), behind - glm::vec3(0.9) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (front.x < top.x && front.x > bottom.x) {
			if (front.y < top.y && front.y > bottom.y) {
				if (front.z > behind.z) {
					return 1;
				}
			}
		}
	}
	else {
		if (behind.x == front.x && behind.y == front.y) {
			return 1;
		}
	}
	return 0;
}
GLboolean isBehind(glm::vec3 behind, glm::vec3 front, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { front + glm::vec3(0.9), front - glm::vec3(0.9) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (behind.x < top.x && behind.x > bottom.x) {
			if (behind.y < top.y && behind.y > bottom.y) {
				if (behind.z < front.z) {
					return 1;
				}
			}
		}
	}
	else {
		if (behind.x == front.x && behind.y == front.y) {
			return 1;
		}
	}
	return 0;
}

glm::ivec3 Entity::determinCollision(World& world, glm::vec3 deltaV) {
	glm::ivec3 res(0);
	if (glm::all(glm::equal(deltaV, glm::vec3(0)))) return res; 
	glm::ivec3 rounded = glm::round(pos + deltaV);
	if (rounded == glm::ivec3(0, -1, 1)) {
		int g = 0;
	}
	Faces_P mesh;
	std::vector<Chunk*> chunks = world.getSubChunkOccupied(rounded, 1);
	for (auto& chunk : chunks) {
		if (!chunk) continue;
		auto t = chunk->getMeshes();
		mesh.insert(mesh.end(), t.begin(), t.end());
	}
	BoxCollider boxColl = BoxCollider(glm::vec3(1.0f), glm::vec3(0));
	for (auto& face : mesh) {
		boxColl.setPosition(std::get<2>(*face)); // COMMENT WHEN TESTING
		for (GLubyte i = 0; i < 3; i++) {
			GLfloat& component = deltaV[i];
			if (component == 0) continue;
			glm::vec3 p = this->pos;
			p[i] += component;
			collider.setPosition(p);
			GLboolean colliding = collider.isColliding(boxColl);
			if (colliding) {
				res[i] = 1;
				if (component < 0) res[i] = -1;
			}
		}
	}
	return res;
}
glm::vec3 Entity::getCenter() {
	return getCenter(pos);
}
glm::vec3 Entity::getCenter(glm::vec3 pos) {
	return pos;
}