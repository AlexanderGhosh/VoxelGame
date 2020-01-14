#pragma once
#include "constants.h"

namespace Physics {
	template <typename T>
	struct Clamp;
	class Material;
	class BoxCollider;
	class Object;
	struct Update;
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
		BoxCollider(glm::vec3& pos, GLfloat size);
		GLboolean checkCollision(Object* object);
	private:
		glm::vec3 position;
		GLfloat size;
	};


	enum TAG {
		Null,
		COLLISION
	};

	struct Update {
		Object* Sender;
		TAG Tag;
		glm::vec3 Data;
		glm::vec3 Positon;
		GLboolean null;
		Update();
	};
	template <typename T>
	struct Clamp {
		T Min;
		T Max;
		Clamp();
		Clamp(T min, T max);
		void clapValue(T& value);
	};


	class Object
	{
	public:
		Object();
		Object(GLfloat mass, Material& material);
		Object(GLfloat mass, BoxCollider collider);
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

		Update getUpdate();
		void doUpdate(Update update);

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
		void setMaterial(Material& value);
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
};


