#pragma once
#include <vector>
#include "World.h"
#include "Physics.h"
namespace Physics {
	class Engine {
	public:
		Engine();
		static void addObject(const Object& object);
		static void addUpdate(Update& update);
		static void addUpdates(std::vector<Update>& updates);
		static void doUpdates(World& world);
	private:
		static std::vector<Update> updateBuffer;
		static std::vector<Object> objects;
		static std::vector<Material> materials;
		static std::vector<Collider> colliders;
	};
};
