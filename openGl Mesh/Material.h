#pragma once
#include <glm.hpp>

class Material
{
public:
	Material();
	Material(const float a_x, const float a_y, const float a_z, const float a_a);
	Material(const glm::vec4& albedo);

	const glm::vec4& getAlbedo() const;
	void setAlbedo(const glm::vec4& albedo);
private:
	glm::vec4 albedo;
};

