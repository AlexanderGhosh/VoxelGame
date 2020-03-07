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
	collider = BoxCollider(glm::vec3(0.85f), pos);
}
Camera& Player::getCamera() {
	return cam;
}
glm::vec3 Player::getPosition() {
	return pos;
}
std::string Player::updatePosition(GLfloat deltaTime, World& world) {
	std::string res = "";
	Entity::updatePosition(deltaTime, world, res);
	// cam.setPosition(pos + camera_offset);
	return res;
}
void Player::create() {
	// Entity(true);
	for(GLuint i = 0; i < 2; i++){
		pos.y += i;
		std::string tex = "player/";
		tex += i == 0 ? "bottom" : "top";

		Texture_Names name = i == 0 ? Texture_Names::PLAYER_BOTTOM : Texture_Names::PLAYER_TOP;
		auto texture = toIndex(name);
		Face face = { FACES[FRONT], TEXTURES[texture], pos };
		body.push_back(face);

		face = { FACES[BACK], TEXTURES[texture], pos };
		body.push_back(face);

		if (i != 0) {
			face = { FACES[TOP], TEXTURES[texture], pos };
			body.push_back(face);
		}
		if (i == 0) {
			face = { FACES[BOTTOM], TEXTURES[texture], pos };
			body.push_back(face);
		}
		face = { FACES[RIGHT], TEXTURES[texture], pos };
		body.push_back(face);

		face = { FACES[LEFT], TEXTURES[texture], pos };
		body.push_back(face);
	}

	renderer.loadMeshes(&body);
}
void Player::render(glm::mat4 projection, Camera* cam) {
	renderer.render(this->cam, projection);
}
void Player::updateCamera(GLfloat xOff, GLfloat yOff) {
	cam.ProcessMouseMovement(xOff, yOff);
	// forward = cam.GetFront() * glm::vec3(1, 0, 1);
	// right = cam.GetRight() * glm::vec3(1, 0, 1);
}