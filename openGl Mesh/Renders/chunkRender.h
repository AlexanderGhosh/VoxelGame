#pragma once
#pragma once
#include "../SOIL2/SOIL2.h"
#include "../Mesh.h"
#include "../Shaders/Shader.h"
#include "../Entity.h"
#include "../Camera.h"
#include "../Buffer.h"
#include <gtc/matrix_transform.hpp>
namespace Render {
	class ChunkMeshRender {
	public:
		ChunkMeshRender() : shader("block2") {
			canRender = GL_FALSE;
		}
		ChunkMeshRender(std::string shaderName);
		void loadMeshes(std::vector<Mesh::FaceMesh>& m);
		void render(Camera p1, glm::mat4 projection);
		void destroy();
	private:
		Shader shader;
		std::vector<Mesh::FaceMesh> meshes;
		Buffer buffer;
		GLboolean canRender;
	};
};
