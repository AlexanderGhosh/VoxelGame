#pragma once
#include "../Component.h"
#include <array>
#include <glm.hpp>

namespace Components{
    class RigidBody;
    // Allows the player to fly like in minecraft creative mode
    class FlightControls : public Component {
    private:
        RigidBody* _rigidbody;
        float _speed;
    public:
        FlightControls();
        FlightControls(const float speed);

        // fwd is not front it is the direction that is considered forward
        void processKeys(const std::array<bool, 1024>& keysPressed, const glm::vec3& fwd, const float deltaTime);

        void setRigidBody(RigidBody* rb);
        void setSpeed(const float speed);
    };
}