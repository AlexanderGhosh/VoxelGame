#include "Player.h"
Player::Player() {
	object.setPosition({ 0, 0, 0 });
	movementSpeed = 6;
	cam.setPosition({ 0, 1.25, -0.75 });
	object.setPhysical(GL_TRUE);
	renderer = Render::ChunkMeshRender(false, "");
}
Player::Player(GLboolean init) : renderer(init, "") {
	if (init) {
		object.setPosition({ 0, 0, 0 });
		movementSpeed = 6;
		cam.setPosition({ 0, 1.25, -0.75 });
		object.setPhysical(GL_TRUE);
	}
}
Player::Player(glm::vec3 position, glm::vec3 camOff) {
	object.setPosition(position);
	movementSpeed = 6;
	camera_offset = camOff;
	cam.setPosition(camOff);
}
glm::vec3 Player::getPosition() {
	return object.getPosition();
}
void Player::create() {
	glm::vec3 pos = object.getPosition();
	for(GLuint i = 0; i < 2; i++){
		pos.y += i;
		std::string tex = "player/";
		tex += i == 0 ? "bottom" : "top";

		TEXTURE_NAMES name = i == 0 ? PLAYER_BOTTOM : PLAYER_TOP;

		Face face = { FACES[FRONT], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[BACK], TEXTURES[name], pos };
		faces.push_back(face);

		if (i != 0) {
			face = { FACES[TOP], TEXTURES[name], pos };
			faces.push_back(face);
		}
		if (i == 0) {
			face = { FACES[BOTTOM], TEXTURES[name], pos };
			faces.push_back(face);
		}
		face = { FACES[RIGHT], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[LEFT], TEXTURES[name], pos };
		faces.push_back(face);
	}
	std::vector<Face*> arg;
	for (auto& f : faces) {
		arg.push_back(&f);
	}

	renderer.loadMeshes(&faces);
	prevPos = object.getPosition();
}
void Player::render(glm::mat4 projection) {
	renderer.setPosition(object.getPosition());
	renderer.render(cam, projection);
}
Physics::Update Player::processMovement(Camera_Movement movement, GLfloat deltaTime) {
	//cam.ProcessMovement(movement, deltaTime);
	Physics::Update update;
	update.Sender = &object;
	update.Tag = Physics::COLLISION;
	update.Positon = object.getPosition();
	update.Extra = faces;
	glm::vec3 pos = object.getPosition();
	switch (movement)
	{
	case FORWARD:
		object.setPosition(pos + glm::vec3(0, 0, -1) *deltaTime * movementSpeed);
		break;
	case BACKWARD:
		object.setPosition(pos + glm::vec3(0, 0, -1) * -deltaTime * movementSpeed);
		break;
	case LEFT_C:
		object.setPosition(pos + glm::vec3(1, 0, 0) * -deltaTime * movementSpeed);
		break;
	case RIGHT_C:
		object.setPosition(pos + glm::vec3(1, 0, 0) * deltaTime * movementSpeed);
		break;
	case UP_C:
		object.setPosition(pos + glm::vec3(0, 1, 0) * deltaTime * movementSpeed);
		break;
	case DOWN_C:
		object.setPosition(pos + glm::vec3(0, 1, 0) * -deltaTime * movementSpeed);
		break;
	}

	update.Data = object.getPosition();
	object.setPosition(update.Positon);
	glm::vec3 p = object.getPosition() - pos;
	//renderer.addPosition(p);
	return update;
}
void Player::update() {
}
void Player::processMouse(GLfloat xOffset, GLfloat yOffset, GLfloat x, GLboolean constrainPitch) {
	x *= 0.01f;
	object.setRotation(cam.GetFront());

	glm::vec3 campos = { std::cos(x), camera_offset.y, camera_offset.z * std::sin(x) };
	glm::vec3 pos = object.getPosition();
	campos += pos;

	// cam.setPosition(campos);
	cam.ProcessMouseMovement(xOffset, yOffset);
	pos = glm::rotate(pos, object.getRotation().x, { 0, 1, 0 });
	// renderer.setRotation(object.getRotation(), false);
}
Camera& Player::getCamera() {
	return cam;
}