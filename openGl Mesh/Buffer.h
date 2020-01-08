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

	Structure();
	Structure(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens, std::vector<GLfloat> data_);
};
bool operator==(const Structure& s1, const Structure& s2);
bool operator!=(const Structure& s1, const Structure& s2);

class Buffer
{
public:
	Buffer();
	GLuint getTriangleCnt();
	GLboolean loadBuffers();
	void loadData(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens, std::vector<GLfloat> data);
private:
	std::array<GLuint, 2> vertex_objs;
	Structure data_struct;
};

