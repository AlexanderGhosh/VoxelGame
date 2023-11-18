#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

Shader::Shader() : name(), hasGeom(), program() { }

Shader::Shader(std::string shaderName, bool hasGeom) : Shader() {
	name = shaderName;
	this->hasGeom = hasGeom;
}
unsigned int Shader::getId() const
{
	return program;
}
void Shader::bind() const {
	glUseProgram(program);
}
void Shader::unBind() const {
	glUseProgram(0);
}
const int Shader::getLocation(const std::string& name) const {
	return glGetUniformLocation(program, name.c_str());
}

void Shader::setLocation(const int& location, const glm::mat4& value) const {
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
}
void Shader::setLocation(const int& location, const glm::vec4& value) const {
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Shader::setLocation(const int& location, const glm::vec3& value) const {
	glUniform3f(location, value.x, value.y, value.z);
}
void Shader::setLocation(const int& location, const glm::vec2& value) const
{
	glUniform2f(location, value.x, value.y);
}
void Shader::setLocation(const int& location, const float& value) const {
	glUniform1f(location, value);
}
void Shader::setLocation(const int& location, const int& value) const {
	glUniform1i(location, value);
}
bool Shader::setValue(const std::string& name, const int& value) const {
	int loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
bool Shader::setValue(const std::string& name, const glm::vec4& value) const {
	int loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
bool Shader::setValue(const std::string& name, const glm::vec3& value) const {
	int loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
bool Shader::setValue(const std::string& name, const glm::vec2& value) const
{
	int loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
bool Shader::setValue(const std::string& name, const float& value) const {
	int loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
bool Shader::setValue(const std::string& name, const glm::mat4& value) const {
	int loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
void Shader::setUp() {
	std::string vertexCode, geometryCode, fragmentCode;
	std::ifstream vShaderFile, gShaderFile, fShaderFile;
	vShaderFile.exceptions(std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		vShaderFile.open(("Shaders/" + name + ".vert").c_str());
		fShaderFile.open(("Shaders/" + name + ".frag").c_str());
		std::stringstream vShaderStream, gShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();
		
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (hasGeom) {
			gShaderFile.open(("Shaders/" + name + ".geom").c_str());
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader file faild to be read" << name << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, geometry, fragment;
	int success;
	char infoLog[512];

	//vertex
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Vertex compilation failed: " << std::string(infoLog) << name << std::endl;
	}

	//geometry
	if (hasGeom) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "Geometry compilation failed: " << std::string(infoLog) << name << std::endl;
		}
	}

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment compilation failed: " << std::string(infoLog) << name << std::endl;
	}

	// the program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	if(hasGeom) glAttachShader(program, geometry);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Unable to link a shader: " << std::string(infoLog) << name << std::endl;
	}

	glDetachShader(program, vertex);
	if (hasGeom) glDetachShader(program, geometry);
	glDetachShader(program, fragment);

	glDeleteShader(vertex);
	if (hasGeom) glDeleteShader(geometry);
	glDeleteShader(fragment);

	glUseProgram(0);
}
void Shader::setName(const std::string& name) {
	this->name = name;
}