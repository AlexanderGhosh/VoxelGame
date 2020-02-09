#pragma once
#include <vector>
#include "World.h"
#include "Physics.h"
#include "Physics/Ray.h"
namespace Physics {
	class Engine {
	public:
		Engine();
		static void addObject(Object* object);
		static void addUpdate(Update& update);
		static void addUpdates(std::vector<Update>& updates);
		static void applyPhysics(World& world, GLfloat deltaTime);
		static void determinCollision(World& world);
	private:
		static std::vector<Update> updateBuffer;
		static std::vector<Object*> objects;
		static std::vector<Material> materials;
		static std::vector<Collider> colliders;
		static void applyForces(GLfloat deltaTime);
		static void doUpdates(World& world, GLfloat deltaTime);
	};
};
