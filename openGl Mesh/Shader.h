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
	GLint getLocation(std::string name);
	void setLocation(GLint& location, glm::mat4& value);
	void setLocation(GLint& location, glm::vec3& value);
	GLboolean setValue(std::string name, glm::vec3& value);
private:
	GLuint program;
};