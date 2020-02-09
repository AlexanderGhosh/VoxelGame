#pragma once
#include "constants.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
namespace Physics {
	template <typename T>
	struct Clamp;
	class Material;
	class Collider;
	class Object;
	struct Update;
	class Material {
	public:
		Material();
		Material(GLfloat firction, GLfloat bouncy);
		GLboolean isNull();
		GLfloat& getFrictionConstant();
		GLfloat& getBouncyness();
	private:
		GLboolean null;
		GLfloat frictionConstant;
		GLfloat bouncines;
		// glm::vec3 position;
	};

	enum COLLIDERS {
		BOX,
		SPHERE,
		MESH
	};
	class Collider {
	public:
		Collider();
		Collider(glm::vec3& pos, GLfloat size);
		GLboolean checkCollision(Object* object);
		GLboolean checkCollision(Collider* collider);
		GLboolean isNull();
	private:
		GLboolean null;
		glm::vec3 position;
		GLfloat size;
		GLfloat tolerance;
		COLLIDERS type;
		std::vector<Buffer*> mesh;
	};

	enum TAG {
		Null,
		COLLISION,
		GRAVITY_T,
		ACCELERATION
	};

	struct Update {
		Object* Sender;
		TAG Tag;
		glm::vec3 PrevPosition;
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec3 PrevVelocity;
		std::vector<Face> Vertices;
		Update();
	};
	template <typename T>
	struct Clamp {
		GLboolean null;
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
		Object(GLfloat mass, Collider collider);
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
		void addUpdate(Update update);
		void doUpdate(Update& update);

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
		Collider& getCollider();
		std::vector<Update> getUpdates();
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

		std::vector<Face> body;
		glm::bvec3 colliding;
		glm::vec3 displacementFromCollison;
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
		Collider collider;
		std::vector<Update> updates;

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


