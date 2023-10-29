#pragma once
#include <gtx/string_cast.hpp>
#include <random>
#include <algorithm>


#include "../../Renders/chunkRender.h"
#include "../Physics/BoxCollider.h"
#include "../World/Chunks/ChunkColumn.h"
#include "PlayerInv.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Entity
{
public:
	Entity(glm::vec3 camOffset, bool clipping, bool flying);
	Entity(bool init = GL_FALSE, glm::vec3 dimentions = glm::vec3(1.0f));
	glm::vec3& getPosition();
	glm::vec3& getVelocity();
	glm::vec3& getAcceleration();
	BoxCollider& getCollider();
	Faces& getBody();
	Texture_Names* getTextures();
	int& getHealth();
	Camera& getCamera();
	bool& getFlying();
	void getNewTarget(glm::vec3 playerPos);
	PlayerInv& getInventory();
	bool getSubmerged();
	
	void setPosition(const glm::vec3& pos);
	void setVelocity(const glm::vec3& vel);
	void setAcceleration(const glm::vec3& acc);
	void setMovementSpeed(const float& speed);
	void setTextues(Texture_Names texB, Texture_Names texT);
	void setTarget(glm::vec3 targ);
	void setInvincable(bool inv);

	void updatePosition(float deltaTime, std::vector<ChunkColumn*>& adjacesnt);
	void updateCamera(float xOff, float yOff);

	void move(Move_Dir dir);
	void moveToTarget(glm::vec3 playerPos = glm::vec3(10000000));
	void create();

	void render(glm::mat4 projection, Camera& cam);

	void attack(Entity& e);
	void takeDamage(unsigned int dmgTaken);
	void update(glm::mat4 projection, Camera& cam, std::vector<ChunkColumn*>& adjacent, ChunkColumn* occupied, float deltaTime, glm::vec3 playerPos = glm::vec3(10000000));

	std::string getTag();
	bool isDead;
	void lookAt(glm::vec3 pos);
private:
	glm::ivec3 determinCollision(std::vector<ChunkColumn*>& adjacesnt, glm::vec3 deltaV);
	void calcMovementPath();
	void moveBlock(Move_Dir dir);
	void lookAt(Move_Dir dir);
	void toggleShowDamage();
	void checkDead();
	glm::vec3 wanderTarget();

	void clampVelocity();

	// player stuff
	Camera playerCam;
	bool hasCamera, isClipping, canFly, loaded;
	glm::vec3 cameraOffset;
	PlayerInv inventory;


	glm::vec3 pos, vel, acc;
	glm::vec3 forward, right;
	float movementSpeed, jumpForce, jumpForce_orig, resToGravity;
	std::vector<Face> body;
	Render::ChunkMeshRender renderer;
	bool grounded, submerged;
	bool hasBody;
	BoxCollider collider;
	Texture_Names textures[2];

	glm::vec3 targetPosition, prevBlock, nxtBlock;
	bool hasTarget, wandering;
	unsigned int stage;
	std::vector<Move_Dir> movementPath;

	bool showDmg;
	bool hasAttacked;
	GLubyte attackCooldown; // in frames
	GLubyte attackTimer;
	bool invunrable;
	GLubyte invunrableTimer;
	GLubyte invunrableCooldown; // in frames

	std::string tag;
	int health;
	unsigned int attackDmg;
	bool invincable;
};

