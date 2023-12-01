#pragma once
#include "../Component.h"
#include <glm.hpp>

namespace reactphysics3d {
    class RigidBody;
    class BoxShape;
    class Material;
}
class PhysicsManager;

namespace Components {
    struct RigidBody_Details {
        bool _hasGravity;
        // directly convertable to reacts equivilant
        enum BodyType {
            STATIC,
            KINEMATIC,
            DYNAMIC
        } _bodyType;
        reactphysics3d::BoxShape* _collider;
        RigidBody_Details() : _hasGravity(), _bodyType(), _collider(){ }
    };

    class Transform;
    // only supports one collider
    class RigidBody : public Component
    {
    private:
        PhysicsManager* _manager;
        Transform* _transform;
        reactphysics3d::RigidBody* _reactRigidBody;
        RigidBody_Details _details;
    public:
        RigidBody();
        void setTransform(Transform* transform);

        // used in the start method not anywhere else
        // only supports one collider
        void setDetails(const RigidBody_Details& detials);
        
        // creates the react RB
        void start() override;

        // updates reacts transform with the render transform
        void preFixedUpdate() override;
        // updates render trhansform with reacts transform
        void postFixedUpdate() override;

        void addVelocity(const glm::vec3& delta);

        // asumes only one collider
        reactphysics3d::Material& getMaterial();

        void destroy() override;
    };
}

