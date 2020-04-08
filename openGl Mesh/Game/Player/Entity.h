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
	Entity(glm::vec3 camOffset, GLboolean clipping, GLboolean flying);
	Entity(GLboolean init = GL_FALSE, glm::vec3 dimentions = glm::vec3(1.0f));
	glm::vec3& getPosition();
	glm::vec3& getVelocity();
	glm::vec3& getAcceleration();
	BoxCollider& getCollider();
	Faces& getBody();
	Texture_Names* getTextures();
	GLint& getHealth();
	Camera& getCamera();
	GLboolean& getFlying();
	void getNewTarget(glm::vec3 playerPos);
	PlayerInv& getInventory();
	
	void setPosition(const glm::vec3& pos);
	void setVelocity(const glm::vec3& vel);
	void setAcceleration(const glm::vec3& acc);
	void setMovementSpeed(const GLfloat& speed);
	void setTextues(Texture_Names texB, Texture_Names texT);
	void setTarget(glm::vec3 targ);
	void setInvincable(GLboolean inv);

	void updatePosition(GLfloat deltaTime, std::vector<ChunkColumn*>& adjacesnt);
	void updateCamera(GLfloat xOff, GLfloat yOff);

	void move(Move_Dir dir);
	void moveToTarget(glm::vec3 playerPos = glm::vec3(10000000));
	void create();

	void render(glm::mat4 projection, Camera& cam);

	void attack(Entity& e);
	void takeDamage(GLuint dmgTaken);
	void update(glm::mat4 projection, Camera& cam, std::vector<ChunkColumn*>& adjacent, ChunkColumn* occupied, GLfloat deltaTime, glm::vec3 playerPos = glm::vec3(10000000));

	std::string getTag();
	GLboolean isDead;
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
	GLboolean hasCamera, isClipping, canFly, loaded;
	glm::vec3 cameraOffset;
	PlayerInv inventory;


	glm::vec3 pos, vel, acc;
	glm::vec3 forward, right;
	GLfloat movementSpeed;
	GLfloat jumpForce;
	std::vector<Face> body;
	Render::ChunkMeshRender renderer;
	GLboolean grounded;
	GLboolean hasBody;
	BoxCollider collider;
	Texture_Names textures[2];

	glm::vec3 targetPosition, prevBlock, nxtBlock;
	GLboolean hasTarget, wandering;
	GLuint stage;
	std::vector<Move_Dir> movementPath;

	GLboolean showDmg;
	GLboolean hasAttacked;
	GLubyte attackCooldown; // in frames
	GLubyte attackTimer;
	GLboolean invunrable;
	GLubyte invunrableTimer;
	GLubyte invunrableCooldown; // in frames

	std::string tag;
	GLint health;
	GLuint attackDmg;
	GLboolean invincable;
};

