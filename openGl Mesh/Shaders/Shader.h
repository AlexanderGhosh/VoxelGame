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

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
};
class Shader {
public:
	Shader() {

	}
	Shader(std::string shaderName);
	void setUp();
	void setName(std::string name);
	void bind();
	void unBind();
	GLboolean setValue(std::string name, glm::mat4& value);
	GLboolean setValue(std::string name, glm::vec4& value);
	GLboolean setValue(std::string name, glm::vec3& value);
	GLboolean setValue(std::string name, glm::vec2& value);
	GLboolean setValue(std::string name, GLfloat& value);
	GLboolean setValue(std::string name, const GLint& value);
	GLboolean setValue(std::string name, Material& value);
private:
	GLuint program;
	std::string name;

	GLint getLocation(std::string name);
	void setLocation(GLint& location, glm::mat4& value);
	void setLocation(GLint& location, glm::vec4& value);
	void setLocation(GLint& location, glm::vec3& value);
	void setLocation(GLint& location, glm::vec2& value);
	void setLocation(GLint& location, GLfloat& value);
	void setLocation(GLint& location, const GLint& value);
};