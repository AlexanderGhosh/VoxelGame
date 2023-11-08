#include "Transform.h"

Transform::Transform() : Component(), position(), scale()
{
}

Transform::Transform(unsigned int id) : Transform()
{
    _id = id;
}