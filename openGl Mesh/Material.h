#pragma once
#include <glm.hpp>

class Material
{
public:
	Material();
	Material(const glm::vec4& albedo1, const glm::vec4& albedo2);

	const glm::vec4& getAlbedo1() const;
	void setAlbedo1(const glm::vec4& albedo);

	const glm::vec4& getAlbedo2() const;
	void setAlbedo2(const glm::vec4& albedo);
private:
	glm::vec4 albedo1, albedo2;
};

