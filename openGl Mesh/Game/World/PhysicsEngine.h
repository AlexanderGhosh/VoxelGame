#pragma once
#include <vector>
#include "World.h"
#include "Physics.h"
namespace Physics {
	class Engine {
	public:
		static void addObject(const Object& object);
		static void addUpdate(Update& update);
		static void doUpdates(World& world);
	private:
		static std::vector<Update> updateBuffer;
		static std::vector<Object> objects;
		static std::vector<Material> materials;
		static std::vector<BoxCollider> colliders;
		Engine();
	};
};
