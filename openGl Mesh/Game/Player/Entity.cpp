#include "Entity.h"
Entity::Entity(glm::vec3 camOffset, GLboolean clipping, GLboolean flying) : Entity(1) {
	hasCamera = 1;
	movementSpeed = PLAYER_SPEED;
	this->cameraOffset = camOffset;
	collider = BoxCollider(glm::vec3(1), pos);
	collider.setDimentions(glm::vec3(0.85f), glm::vec3(0.85f, 2.55f, 0.85f));
	this->hasBody = 0;
	/*invBar = {
		Blocks::GRASS,
		Blocks::DIRT,
		Blocks::STONE,
		Blocks::WATER,
		Blocks::LOG,
		Blocks::LEAF,
		Blocks::AIR,
		Blocks::AIR,
		Blocks::AIR
	};
	invSlot = 0;*/
	isClipping = clipping;
	canFly = flying;
	tag = "player";
}
Entity::Entity(GLboolean init, glm::vec3 dimentions) : attackCooldown(90), invunrableCooldown(60) {
	loaded = 1;
	hasCamera = 0;
	pos = { 0, 0, 0 };
	vel = { 0, 0, 0 };
	acc = { 0, 0, 0 };
	forward = { 0, 0, -1 };
	right = { 1, 0, 0 };
	movementSpeed = 1; // m/s
	jumpForce = 5;
	grounded = 0;
	hasBody = GL_FALSE;
	textures[0] = Texture_Names::ERROR;
	textures[1] = Texture_Names::ERROR;
	collider = BoxCollider(dimentions, this->pos);
	if (init) {
		renderer = Render::ChunkMeshRender(true, "");
		renderer.setRotation({ 0, 1, 0 }, 0);
	}
	else {
		renderer = Render::ChunkMeshRender(false, "");
	}

	targetPosition = glm::vec3(0);
	hasTarget = 0;

	hasAttacked = 0;
	attackTimer = 0;
	showDmg = 0;

	invunrableTimer = 0;
	invunrable = 0;

	isDead = 0;
	health = 100;
	attackDmg = 10;
	tag = "hostile";
	isClipping = 1;
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

void Entity::setPosition(const glm::vec3& pos) {
	this->pos = pos;
}

void Entity::setVelocity(const glm::vec3& vel) {
	this->vel = vel;
}
void Entity::setAcceleration(const glm::vec3& acc) {
	this->acc = acc;
}
void Entity::setMovementSpeed(const GLfloat& speed) {
	movementSpeed = speed;
}

void Entity::setTextues(Texture_Names texB, Texture_Names texT)
{
	textures[0] = texB;
	textures[1] = texT;
}

void Entity::setTarget(glm::vec3 targ)	{
	if (glm::round(targ) == glm::round(pos)) return;
	stage = 0;
	movementPath.clear();
	hasTarget = 1;
	targetPosition = targ;
	prevBlock = glm::round(pos);
	calcMovementPath();
	nxtBlock = prevBlock;
	if (movementPath.size() == 0) {
		hasTarget = 0;
		return;
	}
	switch (movementPath[stage])
	{
	case Move_Dir::FORWARD:
		nxtBlock.z--;
		break;
	case Move_Dir::BACKWARD:
		nxtBlock.z++;
		break;
	case Move_Dir::LEFT:
		nxtBlock.x--;
		break;
	case Move_Dir::RIGHT:
		nxtBlock.x++;
		break;
	}
}

void Entity::setInvincable(GLboolean inv)
{
	invincable = inv;
}

void Entity::getNewTarget() {
	stage = 0;
	hasTarget = !hasTarget;
	targetPosition = wanderTarget();
	prevBlock = glm::round(pos);
	calcMovementPath();
	nxtBlock = prevBlock;
	switch (movementPath[stage])
	{
	case Move_Dir::FORWARD:
		nxtBlock.z++;
		break;
	case Move_Dir::BACKWARD:
		nxtBlock.z--;
		break;
	case Move_Dir::LEFT:
		nxtBlock.x--;
		break;
	case Move_Dir::RIGHT:
		nxtBlock.x++;
		break;
	}
}

void Entity::updatePosition(GLfloat deltaTime, std::vector<ChunkColumn*>& adjacesnt) {
	deltaTime = 1.0f / 60.0f;
	if (!canFly) {
		grounded = 0;
		vel += acc * deltaTime;
		vel.y -= GRAVITY * deltaTime;
	}
	if (isClipping) {
		glm::ivec3 collisions = determinCollision(adjacesnt, vel * deltaTime);
		if (collisions.y == -1) {
			grounded = 1;
			vel.y = 0;
		}
		if (collisions.y == 1) {
			vel.y = 0;
		}
		if (collisions.x == -1) {
			vel.x = 0; if (hasTarget) {
				move(Move_Dir::UP);
			}
		}
		if (collisions.x == 1) {
			vel.x = 0; if (hasTarget) {
				move(Move_Dir::UP);
			}
		}
		if (collisions.z == -1) {
			vel.z = 0; if (hasTarget) {
				move(Move_Dir::UP);
			}
		}
		if (collisions.z == 1) {
			vel.z = 0; if (hasTarget) {
				move(Move_Dir::UP);
			}
		}
	}

	if (hasCamera) {
		playerCam.setPosition(pos + cameraOffset);
	}
	clampVelocity();
	pos += vel * deltaTime;
	if (!hasBody) return;
	renderer.setPosition(this->pos);
}

void Entity::updateCamera(GLfloat xOff, GLfloat yOff)
{
	playerCam.ProcessMouseMovement(xOff, yOff);
	forward = glm::normalize(playerCam.GetFront() * glm::vec3(1, 0, 1));
	right = glm::normalize(playerCam.GetRight() * glm::vec3(1, 0, 1));
}

void Entity::move(Move_Dir dir) {
	GLfloat jf = canFly ? movementSpeed : jumpForce;
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
		if (!grounded && !canFly) break;
		vel.y += jf;
		break;
	case Move_Dir::DOWN:
		if (!canFly) break;
		vel.y -= jf;
		break;
	}
}
void Entity::moveToTarget() {
	if (vel.y > 0) return;
	if (!hasTarget) {
		return;
		getNewTarget();
	}
	moveBlock(movementPath[stage]);
}
void Entity::create() {
	hasBody = 1;
	std::vector<glm::vec3> relativePositions;
	for (GLuint i = 0; i < 2; i++) {
		glm::vec3 rel(0);
		if (i) {
			rel.y = 0.8;
		}
		pos.y += i;
		std::string tex = "player/";
		tex += !i ? "bottom" : "top";

		Texture_Names name = textures[i];
		auto texture = toIndex(name);
		Face face = { FACES[FRONT], TEXTURES[texture], pos };
		body.push_back(face);
		relativePositions.push_back(rel);

		face = { FACES[BACK], TEXTURES[texture], pos };
		body.push_back(face);
		relativePositions.push_back(rel);

		if (i != 0) {
			face = { FACES[TOP], TEXTURES[texture], pos };
			body.push_back(face);
			relativePositions.push_back(rel);
		}
		if (i == 0) {
			face = { FACES[BOTTOM], TEXTURES[texture], pos };
			body.push_back(face);
			relativePositions.push_back(rel);
		}
		face = { FACES[RIGHT], TEXTURES[texture], pos };
		body.push_back(face);
		relativePositions.push_back(rel);

		face = { FACES[LEFT], TEXTURES[texture], pos };
		body.push_back(face);
		relativePositions.push_back(rel);
	}
	renderer.setRelativePositions(relativePositions);
	renderer.loadMeshes(&body);
}


