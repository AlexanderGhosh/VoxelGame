#pragma once
#include <iostream>
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
	GLuint total_data_len;
	GLuint layout_count;
	std::vector<GLuint> individual_lens;

	Structure() {
		total_data_len = 9;
		layout_count = 3;
		individual_lens = { 3, 3, 3 };
	}
	Structure(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens);
	void merge(Structure& struc);
	
};
bool operator==(const Structure& s1, const Structure& s2);
bool operator!=(const Structure& s1, const Structure& s2);

class Buffer
{
public:
	Buffer() {
		VBO = 0;
		VAO = 0;
		canRender = GL_FALSE;
		structure = Structure();
	}
	GLuint getTriangleCount();
	void render();
	void createBuffers();
	GLuint getVBO();
	GLuint getVAO(); 
	std::vector<GLfloat>& getBufferData();
	void setStructure(Structure structure);
	void setBufferData(std::vector<GLfloat> bufferData);
	void merge(Buffer& b);
	void destroy();
private:
	GLuint VBO;
	GLuint VAO;
	std::vector<GLfloat> bufferData;
	Structure structure;
	GLboolean canRender;
};

