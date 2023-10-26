#include "Shader.h"

Shader::Shader(std::string shaderName, bool hasGeom) : Shader() {
	name = shaderName;
	this->hasGeom = hasGeom;
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
void Shader::setLocation(GLint& location, glm::vec4& value) {
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Shader::setLocation(GLint& location, glm::vec3& value) {
	glUniform3f(location, value.x, value.y, value.z);
}
void Shader::setLocation(GLint& location, glm::vec2& value)
{
	glUniform2f(location, value.x, value.y);
}
void Shader::setLocation(GLint& location, GLfloat& value) {
	glUniform1f(location, value);
}
void Shader::setLocation(GLint& location, const GLint& value) {
	glUniform1i(location, value);
}
GLboolean Shader::setValue(std::string name, const GLint& value) {
	GLint loc = getLocation(name);
	if (loc == -1) return GL_FALSE;
	setLocation(loc, value);
	return GL_TRUE;
}
GLboolean Shader::setValue(std::string name, Material& value)
{
	GLboolean res = 0;
	res = setValue(name + ".ambient", value.ambient);
	if (!res) return 0;
	res = setValue(name + ".diffuse", value.diffuse);
	if (!res) return 0;
	res = setValue(name + ".specular", value.specular);
	if (!res) return 0;
	res = setValue(name + ".shininess", value.shininess);
	if (!res) return 0;
}
GLboolean Shader::setValue(std::string name, glm::vec4& value) {
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
GLboolean Shader::setValue(std::string name, glm::vec2& value)
{
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
void Shader::setUp() {
	std::string vertexCode, geometryCode, fragmentCode;
	std::ifstream vShaderFile, gShaderFile, fShaderFile;
	vShaderFile.exceptions(std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		vShaderFile.open(("Shaders/" + name + "_v.glsl").c_str());
		fShaderFile.open(("Shaders/" + name + "_f.glsl").c_str());
		std::stringstream vShaderStream, gShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();
		
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (hasGeom) {
			gShaderFile.open(("Shaders/" + name + "_g.glsl").c_str());
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader file faild to be read" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, geometry, fragment;
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

	//geometry
	if (hasGeom) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "Geometry compilation failed: " << std::string(infoLog) << std::endl;
		}
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

	// the program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	if(hasGeom) glAttachShader(program, geometry);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Unable to link a shader: " << std::string(infoLog) << std::endl;
	}

	glDetachShader(program, vertex);
	if (hasGeom) glDetachShader(program, geometry);
	glDetachShader(program, fragment);

	glDeleteShader(vertex);
	if (hasGeom) glDeleteShader(geometry);
	glDeleteShader(fragment);

	glUseProgram(0);
}
void Shader::setName(std::string name) {
	this->name = name;
}