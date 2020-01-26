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
	Texture(GLboolean loadTex);
	Texture(std::string name, GLboolean is2D = GL_FALSE);
	void bind();
	GLuint& getTexMap();
	std::string& getName();
	GLboolean& get2D();
	glm::ivec2& getDimentions();
	void unBind();
	GLboolean load3D(const std::string& name);
	GLboolean load2D(std::string& name);
private:
	GLboolean created;
	glm::ivec2 dimentions;
	GLuint texMap;
	GLboolean is2D;
	std::string name;
};

