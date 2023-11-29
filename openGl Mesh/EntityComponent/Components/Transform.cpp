#include "Transform.h"

using namespace Components;

Transform::Transform() : Component(), position(), scale(1)
{
}

Transform::Transform(unsigned int id) : Transform()
{
    _id = id;
}