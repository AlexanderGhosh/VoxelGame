#pragma once
#include <array>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(std::string shaderName);
	void bind();
	GLuint getLocation(std::string name);
	void setLocation(GLuint& location, glm::mat4& value);
private:
	GLuint program;
};