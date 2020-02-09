#include "Ray.h"
namespace Physics {
	Ray::Ray() {
		dir = { 0, 0, 0 };
		max = 0;
		jumps = 0.1f;
		mag = 0.0f;
		ray = dir;
	}
	Ray::Ray(glm::vec3 dir, GLfloat max) {
		this->dir = dir;
		this->max = max;
		jumps = 0.1f;
		mag = 0.0f;
		ray = dir;
	}
	Ray::Ray(glm::vec3 ray) {
		max = glm::length(ray);
		dir = glm::normalize(ray);
		jumps = 0.1f;
		mag = 0.0f;
		this->ray = dir;
	}
	Ray::Ray(glm::vec3 origin, glm::vec3 dest, GLfloat divisons) {
		max = glm::distance(origin, dest);
		dir = glm::normalize(dest - origin);
		jumps = divisons < 0 ? 0.9f : max / divisons;
		mag = 0.0f;
		ray = dir;
	}
	void Ray::increment() {
		if (mag <= max) {
			mag += jumps;
			ray = dir * mag;
		}
	}
	void Ray::deincrement() {
		if (mag > max) {
			mag -= jumps;
			ray = dir * mag;
		}
	}
	GLboolean Ray::checkIntersection(glm::vec3 origin, std::vector<Face*> potentialCollision) {
		increment();
		std::vector<glm::vec3> corners = { {1, 1, 0}, {0, 0, -1} };
		// origin.x += 0.5;
		// origin.z -= 0.5;
		glm::vec3 r = ray + origin;
		for (auto& face : potentialCollision) {
			std::vector<glm::vec3> pos_ = { corners[0] + std::get<2>(*face), corners[1] + std::get<2>(*face) };
			if (r.x <= pos_[0].x && r.x >= pos_[1].x) {
				if (r.y <= pos_[0].y && r.y >= pos_[1].y) {
					if (r.z <= pos_[0].z && r.z >= pos_[1].z) {
						return true;
					}
				}
			}
		}
		return false;
	}
	GLboolean Ray::checkIntersection(glm::vec3 origin, std::vector<Face>& potentialCollision) {
		std::vector<Face*> res;
		for (auto& face : potentialCollision) {
			res.push_back(&face);
		}
		return checkIntersection(origin, potentialCollision);
	}
	glm::vec3 Ray::getStartOfCollision(glm::vec3 origin, std::vector<Face*> faces) {
		if (jumps > max) {
			return dir * max;
		}
		int c = 0;
		while (mag < max) {
			c++;
			if (checkIntersection(origin, faces)) {
				return ray;
			}
		}
		return ray;
	}
}