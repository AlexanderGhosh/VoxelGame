#include "PhysicsEngine.h"
namespace Physics {
	std::vector<Update> Engine::updateBuffer;
	std::vector<Object*> Engine::objects;
	std::vector<Material> Engine::materials;
	std::vector<Collider> Engine::colliders;

	Engine::Engine() {

	}
	Update createUpdate(Object& obj, glm::vec3 newVel, TAG tag = COLLISION) {
		Update res;
		res.Tag = tag;
		res.PrevPosition = obj.getPosition();
		res.PrevVelocity = obj.getVelocity();
		res.Vertices = obj.body;
		res.Sender = &obj;
		res.Position = obj.getPosition() + newVel;
		res.Velocity = newVel;
		if (newVel.y < -53) {
			res.Velocity.y = -53.0f;
		}
		return res;
	}
	glm::vec3 calcDrag(Object& obj) {
		glm::vec3 areas = { 2, 1, 2 }; //m^2
		GLfloat density = obj.getMass() / 2;
		glm::vec3 drag = DRAG_COEFFICIENT * density * (glm::pow(obj.getVelocity(), glm::vec3(2)) * areas) / 2.0f;
		return drag;
	}
	void Engine::applyForces(GLfloat deltaTime) {
		for (Object*& obj : Engine::objects) {
			obj->setAcceleration({ 0, 0, 0 });
			if (!obj->getKinematic()) {
				obj->getAcceleration().y += -GRAVITY;											                        // gravity
				obj->getAcceleration().x += obj->getMaterial().getFrictionConstant() * -GRAVITY * obj->getVelocity().x; // friction x
				obj->getAcceleration().z += obj->getMaterial().getFrictionConstant() * -GRAVITY * obj->getVelocity().x; // friction z
				obj->getAcceleration() += calcDrag(*obj);															    // air resistance

				glm::vec3 newVel = (obj->getVelocity() + obj->getAcceleration()) * deltaTime;
				updateBuffer.insert(updateBuffer.begin(), createUpdate(*obj, newVel, ACCELERATION));
			}
		}
	}
	void Engine::applyPhysics(World& world, GLfloat deltaTime) {
		determinCollision(world);
		Engine::applyForces(deltaTime);
		Engine::doUpdates(world, deltaTime);
		// std::cout << glm::to_string(objects[0]->getPosition()) << std::endl;
	}
	void Engine::doUpdates(World& world, GLfloat deltaTime) {
		for (auto& update : Engine::updateBuffer) {
			if (update.Tag == Null) continue;
			Object& obj = *update.Sender;
			glm::vec3 t;
			switch (update.Tag)
			{
			case ACCELERATION:
				glm::vec3& newPos = update.Position;
				Ray ray(update.PrevPosition, newPos, 10);
				update.PrevPosition = obj.getPosition();

				Chunk& chunk = world.getOccupiedChunk(glm::round(newPos)); // get occupied needs to return a collection of chunks bassed on a ray
				if (chunk.isNull()) continue;
				t = ray.getStartOfCollision(update.PrevPosition, chunk.getMeshes());

				/*if (glm::any(obj.colliding)) {								// the bouncy  ness of th eblock collided with
					glm::vec3 momentum = obj.getVelocity() * obj.getMass() + 0.0f; // momentum
					momentum *= -obj.colliding;
					momentum /= obj.getMass();
					obj.getAcceleration() += momentum; // momentum

					if (obj.colliding.y) {
						obj.getAcceleration().y += GRAVITY; // reaction force
					}
				}*/
				if (obj.colliding.x) {
					t.x = glm::round(t.x);
				}
				if (obj.colliding.y) {
					t.y = glm::round(t.y);
				}
				if (obj.colliding.z) {
					t.z = glm::round(t.z);
				}
				update.Position = t + update.PrevPosition;
				break;
			}
			obj.doUpdate(update);
		}
		updateBuffer.clear();
	}
	void Engine::determinCollision(World& world) {
		for (auto& obj : objects) {
			obj->colliding = glm::vec3(0);
			for (auto& face : obj->body) {

				Chunk& chunk = world.getOccupiedChunk(glm::round(obj->getPosition())); // get occupied needs to return a collection of chunks bassed on a ray
				if (chunk.isNull()) continue;

				// if (obj->getVelocity() == glm::vec3(1)) return;
				glm::vec3 dir(0);
				switch (std::get<0>(face)->type)
				{
				case TOP:
					dir.y += 1;
					break;
				case BOTTOM:
					dir.y -= 1;
					break;
				case FRONT:
					dir.z += 1;
					break;
				case BACK:
					dir.z -= 1;
					break;
				case LEFT:
					dir.x += 1;
					break;
				case RIGHT:
					dir.y -= 1;
					break;
				}
				Ray ray(dir, 0.9f);
				if (ray.checkIntersection(obj->getPosition(), chunk.getMeshes())) {
					obj->colliding = dir;
					//updateBuffer.clear();
					// std::cout << "collided\n";
				}
			}
		}
	}
	void Engine::addUpdates(std::vector<Update>& updates) {
		for (auto& update : updates) {
			if (update.Tag == Physics::Null) continue;
			Engine::updateBuffer.push_back(update);
		}
	}
	void Engine::addUpdate(Update& update) {
		if (update.Tag == Physics::Null) return;
		Engine::updateBuffer.push_back(update);
	}
	void Engine::addObject(Object* object) {
		Engine::objects.push_back(object);
	}
}