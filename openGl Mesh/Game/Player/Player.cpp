#include "Player.h"
Player::Player() :renderer(false) {
	object.setPosition({ 0, 0, 0 });
	movementSpeed = 6;
	cam.setPosition({ 0, 1.25, -0.75 });
	object.setPhysical(GL_TRUE);
}
Player::Player(glm::vec3 position, glm::vec3 camOff) {
	object.setPosition(position);
<<<<<<< HEAD
	object.setVelocity({ 0, 0, 0 });
	object.setMass(60);
	movementSpeed = 1;
	camera_offset = camOff;
=======
	movementSpeed = 6;
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
	cam.setPosition(camOff);
	object.setKinematic(false);
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

<<<<<<< HEAD
		TEXTURE_NAMES name = i == 0 ? PLAYER_BOTTOM : PLAYER_TOP;
		
		Face face = { FACES[TOP], TEXTURES[name], pos };
		
=======
		FaceMesh face(FACES[FRONT], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);

		face = FaceMesh(FACES[BACK], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);

>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
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
<<<<<<< HEAD
		face = { FACES[FRONT], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[BACK], TEXTURES[name], pos };
		faces.push_back(face);

		face = { FACES[RIGHT], TEXTURES[name], pos };
=======
		face = FaceMesh(FACES[RIGHT], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
		faces.push_back(face);

		face = FaceMesh(FACES[LEFT], TEXTURES[GRASS]);
		face.setPosition(pos);
		face.setTexture(tex);
		faces.push_back(face);
	}
<<<<<<< HEAD

	renderer.loadMeshes(&faces);
=======
	// renderer.loadMeshes(&faces);
	renderer.setShader("block2");
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
	prevPos = object.getPosition();
	object.body = faces;
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
<<<<<<< HEAD
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
=======
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
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
}