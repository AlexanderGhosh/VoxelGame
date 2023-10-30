#pragma once
#include <glm.hpp>
#include <string>
#include "../SOIL2/SOIL2.h"

class Texture
{
public:
	Texture();
	Texture(std::string name, std::string overload);
	Texture(bool loadTex);
	Texture(std::string name);

	void bind() const;
	const unsigned int& getTexMap() const;
	const std::string& getName() const;
	const bool get2D() const;
	const glm::ivec2& getDimentions() const;
	void unBind() const;
	bool load3D(const std::string& name);
	bool load2D(const std::string& name);
private:
	bool created;
	glm::ivec2 dimentions;
	unsigned int texMap;
	bool is2D;
	std::string name;
};

