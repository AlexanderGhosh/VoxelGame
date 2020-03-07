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

void Entity::updatePosition(GLfloat deltaTime, World& world) {
	deltaTime = 1.0f / 60.0f;
	grounded = 0;
	vel += acc * deltaTime;
	vel.y -= GRAVITY * deltaTime;
	auto collisions = determinCollision(world, vel * deltaTime);
	// GLboolean collisions = 0;
	if (!collisions) {
		pos += vel * deltaTime;
		collider.setPosition(pos);
	}
	else {
		collider.setPosition(pos);
		vel = { 0, 0, 0 };
	}

	/*if (collisions.y == -1) {
		grounded = 1;
	}
	if (collisions.x == 1) {
		if (vel.x > 0) vel.x = 0;
	}
	if (collisions.x == -1) {
		if (vel.x < 0) vel.x = 0;
	}
	if (collisions.z == 1) {
		if (vel.z > 0) vel.z = 0;
	}
	if (collisions.z == -1) {
		if (vel.z < 0) vel.z = 0;
	}*/

	// if (!grounded) vel.y -= GRAVITY * deltaTime;
	// else if (vel.y < 0) vel.y = 0;
	// pos += vel * deltaTime;
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

GLboolean Entity::determinCollision(World& world, glm::vec3 deltaV) {
	/*auto rayOrigin = getCenter();
	auto rayEnd = rayOrigin + deltaV;
	auto r = glm::round(rayEnd);
	// Chunk& chunkOccupied = world.getOccupiedChunk(rayEnd);
	std::pair<glm::vec3, glm::vec3> bounds = { rayEnd + glm::vec3(1), rayEnd - glm::vec3(1) };

	// if (chunkOccupied.isNull()) return { 0, 0, 0 };
	glm::vec3 res(0);
	for (auto& mesh : world.getWorldMesh()) {
		Face& face = *mesh;
		glm::vec3 p = std::get<2>(face);
		Buffer* b = std::get<0>(face);
		glm::vec3 diff = rayEnd - p;
		switch (b->type)
		{
		case TOP:
			if (diff.y < 0.905 && isBellow(p, rayEnd)) res.y = -1;
			break;
		case BOTTOM:
			//if (diff.y > -1) return 1;
			break;
		case FRONT:
			if (diff.z < 0.935 && isBehind(p, rayEnd)) res.z = -1; // -1
			break;
		case BACK:
			if (diff.z > -0.935 && diff.z < -0.0f && isInFront(p, rayEnd)) res.z = 1; // 1
			break;
		case LEFT: 
			if (diff.x > -0.935 && diff.x < -0.0f && isRight(p, rayEnd)) res.x = 1; // 1
			break; 
		case RIGHT: 
			if (diff.x < 0.935 && isLeft(p, rayEnd)) res.x = -1; // 1.03 -1
			break;
		}
	}

	return res;*/
	glm::vec3 nPos = pos + deltaV;
	collider.setPosition(nPos);
	auto& worldMesh = world.getWorldMesh();
	std::vector<glm::vec3> blockPositions;
	for (auto& face : worldMesh) {
		blockPositions.push_back(std::get<2>(*face));
	}
	
	BoxCollider boxColl = BoxCollider(glm::vec3(1.0f), glm::vec3(0));
	for (auto& pos : blockPositions) {
		boxColl.setPosition(pos);
		GLboolean colliding = collider.isColliding(boxColl);
		if (colliding) return colliding;
	}
	return 0;
}
glm::vec3 Entity::getCenter() {
	return getCenter(pos);
}
glm::vec3 Entity::getCenter(glm::vec3 pos) {
	return pos;
}