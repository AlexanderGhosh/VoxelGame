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
	std::vector<GLushort>& indices = bufferIndices;
	if (bufferData.size() < 1) {
		std::cout << "No data found in buffer" << std::endl;
		return;
	}

	/*glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);*/



	glGenBuffers(1, &VBO); // VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(GLfloat), bufferData.data(), GL_STATIC_DRAW);

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
void Buffer::render(GLboolean d) {
	if (!canRender) {
		createBuffers();
		// std::cout << "created buffer\n";
	}
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBindVertexArray(VAO);
	if (d) {
		draw();
	}
}
void Buffer::draw() {
	glDrawArrays(GL_TRIANGLES, 0, getTriangleCount());
	// glDrawElements(GL_TRIANGLES, bufferIndices.size(), GL_UNSIGNED_SHORT, 0);
}
void Buffer::setStructure(Structure structure) {
	this->structure;
}
void Buffer::setBufferData(std::vector<GLfloat> bufferData) {
	this->bufferData = bufferData;
	this->originalData = bufferData;
}
void Buffer::merge(Buffer& b) {
	structure.merge(b.structure);
	bufferData.insert(bufferData.end(), b.bufferData.begin(), b.bufferData.end());
	originalData = b.originalData;
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
void Buffer::addPositions(std::vector<glm::mat4>& positions) {
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * positions.size(), positions.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	hasInstances = 1;
}
void Buffer::resetData() {
	bufferData = originalData;
	hasInstances = 0;
	// destroy();
	createBuffers();
}
std::vector<GLfloat> Buffer::getVertices(GLboolean unique) {
	std::vector<GLfloat> res;
	if (!unique) {
		for (GLuint i = 0; i < (GLuint)bufferData.size(); i += structure.total_data_len) {
			res.insert(res.end(), bufferData.begin() + i, bufferData.begin() + i + 3);
		}
	}
	else {
		std::map<GLfloat, GLfloat> uniq;
		for (int i = 0; i < bufferData.size(); i += 3) {
			try {
				uniq.insert({ bufferData[i], 0 });
			}catch(std::exception e){ }
			try {
				uniq.insert({ bufferData[i + 1], 0 });
			}
			catch (std::exception e) {}
			try {
				uniq.insert({ bufferData[i + 2], 0 });
			}
			catch (std::exception e) {}
		}
		for (auto& p : uniq) {
			res.push_back(p.first);
		}
	}
	return res;
}
std::vector<glm::vec3> Buffer::getVertices() {
	glm::vec3 vertex;
	std::vector<glm::vec3> res;
	for (GLuint i = 0; i < (GLuint)bufferData.size(); i += structure.total_data_len) {
		vertex = { bufferData[i], bufferData[i + 1] , bufferData[i + 2] };
		GLboolean found = 0;
		for (auto const& contence : res) {
			if (contence == vertex) {
				found = 1;
				break;
			}
		}
		if (!found) {
			res.push_back(vertex);
		}
	}
	return res;
}