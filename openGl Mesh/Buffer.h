#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw3.h>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>


enum FACES_NAMES {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	NULL_
};
struct Structure {
	unsigned int total_data_len;
	unsigned int layout_count;
	std::vector<unsigned int> individual_lens;

	Structure() {
		total_data_len = 9;
		layout_count = 3;
		individual_lens = { 3, 3, 3 };
	}
	Structure(unsigned int dataLength, unsigned int layoutsCount, std::vector<unsigned int> dataLens);
	void merge(Structure& struc);
	
};
bool operator==(const Structure& s1, const Structure& s2);
bool operator!=(const Structure& s1, const Structure& s2);

class Buffer
{
public:
	FACES_NAMES type;
	Buffer() {
		VBO = 0;
		VAO = 0;
		IBO = 0;
		canRender = GL_FALSE;
		structure = Structure();
	}
	unsigned int getTriangleCount();
	void render(bool draw = GL_TRUE);
	void createBuffers();
	unsigned int getVBO();
	unsigned int getVAO(); 
	std::vector<float>& getBufferData();
	void setStructure(Structure structure);
	void setBufferData(std::vector<float> bufferData);
	void merge(Buffer& b);
	void destroy();
	void endRender();
	void draw();
	void addPositions(std::vector<glm::mat4>& positions);
	void resetData();
	std::vector<float> getVertices(bool unique);
	std::vector<glm::vec3> getVertices(bool unique, bool overload);
private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int IBO;
	bool hasInstances;
	std::vector<float> bufferData;
	std::vector<GLushort> bufferIndices;
	Structure structure;
	bool canRender;
	std::vector<float> originalData;
};

