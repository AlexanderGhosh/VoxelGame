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
		std::cout << "Unable to load a shader: " << std::string(infoLog) << std::endl;
		//std::cout << "Vertex compilation failed: " << infoLog;
	}

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Unable to load a shader: " << std::string(infoLog) << std::endl;
		//std::cout << "Vertex compilation failed: " << infoLog;
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
void Shader::use() {
	glUseProgram(program);
}
void Shader::bind() {
}
GLuint Shader::getLocation(std::string name) {
	return glGetUniformLocation(program, name.c_str());
}

void Shader::setLocation(GLuint& location, glm::mat4& value) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}