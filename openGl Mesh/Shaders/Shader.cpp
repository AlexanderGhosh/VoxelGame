#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

Shader::Shader() : _program(), _vert(), _geom(), _frag() { }

Shader::Shader(std::string shaderName, bool hasGeom) : Shader() {
	_vert = shaderName;
	_frag = shaderName;
	if (hasGeom) _geom = shaderName;
}

Shader::Shader(const std::string& vert, const std::string& frag) : Shader()
{
	_vert = vert;
	_frag = frag;
}

Shader::Shader(const std::string& vert, const std::string& geom, const std::string& frag) : Shader()
{
	_vert = vert;
	_geom = geom;
	_frag = frag;
}

Shader::~Shader()
{
	if (_program == 0) return;
	glDeleteProgram(_program);
}

Shader::Shader(const Shader& other) : Shader()
{
	_vert = other._vert;
	_geom = other._geom;
	_frag = other._frag;
}

Shader& Shader::operator=(const Shader& other)
{
	_vert = other._vert;
	_geom = other._geom;
	_frag = other._frag;
	return *this;
}

Shader::Shader(Shader&& other) noexcept : Shader()
{
	_program = other._program;
	_vert = other._vert;
	_geom = other._geom;
	_frag = other._frag;
	other._program = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	_program = other._program;
	_vert = other._vert;
	_geom = other._geom;
	_frag = other._frag;
	other._program = 0;
	return *this;
}

unsigned int Shader::getId() const
{
	return _program;
}

void Shader::bind() const {
	glUseProgram(_program);
}

void Shader::unBind() const {
	glUseProgram(0);
}

const int Shader::getLocation(const std::string& name) const {
	return glGetUniformLocation(_program, name.c_str());
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
bool Shader::setValueUBO(const std::string& name, const int& location) const
{
	unsigned int loc = glGetUniformBlockIndex(_program, name.c_str());
	if (loc == -1) return false;
	glUniformBlockBinding(_program, loc, 0);
	return true;
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
		vShaderFile.open(("Shaders/" + _vert + ".vert").c_str());
		fShaderFile.open(("Shaders/" + _frag + ".frag").c_str());
		std::stringstream vShaderStream, gShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();
		
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (_geom != "") {
			gShaderFile.open(("Shaders/" + _geom + ".geom").c_str());
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader file faild to be read" << _vert << " | " << _geom << " | " << _frag << std::endl;
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
		std::cout << "Vertex compilation failed: " << std::string(infoLog) << _vert << std::endl;
	}

	//geometry
	if (_geom != "") {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "Geometry compilation failed: " << std::string(infoLog) << _geom << std::endl;
		}
	}

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment compilation failed: " << std::string(infoLog) << _frag << std::endl;
	}

	// the program
	_program = glCreateProgram();
	glAttachShader(_program, vertex);

	if(_geom != "") glAttachShader(_program, geometry);

	glAttachShader(_program, fragment);
	glLinkProgram(_program);

	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(_program, 512, NULL, infoLog);
		std::cout << "Unable to link a shader: " << std::string(infoLog) << _vert << " | " << _geom << " | " << _frag << std::endl;
	}

	glDetachShader(_program, vertex);
	if (_geom != "") glDetachShader(_program, geometry);
	glDetachShader(_program, fragment);

	glDeleteShader(vertex);
	if (_geom != "") glDeleteShader(geometry);
	glDeleteShader(fragment);

	glUseProgram(0);
}