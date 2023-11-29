#pragma once
#include "../Component.h"
#include <glm.hpp>

namespace Components {
    class Transform;
    class RigidBody : public Component
    {
    private:
        Transform* _transform;
        glm::vec3 _velocity;

    public:
        RigidBody();
        void setTransform(Transform* transform);
        void intergrateVelocity(const float deltaTime) const;
        void applyGravity(const float deltaTime);

        void update(const float deltaTime) override;
    };
}

