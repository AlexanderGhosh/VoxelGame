#pragma once
#include <chrono>
#include "Game/World/constants.h"
#include "Game/Player/Camera.h"
// using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Drawable
{
public:
	Drawable();
	Drawable(Faces* mesh);
	void render(Camera& cam, glm::mat4 projection);
	void setUp(std::unordered_map<GLuint, FaceB_p>& meshes);
private:
	std::unordered_map<GLuint, FaceB> buffers;
};
