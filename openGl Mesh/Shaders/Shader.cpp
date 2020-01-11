#include "Shader.h"

Shader::Shader(std::string shaderName) {
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		vShaderFile.open(("Shaders/" + shaderName + "_v.gls").c_str());
		fShaderFile.open(("Shaders/" + shaderName + "_f.gls").c_str());
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader file faild to be read" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	//vertex
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Vertex compilation failed: " << std::string(infoLog) << std::endl;
	}

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment compilation failed: " << std::string(infoLog) << std::endl;
	}
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Unable to link a shader: " << std::string(infoLog) << std::endl;
	}

	glDetachShader(program, vertex);
	glDetachShader(program, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
void Shader::bind() {
	glUseProgram(program);
}
void Shader::unBind() {
	glUseProgram(0);
}
GLint Shader::getLocation(std::string name) {
	return glGetUniformLocation(program, name.c_str());
}

void Shader::setLocation(GLint& location, glm::mat4& value) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setLocation(GLint& location, glm::vec3& value) {
	glUniform3f(location, value.x, value.y, value.z);
}
void Shader::setLocation(GLint& location, GLfloat& value) {
	glUniform1f(location, value);
}
void Shader::setLocation(GLint& location, GLint& value) {
	glUniform1f(location, value);
}
GLboolean Shader::setValue(std::string name, GLint& value) {
	GLint loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
GLboolean Shader::setValue(std::string name, glm::vec3& value) {
	GLint loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
GLboolean Shader::setValue(std::string name, GLfloat& value) {
	GLint loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
GLboolean Shader::setValue(std::string name, glm::mat4& value) {
	GLint loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}