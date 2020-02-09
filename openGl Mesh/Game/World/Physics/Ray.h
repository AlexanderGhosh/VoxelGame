#pragma once
#include "../constants.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
namespace Physics {
	class Ray
	{
	public:
		Ray();
		Ray(glm::vec3 dir, GLfloat max);
		Ray(glm::vec3 ray);
		Ray(glm::vec3 origin, glm::vec3 dest, GLfloat divisions = -1);
		glm::vec3 getStartOfCollision(glm::vec3 origin, std::vector<Face*> faces);
		GLboolean checkIntersection(glm::vec3 origin, std::vector<Face*> potentialCollision);
		GLboolean checkIntersection(glm::vec3 origin, std::vector<Face>& potentialCollision);
	private:
		glm::vec3 ray;
		glm::vec3 dir;
		GLfloat mag;
		GLfloat max;
		GLfloat jumps;

		void increment();
		void deincrement();
		
	};
};