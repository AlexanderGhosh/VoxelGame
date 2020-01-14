#pragma once
#pragma once
#include "../SOIL2/SOIL2.h"
#include "../Mesh.h"
#include "../Shaders/Shader.h"
#include "../Game/Player/Camera.h"
#include <gtc/matrix_transform.hpp>
namespace Render {
	class ChunkMeshRender {
	public:
		ChunkMeshRender() : shader("block2") {
			canRender = GL_FALSE;
		}
		ChunkMeshRender(GLboolean init);
		ChunkMeshRender(std::string shaderName);
		void loadMeshes(std::vector<Mesh::FaceMesh>& m);
		void render(Camera p1, glm::mat4 projection);
		void destroy();
		void setPosition(glm::vec3 position);
		void setShader(std::string name);
		void addPosition(glm::vec3 positon);
		void setRotaion(const glm::vec3& rotation);
	private:
		Shader shader;
		std::vector<Mesh::FaceMesh> meshes;
		GLboolean canRender;
	};
};
