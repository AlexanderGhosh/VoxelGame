#pragma once
#include <iostream>
#include <gl/glew.h>
#include <glm.hpp>
#include <string>
#include <vector>
#include "../SOIL2/SOIL2.h"
class Texture
{
public:
	Texture() {
		created = GL_FALSE;
	}
	Texture(std::string name, GLboolean is2D = GL_FALSE);
	GLboolean created;
	void bind();
	GLboolean load3D(std::string& name);
	GLboolean load2D(std::string& name);
	GLuint getTexMap() {
		return texMap;
	}
private:
	glm::ivec2 dimentions;
	GLuint texMap;
	GLboolean is2D;
};

