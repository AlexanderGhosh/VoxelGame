#pragma once
#include "Game/World/constants.h"
#include "Game/Player/Camera.h"
class Drawable
{
public:
	Drawable();
	Drawable(std::vector<Mesh::FaceMesh>& sortedMeshes);
	void render(Camera& cam, glm::mat4 projection);
	void setUp(std::vector<Mesh::FaceMesh>& sortedMeshes);
private:
	std::vector<std::tuple<Buffer, Texture*, GLuint>> buffers;
	// std::vector<std::tuple<Buffer*, Texture*, GLuint>> buffers; // these are the buffer that will be drawn
};

