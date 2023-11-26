#include "Material.h"

Material::Material() : albedo1(0), albedo2(0)
{
}
Material::Material(const glm::vec4& albedo1, const glm::vec4& albedo2) : Material()
{
	this->albedo1 = albedo1;
	this->albedo2 = albedo2;
}

const glm::vec4& Material::getAlbedo1() const
{
	return albedo1;
}

const glm::vec4& Material::getAlbedo2() const
{
	return albedo2;
}

void Material::setAlbedo1(const glm::vec4& albedo)
{
	this->albedo1 = albedo;
}
void Material::setAlbedo2(const glm::vec4& albedo)
{
	this->albedo2 = albedo;
}
