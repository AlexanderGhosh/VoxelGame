#pragma once
#include <iostream>
#include <gl/glew.h>
#include <glm.hpp>
#include <string>
#include <vector>
#include <map>
#include "../SOIL2/SOIL2.h"
class Texture
{
public:
	Texture(std::string name, std::string overload);
	Texture() {
		created = GL_FALSE;
	}
	Texture(bool loadTex);
	// Texture(std::string name, bool is2D);
	Texture(std::string name);
	void bind();
	unsigned int& getTexMap();
	std::string& getName();
	bool& get2D();
	glm::ivec2& getDimentions();
	void unBind();
	bool load3D(const std::string& name);
	bool load2D(std::string& name);
private:
	bool created;
	glm::ivec2 dimentions;
	unsigned int texMap;
	bool is2D;
	std::string name;
};

