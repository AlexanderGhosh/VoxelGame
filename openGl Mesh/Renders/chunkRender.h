#pragma once
#pragma once
#include "../SOIL2/SOIL2.h"
#include "../Mesh.h"
#include "../Shaders/Shader.h"
#include "../Game/Player/Camera.h"
#include "../Game/World/constants.h"
#include <gtc/matrix_transform.hpp>
using Face = std::tuple <Buffer*, Texture*, glm::vec3>;
namespace Render {
	class ChunkMeshRender {
	public:
		ChunkMeshRender();
		ChunkMeshRender(bool init, std::string t);
		ChunkMeshRender(std::string shaderName);
		ChunkMeshRender(Shader* shader);

		void loadMeshes(std::vector<Face>* m);
		void render(Camera p1, glm::mat4 projection);
		void destroy();
		void setPosition(glm::vec3 position);
		void setShader(std::string name);
		void setShader(Shader* shader);
		void setRelativePositions(std::vector<glm::vec3>& rel);
		void setRotation(glm::vec3 rotAxis, float rotAngle);

		void addPosition(glm::vec3 positon);
		void cleanUp();

		// getters
		Shader& getShader();
	private:
		Shader shader;
		std::vector<Face>* meshes;
		std::vector<glm::vec3> relativePositions;
		bool canRender;
		float angle;
		glm::vec3 axis;
	};
};
