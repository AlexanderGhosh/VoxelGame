#include "Physics.h"
namespace Physics {
	Object::Object() {
		mass = 100;
	}
	Object::Object(GLfloat mass, Material& material) {
		setMass(mass);
		setMaterial(material);
	}
	void Object::addForce(GLfloat force, glm::vec3 direction) {
		glm::vec3 force = direction * force;
		acceleration += force / mass;
	}
	void Object::addForce(glm::vec3 force) {
		acceleration += force / mass;
	}
	void Object::addForce(GLfloat force) {
		acceleration += force / mass;
	}
	void Object::addAngularForce(GLfloat force, glm::vec3 rotaion) {
		glm::vec3 force = rotaion * force;
		angularAcceleration += force / mass;
	}
	void Object::addAngularForce(glm::vec3 force) {
		angularAcceleration += force / mass;
	}
	void Object::addAngularForce(GLfloat force) {
		angularAcceleration += force / mass;
	}

	void Object::clampVelocity(const glm::vec3& max, const glm::vec3& min) {
		velocityClamps = { max, min };
		doVelocityClamp = GL_TRUE;
	}
	void Object::clampSpeed(const GLfloat& max, const GLfloat& min) {
		speedClamps = { max, min };
		doSpeedClamp = GL_TRUE;
	}
	void Object::clampAngularVelocity(const glm::vec3& max, const glm::vec3& min) {
		angularVelocityClamps = { max, min };
		doAngularVelocityClamp = GL_TRUE;
	}
	void Object::clampAngularSpeed(const GLfloat& max, const GLfloat& min) {
		angularSpeedClamps = { max, min };
		doAngularSpeedClamp = GL_TRUE;
	}

	// getters
	GLboolean& Object::getKinematic() {
		return isKinematic;
	}
	GLboolean& Object::getPhysical() {
		return isPhysical;
	}
	glm::vec3& Object::getPosition() {
		return position;
	}
	glm::vec3& Object::getRotation() {
		return rotation;
	}
	glm::vec3& Object::getVelocity() {
		return velocity;
	}
	glm::vec3& Object::getAcceleration() {
		return acceleration;
	}
	GLfloat Object::getSpeed() {
		return glm::length(velocity);
	}
	GLfloat& Object::getMass() {
		return mass;
	}
	glm::vec3& Object::getAngluarVelocity() {
		return angularVelocity;
	}
	glm::vec3& Object::getAngularAcceleration() {
		return angularAcceleration;
	}
	GLfloat Object::getAngularSpeed() {
		return glm::length(angularVelocity);
	}
	Material& Object::getMaterial() {
		return material;
	}

	//setters
	void Object::setKinematic(const GLboolean& value) {
		isKinematic = value;
	}
	void Object::setPhysical(const GLboolean& value) {
		isPhysical = value;
	}
	void Object::setPosition(const glm::vec3& value) {
		position = value;
	}
	void Object::setRotation(const glm::vec3& value) {
		rotation = value;
	}
	void Object::setVelocity(const glm::vec3& value) {
		velocity = value;
	}
	void Object::setAcceleration(const glm::vec3& value) {
		acceleration = value;
	}
	void Object::setMass(const GLfloat& value) {
		mass = value;
	}
	void Object::setAngularVelocity(const glm::vec3& value) {
		angularVelocity = value;
	}
	void Object::setAngularAcceleration(const glm::vec3& value) {
		angularAcceleration = value;
	}
	void Object::setMaterial(const Material& value) {
		material = value;
	}

	void Object::clamp() {
		if (doVelocityClamp) {
			glm::vec3 vel = getVelocity();
			glm::vec3 min = velocityClamps.Min;
			if (vel.x < min.x) {
				vel.x = min.x;
			}
			if (vel.y < min.y) {
				vel.y = min.y;
			}
			if (vel.z < min.z) {
				vel.z = min.z;
			}
			glm::vec3 max = velocityClamps.Max;
			if (vel.x < max.x) {
				vel.x = max.x;
			}
			if (vel.y < max.y) {
				vel.y = max.y;
			}
			if (vel.z < max.z) {
				vel.z = max.z;
			}
		}
		if (doSpeedClamp) {
			GLfloat speed = getSpeed();
			if (speed < speedClamps.Min) {
				glm::vec3 unit = glm::normalize(getVelocity());
				unit *= speedClamps.Min;
				setPosition(unit);
			}
			if (speed > speedClamps.Max) {
				glm::vec3 unit = glm::normalize(getVelocity());
				unit *= speedClamps.Max;
				setPosition(unit);
			}
		}

		if (doAngularVelocityClamp) {
			glm::vec3 vel = getAngluarVelocity();
			glm::vec3 min = angularVelocityClamps.Min;
			if (vel.x < min.x) {
				vel.x = min.x;
			}
			if (vel.y < min.y) {
				vel.y = min.y;
			}
			if (vel.z < min.z) {
				vel.z = min.z;
			}
			glm::vec3 max = angularVelocityClamps.Max;
			if (vel.x < max.x) {
				vel.x = max.x;
			}
			if (vel.y < max.y) {
				vel.y = max.y;
			}
			if (vel.z < max.z) {
				vel.z = max.z;
			}
		}
		if (doAngularSpeedClamp) {
			GLfloat speed = getAngularSpeed();
			if (speed < angularSpeedClamps.Min) {
				glm::vec3 unit = glm::normalize(getVelocity());
				unit *= speedClamps.Min;
				setPosition(unit);
			}
			if (speed > angularSpeedClamps.Max) {
				glm::vec3 unit = glm::normalize(getVelocity());
				unit *= speedClamps.Max;
				setPosition(unit);
			}
		}
	}



	GLboolean BoxCollider::checkCollision(Object& object) {
		glm::vec3 pos = object.getPosition();
		glm::vec3 upper = posiotion + size;
		glm::vec3 lower = posiotion - size;
		if (pos.x < upper.x && pos.y < upper.y && pos.z < upper.z) {
			if (pos.x > lower.x&& pos.y > lower.y&& pos.z > lower.z) {
				return GL_FALSE;
			}
		}
		return GL_FALSE;
	}

	template<typename T>
	void Engine<T>::doUpdates() {
		for (auto& update : updateBuffer) {
			if (update.Tag == TAG::COLLISION) {
				// get the closest chunk
				// check its collison mesh
				// if collied dont do update
				// else do update
			}
		}
	}


};