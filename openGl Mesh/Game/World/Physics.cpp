#include "Physics.h"
namespace Physics {
	Object::Object() {
		setMass(100);
		setAcceleration({ 0, 0, 0 });
		//Physics::addObject(*this);
	}
	Object::Object(GLfloat mass, Material& material) {
		setMass(mass);
		setMaterial(material);
		setAcceleration({ 0, 0, 0 });
	}
	Object::Object(GLfloat mass, Collider collider) {
		setMass(mass);
		this->collider = collider;
		setAcceleration({ 0, 0, 0 });
	}
	void Object::addForce(GLfloat force, glm::vec3 direction) {
		if (!isKinematic) return;
		glm::vec3 f = direction * force;
		acceleration += force / mass;
	}
	void Object::addForce(glm::vec3 force) {
		if (!isKinematic) return;
		acceleration += force / mass;
	}
	void Object::addForce(GLfloat force) {
		if (!isKinematic) return;
		acceleration += force / mass;
	}
	void Object::addAngularForce(GLfloat force, glm::vec3 rotaion) {
		if (!isKinematic) return;
		glm::vec3 f = rotaion * force;
		angularAcceleration += force / mass;
	}
	void Object::addAngularForce(glm::vec3 force) {
		if (!isKinematic) return;
		angularAcceleration += force / mass;
	}
	void Object::addAngularForce(GLfloat force) {
		if (!isKinematic) return;
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
	Collider& Object::getCollider() {
		return collider;
	}
	std::vector<Update> Object::getUpdates() {
		return updates;
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
	void Object::setMaterial(Material& value) {
		material = value;
	}
	void Object::addUpdate(Update update) {
		updates.push_back(update);
	}

	void Object::clamp() {
		if (doVelocityClamp) {
			glm::vec3 vel = velocity;
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
				glm::vec3 unit = glm::normalize(velocity);
				unit *= speedClamps.Min;
				setPosition(unit);
			}
			if (speed > speedClamps.Max) {
				glm::vec3 unit = glm::normalize(velocity);
				unit *= speedClamps.Max;
				setPosition(unit);
			}
		}

		if (doAngularVelocityClamp) {
			glm::vec3 vel = angularVelocity;
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
				glm::vec3 unit = glm::normalize(angularVelocity);
				unit *= angularSpeedClamps.Min;
				setPosition(unit);
			}
			if (speed > angularSpeedClamps.Max) {
				glm::vec3 unit = glm::normalize(angularVelocity);
				unit *= angularSpeedClamps.Max;
				setPosition(unit);
			}
		}
	}
	Update Object::getUpdate() {
		Update update;
		if (isKinematic) {
			update.Tag = Null;
			return update;
		}
		update.Sender = this;
		update.PrevPosition = position;
		update.PrevVelocity = velocity;
		update.Vertices = body;

		update.Position = position + velocity;
		update.Velocity = velocity + acceleration + glm::vec3(0, GRAVITY, 0);
		update.Tag = COLLISION;

		return update;
	}
	void Object::doUpdate(Update& update) {
		position += update.Position - update.PrevPosition;
		// setPosition(update.Position); 
		setVelocity(update.Velocity);
	}

	Collider::Collider() {
		position = { 0, 0, 0 };
		size = 0;
		tolerance = 0.1;
		type = SPHERE;
	}
	Collider::Collider(glm::vec3& pos, GLfloat size) {
		position = pos;
		this->size = size;
		tolerance = 0.1;
		type = SPHERE;
	}
	GLboolean Collider::checkCollision(Object* obj) {
		if (!obj->getCollider().isNull()) return false;
		if (type == SPHERE) {
			GLfloat distance = glm::distance(position, obj->getPosition());
			distance = std::abs(distance);
			if (distance > obj->getCollider().size + obj->getCollider().tolerance + tolerance + size) {
				return GL_FALSE;
			}
			return GL_TRUE;
		}
		else {
			return GL_FALSE;
		}
	}
	GLboolean Collider::checkCollision(Collider* col) {
		if (!col->isNull()) return false;
		if (type == SPHERE) {
			GLfloat distance = glm::distance(position, col->position);
			distance = std::abs(distance);
			if (distance > col->size + col->tolerance + tolerance + size) {
				return GL_FALSE;
			}
			return GL_TRUE;
		}
		else {
			return GL_FALSE;
		}
	}
	GLboolean Collider::isNull() {
			return null;
		}

	Material::Material() {
		null = GL_TRUE;
		bouncines = 0;
		frictionConstant = 1;
		// position = { 0, 0, 0 };
	}
	Material::Material(GLfloat firction, GLfloat bouncy) {
		null = GL_FALSE;
		bouncines = bouncy;
		frictionConstant = firction;
		// position = { 0, 0, 0 };
	}
	GLboolean Material::isNull() {
		return null;
	}
	GLfloat& Material::getFrictionConstant() {
		return frictionConstant;
	}
	GLfloat& Material::getBouncyness() {
		return bouncines;
	}

	template <typename T>
	Clamp<T>::Clamp(T min, T max) {
		Min = min;
		Max = max;
		null = GL_FALSE;
	}
	template <typename T>
	Clamp<T>::Clamp() {
		null = GL_TRUE;
	}
	template <typename T>
	void Clamp<T>::clapValue(T& value) {
		if (value < Min) value = Min;
		else if (value > Max) value = Max;
	}
	Update::Update() : Tag(Null) {
		Sender = nullptr;
		PrevPosition = { 0, 0, 0 };
		Position = { 0, 0, 0 };
	}
};