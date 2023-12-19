#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

Shader::Shader() : program_(), vert_(), geom_(), frag_() { }

Shader::Shader(std::string shaderName, bool hasGeom) : Shader() {
	vert_ = shaderName;
	frag_ = shaderName;
	if (hasGeom) geom_ = shaderName;
}

Shader::Shader(const std::string& vert, const std::string& frag) : Shader()
{
	vert_ = vert;
	frag_ = frag;
}

Shader::Shader(const std::string& vert, const std::string& geom, const std::string& frag) : Shader()
{
	vert_ = vert;
	geom_ = geom;
	frag_ = frag;
}

Shader::~Shader()
{
	if (program_ == 0) return;
	glDeleteProgram(program_);
	program_ = 0;
}

Shader::Shader(const Shader& other) : Shader()
{
	vert_ = other.vert_;
	geom_ = other.geom_;
	frag_ = other.frag_;
}

Shader& Shader::operator=(const Shader& other)
{
	vert_ = other.vert_;
	geom_ = other.geom_;
	frag_ = other.frag_;
	return *this;
}

Shader::Shader(Shader&& other) noexcept : Shader()
{
	program_ = other.program_;
	vert_ = other.vert_;
	geom_ = other.geom_;
	frag_ = other.frag_;
	other.program_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	program_ = other.program_;
	vert_ = other.vert_;
	geom_ = other.geom_;
	frag_ = other.frag_;
	other.program_ = 0;
	return *this;
}

unsigned int Shader::getId() const
{
	return program_;
}

void Shader::bind() const {
	glUseProgram(program_);
}

void Shader::unBind() const {
	glUseProgram(0);
}

const int Shader::getLocation(const std::string& name) const {
	return glGetUniformLocation(program_, name.c_str());
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
	unsigned int loc = glGetUniformBlockIndex(program_, name.c_str());
	if (loc == -1) return false;
	glUniformBlockBinding(program_, loc, 0);
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
		vShaderFile.open(("Shaders/" + vert_ + ".vert").c_str());
		fShaderFile.open(("Shaders/" + frag_ + ".frag").c_str());
		std::stringstream vShaderStream, gShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();
		
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (geom_ != "") {
			gShaderFile.open(("Shaders/" + geom_ + ".geom").c_str());
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader file faild to be read" << vert_ << " | " << geom_ << " | " << frag_ << std::endl;
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
		std::cout << "Vertex compilation failed: " << std::string(infoLog) << vert_ << std::endl;
	}

	//geometry
	if (geom_ != "") {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "Geometry compilation failed: " << std::string(infoLog) << geom_ << std::endl;
		}
	}

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment compilation failed: " << std::string(infoLog) << frag_ << std::endl;
	}

	// the program
	program_ = glCreateProgram();
	glAttachShader(program_, vertex);

	if(geom_ != "") glAttachShader(program_, geometry);

	glAttachShader(program_, fragment);
	glLinkProgram(program_);

	glGetProgramiv(program_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program_, 512, NULL, infoLog);
		std::cout << "Unable to link a shader: " << std::string(infoLog) << vert_ << " | " << geom_ << " | " << frag_ << std::endl;
	}

	glDetachShader(program_, vertex);
	if (geom_ != "") glDetachShader(program_, geometry);
	glDetachShader(program_, fragment);

	glDeleteShader(vertex);
	if (geom_ != "") glDeleteShader(geometry);
	glDeleteShader(fragment);

	glUseProgram(0);
}