void Entity::render(glm::mat4 projection, Camera& cam) {
	if (!hasBody) return;
	if (hasCamera) cam = playerCam;
	renderer.render(cam, projection);
}
void Entity::attack(Entity& e) {
	if (hasAttacked) return;
	GLfloat dist = glm::abs(glm::distance(e.pos, pos));
	if (dist > 1.5f) return;
	if (tag != "player") {
		int t = 0;
	}
	if (e.tag != "player" || e.tag == "player" && tag == "player") return;
	e.takeDamage(attackDmg);
	hasAttacked = 1;
}
void Entity::takeDamage(GLuint dmgTaken) {
	if (!invunrable && !invincable) {
		invunrable = 1;
		invunrableTimer = 0;
		health -= dmgTaken;
		toggleShowDamage();
		move(Move_Dir::UP);
		std::cout << "entity with tag: " << tag << " health: " << std::to_string(health) << std::endl;
	}
}
void Entity::update(glm::mat4 projection, Camera& cam, std::vector<ChunkColumn*>& adjacent, ChunkColumn* occupied) {
	checkDead();
	if (!occupied) {
		loaded = 0;
	}
	else {
		loaded = 1;
	}
	if (isDead || !loaded) return;

	// attack timmer
	if (hasAttacked) {
		attackTimer++;
		if (attackTimer >= attackCooldown) {
			hasAttacked = 0;
			attackTimer = 0;
		}
	}
	// invunrable timer
	if (invunrable) {
		invunrableTimer++;
		if (invunrableTimer >= invunrableCooldown) {
			invunrable = 0;
			toggleShowDamage();
		}
	}
	if(!hasCamera)
		moveToTarget();
	
	updatePosition(1.0f / 60.0f, adjacent);

	render(projection, cam);
}

