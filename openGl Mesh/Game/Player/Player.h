#pragma once
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/string_cast.hpp>
#include <tuple>
#include "Camera.h"
#include "../../Renders/chunkRender.h"
#include "../../Drawable.h"
#include "Entity.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3 >;
class Player : public Entity
{
public:
	Player();
	Player(glm::vec3 position, glm::vec3 camOff, GLboolean attachCam = 1, GLboolean render = 0, GLboolean clipping = 1, GLboolean flying = 0);
	glm::vec3 getPosition();
	void create();
	void render(glm::mat4 projection, Camera* cam = nullptr);
	Camera& getCamera(); 
	void updatePosition(GLfloat deltaTime, std::vector<ChunkColumn*>& adjacentChunks);
	void updateCamera(GLfloat xOff, GLfloat yOff);
	GLubyte getSlot();
	std::array<Blocks, 9> getInvBar();
	void setInvSlot(GLubyte slot);

	void attack(Entity& e);
	// void move(Move_Dir dir);

	GLboolean isClipping, canFly;
private:
	Camera cam;
	glm::vec3 camera_offset;
	GLboolean attachCam, canRender;
	std::array<Blocks, 9> invBar;
	GLubyte invSlot;

};

