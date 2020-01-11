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
	Texture() {
		created = GL_FALSE;
	}
	Texture(std::string name, GLboolean is2D = GL_FALSE);
	void bind();
	GLboolean load3D(std::string& name);
	GLboolean load2D(std::string& name);
	GLuint& getTexMap();
	std::string& getName();
	GLboolean& get2D();
	glm::ivec2& getDimentions();
	void unBind();
private:
	GLboolean created;
	glm::ivec2 dimentions;
	GLuint texMap;
	GLboolean is2D;
	std::string name;
	//static std::map<std::string, unsigned char> texData;
};

