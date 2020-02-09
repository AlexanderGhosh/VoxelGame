#pragma once
#include <vector>
#include "World.h"
#include "Physics.h"
#include "Physics/Ray.h"
namespace Physics {
	class Engine {
	public:
<<<<<<< HEAD
		Engine();
		static void addObject(Object* object);
		static void addUpdate(Update& update);
		static void addUpdates(std::vector<Update>& updates);
		static void applyPhysics(World& world, GLfloat deltaTime);
		static void determinCollision(World& world);
=======
		static void addObject(const Object& object);
		static void addUpdate(Update& update);
		static void doUpdates(World& world);
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
	private:
		// static std::vector<Event> enventBuffer;
		static std::vector<Update> updateBuffer;
		static std::vector<Object*> objects;
		static std::vector<Material> materials;
<<<<<<< HEAD
		static std::vector<Collider> colliders;
		static void applyForces(GLfloat deltaTime);
		static void doUpdates(World& world, GLfloat deltaTime);
=======
		static std::vector<BoxCollider> colliders;
		Engine();
>>>>>>> parent of b4d0d51... physics engine working in super flat not extensivly tested also 3d chunks
	};
};
