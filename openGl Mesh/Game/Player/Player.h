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
	Player(glm::vec3 position, glm::vec3 camOff);
	glm::vec3 getPosition();
	void create();
	void render(glm::mat4 projection, Camera* cam = nullptr);
	Camera& getCamera(); 
	std::string updatePosition(GLfloat deltaTime, World& world);
	void updateCamera(GLfloat xOff, GLfloat yOff);
private:
	Camera cam;
	glm::vec3 camera_offset;
};

