//#include "Player.h"
//Player::Player() : Entity(false, { 1, 2, 1 }) {
//	movementSpeed = PLAYER_SPEED;
//	cam.setPosition({ 0, 1.25, -0.75 });
//	renderer = Render::ChunkMeshRender(false, "");
//}
//Player::Player(glm::vec3 position, glm::vec3 camOff, GLboolean attachCam, GLboolean render, GLboolean clipping, GLboolean flying) : Entity(true) {
//	movementSpeed = PLAYER_SPEED;
//	camera_offset = camOff;
//	cam.setPosition(camOff + position);
//	pos = position;
//	collider = BoxCollider(glm::vec3(0.85f), pos);
//	collider.setDimentions(glm::vec3(0.85f), glm::vec3(0.85f, 2.55f, 0.85f));
//	this->attachCam = attachCam;
//	this->hasBody = render;
//	invBar = {
//		Blocks::GRASS,
//		Blocks::DIRT,
//		Blocks::STONE,
//		Blocks::WATER,
//		Blocks::LOG,
//		Blocks::LEAF,
//		Blocks::AIR,
//		Blocks::AIR,
//		Blocks::AIR
//	};
//	invSlot = 0;
//	isClipping = clipping;
//	canFly = flying;
//	tag = "player";
//}
//Camera& Player::getCamera() {
//	return cam;
//}
//glm::vec3 Player::getPosition() {
//	return pos;
//}
//void Player::updatePosition(GLfloat deltaTime, std::vector<ChunkColumn*>& adjacentChunks) {
//	Entity::updatePosition(deltaTime, adjacentChunks, isClipping, canFly);
//	if (attachCam) cam.setPosition(pos + camera_offset);
//	return;
//}
//void Player::create() {
//	setTextues(Texture_Names::PLAYER_BOTTOM, Texture_Names::PLAYER_TOP);
//	Entity::create();
//}
//void Player::render(glm::mat4 projection, Camera* cam) {
//	if (!canRender) return;
//	renderer.render(this->cam, projection);
//}
//void Player::updateCamera(GLfloat xOff, GLfloat yOff) {
//	cam.ProcessMouseMovement(xOff, yOff);
//	if (attachCam) {
//		forward = cam.GetFront() * glm::vec3(1, 0, 1);
//		right = cam.GetRight() * glm::vec3(1, 0, 1);	}
//}
//GLubyte Player::getSlot() {
//	return invSlot;
//}
//std::array<Blocks, 9> Player::getInvBar() {
//	return invBar;
//}
//void Player::setInvSlot(GLubyte slot) {
//	invSlot = slot;
//}
//
//void Player::attack(Entity& e) {
//	e.takeDamage(attackDmg);
//}
//
//Player& Player::operator=(Entity& e)
//{
//	Player& res = *this;
//	res.pos = e.getPosition();
//	res.vel = e.getVelocity();
//	res.acc = e.getAcceleration();
//	res.health = e.getHealth();
//	res.tag = e.getTag();
//	res.isDead = res.isDead;
//	// res.attackCooldown = res.getAttackCooldown();
//	return res;
//}
//
///*void Player::move(Move_Dir dir)
//{
//	Entity::move(dir, canFly);
//}*/
