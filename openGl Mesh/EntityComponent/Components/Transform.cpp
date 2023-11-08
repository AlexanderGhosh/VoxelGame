#include "Transform.h"

Transform::Transform() : position(), scale()
{
}

Transform::Transform(const glm::vec3& pos, const float scale) : position(pos), scale(scale)
{
}

const glm::vec3& Transform::getPosition() const
{
    return position;
}

const float& Transform::getScale() const
{
    return scale;
}

void Transform::setPosition(const glm::vec3& pos)
{
    position = pos;
}

void Transform::setScale(const float& scale)
{
    this->scale = scale;
}
