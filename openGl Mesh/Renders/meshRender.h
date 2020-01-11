#pragma once
#include "../SOIL2/SOIL2.h"
#include "../Mesh.h"
#include "../Shaders/Shader.h"
#include "../Game/Player/Entity.h"
#include "../Game/Player/Camera.h"
#include "../Buffer.h"
#include <gtc/matrix_transform.hpp>
namespace Render {
	class FaceMeshRender {
	public:
		FaceMeshRender() : shader("block2") {
			canRender = GL_FALSE;
		}
		FaceMeshRender(std::string shaderName);
		void loadMesh(Mesh::FaceMesh& m);
		void render(Camera p1, glm::mat4 projection);
		void setTexture(std::string name);
		void setPosition(glm::vec3 position);
		void destroy();
	private:
		Shader shader;
		Mesh::FaceMesh mesh;
		Buffer buffer;
		GLboolean canRender;
	};
};