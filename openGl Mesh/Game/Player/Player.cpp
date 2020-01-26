#include "Player.h"
Player::Player() :renderer(false) {
	object.setPosition({ 0, 0, 0 });
	movementSpeed = 6;
	cam.setPosition({ 0, 1.25, -0.75 });
	object.setPhysical(GL_TRUE);
}
Player::Player(glm::vec3 position, glm::vec3 camOff) {
	object.setPosition(position);
	movementSpeed = 6;
	cam.setPosition(camOff);
}
glm::vec3 Player::getPosition() {
	return object.getPosition();
}
void Player::create() {
	std::vector<FaceMesh> faces;
	glm::vec3 pos = object.getPosition();
	for(GLuint i = 0; i < 2; i++){
		pos = { pos.x, pos.y + i, pos.z };
		std::string tex = "player/";
		tex += i == 0 ? "bottom" : "top";

		FaceMesh face(FACES[FRONT], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);

		face = FaceMesh(FACES[BACK], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);

		if (i != 0) {
			face = FaceMesh(FACES[TOP], TEXTURES[GRASS]);
			face.setPosition(pos);
			face.setTexture(tex);
			faces.push_back(face);
		}
		if (i == 0) {
			face = FaceMesh(FACES[BOTTOM], TEXTURES[GRASS]);
			face.setPosition(pos);
			face.setTexture(tex);
			faces.push_back(face);
		}
		face = FaceMesh(FACES[RIGHT], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);

		face = FaceMesh(FACES[LEFT], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);
	}
	// renderer.loadMeshes(&faces);
	renderer.setShader("block2");
	prevPos = object.getPosition();
}
void Player::render(glm::mat4 projection) {
	renderer.addPosition(object.getPosition() - prevPos);
	renderer.setRotaion(object.getRotation());
	renderer.render(cam, projection);
	//Physics::Update update = object.getUpdate();
	//glm::vec3 pos = object.getPosition();
	/*if (pos.x == prevPos.x && pos.y == prevPos.y && pos.z== prevPos.z) {
		update.null = GL_TRUE;
	}
	else {
		update.null = GL_FALSE;
	}*/
	prevPos = object.getPosition();
	//return update;
}
Physics::Update Player::processMovement(Camera_Movement movement, GLfloat deltaTime) {
	cam.ProcessMovement(movement, deltaTime);
	Physics::Update update;
	update.Sender = &object;
	update.null = GL_FALSE;
	update.Tag = Physics::COLLISION;
	update.Positon = object.getPosition();

	glm::vec3 pos = object.getPosition();
	if (movement == FORWARD) {
		object.setPosition(pos + cam.GetFront() * deltaTime * movementSpeed);
	}
	if (movement == BACKWARD) {
		object.setPosition(pos + cam.GetFront() * -deltaTime * movementSpeed);
	}
	if (movement == LEFT) {
		object.setPosition(pos + cam.GetRight() * -deltaTime * movementSpeed);
	}
	if (movement == RIGHT) {
		object.setPosition(pos + cam.GetRight() * deltaTime * movementSpeed);
	}

	update.Data = object.getPosition();

	return update;
}
void Player::processMouse(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch) {
	cam.ProcessMouseMovement(xOffset, yOffset);
	object.setRotation(cam.GetFront());
}