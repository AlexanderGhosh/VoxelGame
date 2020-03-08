#pragma once
#include "Game/World/constants.h"
#include "Game/Player/Camera.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Drawable
{
public:
	Drawable();
	Drawable(std::vector<Face*>& sortedMeshes);
	void render(Camera& cam, glm::mat4 projection);
	void setUp(std::vector<Face*>& sortedMeshes);
	void setUp(std::vector<Face*> sortedMeshes, const char* overload);
	std::vector<Face*> sortMesh(std::vector<Face*>& meshes);
private:
	std::vector<std::tuple<Buffer, Texture*, GLuint>> buffers;
	std::vector<Face*> meshes;
	// std::vector<std::tuple<Buffer*, Texture*, GLuint>> buffers; // these are the buffer that will be drawn
};
