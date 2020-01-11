#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
namespace Physics {
	class Object
	{
	public:
		Object();
		void addForce(glm::vec3 direction);

	private:
		GLboolean isKinematic;
		GLboolean isPhysical;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 velocity;
		glm::vec3 acceleration;
	};
	class Material {
	public:
		Material();

	private:
		GLfloat frictionConstant;
		GLfloat bouncines;
		glm::vec3 position;
	};
	class Engine {
	public:

	private:
		std::vector<Object> objects;
		std::vector<Material> materials;
	};
};

