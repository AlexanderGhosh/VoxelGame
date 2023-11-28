#pragma once
#include "../Component.h"
#include <array>
#include <glm.hpp>

namespace Components{
    class Transform;
    // Allows the player to fly like in minecraft creative mode
    class FlightControls : public Component {
    private:
        Transform* _transform;
        float _speed;
    public:
        FlightControls();
        FlightControls(const float speed);

        // fwd is not front it is the direction that is considered forward
        void processKeys(const std::array<bool, 1024>& keysPressed, const glm::vec3& fwd, const float deltaTime);

        void setTransform(Transform* transform);
        void setSpeed(const float speed);
    };
}