#include "Player.h"
Player::Player() : Entity(false) {
	
	movementSpeed = PLAYER_SPEED;
	cam.setPosition({ 0, 1.25, -0.75 });
	renderer = Render::ChunkMeshRender(false, "");
}
Player::Player(glm::vec3 position, glm::vec3 camOff) : Entity(true) {
	movementSpeed = PLAYER_SPEED;
	camera_offset = camOff;
	cam.setPosition(camOff);
	pos = position;
}
Camera& Player::getCamera() {
	return cam;
}
glm::vec3 Player::getPosition() {
	return pos;
}
void Player::create() {
	// Entity(true);
	for(GLuint i = 0; i < 2; i++){
		pos.y += i;
		std::string tex = "player/";
		tex += i == 0 ? "bottom" : "top";

		TEXTURE_NAMES name = i == 0 ? PLAYER_BOTTOM : PLAYER_TOP;

		Face face = { FACES[FRONT], TEXTURES[name], pos };
		body.push_back(face);

		face = { FACES[BACK], TEXTURES[name], pos };
		body.push_back(face);

		if (i != 0) {
			face = { FACES[TOP], TEXTURES[name], pos };
			body.push_back(face);
		}
		if (i == 0) {
			face = { FACES[BOTTOM], TEXTURES[name], pos };
			body.push_back(face);
		}
		face = { FACES[RIGHT], TEXTURES[name], pos };
		body.push_back(face);

		face = { FACES[LEFT], TEXTURES[name], pos };
		body.push_back(face);
	}

	renderer.loadMeshes(&body);
}
void Player::render(glm::mat4 projection, Camera* cam) {
	renderer.render(this->cam, projection);
}
void Player::processMouse(GLfloat xOffset, GLfloat yOffset, GLfloat x, GLboolean constrainPitch) {
	cam.ProcessMouseMovement(xOffset, yOffset);
}