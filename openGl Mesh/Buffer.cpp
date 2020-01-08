#include "Buffer.h"
Structure::Structure(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens, std::vector<GLfloat> data_) {
	data_l = dataLength;
	layout_c = layoutsCount;
	data_ls = dataLens;
	data = data_;
}

bool operator==(const Structure& s1, const Structure& s2) {
	if (s1.data_l == s2.data_l && s1.layout_c == s2.layout_c) {
		for (GLuint i = 0; i < s1.data_ls.size(); i++) {
			if (s1.data_ls[i] != s2.data_ls[i]) {
				return false;
			}
		}
		return true;
	}
	return false;
}
bool operator!=(const Structure& s1, const Structure& s2) {
	return !(s1 == s2);
}

void Buffer::loadData(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens, std::vector<GLfloat> data) {
	data_s = Structure(dataLength, layoutsCount, dataLens, data);
}
void Buffer::loadData(Structure& struc) {
	data_s = struc;
}
GLboolean Buffer::loadBuffers() {
	std::vector<GLfloat> vertices = data_s.data;
	glGenBuffers(1, &vertex_objs[0]); // VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertex_objs[1]); // VAO
	glBindVertexArray(vertex_objs[1]);

	GLint pointerMul = 0;
	for (GLuint i = 0; i < data_s.data_ls.size(); i++)
	{
		pointerMul += (i == 0) ? 0 : data_s.data_ls[i - 1];
		glVertexAttribPointer(i, data_s.data_ls[i], GL_FLOAT, GL_FALSE, data_s.data_l * sizeof(GLfloat), (GLvoid*)(pointerMul * sizeof(GLfloat)));
		glEnableVertexAttribArray(i);
	}
	glBindVertexArray(0);
	return GL_TRUE;
}
GLuint Buffer::getVBO() {
	return vertex_objs[0];
}
GLuint Buffer::getVAO() {
	return vertex_objs[1];
}
GLuint Buffer::getTriangleCnt() {
	return data_s.data.size() / data_s.data_l;
}
void Buffer::destroy() {
	glDeleteVertexArrays(1, &vertex_objs[1]);
	glDeleteBuffers(1, &vertex_objs[0]);
}