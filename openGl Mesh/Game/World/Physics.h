#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
#include <array>

#include "World.h"
namespace Physics {
	class Material; 
	template<typename T>
	class Engine;
	class Colider;
	enum TAG {
		COLLISION
	};
	template<typename T>
	struct Update {
		TAG Tag;
		T Data;
		glm::vec3 Positon;
	};
	template<typename T>
	struct Clamp {
		T Min;
		T Max;
		Clamp(T min, T max) {
			Min = min;
			Max = max;
		}
	};
	class Object
	{
	public:
		Object();
		Object(GLfloat mass, Material& material);
		void addForce(GLfloat force, glm::vec3 direction);
		void addForce(glm::vec3 force);
		void addForce(GLfloat force);
		void addAngularForce(GLfloat force, glm::vec3 direction);
		void addAngularForce(glm::vec3 force);
		void addAngularForce(GLfloat force);
		void clampVelocity(const glm::vec3& max, const glm::vec3& min);
		void clampSpeed(const GLfloat& max, const GLfloat& min);
		void clampAngularVelocity(const glm::vec3& max, const glm::vec3& min);
		void clampAngularSpeed(const GLfloat& max, const GLfloat& min);

		void update();

		// getters
		GLboolean& getKinematic();
		GLboolean& getPhysical();
		glm::vec3& getPosition();
		glm::vec3& getRotation();
		glm::vec3& getVelocity();
		glm::vec3& getAcceleration();
		GLfloat getSpeed();
		GLfloat& getMass();
		glm::vec3& getAngluarVelocity();
		glm::vec3& getAngularAcceleration();
		GLfloat getAngularSpeed();
		Material& getMaterial();
		BoxCollider& getCollider();
		// setter
		void setKinematic(const GLboolean& value);
		void setPhysical(const GLboolean& value);
		void setPosition(const glm::vec3& value);
		void setRotation(const glm::vec3& value);
		void setVelocity(const glm::vec3& value);
		void setAcceleration(const glm::vec3& value);
		void setMass(const GLfloat& value);
		void setAngularVelocity(const glm::vec3& value);
		void setAngularAcceleration(const glm::vec3& value);
		void setMaterial(const Material& value);
		void setCollider(const BoxCollider& value);
	private:
		GLfloat mass;
		GLboolean isKinematic;
		GLboolean isPhysical;
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		glm::vec3 rotation;
		glm::vec3 angularVelocity;
		glm::vec3 angularAcceleration;
		Material material;
		BoxCollider collider;

		GLboolean doVelocityClamp;
		GLboolean doSpeedClamp;
		GLboolean doAngularVelocityClamp;
		GLboolean doAngularSpeedClamp;
		Clamp<glm::vec3> velocityClamps;
		Clamp<GLfloat> speedClamps;
		Clamp<glm::vec3> angularVelocityClamps;
		Clamp<GLfloat> angularSpeedClamps;

		void clamp();
	};
	class Material {
	public:
		Material();
		GLboolean isNull();
	private:
		GLboolean null;
		GLfloat frictionConstant;
		GLfloat bouncines;
		glm::vec3 position;
	};
	class BoxCollider {
	public:
		BoxCollider();
		
	private:
		glm::vec3& posiotion;
		GLfloat size;
		GLboolean checkCollision(Object& object);
	};
	template<typename T>
	class Engine {
	public:
		static World world;
		static void addObject(const Object& object);
		static void doUpdates();
	private:
		static std::vector<Update<T>> updateBuffer; // needs a type
		static std::vector<Object> objects;
		static std::vector<Material> materials;
		static std::vector<BoxCollider> colliders;
		static void doCollision();
	};
};