glm::ivec3 Entity::determinCollision(std::vector<ChunkColumn*>& adjacesnt, glm::vec3 deltaV) {
	glm::ivec3 res(0);
	if (glm::all(glm::equal(deltaV, glm::vec3(0)))) return res; 
	glm::ivec3 rounded = glm::round(pos + deltaV);
	if (rounded == glm::ivec3(0, -1, 1)) {
		int g = 0;
	}
	std::unordered_map<GLuint, FaceB_p> worldMesh;
	for (auto& chunk : adjacesnt) {
		std::unordered_map<GLuint, FaceB_p>& mesh = chunk->getMesh();
		for (auto& m : mesh) {
			const GLuint key = m.first;
			FaceB_p& faces = m.second;
			try {
				FaceB_p& faces2 = worldMesh.at(key);
				std::vector<glm::mat4>& models = std::get<2>(faces);
				std::get<2>(faces2).insert(std::get<2>(faces2).end(), models.begin(), models.end());
			}
			catch (std::exception e) {
				worldMesh.insert({ key, faces });
			}
		}
	}
	if (worldMesh.size() == 0) return res;
	Faces mesh;

	for (auto& m : worldMesh) {
		auto t = toFaces(m.second);
		mesh.insert(mesh.end(), t.begin(), t.end());
	}
	BoxCollider boxColl = BoxCollider(glm::vec3(1.0f), glm::vec3(0));
	for (auto& face : mesh) {
		boxColl.setPosition(std::get<2>(face));
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
void Entity::calcMovementPath() {
	if (!hasTarget) return;
	/*if (glm::distance(targetPosition, pos) > 32) {
		getNewTarget();
		return;
	}*/
	glm::vec3 targ = glm::round(targetPosition);
	glm::vec3 diff = glm::round(targetPosition - pos);
	for (GLuint x = 0; x < std::abs(diff.x); x++) {
		movementPath.push_back(diff.x < 0 ? Move_Dir::LEFT : Move_Dir::RIGHT);
	}
	for (GLuint z = 0; z < std::abs(diff.z); z++) {
		movementPath.push_back(diff.z < 0 ? Move_Dir::FORWARD : Move_Dir::BACKWARD);
	}
}
void Entity::moveBlock(Move_Dir dir) {
	glm::vec2 comp = { pos.x, pos.z };
	comp = glm::round(comp);
	if (comp == glm::vec2(nxtBlock.x, nxtBlock.z)) {
		vel.x = 0; 
		vel.z = 0;
		if (!grounded) return;
		prevBlock = nxtBlock;
		stage++;
		nxtBlock = prevBlock;

		if (stage >= movementPath.size()) {
			hasTarget = 0;
			getNewTarget();
			return;
		}
		lookAt(movementPath[stage]);
		switch (movementPath[stage])
		{
		case Move_Dir::FORWARD:
			nxtBlock.z--;
			break;
		case Move_Dir::BACKWARD:
			nxtBlock.z++;
			break;
		case Move_Dir::LEFT:
			nxtBlock.x--;
			break;
		case Move_Dir::RIGHT:
			nxtBlock.x++;
			break;
		}
		return;
	}

	move(dir);
}
void Entity::lookAt(Move_Dir dir) {
	if (!hasBody) return;
	GLfloat angle = 90;
	switch (dir)
	{
	case Move_Dir::FORWARD:
		angle = 180;
		break;
	case Move_Dir::BACKWARD:
		angle = 0;
		break;
	case Move_Dir::LEFT:
		angle = 270;
		break;
	case Move_Dir::RIGHT:
		angle = 90;
		break;
	}
	renderer.setRotation({ 0, 1, 0 }, angle);
}
void Entity::lookAt(glm::vec3 pos)
{
	glm::vec3 diff = pos - this->pos;
	GLfloat angle = glm::degrees(glm::atan(diff.x / diff.z));
	if (diff.z < 0) {
		angle = 180.0f + angle;
	}
	renderer.setRotation({ 0, 1, 0 }, angle);
}
void Entity::toggleShowDamage()
{
	showDmg = !showDmg;
	Shader& shader = renderer.getShader();
	shader.bind();
	shader.setValue("isDamaged", showDmg);
}

void Entity::checkDead() {
	if (health <= 0) {
		isDead = 1;
		std::cout << "entity with tag: " << tag << " is dead\n";
	}
}

glm::vec3 Entity::wanderTarget()
{
	glm::ivec3 p = glm::round(pos);
	std::array<glm::ivec3, 2> range = {
		p + glm::ivec3(WANDER_RANGE),
		p - glm::ivec3(WANDER_RANGE),
	};
	for (auto& r : range) {
		for (GLubyte i = 0; i < 3; i++) {
			if (!r[i]) r++;
		}
	}
	GLfloat x = rand() % range[1].x + range[0].x;
	GLfloat z = rand() % range[1].z + range[0].z;
	return { x, 0, z };
}

void Entity::clampVelocity()
{
	for (GLubyte i = 0; i < 3; i++) {
		if (i == 1) continue;
		if (vel[i] > movementSpeed) {
			vel[i] = movementSpeed;
		}
		if (vel[i] < -movementSpeed) {
			vel[i] = -movementSpeed;
		}
	}
}

std::string Entity::getTag()
{
	return tag;
}

BoxCollider& Entity::getCollider()
{
	return collider;
}

Faces& Entity::getBody()
{
	return body;
}

Texture_Names* Entity::getTextures()
{
	return textures;
}

GLint& Entity::getHealth()
{
	return health;
}

Camera& Entity::getCamera()
{
	return playerCam;
}

GLboolean& Entity::getFlying()
{
	return canFly;
}
