#pragma once
#include "Manager.h"
#include <glm.hpp>
#include <reactphysics3d/reactphysics3d.h>
#include "../Helpers/Constants.h"

namespace Components {
    class Transform;
}


class ChunkColumn;

class PhysicsManager : public Manager <PhysicsManager>
{
    friend class Manager;
public:
    // sets the physics comm ptr
    // initalises PhysicsManager
    void setPhysCommon(reactphysics3d::PhysicsCommon* common);

    // returns a pointer to a newly created rb
    reactphysics3d::RigidBody* createRigidBody(Components::Transform* transform) const;
    void removeRigidBody(reactphysics3d::RigidBody* rb) const;

    reactphysics3d::BoxShape* createBoxShape(const glm::vec3& extent) const;

#ifdef PHYSICS_DEBUG_RENDERER
    const reactphysics3d::DebugRenderer* getDebugRenderer() const;
#endif // PHYSICS_DEBUG_RENDERER


    void setTerrain(const ChunkColumn& chunk) const;

    void step();
    void destroy() override;
private:
    PhysicsManager();
    reactphysics3d::PhysicsCommon* _physCommon;
    reactphysics3d::PhysicsWorld* _physWorld;
    // this is used to represent the entire terrain collider
    reactphysics3d::RigidBody* _terrainBody;
    // added for earch column in the mesh and just places at different heights (assumes no holes in the mesh)
    reactphysics3d::BoxShape* _columnCollider;
#ifdef PHYSICS_DEBUG_RENDERER
    reactphysics3d::DebugRenderer* _debugRenderer;
#endif // PHYSICS_DEBUG_RENDERER

};