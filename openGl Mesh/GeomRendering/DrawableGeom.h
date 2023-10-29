#pragma once
#include <glm.hpp>
#include <list>
#include <unordered_map>
#include "../Game/World/constants.h"
#include "DrawData.h"

class Camera;
class Shader;
class DrawableGeom
{
public:
	DrawableGeom();
	~DrawableGeom();
	void render(Camera& cam, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap);
	void setUp(const std::unordered_map<GLuint, FaceB_p>& mesh);
	void setUp(const Chunks& chunks);

private:
	std::list<DrawData> data;
	void draw(unsigned int depthMap, Shader* shader);
};

