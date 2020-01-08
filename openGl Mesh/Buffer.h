#pragma once
#include <array>
#include <vector>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

struct Structure {
	std::vector<GLfloat> data;
	GLuint layout_c;
	GLuint data_l;
	std::vector<GLuint> data_ls;

	Structure() {
		data = std::vector<GLfloat>();
		layout_c = 0;
		data_l = 0;
		data_ls = std::vector<GLuint>();
	}
	Structure(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens, std::vector<GLfloat> data_);
};
bool operator==(const Structure& s1, const Structure& s2);
bool operator!=(const Structure& s1, const Structure& s2);

class Buffer
{
public:
	Buffer() {
		vertex_objs = std::array<GLuint, 2>();
		data_s = Structure();
	}
	GLuint getTriangleCnt();
	GLboolean loadBuffers();
	void loadData(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens, std::vector<GLfloat> data);
	void loadData(Structure& struc);
	GLuint getVBO();
	GLuint getVAO();
private:
	std::array<GLuint, 2> vertex_objs;
	Structure data_s;
};

