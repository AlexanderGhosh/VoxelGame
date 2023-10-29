#pragma once
#include <array>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw3.h>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};
class Shader {
public:
	Shader() : program(0), name(), hasGeom(false) {

	}
	Shader(std::string shaderName, bool hasGeom = false);
	void setUp();
	void setName(std::string name);
	void bind();
	void unBind();
	bool setValue(std::string name, glm::mat4& value);
	bool setValue(std::string name, glm::vec4& value);
	bool setValue(std::string name, glm::vec3& value);
	bool setValue(std::string name, glm::vec2& value);
	bool setValue(std::string name, float& value);
	bool setValue(std::string name, const int& value);
	bool setValue(std::string name, Material& value);
private:
	bool hasGeom;
	unsigned int program;
	std::string name;

	int getLocation(std::string name);
	void setLocation(int& location, glm::mat4& value);
	void setLocation(int& location, glm::vec4& value);
	void setLocation(int& location, glm::vec3& value);
	void setLocation(int& location, glm::vec2& value);
	void setLocation(int& location, float& value);
	void setLocation(int& location, const int& value);
};