#include "Buffer.h"
Structure::Structure(GLuint dataLength, GLuint layoutsCount, std::vector<GLuint> dataLens) {
	total_data_len = dataLength;
	layout_count = layoutsCount;
	individual_lens = dataLens;
}
void Structure::merge(Structure& structure) {
	total_data_len = structure.total_data_len;
	layout_count = structure.layout_count;
	individual_lens = structure.individual_lens;
}
bool operator==(const Structure& s1, const Structure& s2) {
	if (s1.total_data_len == s2.total_data_len && s1.layout_count == s2.layout_count) {
		for (GLuint i = 0; i < s1.layout_count; i++) {
			if (s1.individual_lens[i] != s2.individual_lens[i]) {
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


GLuint Buffer::getVBO() {
	return VBO;
}
GLuint Buffer::getVAO() {
	return VAO;
}
std::vector<GLfloat>& Buffer::getBufferData() {
	return bufferData;
}
GLuint Buffer::getTriangleCount() {
	return bufferData.size() / structure.total_data_len;
}
void Buffer::createBuffers() {
	std::vector<GLfloat>& vertices = bufferData;
	if (vertices.size() < 1) {
		std::cout << "No data found in buffer" << std::endl;
		return;
	}
	glGenBuffers(1, &VBO); // VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO); // VAO
	glBindVertexArray(VAO);

	GLint pointerMul = 0;
	for (GLuint i = 0; i < structure.layout_count; i++)
	{
		pointerMul += (i == 0) ? 0 : structure.individual_lens[i - 1];
		glVertexAttribPointer(i, structure.individual_lens[i], GL_FLOAT, GL_FALSE, structure.total_data_len * sizeof(GLfloat), (GLvoid*)(pointerMul * sizeof(GLfloat)));
		glEnableVertexAttribArray(i);
	}
	glBindVertexArray(0);
	canRender = GL_TRUE;
}
void Buffer::render() {
	if (!canRender) {
		createBuffers();
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, getTriangleCount());
}
void Buffer::setStructure(Structure structure) {
	this->structure;
}
void Buffer::setBufferData(std::vector<GLfloat> bufferData) {
	this->bufferData = bufferData;
}
void Buffer::merge(Buffer& b) {
	structure.merge(b.structure);
	bufferData.insert(bufferData.end(), b.bufferData.begin(), b.bufferData.end());
}
void Buffer::destroy() {
	structure = Structure();
	bufferData = std::vector<GLfloat>();
	canRender = GL_FALSE;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
void Buffer::endRender() {
	glBindVertexArray(0);
}