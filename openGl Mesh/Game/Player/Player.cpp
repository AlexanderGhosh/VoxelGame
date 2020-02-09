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
	object.setVelocity({ 0, 0, 0 });
	object.setMass(60);
	movementSpeed = 1;
	camera_offset = camOff;
	cam.setPosition(camOff);
	object.setKinematic(false);
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
		
		Face face = { FACES[TOP], TEXTURES[name], pos };
		
		if (i != 0) {
			face = { FACES[TOP], TEXTURES[name], pos };
			faces.push_back(face);
		}
		if (i == 0) {
			face = { FACES[BOTTOM], TEXTURES[name], pos };
			faces.push_back(face);
		}
		face = { FACES[FRONT], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[BACK], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[RIGHT], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[LEFT], TEXTURES[name], pos };
		faces.push_back(face);
	}

	renderer.loadMeshes(&faces);
	prevPos = object.getPosition();
	object.body = faces;
}
void Player::render(glm::mat4 projection) {
	renderer.setPosition(object.getPosition());
	renderer.render(cam, projection);
}
Physics::Update Player::processMovement(Camera_Movement movement, GLfloat deltaTime) {
	// object.setVelocity({ 0, 0, 0 });
	Physics::Update update;
	update.Tag = Physics::COLLISION;
	update.Sender = &object;
	update.PrevPosition = object.getPosition();
	update.PrevVelocity = object.getVelocity();
	glm::vec3 newPos = object.getPosition();
	glm::vec3 newVel = object.getVelocity();
	switch (movement)
	{
	case FORWARD:
		newPos += glm::vec3(0, 0, -1) * deltaTime * movementSpeed;
		break;
	case BACKWARD:
		newPos += glm::vec3(0, 0, 1) * deltaTime * movementSpeed;
		break;
	case LEFT_C:
		newPos += glm::vec3(-1, 0, 0) * deltaTime * movementSpeed;
		break;
	case RIGHT_C:
		newPos += glm::vec3(1, 0, 0) * deltaTime * movementSpeed;
		break;
	case UP_C:
		newPos += glm::vec3(0, 1, 0) * deltaTime * movementSpeed;
		break;
	case DOWN_C:
		newPos += glm::vec3(0, -1, 0) * deltaTime * movementSpeed;
		break;
	}
	// cam.setPosition(newPos - object.getPosition() + cam.GetPosition());
	// cam.updateCameraVectors();
	update.Position = newPos;
	update.Velocity = newVel;
	return update;
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
Physics::Object& Player::getObject() {
	return object;
}