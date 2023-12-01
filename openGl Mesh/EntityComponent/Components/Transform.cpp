#include "Transform.h"
#include <ext/matrix_transform.hpp>

using namespace Components;

Transform::Transform() : Component(), position(), scale(1)
{
}

Transform::Transform(unsigned int id) : Transform()
{
    _id = id;
}

glm::mat4 Components::Transform::getModel() const
{
    glm::mat4 model(1);
    model = glm::scale(model , scale);
    model = glm::translate(model, position);
    return model;
}
