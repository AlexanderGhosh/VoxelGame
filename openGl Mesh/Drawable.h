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
	void render(Camera& cam, glm::mat4 projection, glm::mat4 lightMatrix, GLuint depthMap);
	void render(glm::mat4 lightProjection, glm::mat4 lightView);
	void setUp(std::unordered_map<GLuint, FaceB_p>& meshes);
private:
	std::unordered_map<GLuint, FaceB> opaqueBuffer, transparantBuffer;
	void draw(GLuint depthMap);
	void clear();
};
