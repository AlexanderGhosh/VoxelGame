#include "Material.h"

Material::Material() : albedo(0)
{
}

Material::Material(const float a_x, const float a_y, const float a_z, const float a_a) : Material()
{
	albedo = glm::vec4(a_x, a_y, a_z, a_a);
}

Material::Material(const glm::vec4& albedo) : Material()
{
	this->albedo = albedo;
}

const glm::vec4& Material::getAlbedo() const
{
	return albedo;
}

void Material::setAlbedo(const glm::vec4& albedo)
{
	this->albedo = albedo;
}
