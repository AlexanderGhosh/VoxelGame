#pragma once
#include "SOIL2/SOIL2.h"
#include "Mesh.h"
#include "Shader.h"
#include "Entity.h"
#include "Camera.h"
#include "Buffer.h"
#include <gtc/matrix_transform.hpp>
class MeshRender {
public:
	glm::vec3 position;
	BlockMesh mesh;
	GLuint VBO, VAO;
	MeshRender();
	MeshRender(glm::vec3 pos);
	void loadMesh(BlockMesh& m);
	void create();
	void render(Camera p1, glm::mat4 projection);
	void destroy();
private:
	std::array<GLuint, 2> texMaps;
	Shader shader;
	Buffer buffers;
	void loadTexture(std::string name);
};